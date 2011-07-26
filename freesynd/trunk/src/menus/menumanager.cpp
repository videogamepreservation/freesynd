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
#include "utils/file.h"
#include "utils/log.h"
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
#include "gfx/fliplayer.h"
#include "system.h"
#include "utils/configfile.h"

MenuManager::MenuManager(): dirtyList_(g_Screen.gameScreenWidth(), g_Screen.gameScreenHeight())
{
    drop_events_ = false;
    background_ = new uint8[g_Screen.gameScreenWidth() * g_Screen.gameScreenHeight()];
    memset(background_, 0, g_Screen.gameScreenHeight() * g_Screen.gameScreenWidth());
    needBackground_ = false;
    language_ = NULL;
    
    current_ = NULL;
    menu_main_ = NULL;
    menu_conf_ = NULL;
    menu_load_save_ = NULL;
    menu_map_ = NULL;
    menu_brief_ = NULL;
    menu_select_ = NULL;
    menu_research_ = NULL;
    menu_loading_ = NULL;
    menu_gameplay_ = NULL;
    menu_debrief_ = NULL;
    menu_logout_ = NULL;
}

MenuManager::~MenuManager()
{}

/*!
 * Destroy all menus and resources.
 */
void MenuManager::destroy() {
    LOG(Log::k_FLG_MEM, "MenuManager", "~MenuManager", ("Destruction..."))

    if (menu_main_) {
        delete menu_main_;
        menu_main_ = NULL;
    }
    if (menu_conf_) {
        delete menu_conf_;
        menu_conf_ = NULL;
    }
    if (menu_load_save_) {
        delete menu_load_save_;
        menu_load_save_ = NULL;
    }
    if (menu_map_) {
        delete menu_map_;
        menu_map_ = NULL;
    }
    if (menu_brief_) {
        delete menu_brief_;
        menu_brief_ = NULL;
    }
    if (menu_select_) {
        delete menu_select_;
        menu_select_ = NULL;
    }
    if (menu_research_) {
        delete menu_research_;
        menu_research_ = NULL;
    }
    if (menu_loading_) {
        delete menu_loading_;
        menu_loading_ = NULL;
    }
    if (menu_gameplay_) {
        delete menu_gameplay_;
        menu_gameplay_ = NULL;
    }
    if (menu_debrief_) {
        delete menu_debrief_;
        menu_debrief_ = NULL;
    }
    if (menu_logout_) {
        delete menu_logout_;
        menu_logout_ = NULL;
    }

    if (background_) {
        delete[] background_;
    }

    if (language_) {
        delete language_;
        language_ = NULL;
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

/*!
 * Change the current menu with the one with the given name.
 * Plays the transition animations between the two menus.
 * \param name The name of the new menu.
 */
void MenuManager::changeCurrentMenu(const char *name)
{
    if (name == NULL)
        name = "main";
    if (menus_.find(name) == menus_.end()) {
        printf("Failed to find menu: %s\n", name);
        return;
    }
    Menu *m = menus_[name];
    bool currentWasSubMenu = false;
    if (current_) {
        // plays the leaving animation only if the menu
        // is not a submenu (see confMenu)
        currentWasSubMenu = current_->isSubMenu();
        leaveMenu(current_, !m->isSubMenu());
    }
    current_ = m;
    if (m) {
        showMenu(m, !currentWasSubMenu);
    }
}

/*!
 * Display the opening animation if the flag is true.
  * After having played the animation, renders one time the menu.
 * \param pMenu The menu to show.
 * \param playAnim True if the intro can be played.
 */
void MenuManager::showMenu(Menu *pMenu, bool playAnim) {
    if (pMenu->hasShowAnim() && playAnim) {
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
void MenuManager::leaveMenu(Menu *pMenu, bool playAnim) {
    pMenu->leave();

    if (pMenu->hasLeaveAnim() && playAnim) {
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

/*!
 * Creates all the menus used in the application.
 */
void MenuManager::createAllMenus()
{
    // TODO : only creates the first menu
    // and creates menus on first access
    menu_main_ = new MainMenu(this);

    menu_conf_ = new ConfMenu(this);
    menu_load_save_ = new LoadSaveMenu(this);
    menu_map_ = new MapMenu(this);
    menu_brief_ = new BriefMenu(this);
    menu_select_ = new SelectMenu(this);
    menu_research_ = new ResearchMenu(this);
    menu_loading_ = new LoadingMenu(this);
    menu_gameplay_ = new GameplayMenu(this);
    menu_debrief_ = new DebriefMenu(this);
    menu_logout_ = new LogoutMenu(this);

    // still to go:  mendlose.dat, mendwin.dat
}

bool MenuManager::isPrintableKey(Key key) {
    return (key >= KEY_a && key <= KEY_z) ||
            (key >= KEY_0 && key <= KEY_9) ||
            (key == KEY_SPACE);
}

char MenuManager::getKeyAsChar(Key key) {
    if (key >= KEY_a && key <= KEY_z) {
        return key - KEY_a + 'A';

    } else if (key >= KEY_0 && key <= KEY_9) {
        return key - KEY_0 + '0';
    } else if(key == KEY_SPACE) {
        return ' ';
    }

    return 0;
}
