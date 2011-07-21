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
    addStatic(0, 40, g_Screen.gameScreenWidth(), "#MAIN_TITLE", FontManager::SIZE_4, true);

    addOption(201, 130, 300, 25, "#MAIN_CONF", FontManager::SIZE_3, KEY_F1, "conf", true, false);
    addOption(201, 164, 300, 25, "#MAIN_BEGIN", FontManager::SIZE_3, KEY_F2, "map", true, false);
    addOption(201, 198, 300, 25, "#MAIN_LOAD_SAVE", FontManager::SIZE_3, KEY_F3, "loadsave", true, false);
    resetButId_ = addOption(201, 232, 300, 25, "#MAIN_RESET", FontManager::SIZE_3, KEY_F4, "main", true, false);
    quitButId_ = addOption(201, 266, 300, 25, "#MAIN_QUIT", FontManager::SIZE_3, KEY_F5, NULL, true, false);
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

void MainMenu::handleAction(const int actionId, void *ctx, const int modKeys)
{
    if (actionId == resetButId_)
        g_App.reset();
    if (actionId == quitButId_)
        menu_manager_->changeCurrentMenu("logout");
}
