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

#include <stdio.h>
#include <assert.h>
#include "app.h"
#include "file.h"
#include "selectmenu.h"

SelectMenu::SelectMenu(MenuManager * m):Menu(m, "select", "mselect.dat", "mselout.dat"), orig_pixels_(0), tab_(2),
cur_agent_(0), tick_count_(0), sel_weapon_(0), sel_mod_(0),
sel_weapon_inst_(0), sel_all_(false)
{
    addStatic(148, 35, "TEAM SELECTION", 3, true);
    addOption(32, 240, "RESEARCH", 1, KEY_F1, "research");
    addOption(52, 268, "TEAM", 1, KEY_F2, NULL);
    addOption(52, 296, "MODS", 1, KEY_F3, NULL);
    addOption(52, 324, "EQUIP", 1, KEY_F4, NULL);
    addOption(43, 352, "ACCEPT", 1, KEY_F5, "loading");
    addOption(535, 352, "MENU", 1, KEY_F6, "main");

    addModOptions();
    addWeaponOptions();
    addRecruitOptions();

    addOption(527, 274, "CANCEL", 1, KEY_F7, NULL, false);
    addOption(517, 324, "PURCHASE", 1, KEY_F8, NULL, false);
    addOption(537, 324, "SELL", 1, KEY_F9, NULL, false);
    setParentMenu("brief");
}

SelectMenu::~SelectMenu()
{
    if (orig_pixels_)
        delete[] orig_pixels_;
}

void SelectMenu::addModOptions()
{
    for (int i = 0; i < g_App.numAvailableMods(); i++) {
        Mod *m = g_App.availableMod(i);
        addOption(504, 110 + 12 * i, m->name(), 0, (Key) (KEY_a + i), NULL,
                  false);
    }
}

void SelectMenu::addWeaponOptions()
{
    for (int i = 0; i < g_App.numAvailableWeapons(); i++) {
        Weapon *w = g_App.availableWeapon(i);
        addOption(504, 110 + 12 * i, w->name(), 0,
                  (Key) (KEY_a + g_App.numAvailableMods() + i), NULL);
    }
}

void SelectMenu::addRecruitOptions()
{
    for (int i = 0; i < g_App.numRecruits(); i++)
        addOption(520, 124 + i * 12, g_App.recruit(i)->name(), 0,
                  (Key) (KEY_0 + i), NULL, false);
}

void SelectMenu::drawAgentSelector(int x, int y)
{
    static int lx, ly;
    if (x != -1)
        lx = x;
    else
        x = lx;
    if (y != -1)
        ly = y;
    else
        y = ly;

    int rnd = tick_count_ % 8;

    uint8 cdata[30 * 33];
    int cwidth = 30;
    int cheight = 33;
    memset(cdata, 255, sizeof(cdata));
    for (int i = 0; i < cwidth; i++) {
        cdata[i] = ((rnd + i) % 8 <= 4) ? 252 : 16;
        cdata[i + (cheight - 1) * cwidth] =
            ((rnd + i) % 8 >= 4) ? 252 : 16;
    }
    for (int j = 0; j < cheight; j++) {
        cdata[j * cwidth] = ((rnd + j) % 8 >= 4) ? 252 : 16;
        cdata[j * cwidth + cwidth - 1] = ((rnd + j) % 8 <= 4) ? 252 : 16;
    }
    g_Screen.scale2x(x, y, cwidth, cheight, cdata);
}

void SelectMenu::drawAgent()
{
    Agent *selected = g_App.teamMember(cur_agent_);

    // write selected agent's name
    char tmp[100];
    sprintf(tmp, "SPECIAL AGENT %s", selected->name());
    g_App.fonts().drawText(158, 86, tmp, 1, false);

    // TODO: mods
    int torso, arms, legs;
    int armsx = 188;
    int armsy = 152;
    int torsoy = 116;
    int legsy = 218;
    if (selected->isMale()) {
        torso = 30;
        arms = 40;
        legs = 32;
    } else {
        torso = 31;
        arms = 44;
        legs = 36;
        armsx += 10;
        armsy += 6;
        torsoy += 2;
        legsy -= 4;
    }

    if (selected->slot(SLOT_LEGS)) {
        legs = selected->slot(SLOT_LEGS)->icon(selected->isMale());
        g_App.fonts().drawText(366, 250, selected->slot(SLOT_LEGS)->name(),
                               0, false);
    }
    if (selected->slot(SLOT_ARMS)) {
        arms = selected->slot(SLOT_ARMS)->icon(selected->isMale());
        g_App.fonts().drawText(366, 226, selected->slot(SLOT_ARMS)->name(),
                               0, false);
    }

    g_App.menuSprites().drawSpriteXYZ(arms, armsx, armsy, 0, false, true);
    g_App.menuSprites().drawSpriteXYZ(torso, 224, torsoy, 0, false, true);
    g_App.menuSprites().drawSpriteXYZ(legs, 224, legsy, 0, false, true);

    if (selected->slot(SLOT_CHEST)) {
        int chest = selected->slot(SLOT_CHEST)->icon(selected->isMale());
        g_App.fonts().drawText(366, 202,
                               selected->slot(SLOT_CHEST)->name(), 0,
                               false);
        int chestx = 216;
        int chesty = 146;
        if (!selected->isMale()) {
            chestx += 8;
            chesty += 2;
        }
        g_App.menuSprites().drawSpriteXYZ(chest, chestx, chesty, 0, false,
                                          true);
    }

    if (selected->slot(SLOT_HEART)) {
        int heart = selected->slot(SLOT_HEART)->icon(selected->isMale());
        g_App.fonts().drawText(366, 160,
                               selected->slot(SLOT_HEART)->name(), 0,
                               false);
        g_App.menuSprites().drawSpriteXYZ(heart, 254, 166, 0, false, true);
    }

    if (selected->slot(SLOT_EYES)) {
        int eyes = selected->slot(SLOT_EYES)->icon(selected->isMale());
        g_App.fonts().drawText(366, 136, selected->slot(SLOT_EYES)->name(),
                               0, false);
        int eyesx = 238;
        if (!selected->isMale()) {
            eyesx += 2;
        }
        g_App.menuSprites().drawSpriteXYZ(eyes, eyesx, 116, 0, false,
                                          true);
    }

    if (selected->slot(SLOT_BRAIN)) {
        int brain = selected->slot(SLOT_BRAIN)->icon(selected->isMale());
        g_App.fonts().drawText(366, 112,
                               selected->slot(SLOT_BRAIN)->name(), 0,
                               false);
        int brainx = 238;
        if (!selected->isMale()) {
            brainx += 2;
        }
        g_App.menuSprites().drawSpriteXYZ(brain, brainx, 114, 0, false,
                                          true);
    }
    // restore lines over agent
    g_Screen.blit(254, 124, 30, 2,
                  orig_pixels_ + 256 + 124 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);
    g_Screen.blit(264, 132, 30, 2,
                  orig_pixels_ + 266 + 132 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);
    g_Screen.blit(266, 174, 36, 2,
                  orig_pixels_ + 268 + 174 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);
    g_Screen.blit(252, 210, 56, 2,
                  orig_pixels_ + 254 + 210 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);
    g_Screen.blit(302, 232, 10, 2,
                  orig_pixels_ + 304 + 232 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);
    g_Screen.blit(264, 256, 30, 2,
                  orig_pixels_ + 266 + 256 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);

    // write inventory
    for (int j = 0; j < 2; j++)
        for (int i = 0; i < 4; i++)
            if (j * 4 + i < selected->numWeapons()) {
                WeaponInstance *w = selected->weapon(j * 4 + i);
                w->drawSmallIcon(366 + i * 32, 308 + j * 32);
                uint8 data[3];
                memset(data, 204, 3);
                if (w->ammo() != -1) {
                    int n = w->ammoRemaining();
                    if (w->ammo() == 0)
                        n = 24;
                    else {
                        n *= 24;
                        n /= w->ammo();
                    }
                    for (int k = 0; k < n; k++)
                        g_Screen.scale2x(366 + i * 32 + k + 4,
                                         308 + j * 32 + 22, 1, 3, data);
                }
            }
}

void SelectMenu::showTeamList()
{
    for (int i = 0; i < g_App.numRecruits(); i++)
        showOption((Key) (KEY_0 + i));
}

void SelectMenu::hideTeamList()
{
    for (int i = 0; i < g_App.numRecruits(); i++)
        hideOption((Key) (KEY_0 + i));
}

void SelectMenu::showModsList()
{
    for (int i = 0; i < g_App.numAvailableMods(); i++)
        showOption((Key) (KEY_a + i));
}

void SelectMenu::hideModsList()
{
    for (int i = 0; i < g_App.numAvailableMods(); i++)
        hideOption((Key) (KEY_a + i));
}

void SelectMenu::showEquipList()
{
    for (int i = 0; i < g_App.numAvailableWeapons(); i++)
        showOption((Key) (KEY_a + g_App.numAvailableMods() + i));
}

void SelectMenu::hideEquipList()
{
    for (int i = 0; i < g_App.numAvailableWeapons(); i++)
        hideOption((Key) (KEY_a + g_App.numAvailableMods() + i));
}

void SelectMenu::handleTick(int elapsed)
{
    tick_count_ += elapsed;
    if (tick_count_ > 300) {
        static int count = 0;
        tick_count_ = count++;
        drawAgentSelector();
        tick_count_ = 0;
    }
}

void SelectMenu::handleShow()
{
    if (tab_ == 2 && sel_weapon_ == 0 && sel_weapon_inst_ == 0) {
        addModOptions();
        addWeaponOptions();
        addRecruitOptions();
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

    // write team member icons and health
    uint8 data[4], datag[4];
    memset(data, 204, 4);
    memset(datag, 10, 4);
    Agent *t1 = g_App.teamMember(0);
    Agent *t2 = g_App.teamMember(1);
    Agent *t3 = g_App.teamMember(2);
    Agent *t4 = g_App.teamMember(3);
    if (t1) {
        if (t1->isActive()) {
            g_App.fonts().drawText(20, 88, "\004", 1, false);
            for (int i = 0; i < t1->health() * 35 / 255; i++)
                g_Screen.scale2x(68, 122 - i, 3, 1, data);
        } else {
            for (int i = 0; i < t1->health() * 35 / 255; i++)
                g_Screen.scale2x(68, 122 - i, 3, 1, datag);
        }
        if (cur_agent_ == 0)
            drawAgentSelector(20, 84);
    }
    if (t2) {
        if (t2->isActive()) {
            g_App.fonts().drawText(82, 88, "\005", 1, false);
            for (int i = 0; i < t1->health() * 35 / 255; i++)
                g_Screen.scale2x(132, 122 - i, 3, 1, data);
        } else {
            for (int i = 0; i < t1->health() * 35 / 255; i++)
                g_Screen.scale2x(132, 122 - i, 3, 1, datag);
        }
        if (cur_agent_ == 1)
            drawAgentSelector(82, 84);
    }
    if (t3) {
        if (t3->isActive()) {
            g_App.fonts().drawText(20, 166, "\006", 1, false);
            for (int i = 0; i < t1->health() * 35 / 255; i++)
                g_Screen.scale2x(68, 200 - i, 3, 1, data);
        } else {
            for (int i = 0; i < t1->health() * 35 / 255; i++)
                g_Screen.scale2x(68, 200 - i, 3, 1, datag);
        }
        if (cur_agent_ == 2)
            drawAgentSelector(20, 162);
    }
    if (t4) {
        if (t4->isActive()) {
            g_App.fonts().drawText(82, 166, "\007", 1, false);
            for (int i = 0; i < t1->health() * 35 / 255; i++)
                g_Screen.scale2x(132, 200 - i, 3, 1, data);
        } else {
            for (int i = 0; i < t1->health() * 35 / 255; i++)
                g_Screen.scale2x(132, 200 - i, 3, 1, datag);
        }
        if (cur_agent_ == 3)
            drawAgentSelector(82, 162);
    }
    if (sel_all_) {
        g_App.menuSprites().drawSpriteXYZ(77, 20, 152, 0, false, true);
    }

    drawAgent();

    if (sel_weapon_ || sel_weapon_inst_) {
        uint8 ldata[62];
        memset(ldata, 16, sizeof(ldata));
        g_Screen.scale2x(502, 268, sizeof(ldata), 1, ldata);
        g_Screen.scale2x(502, 292, sizeof(ldata), 1, ldata);
        g_Screen.scale2x(502, 318, sizeof(ldata), 1, ldata);

        Weapon *w;
        if (sel_weapon_)
            w = g_App.availableWeapon(sel_weapon_ - 1);
        else {
            Agent *selected = g_App.teamMember(cur_agent_);
            w = selected->weapon(sel_weapon_inst_ - 1);
        }
        w->drawBigIcon(502, 108);
        w->drawInfo(504, 196);
    }

    if (sel_mod_) {
        uint8 ldata[62];
        memset(ldata, 16, sizeof(ldata));
        g_Screen.scale2x(502, 268, sizeof(ldata), 1, ldata);
        g_Screen.scale2x(502, 292, sizeof(ldata), 1, ldata);
        g_Screen.scale2x(502, 318, sizeof(ldata), 1, ldata);

        Mod *m = g_App.availableMod(sel_mod_ - 1);
        m->drawInfo(504, 108);
    }

    if (tab_ == 0) {
        g_App.fonts().drawText(504, 106, "CRYO CHAMBER", 0, false);
        uint8 ldata[49];
        memset(ldata, 252, sizeof(ldata));
        g_Screen.scale2x(504, 120, sizeof(ldata), 1, ldata);
        for (int i = 0; i < g_App.numRecruits(); i++)
            for (int j = 0; j < 4; j++)
                if (g_App.teamMember(j) == g_App.recruit(i)) {
                    sprintf(tmp, "%d", j + 1);
                    g_App.fonts().drawText(504, 124 + i * 12, tmp, 0,
                                           true);
                }
        for (int i = g_App.numRecruits(); i < 18; i++)
            g_App.fonts().drawText(520, 124 + i * 12, "EMPTY", 0, true);
    }
}

void SelectMenu::handleShowLate()
{
    g_App.fonts().drawText(52, 268, "TEAM", 1, tab_ != 0);
    g_App.fonts().drawText(52, 296, "MODS", 1, tab_ != 1);
    g_App.fonts().drawText(52, 324, "EQUIP", 1, tab_ != 2);
}

void SelectMenu::toggleAgent(int n)
{
    int nactive = 0;
    for (int i = 0; i < 4; i++)
        if (g_App.teamMember(i) && g_App.teamMember(i)->isActive())
            nactive++;
    Agent *a = g_App.teamMember(n);
    if (a) {
        if (a->isActive() && nactive == 1)
            return;
        a->setActive(!a->isActive());
    }
}

void SelectMenu::handleMouseDown(int x, int y, int button)
{
    if (x >= 20 && x <= 140) {
        if (y >= 84 && y <= 150) {
            if (x >= 82) {
                if (button == 3)
                    toggleAgent(1);
                else
                    cur_agent_ = 1;
            } else {
                if (button == 3)
                    toggleAgent(0);
                else
                    cur_agent_ = 0;
            }
            show(false);
        }
        if (y > 150 && y < 162) {
            sel_all_ = !sel_all_;
            show(false);
        }
        if (y >= 162 && y <= 228) {
            if (x >= 82) {
                if (button == 3)
                    toggleAgent(3);
                else
                    cur_agent_ = 3;
            } else {
                if (button == 3)
                    toggleAgent(2);
                else
                    cur_agent_ = 2;
            }
            show(false);
        }
    }

    Agent *selected = g_App.teamMember(cur_agent_);
    for (int j = 0; j < 2; j++)
        for (int i = 0; i < 4; i++)
            if (j * 4 + i < selected->numWeapons() &&
                x >= 366 + i * 32 && x < 366 + i * 32 + 32 &&
                y >= 308 + j * 32 && y < 308 + j * 32 + 32) {
                tab_ = 2;
                sel_weapon_ = sel_mod_ = 0;
                sel_weapon_inst_ = i + j * 4 + 1;
                hideOption(KEY_F8);
                showOption(KEY_F7);
                showOption(KEY_F9);
                hideTeamList();
                hideModsList();
                hideEquipList();
                show(false);
            }
}

void SelectMenu::handleOption(Key key)
{
    if (key == KEY_F2) {
        sel_weapon_ = sel_mod_ = sel_weapon_inst_ = 0;
        tab_ = 0;
        hideOption(KEY_F7);
        hideOption(KEY_F8);
        hideOption(KEY_F9);
        showTeamList();
        hideModsList();
        hideEquipList();
        show(false);
    }
    if (key == KEY_F3) {
        sel_weapon_ = sel_mod_ = sel_weapon_inst_ = 0;
        tab_ = 1;
        hideOption(KEY_F7);
        hideOption(KEY_F8);
        hideOption(KEY_F9);
        hideTeamList();
        showModsList();
        hideEquipList();
        show(false);
    }
    if (key == KEY_F4) {
        sel_weapon_ = sel_mod_ = sel_weapon_inst_ = 0;
        tab_ = 2;
        hideOption(KEY_F7);
        hideOption(KEY_F8);
        hideOption(KEY_F9);
        hideTeamList();
        hideModsList();
        showEquipList();
        show(false);
    }
    if (key >= KEY_0 && key < (Key) (KEY_0 + g_App.numRecruits())) {
        int i = key - KEY_0;
        Agent *n = g_App.recruit(i);
        bool found = false;
        for (int j = 0; j < 4; j++)
            if (g_App.teamMember(j) == n)
                found = true;
        if (!found) {
            g_App.setTeamMember(cur_agent_, n);
            show(false);
        }
    }
    if (key >= KEY_a && key < (Key) (KEY_a + g_App.numAvailableMods())) {
        int i = key - KEY_a + 1;
        sel_mod_ = i;
        showOption(KEY_F7);
        showOption(KEY_F8);
        hideModsList();
        show(false);
    }
    if (key >= (Key) (KEY_a + g_App.numAvailableMods())
        && key <=
        (Key) (KEY_a + g_App.numAvailableMods() +
               g_App.numAvailableWeapons())) {
        int i = key - (KEY_a + g_App.numAvailableMods()) + 1;
        sel_weapon_ = i;
        showOption(KEY_F7);
        showOption(KEY_F8);
        hideEquipList();
        show(false);
    }
    if (key == KEY_F7) {
        sel_weapon_ = sel_mod_ = sel_weapon_inst_ = 0;
        hideOption(KEY_F7);
        hideOption(KEY_F8);
        hideOption(KEY_F9);
        if (tab_ == 1)
            showModsList();
        else
            showEquipList();
        show(false);
    }
    if (key == KEY_F8 && sel_weapon_) {
        Weapon *w = g_App.availableWeapon(sel_weapon_ - 1);
        if (sel_all_) {
            for (int n = 0; n < 4; n++) {
                Agent *selected = g_App.teamMember(n);
                if (selected->numWeapons() < 8
                    && g_App.money() >= w->cost()) {
                    g_App.setMoney(g_App.money() - w->cost());
                    selected->addWeapon(w->createInstance());
                }
            }
        } else {
            Agent *selected = g_App.teamMember(cur_agent_);
            if (selected->numWeapons() < 8 && g_App.money() >= w->cost()) {
                g_App.setMoney(g_App.money() - w->cost());
                selected->addWeapon(w->createInstance());
            }
        }
        show(false);
    }
    if (key == KEY_F8 && sel_mod_) {
        Mod *m = g_App.availableMod(sel_mod_ - 1);
        if (sel_all_) {
            for (int n = 0; n < 4; n++) {
                Agent *selected = g_App.teamMember(n);
                if ((selected->slot(m->slot()) == NULL
                     || selected->slot(m->slot())->cost() < m->cost())
                    && g_App.money() >= m->cost()) {
                    selected->setSlot(m->slot(), m);
                    g_App.setMoney(g_App.money() - m->cost());
                }
            }
        } else {
            Agent *selected = g_App.teamMember(cur_agent_);
            if ((selected->slot(m->slot()) == NULL
                 || selected->slot(m->slot())->cost() < m->cost())
                && g_App.money() >= m->cost()) {
                selected->setSlot(m->slot(), m);
                g_App.setMoney(g_App.money() - m->cost());
            }
        }
        sel_mod_ = 0;
        hideOption(KEY_F7);
        hideOption(KEY_F8);
        showModsList();
        show(false);
    }
    if (key == KEY_F9 && sel_weapon_inst_) {
        Agent *selected = g_App.teamMember(cur_agent_);
        WeaponInstance *w = selected->removeWeapon(sel_weapon_inst_ - 1);
        g_App.setMoney(g_App.money() + w->cost());
        delete w;
        sel_weapon_inst_ = 0;
        hideOption(KEY_F7);
        hideOption(KEY_F9);
        showEquipList();
        show(false);
    }
}
