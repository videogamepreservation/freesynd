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

#include <stdio.h>
#include <assert.h>

#include "app.h"
#include "system.h"
#include "utils/configfile.h"
#include "utils/file.h"
#include "utils/log.h"
#include "gfx/fliplayer.h"
#include "mainmenu.h"
#include "confmenu.h"
#include "mapmenu.h"
#include "briefmenu.h"
#include "selectmenu.h"
#include "researchmenu.h"
#include "loadsavemenu.h"
#include "loadingmenu.h"
#include "gameplaymenu.h"
#include "debriefmenu.h"
#include "logoutmenu.h"


MenuManager::MenuManager(): 
	dirtyList_(g_Screen.gameScreenWidth(), g_Screen.gameScreenHeight()),
	menuSprites_(), fonts_()
{
    drop_events_ = false;
    background_ = new uint8[g_Screen.gameScreenWidth() * g_Screen.gameScreenHeight()];
    memset(background_, 0, g_Screen.gameScreenHeight() * g_Screen.gameScreenWidth());
    needBackground_ = false;
    language_ = NULL;
    
    current_ = NULL;
	nextMenuId_ = -1;

	pIntroFontSprites_ = NULL;
}

MenuManager::~MenuManager()
{}

/*!
 * Initialize the menu manager.
 * \param loadIntroFont If true loads the intro sprites and font
 */
bool MenuManager::initialize(bool loadIntroFont) {
	bool res = false;
	int size = 0, tabSize = 0;
	uint8 *data, *tabData;

	// Loads menu sprites
	LOG(Log::k_FLG_GFX, "MenuManager", "initialize", ("Loading menu sprites ..."))
    tabData = File::loadOriginalFile("mspr-0.tab", tabSize);
	if (!tabData) {
		FSERR(Log::k_FLG_UI, "MenuManager", "initialize", ("Failed reading file %s", "mspr-0.tab"));
		return false;
	}
	data = File::loadOriginalFile("mspr-0.dat", size);
	if (!data) {
		FSERR(Log::k_FLG_UI, "MenuManager", "initialize", ("Failed reading file %s", "mspr-0.dat"));
		delete[] tabData;
		return false;
	}
	
	res = menuSprites_.loadSprites(tabData, tabSize, data, true);
	delete[] tabData;
	delete[] data;
	if (res) {
		LOG(Log::k_FLG_GFX, "MenuManager", "initialize", ("%d sprites loaded", tabSize / 6))
	} else {
		FSERR(Log::k_FLG_UI, "MenuManager", "initialize", ("Failed loading menu sprites"));
		return false;
	}

	// loads intro sprites
	if (loadIntroFont) {
		LOG(Log::k_FLG_GFX, "MenuManager", "initialize", ("Loading intro sprites ..."))

        tabData = File::loadOriginalFile("mfnt-0.tab", tabSize);
		if (!tabData) {
			FSERR(Log::k_FLG_UI, "MenuManager", "initialize", ("Failed reading file %s", "mfnt-0.tab"));
			return false;
		}
        data = File::loadOriginalFile("mfnt-0.dat", size);
		if (!data) {
			FSERR(Log::k_FLG_UI, "MenuManager", "initialize", ("Failed reading file %s", "mfnt-0.dat"));
			delete[] tabData;
			return false;
		}

		pIntroFontSprites_ = new SpriteManager();
        res = pIntroFontSprites_->loadSprites(tabData, tabSize, data, true);
        delete[] tabData;
        delete[] data;
		if (res) {
			LOG(Log::k_FLG_GFX, "MenuManager", "initialize", ("%d sprites loaded", tabSize / 6))
		} else {
			FSERR(Log::k_FLG_UI, "MenuManager", "initialize", ("Failed loading intro sprites"));
			return false;
		}
	}
	
    // Loads fonts
	LOG(Log::k_FLG_GFX, "MenuManager", "initialize", ("Loading fonts ..."))
	res = fonts_.loadFonts(&menuSprites_, pIntroFontSprites_);

	return res;
}

/*!
 * Destroy all menus and resources.
 */
void MenuManager::destroy() {
    LOG(Log::k_FLG_MEM, "MenuManager", "~MenuManager", ("Destruction..."))

    if (background_) {
        delete[] background_;
    }

    if (language_) {
        delete language_;
        language_ = NULL;
    }

	if (pIntroFontSprites_) {
		delete pIntroFontSprites_;
        pIntroFontSprites_ = NULL;
	}
}

void MenuManager::setLanguage(FS_Lang lang) {
    std::string filename(File::dataFullPath("lang/"));
    switch (lang) {
        case ENGLISH:
            filename.append("english.lng");
            break;
        case FRENCH:
            filename.append("french.lng");
            break;
        case ITALIAN:
            filename.append("italian.lng");
            break;
        case GERMAN:
            filename.append("german.lng");
            break;
        default:
            filename.append("english.lng");
            lang = ENGLISH;
            break;
    }

    try {
        language_ = new ConfigFile(filename);
        curr_language_ = lang;
    } catch (...) {
        printf("ERROR : Unable to load language file %s.\n", filename.c_str());
        language_ = NULL;
    }
}

std::string MenuManager::getMessage(const std::string & id) {
    std::string msg;
    getMessage(id, msg);
    return msg;
}

void MenuManager::getMessage(const std::string & id, std::string & msg) {
    if (!language_ || !language_->readInto(msg, id)) {
        msg = "?";
    }
}

Menu * MenuManager::getMenu(int menuId) {
	// look in the cache
	if (menus_.find(menuId) != menus_.end()) {
		return menus_[menuId];
	}

	// menu is not in cache so create it
	// some menus are not saved in cache as they are not accessed many times
	Menu *pMenu = NULL;
	bool putInCache = true;

	if (menuId == Menu::MENU_MAIN) {
		pMenu =  new MainMenu(this);
		putInCache = false;
	} else if (menuId == Menu::MENU_BRIEF) {
		pMenu =  new BriefMenu(this);
	} else if (menuId == Menu::MENU_CONF) {
		pMenu =  new ConfMenu(this);
		putInCache = false;
	} else if (menuId == Menu::MENU_DEBRIEF) {
		pMenu =  new DebriefMenu(this);
	} else if (menuId == Menu::MENU_GAMEPLAY) {
		pMenu =  new GameplayMenu(this);
	} else if (menuId == Menu::MENU_LOADING) {
		pMenu =  new LoadingMenu(this);
		putInCache = false;
	} else if (menuId == Menu::MENU_LOGOUT) {
		pMenu =  new LogoutMenu(this);
		putInCache = false;
	} else if (menuId == Menu::MENU_RESEARCH) {
		pMenu =  new ResearchMenu(this);
	} else if (menuId == Menu::MENU_SELECT) {
		pMenu =  new SelectMenu(this);
	} else if (menuId == Menu::MENU_LDSAVE) {
		pMenu =  new LoadSaveMenu(this);
		putInCache = false;
	} else if (menuId == Menu::MENU_MAP) {
		pMenu =  new MapMenu(this);
	} else {
		FSERR(Log::k_FLG_UI, "MenuManager", "getMenu", ("Cannot open Menu : unknown id"));
	}

	if (putInCache && pMenu) {
		menus_[menuId] = pMenu;
	}

	return pMenu;
}

/*!
 * Change the current menu with the one with the given name.
 * Plays the transition animations between the two menus.
 */
void MenuManager::changeCurrentMenu()
{
	// Get the next menu
	Menu *pMenu = getMenu(nextMenuId_);
	if (pMenu == NULL) {
		return;
	}

	if (current_) {
		// Give the possibility to the old menu
		// to clean before leaving
        leaveMenu(current_);
		// If menu is not in cache, it means it must be destroyed
		if (menus_.find(current_->getId()) == menus_.end()) {
			delete current_;
		}
    }
    current_ = pMenu;
	nextMenuId_ = -1;
    showMenu(pMenu);
}

void MenuManager::gotoMenu(int menuId) {
	nextMenuId_ = menuId;
	// stop listening for events until window changed
	drop_events_ = true;
}

/*!
 * Display the opening animation if the flag is true.
  * After having played the animation, renders one time the menu.
 * \param pMenu The menu to show.
 * \param playAnim True if the intro can be played.
 */
void MenuManager::showMenu(Menu *pMenu) {
    if (pMenu->hasShowAnim()) {
        // Stop processing event during menu transitions
        drop_events_ = true;
        FliPlayer fliPlayer;
        uint8 *data;
        int size;
        data = File::loadOriginalFile(pMenu->getShowAnimName(), size);
        fliPlayer.loadFliData(data, false);
        fliPlayer.play();
        delete[] data;

    }

    // reset background
    needBackground_ = false;
    memset(background_, 0, g_Screen.gameScreenHeight() * g_Screen.gameScreenWidth());
    dirtyList_.flush();
    pMenu->handleShow();

    // then plot the mouse to draw the button
    // that could be highlighted because the mouse
    // is upon it
    int x,y;
    int state = g_System.getMousePos(&x, &y);
    pMenu->mouseMotionEvent(x, y, state, KMD_NONE);
    
    // First draw completly the menu
    pMenu->render();

    // reopen the event processing
    drop_events_ = false;
}

/*!
 * Displays the closing menu animation if the flag is true.
 * Before playing animation calls Menu.handleLeave().
 * \param pMenu The closing menu
 * \param playAnim True to play the animation.
 */
void MenuManager::leaveMenu(Menu *pMenu) {
    pMenu->leave();

    if (pMenu->hasLeaveAnim()) {
        drop_events_ = true;
        FliPlayer fliPlayer;
        uint8 *data;
        int size;
        data = File::loadOriginalFile(pMenu->getLeaveAnimName(), size);
        fliPlayer.loadFliData(data);
		g_App.gameSounds().play(snd::MENU_CHANGE);
        fliPlayer.play();
        delete[] data;
        drop_events_ = false;
    }
}

/*!
 * Copy all current screen pixels to a back buffer.
 */
void MenuManager::saveBackground() {
    needBackground_ = true;
    memcpy(background_, g_Screen.pixels(),
        g_Screen.gameScreenWidth() * g_Screen.gameScreenHeight());
}

/*!
 * Renders the current menu if there is one 
 * and if it needs to be refreshed.
 */
void MenuManager::renderMenu() {
    if (current_ && !dirtyList_.isEmpty()) {
        if (needBackground_) {
            for (int i=0; i < dirtyList_.getSize(); i++) {
                DirtyRect *rect = dirtyList_.getRectAt(i);
                g_Screen.blitRect(rect->x, rect->y, rect->width, rect->height, background_, false, g_Screen.gameScreenWidth());
            }
        }
        current_->render();
        // flush dirty list
        dirtyList_.flush();
    }
}

/*!
 * Handles the key pressed event.
 * Actually, passes the event to the current menu.
 * \param key The key that was pressed
 * \param modKeys State of all modifier keys
 */
void MenuManager::keyEvent(Key key, const int modKeys)
{
    if (current_ && !drop_events_) {
        current_->keyEvent(key, modKeys);
    }
}

/*!
 * Handles the mouse motion event.
 * Actually, passes the event to the current menu.
 * \param x X screen coordinate
 * \param y Y screen coordinate
 * \param state If button is pressed during mouse motion.
 * \param modKeys State of all modifier keys
 */
void MenuManager::mouseMotionEvent(int x, int y, int state, const int modKeys)
{
    if (current_ && !drop_events_)
        current_->mouseMotionEvent(x, y, state, modKeys);
}

/*!
 * Handles the mouse down event.
 * Actually, passes the event to the current menu.
 * \param x X screen coordinate
 * \param y Y screen coordinate
 * \param button What button was pressed
 * \param modKeys State of all modifier keys
 */
void MenuManager::mouseDownEvent(int x, int y, int button, const int modKeys)
{
#ifdef _DEBUG
    // Display mouse coordinate
    if (modKeys & KMD_SHIFT) {
        printf("Mouse is at %d, %d\n", x, y);
    }
#endif

    if (current_ && !drop_events_) {
        current_->mouseDownEvent(x, y, button, modKeys);
    }
}

/*!
 * Handles the mouse up event.
 * Actually, passes the event to the current menu.
 * \param x X screen coordinate
 * \param y Y screen coordinate
 * \param button What button was released
 * \param modKeys State of all modifier keys
 */
void MenuManager::mouseUpEvent(int x, int y, int button, const int modKeys)
{
    if (current_ && !drop_events_) {
        current_->mouseUpEvent(x, y, button, modKeys);
    }
}
