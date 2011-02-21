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

#ifndef RESEARCHMENU_H
#define RESEARCHMENU_H

/*!
 * Research class.
 */
class ResearchMenu : public Menu {
public:
    ResearchMenu(MenuManager *m);

    void handleTick(int elapsed);
    void handleShow();
    void handleRender();
    void handleShowLate();
    void handleLeave();
    void handleAction(const int actionId, void *ctx, const int modKeys);

protected:
    int tab_;
    uint8 *orig_pixels_;
    int sel_weapon_, sel_field_, sel_mod_;
    /*! Id of the text widget for time.*/
    int txtTimeId_;
    /*! Id of the mods toogle button.*/
    int modsButId_;
    /*! Id of the equipment toogle button.*/
    int equipButId_;
    int cancelSearchId_;
    int researchId_;
    int cancelDescId_;
    int mod0Id_;
    int equip0Id_;
    int modField0Id_;
    int equipField0Id_;

    void updateClock();

    void showFieldList();
    void hideFieldList();

    void showEquipList();
    void hideEquipList();

    void showModsList();
    void hideModsList();

    void addFieldOptions();
    void addWeaponOptions();
    void addModOptions();
};

#endif
