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

ResearchMenu::ResearchMenu(MenuManager * m):Menu(m, "research", "mresrch.dat", "mresout.dat"), tab_(1),
orig_pixels_(0), sel_weapon_(0), sel_field_(0),
sel_mod_(0)
{
    mod0Id_ = 0;
    equip0Id_ = 0;
    addStatic(228, 35, "RESEARCH", FontManager::SIZE_4, true);
    txtTimeId_ = addStatic(500, 9, "", FontManager::SIZE_2, false);       // Time

    modsButId_ = addOption(16, 290, 129, 25, "MODS", FontManager::SIZE_2, KEY_F1, NULL);
    equipButId_ = addOption(16, 318, 129, 25,  "EQUIP", FontManager::SIZE_2, KEY_F2, NULL);
    addOption(16, 346, 129, 25,  "#MENU_ACC_BUT", FontManager::SIZE_2, KEY_F3, "select");
    addOption(500, 347,  128, 25, "#MENU_MAIN_BUT", FontManager::SIZE_2, KEY_F4, "main");
    addFieldOptions();
    addWeaponOptions();
    addModOptions();
    cancelDescId_ = addOption(500, 320,  127, 22,  "CANCEL", FontManager::SIZE_2, KEY_F5, NULL, false);
    researchId_ = addOption(16, 158, 129, 25,  "RESEARCH", FontManager::SIZE_2, KEY_F6, NULL, false);
    cancelSearchId_ = addOption(16, 184, 129, 25,  "CANCEL", FontManager::SIZE_2, KEY_F7, NULL, false);
    setParentMenu("select");
}

void ResearchMenu::handleShowLate()
{
    g_App.fonts().drawText(52, 296, "MODS", 1, tab_ != 0);
    g_App.fonts().drawText(52, 324, "EQUIP", 1, tab_ != 1);
}

void ResearchMenu::showEquipList()
{
    for (int i = 0; i < g_App.numAvailableWeapons(); i++)
        showOption((Key) (KEY_a + g_App.numAvailableMods() + i));
}

void ResearchMenu::hideEquipList()
{
    for (int i = 0; i < g_App.numAvailableWeapons(); i++)
        hideOption((Key) (KEY_a + g_App.numAvailableMods() + i));
}

void ResearchMenu::addWeaponOptions()
{
    for (int i = 0; i < g_App.numAvailableWeapons(); i++) {
        Weapon *w = g_App.availableWeapon(i);
        int id = addOption(504, 110 + 12 * i,  120, 10, w->name(), FontManager::SIZE_1,
                  (Key) (KEY_a + g_App.numAvailableMods() + i), NULL, true, false);

        if (equip0Id_ == 0) {
            equip0Id_ = id;
        }
    }
}

void ResearchMenu::showModsList()
{
    for (int i = 0; i < g_App.numAvailableMods(); i++)
        showOption((Key) (KEY_a + i));
}

void ResearchMenu::hideModsList()
{
    for (int i = 0; i < g_App.numAvailableMods(); i++)
        hideOption((Key) (KEY_a + i));
}

void ResearchMenu::addModOptions()
{
    for (int i = 0; i < g_App.numAvailableMods(); i++) {
        Mod *m = g_App.availableMod(i);
        int id = addOption(504, 110 + 12 * i,  120, 10, m->name(), FontManager::SIZE_1, (Key) (KEY_a + i), NULL,
                  false, false);

        if (mod0Id_ == 0) {
            mod0Id_ = id;
        }
    }
}

void ResearchMenu::showFieldList()
{
    for (int i = KEY_0; i < KEY_4; i++)
        if (tab_ == 1)
            showOption((Key) i);
        else
            hideOption((Key) i);
    for (int i = KEY_4; i <= KEY_9; i++)
        if (tab_ == 0)
            showOption((Key) i);
        else
            hideOption((Key) i);
}

void ResearchMenu::hideFieldList()
{
    for (int i = KEY_0; i <= KEY_9; i++)
        hideOption((Key) i);
}

const char *g_Fields[] =
    { "AUTOMATIC", "HEAVY", "ASSAULT", "MISCELLANEOUS",
    "LEGS V2", "ARMS V2", "CHEST V2", "HEART V2", "EYES V2", "BRAIN V2"
};

void ResearchMenu::addFieldOptions()
{
    equipField0Id_ = addOption(20, 84,  122, 10, g_Fields[0], FontManager::SIZE_1, KEY_0, NULL, true, false);
    addOption(20, 96,  122, 10, g_Fields[1], FontManager::SIZE_1, KEY_1, NULL, true, false);
    addOption(20, 108, 122, 10, g_Fields[2], FontManager::SIZE_1, KEY_2, NULL, true, false);
    addOption(20, 120, 122, 10, g_Fields[3], FontManager::SIZE_1, KEY_3, NULL, true, false);
    modField0Id_ = addOption(20, 84,  122, 10, g_Fields[4], FontManager::SIZE_1, KEY_4, 0, false, false);
    addOption(20, 96,  122, 10, g_Fields[5], FontManager::SIZE_1, KEY_5, 0, false, false);
    addOption(20, 108, 122, 10, g_Fields[6], FontManager::SIZE_1, KEY_6, 0, false, false);
    addOption(20, 120, 122, 10, g_Fields[7], FontManager::SIZE_1, KEY_7, 0, false, false);
    addOption(20, 132, 122, 10, g_Fields[8], FontManager::SIZE_1, KEY_8, 0, false, false);
    addOption(20, 144, 122, 10, g_Fields[9], FontManager::SIZE_1, KEY_9, 0, false, false);
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
    
    if (orig_pixels_ == 0) {
        orig_pixels_ = new uint8[GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT];
        memcpy(orig_pixels_, g_Screen.pixels(),
               GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT);
    }

    // Show the mouse
    g_System.showCursor();

    // Update the time
    updateClock();
}

void ResearchMenu::handleRender()
{
    g_Screen.blit(0, 0, 145, 230, orig_pixels_, false,
                      GAME_SCREEN_WIDTH);
    g_Screen.blit(145, 0, GAME_SCREEN_WIDTH - 145, GAME_SCREEN_HEIGHT,
                      orig_pixels_ + 145, false, GAME_SCREEN_WIDTH);

    g_Screen.drawLogo(18, 14, g_App.getGameSession().getLogo(), g_App.getGameSession().getLogoColour());

    // write money
    char tmp[100];

    sprintf(tmp, "%d", g_App.getGameSession().getMoney());
    g_App.fonts().drawText(560 - g_App.fonts().textWidth(tmp, FontManager::SIZE_2) / 2, 87,
                           tmp, 1, false);

    if (sel_weapon_) {
        uint8 ldata[62];
        memset(ldata, 16, sizeof(ldata));
        g_Screen.scale2x(502, 318, sizeof(ldata), 1, ldata);

        Weapon *w = g_App.availableWeapon(sel_weapon_ - 1);
        w->drawBigIcon(502, 108);
        w->drawInfo(504, 196);
    }

    if (sel_mod_) {
        uint8 ldata[62];
        memset(ldata, 16, sizeof(ldata));
        g_Screen.scale2x(502, 318, sizeof(ldata), 1, ldata);

        Mod *m = g_App.availableMod(sel_mod_ - 1);
        m->drawInfo(504, 108);
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
    if (actionId >= equipField0Id_ && actionId <= (equipField0Id_ + 10 )) {
        sel_field_ = actionId - equipField0Id_ + 1;
        hideFieldList();
        showOption(KEY_F6);
        showOption(KEY_F7);
        needRendering();
    }
    if (actionId >= mod0Id_ && actionId < (mod0Id_ + g_App.numAvailableMods())) {
        sel_mod_ = actionId - mod0Id_ + 1;
        hideModsList();
        showOption(KEY_F5);
        needRendering();
    }
    if (actionId >= equip0Id_ && actionId <= (equip0Id_ + g_App.numAvailableWeapons())) {
        sel_weapon_ = actionId - equip0Id_ + 1;
        hideEquipList();
        showOption(KEY_F5);
        needRendering();
    }
    if (actionId == modsButId_) {
        tab_ = 0;
        showModsList();
        hideEquipList();
        showFieldList();
        needRendering();
    }
    if (actionId == equipButId_) {
        tab_ = 1;
        hideModsList();
        showEquipList();
        showFieldList();
        needRendering();
    }
    if (actionId == cancelDescId_) {
        sel_mod_ = sel_weapon_ = 0;
        hideOption(KEY_F5);
        if (tab_ == 0)
            showModsList();
        else
            showEquipList();
        needRendering();
    }
    if (actionId == cancelSearchId_) {
        sel_field_ = 0;
        hideOption(KEY_F6);
        hideOption(KEY_F7);
        showFieldList();
        needRendering();
    }
}
