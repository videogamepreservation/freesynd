/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2011  Benoit Blancard <benblan@users.sourceforge.net>*
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
#include "loadsavemenu.h"
#include "utils/file.h"

const int LoadSaveMenu::X_ORIGIN = 165;
const int LoadSaveMenu::Y_ORIGIN = 100;
const int LoadSaveMenu::NAME_MAX_SIZE = 16;

LoadSaveMenu::LoadSaveMenu(MenuManager * m):Menu(m, "loadsave", "mlosa.dat",
     "mlosaout.dat")
{
    setParentMenu("main");

    // Title
    addStatic(0, 40, g_Screen.gameScreenWidth(), "#LS_TITLE", FontManager::SIZE_4, true);

    // Load button
    loadButId_ = addOption(46, 346, 99, 25, "#LS_LOAD_BUT", FontManager::SIZE_2, KEY_F2);
    // Save button
    saveButId_ = addOption(147, 346, 99, 25, "#LS_SAVE_BUT", FontManager::SIZE_2, KEY_F3);
    // Main menu button
    addOption(501, 346, 126, 25, "#MENU_MAIN_BUT", FontManager::SIZE_2, KEY_F5, "main");

    g_App.menus().getMessage("#MENU_LB_EMPTY", emptyLbl_);
    
    for (int i=0; i<10; i++) {
        files_.push_back("");
    }

    // by default no line is edited
    editNameId_ = -1;
    caretPosition_ = 0;
}

void LoadSaveMenu::handleShow()
{
    for (int i=0; i<10; i++) {
        files_[i].erase();
    }
    File::getGameSavedNames(files_);

    menu_manager_->saveBackground();
    g_System.showCursor();
}

void LoadSaveMenu::handleLeave() {
    g_System.hideCursor();
    editNameId_ = -1;
}

void LoadSaveMenu::handleAction(const int actionId, void *ctx, const int modKeys) {
    if (actionId == loadButId_) {
        // TODO complete loading
    } else if (actionId == saveButId_) {
        // TODO complete saving
    }
}

bool LoadSaveMenu::handleUnknownKey(Key key, const int modKeys) {
    if (editNameId_ == -1) {
        return false;
    }

    if (key == KEY_LEFT) {
        if (caretPosition_ > 0) {
            caretPosition_--;
        }
    } else if (key == KEY_RIGHT) {
        if (caretPosition_ < files_[editNameId_].size()) {
            caretPosition_++;
        }
    } else if (key == KEY_BACKSPACE) {
        if (caretPosition_ > 0) {
            files_[editNameId_].erase(caretPosition_ - 1, 1);
            caretPosition_--;
        }
    } else if (key == KEY_DELETE) {
        if (caretPosition_ < files_[editNameId_].size()) {
            files_[editNameId_].erase(caretPosition_, 1);
        }
    } else if (key == KEY_HOME) {
        caretPosition_ = 0;
    } else if (key == KEY_END) {
        caretPosition_ = files_[editNameId_].size();
    } else if (menu_manager_->isPrintableKey(key)) {
        if (files_[editNameId_].size() < NAME_MAX_SIZE) {
            std::string str;
            str += menu_manager_->getKeyAsChar(key);
            files_[editNameId_].insert(caretPosition_, str);
            caretPosition_++;
        }
    } else {
        return false;
    }

    redrawList();
    return true;
}

void LoadSaveMenu::handleMouseDown(int x, int y, int button, const int modKeys) {
    if (x > 165 && x < 535) {
        // Gets the line which was clicked
        for (int lineId=0; lineId<10; lineId++) {
            int yUp = Y_ORIGIN + lineId*24;
            if (y > yUp && y < (yUp + 22)) {
                editNameId_ = lineId;
                int size = g_App.fonts().textWidth(files_[editNameId_].c_str(), FontManager::SIZE_2);

                // computes caret position
                if (x > size + X_ORIGIN) {
                    // Clicked after the text so caret is at the end
                    caretPosition_ = files_[editNameId_].size();
                } else {
                    // Clicked in the text so computes what exact letter
                    size_t pos = 1;
                    for (unsigned int i=0; i<files_[editNameId_].size(); i++, pos++) {
                        std::string sub = files_[editNameId_].substr(0, pos);
                        if (x < X_ORIGIN + g_App.fonts().textWidth(sub.c_str(), FontManager::SIZE_2)) {
                            caretPosition_ = pos - 1;
                            break;
                        }
                    }
                }
                redrawList();
                return;
            }
        }
    }
}

void LoadSaveMenu::drawCaret() {
        std::string start = files_[editNameId_].substr(0, caretPosition_);
        int x = X_ORIGIN + g_App.fonts().textWidth(start.c_str(), FontManager::SIZE_2) + 1;
        int y = Y_ORIGIN + editNameId_*24 + 20;

        // width of caret is the same of the letter above
        int length = 10;
        if (caretPosition_ < files_[editNameId_].size()) {
            length = g_App.fonts().textWidth(files_[editNameId_].substr(caretPosition_, 1).c_str(), FontManager::SIZE_2);
        }

        // Draw caret
        g_Screen.drawRect(x, y, length, 2, 252);
}

void LoadSaveMenu::handleRender() {
    int y = Y_ORIGIN + 4;

    for (int i=0; i<10; i++) {
        if (files_[i].size() != 0) {
            g_App.fonts().drawText(X_ORIGIN, y, files_[i].c_str(), FontManager::SIZE_2, i != editNameId_);
        } else if (editNameId_ != i) {
            g_App.fonts().drawText(X_ORIGIN, y, emptyLbl_.c_str(), FontManager::SIZE_2, i != editNameId_);
        }
        y += 24;
    }

    if (editNameId_ != -1) {
        drawCaret();
    }
}
