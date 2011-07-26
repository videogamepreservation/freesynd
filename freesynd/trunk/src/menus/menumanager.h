/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2010  Benoit Blancard <benblan@users.sourceforge.net>*
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

#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include "common.h"
#include "menu.h"
#include "gfx/dirtylist.h"
#include <string>
#include <map>

class SpriteManager;
class ConfigFile;

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
    /*!
     * Available language in the game.
     */
    enum FS_Lang {
        ENGLISH = 0,
        FRENCH = 1,
        ITALIAN = 2,
        GERMAN = 3
    };

    MenuManager();
    ~MenuManager();
    
    void createAllMenus();
    //! Destroy all menus and resources
    void destroy();

    void setLanguage(FS_Lang lang);
    std::string getMessage(const std::string & id);
    void getMessage(const std::string & id, std::string & msg);

    void addMenu(Menu *m) { menus_[m->name()] = m; }

    void handleTick(int elapsed) {
        if (current_)
            current_->handleTick(elapsed);
    }

    //! Switch from menu and plays the transition animation.
    void changeCurrentMenu(const char *name);

    //! Take a snapshot of the screen.
    void saveBackground();

    /*! 
     * Adds a new dirty rectangle
     */
    void addRect(int x, int y, int width, int height) {
        dirtyList_.addRect(x, y, width, height);
    }

    //! Displays the current menu
    void renderMenu();

    //! Returns true if a menu is being displayed
    bool showingMenu() { return current_ != NULL; }

    //! Handles key pressed
    void keyEvent(Key key, const int modKeys);
    //! Handles mouse moved
    void mouseMotionEvent(int x, int y, int state, const int modKeys);
    //! Handles mouse button pressed
    void mouseDownEvent(int x, int y, int button, const int modKeys);
    //! Handles mouse button released
    void mouseUpEvent(int x, int y, int button, const int modKeys);

    bool isPrintableKey(Key key);
    char getKeyAsChar(Key key);

    FS_Lang currLanguage(void) {return curr_language_; }

protected:
    //! Shows the menu opening animation
    void showMenu(Menu *pMenu, bool playAnim);
    //! Shows the menu closing animation
    void leaveMenu(Menu *pMenu, bool playAnim);

protected:
    /** The list of currently loaded menus.*/
    std::map<std::string, Menu *> menus_;
    /** The current menu being displayed.*/
    Menu *current_;
    /** This flag prevents the input events from being processed.*/
    bool drop_events_;
    /** This array stores a copy of the back buffer to reset the screen.*/
    uint8 *background_;
    /*! This flag tells whether current menu needs a background or not.*/
    bool needBackground_;
    /*! Dirty rects list. */
    DirtyList   dirtyList_;
    /*! Language file. */
    ConfigFile  *language_;
    FS_Lang curr_language_;

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
    LogoutMenu *menu_logout_;
};

#endif
