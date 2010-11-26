/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
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
#include "mainmenu.h"

MainMenu::MainMenu(MenuManager * m):Menu(m, "main", "moption.dat",
     "moptout.dat")
{
    std::string str;
    menu_manager_->getMessage("MAIN_TITLE", str);
    int titleX = g_Screen.gameScreenWidth() / 2  - g_App.fonts().textWidth(str.c_str(), FontManager::SIZE_4) / 2;
    addStatic(titleX, 40, str.c_str(), 3, true);
    addOption(208, 134, "#MAIN_CONF", 2, KEY_F1, "conf");
    addOption(208, 168, "#MAIN_BEGIN", 2, KEY_F2, "map");
    addOption(208, 201, "#MAIN_LOAD_SAVE", 2, KEY_F3, "loadsave");
    addOption(208, 236, "#MAIN_RESET", 2, KEY_F4, "main");
    addOption(208, 269, "#MAIN_QUIT", 2, KEY_F5, NULL);
}

void MainMenu::handleShow()
{
    // If we came from the intro, the cursor is invisible
    // otherwise, it does no harm
    g_System.useMenuCursor();
    g_System.showCursor();
}

void MainMenu::handleLeave() {
    g_System.hideCursor();
}

void MainMenu::handleOption(Key key, const int modKeys)
{
    if (key == KEY_F4)
        g_App.reset();
    if (key == KEY_F5)
        menu_manager_->changeCurrentMenu("logout");
}
