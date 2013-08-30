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

#include "editor/animmenu.h"
#include "menus/menumanager.h"
#include "editor/editormenuid.h"
#include "gfx/screen.h"
#include "system.h"
#include "editor/editorapp.h"

AnimMenu::AnimMenu(MenuManager * m):
    Menu(m, fs_edit_menus::kMenuIdFont, fs_edit_menus::kMenuIdGfx, "", "")
{
    isCachable_ = false;
    animId_ = 1960;
    frameId_ = 0;
}

void AnimMenu::handleShow()
{
    // If we came from the intro, the cursor is invisible
    // otherwise, it does no harm
    g_System.useMenuCursor();
    g_System.showCursor();

    menu_manager_->setDefaultPalette();
}

void AnimMenu::handleRender(DirtyList &dirtyList)
{
    g_Screen.clear(0);
    g_App.gameSprites().drawFrame(animId_, frameId_, 100, 100);
}

void AnimMenu::handleLeave() {
    g_System.hideCursor();
}

void AnimMenu::displayFont() {

    
}

bool AnimMenu::handleUnknownKey(Key key, const int modKeys) {
    bool change = false;
    if (key.keyFunc == KFC_UP) {
        if (animId_ < g_App.gameSprites().numAnims() - 1) {
            animId_++;
            frameId_ = 0;
            change = true;
        }
    } else if (key.keyFunc == KFC_DOWN) {
        if (animId_ > 0) {
            animId_--;
            frameId_ = 0;
            change = true;
        }
    } else if (key.keyFunc == KFC_RIGHT) {
        frameId_++;
        if (frameId_ > g_App.gameSprites().lastFrame(animId_)) {
            frameId_ = 0;
        }
        change = true;
    } else if (key.keyFunc == KFC_LEFT) {
        frameId_--;
        if (frameId_ < 0) {
            frameId_ = g_App.gameSprites().lastFrame(animId_);
        }
        change = true;
    }

    if (change) {
        printf("Animation #%d, frame #%d\n", animId_, frameId_);
        needRendering();
    }

    return change;
}

