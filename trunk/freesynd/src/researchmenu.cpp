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
#include "researchmenu.h"

// TODO

ResearchMenu::ResearchMenu(MenuManager * m):Menu(m, "research", "mresrch.dat", "mresout.dat")
{
    tab_ = TAB_EQUIPS;
    pSelectedWeapon_ = NULL;
    pSelectedMod_ = NULL;
    sel_field_ = 0;

    addStatic(228, 35, "RESEARCH", FontManager::SIZE_4, true);
    txtTimeId_ = addStatic(500, 9, "", FontManager::SIZE_2, false);       // Time

    modsButId_ = addToggleAction(16, 290, 129, 25, "MODS", FontManager::SIZE_2, KEY_F1, tab_ == TAB_MODS);
    equipButId_ = addToggleAction(16, 318, 129, 25,  "EQUIP", FontManager::SIZE_2, KEY_F2, tab_ == TAB_EQUIPS);
    addOption(16, 346, 129, 25,  "#MENU_ACC_BUT", FontManager::SIZE_2, KEY_F3, "select");
    addOption(500, 347,  128, 25, "#MENU_MAIN_BUT", FontManager::SIZE_2, KEY_F4, "main");

    pFieldEquipLBox_ = addListBox(20, 84,  122, 120, 4, tab_ == TAB_EQUIPS);
    pFieldModsLBox_ = addListBox(20, 84,  122, 120, 6, tab_ == TAB_MODS);
    addFieldOptions();

    pEquipsLBox_ = addListBox(504, 110,  122, 230, 18, tab_ == TAB_EQUIPS);
    pModsLBox_ = addListBox(504, 110,  122, 230, 18, tab_ == TAB_MODS);
    addWeaponOptions();
    addModOptions();
    cancelDescId_ = addOption(500, 320,  127, 22,  "CANCEL", FontManager::SIZE_2, KEY_F5, NULL, false);
    researchId_ = addOption(16, 158, 129, 25,  "RESEARCH", FontManager::SIZE_2, KEY_F6, NULL, false);
    cancelSearchId_ = addOption(16, 184, 129, 25,  "CANCEL", FontManager::SIZE_2, KEY_F7, NULL, false);
    setParentMenu("select");
}

const char *g_Fields[] =
    { "AUTOMATIC", "HEAVY", "ASSAULT", "MISCELLANEOUS",
    "LEGS V2", "ARMS V2", "CHEST V2", "HEART V2", "EYES V2", "BRAIN V2"
};

void ResearchMenu::addFieldOptions()
{
    pFieldEquipLBox_->add(g_Fields[0], 1);
    pFieldEquipLBox_->add(g_Fields[1], 2);
    pFieldEquipLBox_->add(g_Fields[2], 3);
    pFieldEquipLBox_->add(g_Fields[3], 4);

    pFieldModsLBox_->add(g_Fields[4], 5);
    pFieldModsLBox_->add(g_Fields[5], 6);
    pFieldModsLBox_->add(g_Fields[6], 7);
    pFieldModsLBox_->add(g_Fields[7], 8);
    pFieldModsLBox_->add(g_Fields[8], 9);
    pFieldModsLBox_->add(g_Fields[9], 10);
}

void ResearchMenu::addWeaponOptions()
{
    for (int i = 0; i < g_App.numAvailableWeapons(); i++) {
        Weapon *w = g_App.availableWeapon(i);
        pEquipsLBox_->add(w->name(), i);
    }
}

void ResearchMenu::addModOptions()
{
    for (int i = 0; i < g_App.numAvailableMods(); i++) {
        Mod *m = g_App.availableMod(i);
        pModsLBox_->add(m->name(), i);
    }
}

/*!
 * Shows either the mods list box or equips list box.
 */
void ResearchMenu::showDetailsList() {
    addDirtyRect(500, 105,  125, 235);
    hideOption(KEY_F5);
    pSelectedWeapon_ = NULL;
    pSelectedMod_ = NULL;

    if (tab_ == TAB_MODS) {
        pModsLBox_->setVisible(true);
        pEquipsLBox_->setVisible(false);
    } else {
        pEquipsLBox_->setVisible(true);
        pModsLBox_->setVisible(false);
    }
}

/*!
 * Hides the mods list box or equips list box.
 */
void ResearchMenu::hideDetailsList() {
    pEquipsLBox_->setVisible(false);
    pModsLBox_->setVisible(false);
}

/*!
 * Shows either the search field list box for Equips or Mods.
 */
void ResearchMenu::showFieldList() {
    addDirtyRect(15, 80,  130, 130);

    if (tab_ == TAB_EQUIPS) {
        pFieldEquipLBox_->setVisible(true);
        pFieldModsLBox_->setVisible(false);
    } else {
        pFieldEquipLBox_->setVisible(false);
        pFieldModsLBox_->setVisible(true);
    }

    hideOption(KEY_F6);
    hideOption(KEY_F7);
}

/*!
 * Hides the search field list box for Equips and Mods.
 */
void ResearchMenu::hideFieldList() {
    pFieldEquipLBox_->setVisible(false);
    pFieldModsLBox_->setVisible(false);
}

void ResearchMenu::handleTick(int elapsed)
{
    if (g_Session.updateTime(elapsed)) {
        updateClock();
    }
}

/*! 
 * Update the game time display
 */
void ResearchMenu::updateClock() {
    char tmp[100];
    g_Session.getTimeAsStr(tmp);
    getStatic(txtTimeId_)->setText(tmp);
}

void ResearchMenu::handleShow() {
    
    menu_manager_->saveBackground();

    // Show the mouse
    g_System.showCursor();

    // Update the time
    updateClock();
}

void ResearchMenu::handleRender()
{
    g_Screen.drawLogo(18, 14, g_App.getGameSession().getLogo(), g_App.getGameSession().getLogoColour());

    // write money
    char tmp[100];

    sprintf(tmp, "%d", g_App.getGameSession().getMoney());
    g_App.fonts().drawText(560 - g_App.fonts().textWidth(tmp, FontManager::SIZE_2) / 2, 87,
                           tmp, 1, false);

    if (pSelectedWeapon_) {
        uint8 ldata[62];
        memset(ldata, 16, sizeof(ldata));
        g_Screen.scale2x(502, 318, sizeof(ldata), 1, ldata);

        pSelectedWeapon_->drawBigIcon(502, 108);
        pSelectedWeapon_->drawInfo(504, 196);
    }

    if (pSelectedMod_) {
        uint8 ldata[62];
        memset(ldata, 16, sizeof(ldata));
        g_Screen.scale2x(502, 318, sizeof(ldata), 1, ldata);

        pSelectedMod_->drawInfo(504, 108);
    }

    if (sel_field_) {
        uint8 ldata[63];
        memset(ldata, 16, sizeof(ldata));
        g_App.fonts().drawText(20, 86, g_Fields[sel_field_ - 1], 0, false);
        g_Screen.scale2x(18, 102, sizeof(ldata), 1, ldata);
        g_App.fonts().drawText(20, 106, "MIN FUNDING", 0, true);
        g_App.fonts().drawText(20, 130, "MAX FUNDING", 0, true);
        g_Screen.scale2x(18, 158, sizeof(ldata), 1, ldata);
        g_Screen.scale2x(18, 182, sizeof(ldata), 1, ldata);
    }
}

void ResearchMenu::handleLeave() {
    g_System.hideCursor();
}

void ResearchMenu::handleAction(const int actionId, void *ctx, const int modKeys)
{
    // Field list box : Equips or Mods
    if (actionId == pFieldEquipLBox_->getId() || actionId == pFieldModsLBox_->getId()) {
        // get selected field
        int *id = static_cast<int *> (ctx);
        if (actionId == pFieldEquipLBox_->getId()) {
            sel_field_ = pFieldEquipLBox_->getItemIdAt(*id);
        } else {
            sel_field_ = pFieldModsLBox_->getItemIdAt(*id);
        }
        // Hide list
        hideFieldList();
        // Show Research and Cancel buttons
        showOption(KEY_F6);
        showOption(KEY_F7);

    } else if (actionId == pModsLBox_->getId()) {
        int *id = static_cast<int *> (ctx);
        int mId = pModsLBox_->getItemIdAt(*id);
        pSelectedMod_ = g_App.availableMod(mId);
        hideDetailsList();
        showOption(KEY_F5);

    } else if (actionId == pEquipsLBox_->getId()) {
        int *id = static_cast<int *> (ctx);
        int wId = pEquipsLBox_->getItemIdAt(*id);
        pSelectedWeapon_ = g_App.availableWeapon(wId);
        hideDetailsList();
        showOption(KEY_F5);

    } else if (actionId == modsButId_) {
        if (tab_ != TAB_MODS) {
            tab_ = TAB_MODS;
            sel_field_ = 0;
            showFieldList();
            showDetailsList();
        }
    } else if (actionId == equipButId_) {
        if (tab_ != TAB_EQUIPS) {
            tab_ = TAB_EQUIPS;
            sel_field_ = 0;
            showFieldList();
            showDetailsList();
        }
    } else if (actionId == cancelDescId_) {
        showDetailsList();
    } else if (actionId == cancelSearchId_) {
        sel_field_ = 0;
        showFieldList();
    }
}
