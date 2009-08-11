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

#include "common.h"
#include "menu.h"
#include <string>
#include <map>

class SpriteManager;

class MainMenu;
class ConfMenu;
class LoadSaveMenu;
class MapMenu;
class BriefMenu;
class SelectMenu;
class ResearchMenu;
class LoadingMenu;
class GameplayMenu;
class DebriefMenu;
class LogoutMenu;

/*!
 * Menu manager class.
 */
class MenuManager {
  public:
    MenuManager();
    ~MenuManager();
    void createAllMenus();

    void addMenu(Menu *m) {
        menus_[m->name()] = m;
    }

    void handleTick(int elapsed) {
        if (current_)
            current_->handleTick(elapsed);
    }

    void changeCurrentMenu(const char *name);

    bool showingMenu() {
        return current_ != NULL;
    }

    void keyEvent(Key key, KeyMod mod, bool pressed);
    void mouseMotionEvent(int x, int y, int state);
    void mouseDownEvent(int x, int y, int button);
    void mouseUpEvent(int x, int y, int button);

  protected:
    std::map<std::string, Menu *> menus_;
    Menu *current_;

    MainMenu *menu_main_;
    ConfMenu *menu_conf_;
    LoadSaveMenu *menu_load_save_;
    MapMenu *menu_map_;
    BriefMenu *menu_brief_;
    SelectMenu *menu_select_;
    ResearchMenu *menu_research_;
    LoadingMenu *menu_loading_;
    GameplayMenu *menu_gameplay_;
    DebriefMenu *menu_debrief_;
    Menu *menu_miss_win_;
    Menu *menu_miss_lose_;
    LogoutMenu *menu_logout_;
};
