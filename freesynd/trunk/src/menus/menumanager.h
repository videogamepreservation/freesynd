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

	bool initialize(bool loadIntroFont);
    
    //! Destroy all menus and resources
    void destroy();

    void setLanguage(FS_Lang lang);
    std::string getMessage(const std::string & id);
    void getMessage(const std::string & id, std::string & msg);

	//! Return the menu sprites manager
	SpriteManager &menuSprites() {
        return menuSprites_;
    }

	FontManager &fonts() {
        return fonts_;
    }

	/*! Reads events from the event queue and dispatches them.*/
	void handleEvents();

    void handleTick(int elapsed) {
        if (current_)
            current_->handleTick(elapsed);

		if (nextMenuId_ != -1) {
			changeCurrentMenu();
		}
    }

	// Change the menu
	void gotoMenu(int menuId);

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

    FS_Lang currLanguage(void) {return curr_language_; }

protected:
	//! Returns a menu with the given id
	Menu * getMenu(int menuId);
    //! Shows the menu opening animation
    void showMenu(Menu *pMenu);
    //! Shows the menu closing animation
    void leaveMenu(Menu *pMenu);
	//! Switch from menu and plays the transition animation.
    void changeCurrentMenu();

protected:
    /** The list of currently loaded menus.*/
    std::map<int, Menu *> menus_;
    /** The current menu being displayed.*/
    Menu *current_;
	/** The id of the next menu. -1 by default.*/
	int nextMenuId_;
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

	/*! Sprite manager for menu sprites.*/
	SpriteManager menuSprites_;
	/*! Sprite manager for intro font. */
	SpriteManager *pIntroFontSprites_;
	/*! Font manager.*/
	FontManager fonts_;
};

#endif
