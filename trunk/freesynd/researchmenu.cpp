/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
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
#include "file.h"
#include "researchmenu.h"

// TODO

ResearchMenu::ResearchMenu(MenuManager * m):Menu(m, "research", "mresrch.dat", "mresout.dat"), tab_(1),
orig_pixels_(0), sel_weapon_(0), sel_field_(0),
sel_mod_(0)
{
    addStatic(228, 35, "RESEARCH", 3, true);
    addOption(52, 296, "MODS", 1, KEY_F1, NULL);
    addOption(52, 324, "EQUIP", 1, KEY_F2, NULL);
    addOption(43, 352, "ACCEPT", 1, KEY_F3, "select");
    addOption(535, 352, "MENU", 1, KEY_F4, "main");
    addFieldOptions();
    addWeaponOptions();
    addModOptions();
    addOption(527, 324, "CANCEL", 1, KEY_F5, NULL, false);
    addOption(30, 164, "RESEARCH", 1, KEY_F6, NULL, false);
    addOption(40, 188, "CANCEL", 1, KEY_F7, NULL, false);
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
        addOption(504, 110 + 12 * i, w->name(), 0,
                  (Key) (KEY_a + g_App.numAvailableMods() + i), NULL);
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
        addOption(504, 110 + 12 * i, m->name(), 0, (Key) (KEY_a + i), NULL,
                  false);
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
    addOption(20, 84, g_Fields[0], 0, KEY_0);
    addOption(20, 96, g_Fields[1], 0, KEY_1);
    addOption(20, 108, g_Fields[2], 0, KEY_2);
    addOption(20, 120, g_Fields[3], 0, KEY_3);
    addOption(20, 84, g_Fields[4], 0, KEY_4, 0, false);
    addOption(20, 96, g_Fields[5], 0, KEY_5, 0, false);
    addOption(20, 108, g_Fields[6], 0, KEY_6, 0, false);
    addOption(20, 120, g_Fields[7], 0, KEY_7, 0, false);
    addOption(20, 132, g_Fields[8], 0, KEY_8, 0, false);
    addOption(20, 144, g_Fields[9], 0, KEY_9, 0, false);
}

void ResearchMenu::handleShow()
{
    if (tab_ == 1 && sel_weapon_ == 0) {
        addWeaponOptions();
    }

    if (orig_pixels_ == 0) {
        orig_pixels_ = new uint8[GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT];
        memcpy(orig_pixels_, g_Screen.pixels(),
               GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT);
    } else {
        g_Screen.blit(0, 0, 145, 230, orig_pixels_, false,
                      GAME_SCREEN_WIDTH);
        g_Screen.blit(145, 0, GAME_SCREEN_WIDTH - 145, GAME_SCREEN_HEIGHT,
                      orig_pixels_ + 145, false, GAME_SCREEN_WIDTH);
    }

    g_Screen.drawLogo(18, 14, g_App.logo(), g_App.logoColour());

    // write money
    char tmp[100];
    g_Screen.blit(538, 87, 100, 30,
                  orig_pixels_ + 538 + 87 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);
    sprintf(tmp, "%d", g_App.money());
    g_App.fonts().drawText(560 - g_App.fonts().textWidth(tmp, 1) / 2, 87,
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

void ResearchMenu::handleOption(Key key)
{
    if (key >= KEY_0 && key <= KEY_9) {
        sel_field_ = key - KEY_0 + 1;
        hideFieldList();
        showOption(KEY_F6);
        showOption(KEY_F7);
        show(false);
    }
    if (key >= KEY_a && key < (Key) (KEY_a + g_App.numAvailableMods())) {
        sel_mod_ = key - KEY_a + 1;
        hideModsList();
        showOption(KEY_F5);
        show(false);
    }
    if (key >= KEY_g && key <= KEY_z) {
        sel_weapon_ = key - KEY_g + 1;
        hideEquipList();
        showOption(KEY_F5);
        show(false);
    }
    if (key == KEY_F1) {
        tab_ = 0;
        showModsList();
        hideEquipList();
        showFieldList();
        show(false);
    }
    if (key == KEY_F2) {
        tab_ = 1;
        hideModsList();
        showEquipList();
        showFieldList();
        show(false);
    }
    if (key == KEY_F5) {
        sel_mod_ = sel_weapon_ = 0;
        hideOption(KEY_F5);
        if (tab_ == 0)
            showModsList();
        else
            showEquipList();
        show(false);
    }
    if (key == KEY_F7) {
        sel_field_ = 0;
        hideOption(KEY_F6);
        hideOption(KEY_F7);
        showFieldList();
        show(false);
    }
}
