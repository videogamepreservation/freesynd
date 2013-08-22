/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2013  Benoit Blancard <benblan@users.sourceforge.net>*
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

#include "editor/gfxmenu.h"
#include "menus/menumanager.h"
#include "editor/editormenuid.h"
#include "gfx/screen.h"
#include "system.h"

GfxMenu::GfxMenu(MenuManager * m):
    Menu(m, fs_edit_menus::kMenuIdGfx, fs_edit_menus::kMenuIdMain, "mscrenup.dat", "")
{
    isCachable_ = false;
    addStatic(0, 40, g_Screen.gameScreenWidth(), "GRAPHICS", FontManager::SIZE_4, false);

    addOption(201, 130, 300, 25, "MENU SPRITES", FontManager::SIZE_3, fs_edit_menus::kMenuIdFont, true, false);
    addOption(201, 266, 300, 25, "BACK", FontManager::SIZE_3, fs_edit_menus::kMenuIdMain, true, false);
}

void GfxMenu::handleShow()
{
    // If we came from the intro, the cursor is invisible
    // otherwise, it does no harm
    g_System.useMenuCursor();
    g_System.showCursor();
}

void GfxMenu::handleLeave() {
    g_System.hideCursor();
}

