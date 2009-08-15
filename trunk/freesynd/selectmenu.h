/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
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

#ifndef SELECTMENU_H
#define SELECTMENU_H

/*!
 * Select Menu class.
 */
class SelectMenu : public Menu {
public:
    SelectMenu(MenuManager *m);
    ~SelectMenu();

    void handleTick(int elapsed);
    void handleShow();
    void handleShowLate();
    void handleMouseDown(int x, int y, int button);
    void handleOption(Key key);

protected:
    uint8 *orig_pixels_;
    int tab_;
    int cur_agent_;
    int tick_count_;
    int sel_weapon_, sel_mod_, sel_weapon_inst_;
    bool sel_all_;

    void drawAgentSelector(int x = -1, int y = -1);
    void drawAgent();
    void showTeamList();
    void hideTeamList();
    void showModsList();
    void hideModsList();
    void showEquipList();
    void hideEquipList();

    void addModOptions();
    void addWeaponOptions();
    void addRecruitOptions();

    void toggleAgent(int n);
};

#endif
