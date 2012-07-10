/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2010  Benoit Blancard <benblan@users.sourceforge.net>*
 *   Copyright (C) 2010  Bohdan Stelmakh <chamel@users.sourceforge.net> *
 *   Copyright (C) 2011  Joey Parrish  <joey.parrish@gmail.com>         *
 *                                                                      *
 *    This program is free software;  you can redistribute it and / or  *
 *  modify it  under the  terms of the  GNU General  Public License as  *
 *  published by the Free Software Foundation; either version 2 of the  *
 *  License, or (at your option) any later version.                     *
 *                                                                      *
 *    This program is  distributed in the hope that it will be useful,  *
 *  but WITHOUT  ANY WARRANTY;  without even  the implied  warranty of  *
 *  MERCHANTABILITY  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *
 *  General Public License for more details.                            *
 *                                                                      *
 *    You can view the GNU  General Public License, online, at the GNU  *
 *  project's  web  site;  see <http://www.gnu.org/licenses/gpl.html>.  *
 *  The full text of the license is also included in the file COPYING.  *
 *                                                                      *
 ************************************************************************/

#include "app.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

#include <iostream>
#include <fstream>
#include <set>

#ifdef __APPLE__
// Carbon includes an AIFF header which conflicts with fliplayer.h
// So we will redefine ChunkHeader temporarily to work around that.
#define ChunkHeader CarbonChunkHeader
#include <Carbon/Carbon.h>
#undef ChunkHeader
#endif

#include "gfx/spritemanager.h"
#include "gfx/screen.h"
#include "sound/audio.h"
#include "utils/ccrc32.h"
#include "utils/file.h"
#include "utils/log.h"
#include "utils/configfile.h"
#include "utils/portablefile.h"

App::App(bool disable_sound): running_(true),
screen_(new Screen(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT))
#ifdef SYSTEM_SDL
    , system_(new SystemSDL())
#else
#error A suitable System object has not been defined!
#endif
    , intro_sounds_(disable_sound), game_sounds_(disable_sound), music_(disable_sound)
{
    fullscreen_ = false;
    playIntro_ = true;
#ifdef _DEBUG
    debug_breakpoint_trigger_ = 0;
#endif
}

App::~App() {
}

/*!
 * Destory the application.
 */
void App::destroy() {
    menus_.destroy();
}

static void addMissingSlash(string& str) {
    if (str[str.length() - 1] != '/') str.push_back('/');
}

#ifdef __APPLE__
static bool getResourcePath(string& resourcePath) {
    // let's check to see if we're inside an application bundle first.
    CFBundleRef main = CFBundleGetMainBundle();
    CFStringRef appid = NULL;
    if (main) appid = CFBundleGetIdentifier(main);
    if (!appid) return false;

    // we're in an app bundle.
    printf("OS X application bundle detected.\n");
    CFURLRef url = CFBundleCopyResourcesDirectoryURL(main);
    if (!url) {
        // this shouldn't happen.
        printf("Unable to locate resources.\n");
        exit(1);
    }
    FSRef fs;
    if (!CFURLGetFSRef(url, &fs)) {
        // this shouldn't happen.
        printf("Unable to translate URL.\n");
        exit(1);
    }

    char *buf = (char *)malloc(1024);
    FSRefMakePath(&fs, (UInt8 *)buf, 1024);
    CFRelease(url);

    resourcePath.assign(buf);
    resourcePath.push_back('/');
    free(buf);
    return true;
}
#endif

#ifdef _WIN32
static string exeFolder() {
	char buf[1024];
	GetModuleFileName(NULL, buf, 1024);
	string tmp(buf);
	size_t pos = tmp.find_last_of('\\');
	if (pos != std::string::npos) tmp.erase(pos + 1);
	else tmp = ".";
	return tmp;
}
#endif

string App::defaultIniFolder()
{
	string folder;
#ifdef _WIN32
	// Under windows config file is in the same directory as freesynd.exe
	folder.assign(exeFolder());
	// Since we're defaulting to the exe's folder, no need to try to create a directory.
#elif defined(__APPLE__)
	// Make a symlink for convenience for *nix people who own Macs.
	folder.assign(getenv("HOME"));
	folder.append("/.freesynd");
	symlink("Library/Application Support/FreeSynd", folder.c_str());
	// On OS X, applications tend to store config files in this sort of path.
	folder.assign(getenv("HOME"));
	folder.append("/Library/Application Support/FreeSynd");
	mkdir(folder.c_str(), 0755);
#else
	// Under unix it's in the user home directory
	folder.assign(getenv("HOME"));
	folder.append("/.freesynd");
	mkdir(folder.c_str(), 0755);
#endif
	// note that we don't care if the mkdir() calls above succeed or not.
	// if they fail because they already exist, then it's no problem.
	// if they fail for any other reason, then we won't be able to open
	// or create freesynd.ini, and we'll surely detect that below.

	return folder;
}

bool App::readConfiguration() {
    File::setHomePath(defaultIniFolder());

    try {
        ConfigFile conf(iniPath_);
        string origDataDir;
        string ourDataDir;
        conf.readInto(fullscreen_, "fullscreen", false);
        conf.readInto(playIntro_, "play_intro", true);
        bool origDataDirFound = conf.readInto(origDataDir, "data_dir");
        bool ourDataDirFound = conf.readInto(ourDataDir, "freesynd_data_dir");

        if (ourDataDirFound == false) {
#ifdef _WIN32
            ourDataDir = exeFolder() + "\\data";
#elif defined(__APPLE__)
            if (getResourcePath(ourDataDir)) {
                // this is an app bundle, so let's default the data dir
                // to the one included in the app bundle's resources.
                ourDataDir += "data/";
            } else {
                LOG(Log::k_FLG_GFX, "App", "readConfiguration", ("Unable to locate app bundle resources.\n"));
                return false;
            }
#else
            ourDataDir = PREFIX"/share/freesynd/data";
#endif
        }
        addMissingSlash(ourDataDir);
        File::setOurDataPath(ourDataDir);

        if (origDataDirFound == false) {
            origDataDir = ourDataDir;
        }
        addMissingSlash(origDataDir);
        File::setDataPath(origDataDir);

        switch (conf.read("language", 0)) {
            case 0:
                menus_.setLanguage(MenuManager::ENGLISH);
                break;
            case 1:
                menus_.setLanguage(MenuManager::FRENCH);
                break;
            case 2:
                menus_.setLanguage(MenuManager::ITALIAN);
                break;
            case 3:
                menus_.setLanguage(MenuManager::GERMAN);
                break;
            default:
                menus_.setLanguage(MenuManager::ENGLISH);
                break;
        }

        return true;
    } catch (...) {
        LOG(Log::k_FLG_GFX, "App", "readConfiguration", ("failed to load config file \"%s\"\n", iniPath_.c_str()));
        return false;
    }
}

bool App::testOriginalData() {
    bool test_files = true;
    ConfigFile conf(iniPath_);
    conf.readInto(test_files, "test_data", true);
    if (test_files == false)
        return true;

    std::string crcflname = File::dataFullPath("ref/original_data.crc");
    std::ifstream od(crcflname.c_str());
    if (od.fail()) {
        LOG(Log::k_FLG_GFX, "App", "testOriginalData",
            ("Checksums file for original data is not found. Look at INSTALL/README file for possible solutions."));
        return false;
    }
    printf("Testing original Syndicate data...\n");
    LOG(Log::k_FLG_GFX, "App", "testOriginalData", ("Testing original Syndicate data..."));
    CCRC32 crc32_test;
    crc32_test.Initialize();
    bool rsp = true;
    while (od) {
        std::string line;
        std::getline(od, line);
        if (line.size() > 0) {
            std::string::size_type pos = line.find(' ');
            if (pos != std::string::npos) {
                // skipping commented
                if (line[0] == '#' || line[0] == ';')
                    continue;
                std::string flname = line.substr(0, pos);
                std::string str_crc32 = line.substr(pos+1);
                uint32 ui_crc32 = 0;
                uint32 multiply = 1 << (4 * 7);
                // String hex to uint32
                for (char i = 0; i < 8; i++) {
                    char c = str_crc32[i];
                    if ( c >= '0' && c <= '9')
                        c -= '0';
                    if ( c >= 'a' && c <= 'z')
                        c -= 'a' - 10;
                    ui_crc32 += c * multiply;
                    multiply >>= 4;
                }
                int sz;
                uint8 *data = File::loadOriginalFileToMem(flname, sz);
                if (!data) {
                    LOG(Log::k_FLG_GFX, "App", "testOriginalData", ("file not found \"%s\"\n", flname.c_str()));
                    printf("file not found \"%s\". Look at INSTALL/README file for possible solutions.\n", flname.c_str());
                    rsp = false;
                    continue;
                }
                if (ui_crc32 != crc32_test.FullCRC(data, sz)) {
                    rsp = false;
                    LOG(Log::k_FLG_GFX, "App", "testOriginalData", ("file test failed \"%s\"\n", flname.c_str()));
#ifdef _DEBUG
                    printf("file test failed \"%s\"\n", flname.c_str());
#endif
                }
                delete[] data;
            }
        }
    }
    if (rsp == false) {
        printf("Test failed.\n");
    } else {
        try {
            conf.add("test_data", false);

            std::ofstream file(iniPath_.c_str(), std::ios::out | std::ios::trunc);
            if (file) {
                file << conf;
                file.close();
            } else {
                LOG(Log::k_FLG_GFX, "App", "testOriginalData", ("Could not update configuration file!"))
            }
        } catch (...) {
            LOG(Log::k_FLG_GFX, "App", "testOriginalData", ("Could not update configuration file!"))
        }
        printf("Test passed. crc32 for data is correct.\n");
        LOG(Log::k_FLG_GFX, "App", "testOriginalData", ("Test passed. CRC32 for data is correct."));
    }
    od.close();
    return rsp;
}

bool App::loadWalkData() {
    int size = 0;
    uint8 *data;

    // reads data from file
    data = File::loadOriginalFile("col01.dat", size);
    if (!data) {
        return false;
    }

    memcpy(walkdata_p_, data, 256);
    // little patch to enable full surface description
    // and eliminate unnecessary data
    // 0x10 - non-surface/non-walkable, always above train stop
    // 0x11, 0x12 - train entering surface
    walkdata_p_[0x80] = 0x11;
    walkdata_p_[0x81] = 0x12;
    walkdata_p_[0x8F] = 0x00;
    walkdata_p_[0x93] = 0x00;
    delete[] data;

    return true;
}

void App::updateIntroFlag() {
    try {
        ConfigFile conf(iniPath_);
        conf.add("play_intro", false);

        std::ofstream file(iniPath_.c_str(), std::ios::out | std::ios::trunc);
        if (file) {
            file << conf;
            file.close();
        } else {
            LOG(Log::k_FLG_GFX, "App", "updateIntroFlag", ("Could not update configuration file!"))
        }
    } catch (...) {
        LOG(Log::k_FLG_GFX, "App", "updateIntroFlag", ("Could not update configuration file!"))
    }
}

/*!
 * Initialize application.
 * \param iniPath The path to the config file.
 * \return True if initialization is ok.
 */
bool App::initialize(const std::string& iniPath) {
    iniPath_ = iniPath;

    LOG(Log::k_FLG_INFO, "App", "initialize", ("reading configuration..."))
    if (!readConfiguration()) {
        LOG(Log::k_FLG_GFX, "App", "initialize", ("failed to read configuration..."))
        return false;
    }

    LOG(Log::k_FLG_INFO, "App", "initialize", ("testing original Syndicate data..."))
    if (!testOriginalData()) {
        LOG(Log::k_FLG_GFX, "App", "initialize", ("failed to test original Syndicate data..."))
        return false;
    }

    LOG(Log::k_FLG_INFO, "App", "initialize", ("initializing system..."))
    if (!system_->initialize(fullscreen_)) {
        return false;
    }

	LOG(Log::k_FLG_INFO, "App", "initialize", ("initializing menus..."))
	if (!menus_.initialize(playIntro_)) {
		return false;
	}

	if (!loadWalkData()) {
		return false;
	}

	LOG(Log::k_FLG_INFO, "App", "initialize", ("loading game sprites..."))
	if (!gameSprites().loaded())
		gameSprites().load();

    LOG(Log::k_FLG_INFO, "App", "initialize", ("loading game tileset..."))
	if (!maps().initialize()) {
		return false;
    }

	if (playIntro_) {
		LOG(Log::k_FLG_INFO, "App", "initialize", ("Loading intro sounds..."))
		if (!intro_sounds_.loadSounds(SoundManager::SAMPLES_INTRO)) {
			return false;
		}
	}

    LOG(Log::k_FLG_INFO, "App", "initialize", ("Loading game sounds..."))
    if (!game_sounds_.loadSounds(SoundManager::SAMPLES_GAME)) {
        return false;
    }

    LOG(Log::k_FLG_INFO, "App", "initialize", ("Loading music..."))
    music_.loadMusic();

    LOG(Log::k_FLG_INFO, "App", "initialize", ("Loading game data..."))
    agents_.loadAgents();
    return reset();
}

/*!
 * Activate cheat mode in which all completed missions can be replayed.
 */
void App::cheatRepeatOrCompleteMission() {
    session_.cheatReplayMission();
}

void App::cheatWeaponsAndMods() {
    weapons_.cheatEnableAllWeapons();
    mods_.cheatEnableAllMods();
}

void App::cheatEquipAllMods() {
    for (int agent = 0; agent < AgentManager::MAX_AGENT; agent++) {
        Agent *pAgent = agents_.agent(agent);
        if (pAgent) {
            pAgent->clearSlots();

            pAgent->addMod(mods_.getMod(Mod::MOD_LEGS, Mod::MOD_V3));
            pAgent->addMod(mods_.getMod(Mod::MOD_ARMS, Mod::MOD_V3));
            pAgent->addMod(mods_.getMod(Mod::MOD_EYES, Mod::MOD_V3));
            pAgent->addMod(mods_.getMod(Mod::MOD_BRAIN, Mod::MOD_V3));
            pAgent->addMod(mods_.getMod(Mod::MOD_CHEST, Mod::MOD_V3));
            pAgent->addMod(mods_.getMod(Mod::MOD_HEART, Mod::MOD_V3));
        }
    }
}

/*!
 * Activate cheat mode in which all missions are playable.
 */
void App::cheatAnyMission() {
    session_.cheatEnableAllMission();
}

void App::cheatResurrectAgents() {
    // TODO: Implement cheatResurrectAgents()
}

void App::cheatOwnAllCountries() {
    // TODO: Implement cheatOwnAllCountries()
}

void App::cheatAccelerateTime() {
    g_Session.cheatAccelerateTime();
}

void App::cheatFemaleRecruits() {
    agents_.reset(true);

    for (int i = 0; i < 4; i++)
        session_.setTeamMember(i, agents_.agent(i));
}

void App::cheatEquipFancyWeapons() {
    for (int i = 0; i < AgentManager::MAX_AGENT; i++) {
        if (agents_.agent(i)) {
        agents_.agent(i)->removeAllWeapons();
#ifdef _DEBUG
        agents_.agent(i)->addWeapon(
            weapons_.getWeapon(Weapon::Minigun)->createInstance());
        agents_.agent(i)->addWeapon(
            weapons_.getWeapon(Weapon::TimeBomb)->createInstance());
        agents_.agent(i)->addWeapon(
            weapons_.getWeapon(Weapon::GaussGun)->createInstance());
        agents_.agent(i)->addWeapon(
            weapons_.getWeapon(Weapon::Flamer)->createInstance());
        agents_.agent(i)->addWeapon(
            weapons_.getWeapon(Weapon::Uzi)->createInstance());
        agents_.agent(i)->addWeapon(
            weapons_.getWeapon(Weapon::Persuadatron)->createInstance());
        agents_.agent(i)->addWeapon(
            weapons_.getWeapon(Weapon::Laser)->createInstance());
        agents_.agent(i)->addWeapon(
            weapons_.getWeapon(Weapon::AccessCard)->createInstance());
#else
        agents_.agent(i)->addWeapon(
                weapons_.getWeapon(Weapon::Minigun)->createInstance());
        agents_.agent(i)->addWeapon(
                weapons_.getWeapon(Weapon::Minigun)->createInstance());
        agents_.agent(i)->addWeapon(
                weapons_.getWeapon(Weapon::Persuadatron)->createInstance());
        agents_.agent(i)->addWeapon(
                weapons_.getWeapon(Weapon::TimeBomb)->createInstance());
        agents_.agent(i)->addWeapon(
                weapons_.getWeapon(Weapon::EnergyShield)->createInstance());
        agents_.agent(i)->addWeapon(
                weapons_.getWeapon(Weapon::EnergyShield)->createInstance());
        agents_.agent(i)->addWeapon(
                weapons_.getWeapon(Weapon::Laser)->createInstance());
        agents_.agent(i)->addWeapon(
                weapons_.getWeapon(Weapon::Laser)->createInstance());
#endif
        }
    }
}

/*!
 * Activate a cheat code with the given name. Possible 
 * cheat codes are :
 * - DO IT AGAIN : Possibility to replay a completed mission
 * - NUK THEM : Enable all missions and resurrect dead agents
 * - OWN THEM : All countries belong to the user
 * - ROB A BANK : Puts $100 000 000 in funds
 * - TO THE TOP : Puts $100 000 000 in funds and activates all missions
 * - COOPER TEAM : $100 000 000 in funds, select any mission, all weapons 
 *   and mods
 * - WATCH THE CLOCK : Accelerates time
 *
 * \param name The name of a cheat code.
 */
void App::setCheatCode(const char *name) {
    
    // Repeat mission with previously obtained items, press 'C' or 'Ctrl-C'
    // to instantly complete a mission
    if (!strcmp(name, "DO IT AGAIN"))
        cheatRepeatOrCompleteMission();
    else if (!strcmp(name, "NUK THEM")) {
        // Select any mission, resurrect dead agents
        cheatAnyMission();
        cheatResurrectAgents();
    }
    else if (!strcmp(name, "OWN THEM")) {
        // Own all countries
        cheatOwnAllCountries();
    }
    else if (!strcmp(name, "ROB A BANK")) {
        // $100 000 000 in funds
        cheatFunds();
    }
    else if (!strcmp(name, "TO THE TOP")) {
        // $100 000 000 in funds, select any mission
        cheatFunds();
        cheatAnyMission();
    }
    else if (!strcmp(name, "COOPER TEAM")) {
        // $100 000 000 in funds, select any mission, all weapons and mods
        cheatFemaleRecruits();
        cheatFunds();
        cheatAnyMission();
        cheatWeaponsAndMods();
        cheatEquipAllMods();
        cheatEquipFancyWeapons();
    }
    else if (!strcmp(name, "WATCH THE CLOCK")) {
        // Accelerate time for faster research completion
        cheatAccelerateTime();
    }
}

/*!
 * Reset application data.
 * \returns True if reset is ok.
 */
bool App::reset() {
    // Reset default mods and weapons
    weapons_.reset();
    mods_.reset();

    // Reset default agents
    agents_.reset();

    // Reset user session
    if (!session_.reset()) {
        return false;
    }

    for (int i = 0; i < 4; i++)
        session_.setTeamMember(i, agents_.agent(i));

    return true;
}

void App::waitForKeyPress() {

    while (running_) {
        // small pause while waiting for key, also mouse event
        SDL_Delay(20);
        menus_.handleEvents();
    }
}

/*!
 * This method defines the application loop.
 * \param start_mission Mission id used to start the application in debug mode
 * In standard mode start_mission is always -1.
 */
void App::run(int start_mission) {

    // load palette
    menus().setDefaultPalette();

#if 0
    system_->updateScreen();
    int nx = 0, ny = 0, my = 0;
    for (int i = 0; i < tabSize / 6; i++) {
        Sprite *s = menu_sprites_.sprite(i);
        if (nx + s->width() >= GAME_SCREEN_WIDTH) {
            nx = 0;
            ny += my;
            my = 0;
        }
        if (ny + s->height() > GAME_SCREEN_HEIGHT)
            break;
        s->draw(nx, ny, 0);
        system_->updateScreen();
        nx += s->width();
        if (s->height() > my)
            my = s->height();
    }

    waitForKeyPress();
    exit(1);
#endif

    if (start_mission == -1) {
		if (playIntro_) {
			menus_.gotoMenu(Menu::MENU_FLI_INTRO);
			// Update intro flag so intro won't be played next time
			updateIntroFlag();
		} else {
			// play title before going to main menu
			menus_.gotoMenu(Menu::MENU_FLI_TITLE);
		}
    }
    else {
        // Debug scenario : start directly with the brief menu
        // in the given mission
        // First, we find the block associated with the given
        // mission number
        for (int i = 0; i < 50; i++) {
            if (session_.getBlock(i).mis_id == start_mission) {
                session_.setSelectedBlockId(i);
				break;
            }
        }
        // Then we go to the brief menu
		menus_.gotoMenu(Menu::MENU_BRIEF);
    }

    int lasttick = SDL_GetTicks();
    while (running_) {
		menus_.handleEvents();
        int curtick = SDL_GetTicks();
        int diff_ticks = curtick - lasttick;
        if (diff_ticks < 30) {
            SDL_Delay(30 - diff_ticks);
            continue;
        }
        menus_.handleTick(diff_ticks);
        menus_.renderMenu();
        lasttick = curtick;
        system_->updateScreen();
    }

#ifdef GP2X
#ifndef WIN32
    // return to the menu
    chdir("/usr/gp2x");
    execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
#endif
#endif
}

bool App::saveGameToFile(int fileSlot, std::string name) {
    LOG(Log::k_FLG_IO, "App", "saveGameToFile", ("Saving %s in slot %d", name.c_str(), fileSlot))
    
    PortableFile outfile;
    std::string path;
            
    File::getFullPathForSaveSlot(fileSlot, path);
    LOG(Log::k_FLG_IO, "App", "saveGameToFile", ("Saving to file %s", path.c_str()))

    outfile.open_to_overwrite(path.c_str());

    if (outfile) {
        // write file format version
        outfile.write8(1); // major
        outfile.write8(1); // minor

        // Slot name is 31 characters long, nul-padded
        outfile.write_string(name, 31);
        
        // Session
        session_.saveToFile(outfile);

        // Weapons
        weapons_.saveToFile(outfile);

        // Mods
        mods_.saveToFile(outfile);

        // Agents
        agents_.saveToFile(outfile);

        // save current squad
        for (int i=0; i<4; i++) {
            Agent *pAgent = session_.teamMember(i);
            int id = pAgent ? pAgent->getId() : 0;
            outfile.write32(id);
        }

        // save researches
        session_.researchManager().saveToFile(outfile);

        return true;
    }

    return false;
}

bool App::loadGameFromFile(int fileSlot) {

    std::string path;
    PortableFile infile;

    File::getFullPathForSaveSlot(fileSlot, path);

    infile.open_to_read(path.c_str());

    if (infile) {
        // FIXME: detect original game saves

        // Read version
        unsigned char vMaj = infile.read8();
        unsigned char vMin = infile.read8();
        FormatVersion v(vMaj, vMin);

        // validate that this is a supported version.
        if (v.majorVersion() == 1) {
            // versions 1.0 and 1.1 are supported.

            if (v.minorVersion() > 1) {
                // future version.
                return false;
            }
        } else {
            return false;
        }

        if (v == 0x0100) {
            // the 1.0 format is in native byte order instead of big-endian.
            infile.set_system_endian();
        }

        reset();

        // Read slot name
        std::string slotName;
        // Original game: 20 chars on screen, 20 written, 19 read.
        // v1.0: 25 characters.
        // v1.1: 31 characters.
        slotName = infile.read_string((v == 0x0100) ? 25 : 31, true);

        session_.loadFromFile(infile, v);

        // Weapons
        weapons_.loadFromFile(infile, v);

        // Mods
        mods_.loadFromFile(infile, v);

        // Agents
        agents_.loadFromFile(infile, v);

        // Read squad
        for (int squadInd=0; squadInd<4; squadInd++) {
            int id = infile.read32();
            if (id != 0) {
                for (int iAgnt=0; iAgnt<AgentManager::MAX_AGENT; iAgnt++) {
                    Agent *pAgent = agents_.agent(iAgnt);
                    if (pAgent && pAgent->getId() == id) {
                        session_.setTeamMember(squadInd, pAgent);
                        break;
                    }
                }
            } else {
                session_.setTeamMember(squadInd, NULL);
            }
        }

        // Research
        session_.researchManager().loadFromFile(infile, v);

        return true;
    }

    return false;
}
