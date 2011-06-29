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

#ifndef LOADSAVEMENU_H
#define LOADSAVEMENU_H

/*!
 * Load/Save Menu class.
 */
class LoadSaveMenu : public Menu {
public:
    LoadSaveMenu(MenuManager *m);

    void handleShow();
    void handleRender();
    void handleLeave();

    void handleAction(const int actionId, void *ctx, const int modKeys);
    bool handleUnknownKey(Key key, const int modKeys);
    void handleMouseDown(int x, int y, int button, const int modKeys);

protected:
    void drawCaret();
    void redrawList() { addDirtyRect(X_ORIGIN, Y_ORIGIN, 370, 250); } 

protected:
    static const int X_ORIGIN;
    static const int Y_ORIGIN;
    static const int NAME_MAX_SIZE;

    /*! Id of the load button.*/
    int loadButId_;
    /*! Id of the save button.*/
    int saveButId_;
    /*! List od savefile names.*/
    std::vector<std::string> files_;
    /*! Label for empty lines.*/
    std::string emptyLbl_;
    /*! The id of the line currently being edited. -1 if no line selected.*/
    short editNameId_;
    /*! Position of caret in the name.*/
    size_t caretPosition_;
};

#endif
