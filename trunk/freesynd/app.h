/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
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

#ifndef APP_H
#define APP_H

#include <memory>
#include <vector>

#include "common.h"
#include "system.h"
#include "screen.h"
#include "path.h"
#include "fontmanager.h"
#include "menumanager.h"
#include "mapmanager.h"
#include "mission.h"
#include "missionmanager.h"
#include "agentmanager.h"
#include "weaponmanager.h"
#include "modmanager.h"
#include "vehiclemanager.h"
#include "pedmanager.h"
#include "soundmanager.h"
#include "musicmanager.h"

/*!
 * Application class.
 * Used for managing game settings and workflows.
 */
class App : public Singleton < App > {
    int logo_, logo_colour_;
    int money_;
    std::string company_name_;
    std::string your_name_;
    Agent *team_members_[4];
    std::vector<Agent *> agents_recruited_;
    std::vector<Weapon *> available_weapons_;
    std::vector<Mod *> available_mods_;

    bool running_, playingFli_, skipFli_;
    std::auto_ptr<Screen> screen_;
    std::auto_ptr<System> system_;

    SpriteManager menu_sprites_;
    GameSpriteManager game_sprites_;
    FontManager fonts_;
    HFont game_font_;
    Font intro_font_;
    SpriteManager intro_font_sprites_;
    MenuManager menus_;
    MissionManager missions_;
    AgentManager agents_;
    WeaponManager weapons_;
    ModManager mods_;
    MapManager maps_;
    VehicleManager vehicles_;
    PedManager peds_;
    SoundManager intro_sounds_;
    SoundManager game_sounds_;
    MusicManager music_;

    void cheatFunds() {
        money_ = 100000000;
    }

    void cheatRepeatOrCompleteMission();
    void cheatWeaponsAndMods();
    void cheatEquipAllMods();
    void cheatAnyMission();
    void cheatResurrectAgents();
    void cheatOwnAllCountries();
    void cheatAccelerateTime();
    void cheatFemaleRecruits();
    void cheatEquipFancyWeapons();

  public:
    App();
    virtual ~App();

    SpriteManager &menuSprites() {
        return menu_sprites_;
    }

    GameSpriteManager &gameSprites() {
        return game_sprites_;
    }

    FontManager &fonts() {
        return fonts_;
    }

    Font &introFont() {
        return intro_font_;
    }

    HFont &gameFont() {
        return game_font_;
    }

    MenuManager &menus() {
        return menus_;
    }

    MissionManager &missions() {
        return missions_;
    }

    WeaponManager &weapons() {
        return weapons_;
    }

    MapManager &maps() {
        return maps_;
    }

    VehicleManager &vehicles() {
        return vehicles_;
    }

    PedManager &peds() {
        return peds_;
    }

    SoundManager &introSounds() {
        return intro_sounds_;
    }

    SoundManager &gameSounds() {
        return game_sounds_;
    }

    MusicManager &music() {
        return music_;
    }

    int logo() const {
        return logo_;
    }

    int logoColour() const {
        return logo_colour_;
    }

    void setLogo(int new_logo) {
        logo_ = new_logo;
    }

    void setLogoColour(int colour) {
        logo_colour_ = colour;
    }

    const char *companyName() const {
        return company_name_.c_str();
    }

    const char *yourName() const {
        return your_name_.c_str();
    }

    void setCompanyName(const char *name);

    void setYourName(const char *name) {
        your_name_ = name;
    }

    int money() const {
        return money_;
    }

    void setMoney(int m) {
        money_ = m;
    }

    Agent *teamMember(int n) {
        assert(n < 4);
        return team_members_[n];
    }

    void setTeamMember(int n, Agent *a) {
        team_members_[n] = a;
    }

    int numRecruits() {
        return agents_recruited_.size();
    }

    Agent *recruit(int n) {
        return agents_recruited_[n];
    }

    int numAvailableWeapons() {
        return available_weapons_.size();
    }

    Weapon *availableWeapon(int n) {
        return available_weapons_[n];
    }

    int numAvailableMods() {
        return available_mods_.size();
    }

    Mod *availableMod(int n) {
        return available_mods_[n];
    }

    void run();
    void reset();

    void quit() {
        running_ = false;
    }

    bool isRunning() const {
        return running_;
    }

    void startedPlayingFli() {
        playingFli_ = true;
        skipFli_ = false;
    }

    void finishedPlayingFli() {
        playingFli_ = false;
    }

    bool skipFli() {
        return skipFli_;
    }

    void waitForKeyPress();
    void setPalette(const char *fname, bool sixbit = true);

    void keyEvent(Key & key, KeyMod & mod, bool pressed);
    void mouseDownEvent(int x, int y, int button);
    void mouseUpEvent(int x, int y, int button);
    void mouseMotionEvent(int x, int y, int state);
};

#define g_App   App::singleton()

#endif
