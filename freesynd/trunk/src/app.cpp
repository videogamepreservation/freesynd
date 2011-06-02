/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2010  Benoit Blancard <benblan@users.sourceforge.net>*
 *   Copyright (C) 2010  Bohdan Stelmakh <chamel@users.sourceforge.net> *
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
#endif

#include <iostream>
#include <fstream>
#include <set>

#ifdef SYSTEM_SDL
#include "system_sdl.h"
#endif

#include "gfx/fliplayer.h"
#include "gfx/spritemanager.h"
#include "gfx/screen.h"
#include "sound/audio.h"
#include "utils/file.h"
#include "utils/log.h"
#include "utils/configfile.h"

App::App(): running_(true), playingFli_(false),
skipFli_(false), screen_(new Screen(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT))
#ifdef SYSTEM_SDL
    , system_(new SystemSDL())
#else
#error A suitable System object has not been defined!
#endif
{
    fullscreen_ = false;
    playIntro_ = true;
}

App::~App() {
}

/*!
 * Destory the application.
 */
void App::destroy() {
    menus_.destroy();
}

bool App::readConfiguration(const char *dir) {
    std::string path(dir);
    path.append("freesynd.ini");
    
    File::setHomePath(dir);

    try {
        ConfigFile conf(path);
        conf.readInto(fullscreen_, "fullscreen", false);
        conf.readInto(playIntro_, "play_intro", true);
        string dataDir;
        conf.readInto(dataDir, "data_dir");
        File::setPath(dataDir.c_str());
        
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
      LOG(Log::k_FLG_GFX, "App", "readConfiguration", ("creating default configuration file in %s", path.c_str()))
        ConfigFile conf;
        conf.add("fullscreen", false);
        conf.add("play_intro", true);
        conf.add("data_dir", "./");
        conf.add("language", 0);

        menus_.setLanguage(MenuManager::ENGLISH);

        std::ofstream file(path.c_str(), std::ios::out | std::ios::trunc);
        if (file) {
            file << conf;
            file.close();
            return true;
        } else {
	  printf("Error : Failed to create config file in %s\n", path.c_str());
            return false;
        }
    }
}

void App::updateIntroFlag(const char *dir) {
    std::string path(dir);
    path.append("freesynd.ini");
    try {
        ConfigFile conf(path);
        conf.add("play_intro", false);

        std::ofstream file(path.c_str(), std::ios::out | std::ios::trunc);
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
 * \param dir True if application runs in full screen.
 * \return True if initialization is ok.
 */
bool App::initialize(const char *dir) {
    
    LOG(Log::k_FLG_INFO, "App", "initialize", ("reading configuration..."))
    if (!readConfiguration(dir)) {
        LOG(Log::k_FLG_GFX, "App", "initialize", ("failed to read configuration..."))
        return false;
    }

    LOG(Log::k_FLG_INFO, "App", "initialize", ("initializing system..."))
    if (!system_->initialize(fullscreen_)) {
        return false;
    }

    LOG(Log::k_FLG_INFO, "App", "initialize", ("Loading intro sounds..."))
    if (!intro_sounds_.loadSounds(SoundManager::SAMPLES_INTRO)) {
        return false;
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
            weapons_.getWeapon(Weapon::Pistol)->createInstance());
        agents_.agent(i)->addWeapon(
            weapons_.getWeapon(Weapon::GaussGun)->createInstance());
        agents_.agent(i)->addWeapon(
            weapons_.getWeapon(Weapon::Flamer)->createInstance());
        agents_.agent(i)->addWeapon(
            weapons_.getWeapon(Weapon::Uzi)->createInstance());
        agents_.agent(i)->addWeapon(
            weapons_.getWeapon(Weapon::EnergyShield)->createInstance());
        agents_.agent(i)->addWeapon(
            weapons_.getWeapon(Weapon::Laser)->createInstance());
        agents_.agent(i)->addWeapon(
            weapons_.getWeapon(Weapon::LongRange)->createInstance());
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

/*!
 * Handles the key pressed event.
 * Actually, passes the event to the menu manager.
 * \param key The key that was pressed
 * \param modKeys State of all modifier keys
 */
void App::keyEvent(Key & key, const int modKeys) {

    // TODO : There should be a special menu that 
    // would play Fli so that playing Fli would not
    // be a special event in the application.
    if (playingFli_) {
        skipFli_ = true;
        return;
    }

    if (menus_.showingMenu()) {
        menus_.keyEvent(key, modKeys);
        return;
    }

    // TODO : this part is never reached because the app
    // is either playing a Fli or showing a menu
    // Maybe checked if menus has treated the current event
    // and if not run this code
#ifdef _DEBUG
    switch (key) {
    case KEY_ESCAPE:
    case KEY_q:
        quit();
        break;

    default:
        break;
    }
#endif
}

/*!
 * Handles the mouse down event.
 * Actually, passes the event to the menu manager.
 * \param x X screen coordinate
 * \param y Y screen coordinate
 * \param button What button was pressed
 * \param modKeys State of all modifier keys
 */
void App::mouseDownEvent(int x, int y, int button, const int modKeys) {
    if (menus_.showingMenu()) {
        menus_.mouseDownEvent(x, y, button, modKeys);
        return;
    }
}

/*!
 * Handles the mouse up event.
 * Actually, passes the event to the menu manager.
 * \param x X screen coordinate
 * \param y Y screen coordinate
 * \param button What button was released
 * \param modKeys State of all modifier keys
 */
void App::mouseUpEvent(int x, int y, int button, const int modKeys) {
    if (menus_.showingMenu()) {
        menus_.mouseUpEvent(x, y, button, modKeys);
        return;
    }
}

/*!
 * Handles the mouse motion event.
 * Actually, passes the event to the menu manager.
 * \param x X screen coordinate
 * \param y Y screen coordinate
 * \param state If button is pressed during mouse motion.
 * \param modKeys State of all modifier keys
 */
void App::mouseMotionEvent(int x, int y, int state, const int modKeys) {
    if (menus_.showingMenu()) {
        menus_.mouseMotionEvent(x, y, state, modKeys);
        return;
    }
}

void App::waitForKeyPress() {
    playingFli_ = true;
    skipFli_ = false;

    while (running_ && !skipFli_)
        system_->handleEvents();

    playingFli_ = false;
}

void App::setPalette(const char *fname, bool sixbit) {
    int size;
    uint8 *data = File::loadFile(fname, size);

    if (sixbit)
        system_->setPalette6b3(data);
    else
        system_->setPalette8b3(data);

    delete[] data;
}

/*!
 * This method defines the application loop.
 * \param dir Directory of the application config file
 * \param start_mission Mission id used to start the application
 */
void App::run(const char *dir, int start_mission) {
    int size = 0, tabSize = 0;
    uint8 *data, *tabData;

    FliPlayer fliPlayer;

    // Play intro only the first time the application is run
    // start_mission is available only in DEBUG mode to jump to a mission
    if (playIntro_ && start_mission == -1) {
        LOG(Log::k_FLG_GFX, "App", "run", ("Loading resource for the intro"))
        // this font is for the intro
        tabData = File::loadFile("mfnt-0.tab", tabSize);
        data = File::loadFile("mfnt-0.dat", size);
        intro_font_sprites_.loadSprites(tabData, tabSize, data, true);
        LOG(Log::k_FLG_GFX, "App", "run", ("%d sprites loaded from mfnt-0.dat", tabSize / 6))
        delete[] tabData;
        delete[] data;
        intro_font_.setSpriteManager(&intro_font_sprites_, 1);

        // play intro
        LOG(Log::k_FLG_GFX, "App", "run", ("Playing the intro"))
        data = File::loadFile("intro.dat", size);
        fliPlayer.loadFliData(data);
        music().playTrack(MusicManager::TRACK_INTRO);
        fliPlayer.play(true);
        music().stopPlayback();
        delete[] data;

        // Update intro flag so intro won't be played next time
        updateIntroFlag(dir);
    }

    // load palette
    data = File::loadFile("hpal01.dat", size);
    system_->setPalette6b3(data);
    delete[] data;

    // load palette
    setPalette("mselect.pal");

    // load "req"
    // TODO: what's this for?
    data = File::loadFile("hreq.dat", size);
    delete[] data;

    // load mspr-0 sprites
    LOG(Log::k_FLG_GFX, "App", "run", ("Loading sprites from mspr-0.dat ..."))
    tabData = File::loadFile("mspr-0.tab", tabSize);
    data = File::loadFile("mspr-0.dat", size);
    LOG(Log::k_FLG_GFX, "App", "run", ("%d sprites loaded", tabSize / 6))
    menu_sprites_.loadSprites(tabData, tabSize, data, true);
    delete[] tabData;
    delete[] data;
    fonts_.loadFont(&menu_sprites_, FontManager::SIZE_4, true, 1076, 'A');
    fonts_.loadFont(&menu_sprites_, FontManager::SIZE_3, true, 802, 'A');
    fonts_.loadFont(&menu_sprites_, FontManager::SIZE_2, true, 528, 'A');
    fonts_.loadFont(&menu_sprites_, FontManager::SIZE_1, true, 254, 'A');
    fonts_.loadFont(&menu_sprites_, FontManager::SIZE_4, false, 939, 'A');
    fonts_.loadFont(&menu_sprites_, FontManager::SIZE_3, false, 665, 'A');
    fonts_.loadFont(&menu_sprites_, FontManager::SIZE_2, false, 391, 'A');
    fonts_.loadFont(&menu_sprites_, FontManager::SIZE_1, false, 117, 'A');
    game_font_.load();

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
    //this is walk data
    // load "col01"
    data = File::loadFile("col01.dat", size);
    // original walk data
    memcpy(walkdata_, data, 256);
    // walkdata_ patched version
    // 0x00 - non-surface/non-walkable(if above surface is walkable)
    // 0x01 - stairs low y + 1, high y - 1
    // 0x02 - stairs low y - 1, high y + 1
    // 0x03 - stairs low x - 1, high x + 1
    // 0x04 - stairs low x + 1, high x - 1
    // 0x05-0x09, 0x0B,0x0D-0x0F - simple surfaces(0x0D is not so simple)
    // also some of them are roads for vehicles, look tile picture for info
    // 0x0C - fences
    // 0x0A - non-surface/non-walkable
    // 0x10 - non-surface/non-walkable, always above train stop
    // 0x11, 0x12 - train entering surface
    memcpy(walkdata_p_, data, 256);
    // little patch to enable full surface description
    // and eliminate unnecessary data
    walkdata_p_[0x02] = 0x00;
    walkdata_p_[0x80] = 0x11;
    walkdata_p_[0x81] = 0x12;
    walkdata_p_[0x8F] = 0x00;
    walkdata_p_[0x93] = 0x00;
    delete[] data;

    // load palette
    setPalette("mselect.pal");

    if (start_mission == -1) {
        // play title
        data = File::loadFile("mtitle.dat", size);
        fliPlayer.loadFliData(data);
        fliPlayer.play();
        delete[] data;
        if (!gameSprites().loaded())
            gameSprites().load();
        waitForKeyPress();

        // play the groovy menu startup anim
	    g_App.gameSounds().play(snd::MENU_UP);
        data = File::loadFile("mscrenup.dat", size);
        fliPlayer.loadFliData(data);
        fliPlayer.play();
        delete[] data;
    } else {
        if (!gameSprites().loaded())
            gameSprites().load();
    }

    menus_.createAllMenus();

    if (start_mission == -1) {
        // Regular scenario : start with the main menu
        menus_.changeCurrentMenu("main");
    } else {
        // Debug scenario : start directly with the brief menu
        // in the given mission
        // First, we find the block associated with the given
        // mission number
        for (int i = 0; i < 50; i++) {
            if (session_.getBlock(i).mis_id == start_mission) {
                session_.setSelectedBlockId(i);
            }
        }
        // Then we go to the brief menu
        menus_.changeCurrentMenu("brief");
        // show the cursor because at first it's hidden
        // and normally it's the main menu which shows it
        g_System.showCursor();
    }

    int lasttick = SDL_GetTicks();
    while (running_) {
        system_->handleEvents();
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
    
    std::ofstream outfile;
    std::string path;
            
    File::getFullPathForSaveSlot(fileSlot, path);
    LOG(Log::k_FLG_IO, "App", "saveGameToFile", ("Saving to file %s", path.c_str()))

    outfile.open(path.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

    if (outfile.is_open()) {
        // write file format version
        unsigned char vMaj = 1, vMin = 0;
        outfile.write(reinterpret_cast<const char*>(&vMaj), sizeof(unsigned char));
        outfile.write(reinterpret_cast<const char*>(&vMin), sizeof(unsigned char));

        char buf[25];
        memset(buf, '\0', 25);
        // Slot name is on 25 caracters max
        fs_strcpy(buf, 25, name.c_str());
        outfile.write(buf, 25);
        
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
            outfile.write(reinterpret_cast<const char*>(&id), sizeof(int));
        }

        // save researches
        session_.researchManager().saveToFile(outfile);

        outfile.close();

        return true;
    }

    return false;
}

bool App::loadGameFromFile(int fileSlot) {

    std::string path;
    std::ifstream infile;

    File::getFullPathForSaveSlot(fileSlot, path);

    infile.open(path.c_str(), std::ios::in | std::ios::binary);

    if (infile.is_open()) {
        // Read version
        unsigned char vMaj = 1, vMin = 0;
        infile.read(reinterpret_cast<char*>(&vMaj), sizeof(unsigned char));
        infile.read(reinterpret_cast<char*>(&vMin), sizeof(unsigned char));

        char buf[25];
        memset(buf, '\0', 25);

        reset();
        //Read slot name
        infile.read(buf, 25);

        session_.loadFromFile(infile);

        // Weapons
        weapons_.loadFromFile(infile);

        // Mods
        mods_.loadFromFile(infile);

        // Agents
        agents_.loadFromFile(infile);

        // Read squad
        for (int i=0; i<4; i++) {
            int id = 0;
            infile.read(reinterpret_cast<char*>(&id), sizeof(int));
            // TODO : finish
        }

        // Research
        session_.researchManager().loadFromFile(infile);

        infile.close();
        return true;
    }

    return false;
}