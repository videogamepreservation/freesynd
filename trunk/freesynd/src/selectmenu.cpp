/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
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
#include "selectmenu.h"

SelectMenu::SelectMenu(MenuManager * m):Menu(m, "select", "mselect.dat", "mselout.dat"), tab_(2),
cur_agent_(0), tick_count_(0), sel_weapon_(0), sel_mod_(0),
sel_weapon_inst_(0), sel_all_(false)
{
    mod0Id_ = 0;
    equip0Id_ = 0;
    agent0Id_ = 0;
    addStatic(148, 35, "TEAM SELECTION", FontManager::SIZE_4, true);
    txtTimeId_ = addStatic(500, 9, "", FontManager::SIZE_2, false);       // Time

    addOption(16, 234, 129, 25, "RESEARCH", FontManager::SIZE_2, KEY_F1, "research");
    teamButId_ = addToggleAction(16, 262, 129, 25, "TEAM", FontManager::SIZE_2, KEY_F2, false);
    modsButId_ = addToggleAction(16, 290, 129, 25, "MODS", FontManager::SIZE_2, KEY_F3, false);
    equipButId_ = addToggleAction(16, 318, 129, 25, "EQUIP", FontManager::SIZE_2, KEY_F4, true);
    addOption(16, 346, 129, 25, "#MENU_ACC_BUT", FontManager::SIZE_2, KEY_F5, "loading");
    addOption(500, 347,  128, 25, "#MENU_MAIN_BUT", FontManager::SIZE_2, KEY_F6, "main");

    addModOptions();
    addWeaponOptions();

    cancelButId_ = addOption(500, 270,  127, 22, "CANCEL", FontManager::SIZE_2, KEY_F7, NULL, false);
    purchaseButId_ = addOption(500, 320,  127, 22, "PURCHASE", FontManager::SIZE_2, KEY_F8, NULL, false);
    sellButId_ = addOption(500, 320,  127, 22, "SELL", FontManager::SIZE_2, KEY_F9, NULL, false);
    setParentMenu("brief");

    rnd_ = 0;
}

SelectMenu::~SelectMenu()
{
    
}

void SelectMenu::addModOptions()
{
    for (int i = 0; i < g_App.numAvailableMods(); i++) {
        Mod *m = g_App.availableMod(i);
        int id = addOption(504, 110 + 12 * i,  120, 10, m->name(), FontManager::SIZE_1, 
                            (Key) (KEY_a + i), NULL, false, false);
        if (mod0Id_ == 0) {
            mod0Id_ = id;
        }
    }
}

void SelectMenu::addWeaponOptions()
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

void SelectMenu::addRecruitOptions()
{
    for (int i = 0; i < AgentManager::MAX_AGENT; i++) {
        if (g_App.agents().agent(i)) {
            int id = addOption(520, 124 + i * 12,  104, 10, g_App.agents().agent(i)->name(), 
                        FontManager::SIZE_1, (Key) (KEY_0 + i), NULL, false, false);

            if (agent0Id_ == 0) {
                agent0Id_ = id;
            }
        }
    }
}

void SelectMenu::clearRecruitOptions()
{
    if (agent0Id_ != 0) {
        for (int i = 0; i < AgentManager::MAX_AGENT; i++) {
            if (g_App.agents().agent(i)) {
                for (std::list < ActionWidget * >::iterator it = actions_.begin();
                    it != actions_.end(); it++) {

                    if ((*it)->getId() == agent0Id_ + i) {
                        delete (*it);
                        it = actions_.erase(it);
                        // TODO faire un delete
                        break;
                    }
                }
            }
        }

        agent0Id_ = 0;
    }
}
/*!
 * Draws a dashed line around the currently selected agent selector.
 * \param x Coordinates of the top left corner 
 * \param y Coordinates of the top left corner 
 */
void SelectMenu::drawAgentSelector(int x, int y) {
    // First create a transparent sprite
    uint8 cdata[30 * 33];
    int cwidth = 30;
    int cheight = 33;
    memset(cdata, 255, sizeof(cdata));

    // Draws the upper and lower lines
    for (int i = 0; i < cwidth; i++) {
        cdata[i] = ((rnd_ + i) % 8 <= 4) ? 252 : 16;
        cdata[i + (cheight - 1) * cwidth] =
            ((rnd_ + i) % 8 >= 4) ? 252 : 16;
    }

    // Draws the right and left line
    for (int j = 0; j < cheight; j++) {
        cdata[j * cwidth] = ((rnd_ + j) % 8 >= 4) ? 252 : 16;
        cdata[j * cwidth + cwidth - 1] = ((rnd_ + j) % 8 <= 4) ? 252 : 16;
    }

    // blits the sprite at given position
    g_Screen.scale2x(x, y, cwidth, cheight, cdata);
}

void SelectMenu::drawAgent()
{
    Agent *selected = g_Session.teamMember(cur_agent_);
    if (selected == NULL)
        return;

    // write selected agent's name
    char tmp[100];
    sprintf(tmp, "SPECIAL AGENT %s", selected->name());
    g_App.fonts().drawText(158, 86, tmp, FontManager::SIZE_2, false);

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
                               FontManager::SIZE_1, false);
    }
    if (selected->slot(SLOT_ARMS)) {
        arms = selected->slot(SLOT_ARMS)->icon(selected->isMale());
        g_App.fonts().drawText(366, 226, selected->slot(SLOT_ARMS)->name(),
                               FontManager::SIZE_1, false);
    }

    g_App.menuSprites().drawSpriteXYZ(arms, armsx, armsy, 0, false, true);
    g_App.menuSprites().drawSpriteXYZ(torso, 224, torsoy, 0, false, true);
    g_App.menuSprites().drawSpriteXYZ(legs, 224, legsy, 0, false, true);

    if (selected->slot(SLOT_CHEST)) {
        int chest = selected->slot(SLOT_CHEST)->icon(selected->isMale());
        g_App.fonts().drawText(366, 202,
                               selected->slot(SLOT_CHEST)->name(), FontManager::SIZE_1,
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
                               selected->slot(SLOT_HEART)->name(), FontManager::SIZE_1,
                               false);
        g_App.menuSprites().drawSpriteXYZ(heart, 254, 166, 0, false, true);
    }

    if (selected->slot(SLOT_EYES)) {
        int eyes = selected->slot(SLOT_EYES)->icon(selected->isMale());
        g_App.fonts().drawText(366, 136, selected->slot(SLOT_EYES)->name(),
                               FontManager::SIZE_1, false);
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
                               selected->slot(SLOT_BRAIN)->name(), FontManager::SIZE_1,
                               false);
        int brainx = 238;
        if (!selected->isMale()) {
            brainx += 2;
        }
        g_App.menuSprites().drawSpriteXYZ(brain, brainx, 114, 0, false,
                                          true);
    }
    // restore lines over agent
/*    g_Screen.blit(254, 124, 30, 2,
                  background_ + 256 + 124 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);
    g_Screen.blit(264, 132, 30, 2,
                  background_ + 266 + 132 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);
    g_Screen.blit(266, 174, 36, 2,
                  background_ + 268 + 174 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);
    g_Screen.blit(252, 210, 56, 2,
                  background_ + 254 + 210 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);
    g_Screen.blit(302, 232, 10, 2,
                  background_ + 304 + 232 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);
    g_Screen.blit(264, 256, 30, 2,
                  background_ + 266 + 256 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);*/

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
    for (int i = 0; i < AgentManager::MAX_AGENT; i++)
        showOption((Key) (KEY_0 + i));
}

void SelectMenu::hideTeamList()
{
    for (int i = 0; i < AgentManager::MAX_AGENT; i++)
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
    // Updates the moving agent selector
    if (tick_count_ > 300) {
        rnd_ = ++rnd_ % 8;
        tick_count_ = 0;
        needRendering();
    }

    // Update the clock
    if (g_Session.updateTime(elapsed)) {
        updateClock();
    }
}

/*! 
 * Update the game time display
 */
void SelectMenu::updateClock() {
    char tmp[100];
    g_Session.getTimeAsStr(tmp);
    getStatic(txtTimeId_)->setText(tmp);
}

void SelectMenu::handleShow() {

    menu_manager_->saveBackground();

    cur_agent_ = 0;
    for (int i = 0; i < 4; i++) {
        if (g_Session.teamMember(i) != NULL) {
            cur_agent_ = i;
            break;
        }
    }
    clearRecruitOptions();
    addRecruitOptions();

    // Show the mouse
    g_System.showCursor();

    // Update the time
    updateClock();
}

void SelectMenu::handleRender() {
    g_Screen.drawLogo(18, 14, g_App.getGameSession().getLogo(), g_App.getGameSession().getLogoColour());

    // write money
    char tmp[100];
    sprintf(tmp, "%d", g_App.getGameSession().getMoney());
    g_App.fonts().drawText(560 - g_App.fonts().textWidth(tmp, FontManager::SIZE_2) / 2, 87,
                           tmp, FontManager::SIZE_2, false);

    // write team member icons and health
    uint8 data[4], datag[4];
    memset(data, 204, 4);
    memset(datag, 10, 4);
    Agent *t1 = g_Session.teamMember(0);
    Agent *t2 = g_Session.teamMember(1);
    Agent *t3 = g_Session.teamMember(2);
    Agent *t4 = g_Session.teamMember(3);
    if (t1) {
        if (t1->isActive()) {
            g_App.menuSprites().drawSpriteXYZ(Sprite::MSPR_SELECT_1, 20, 84, 0, false, true);
            for (int i = 0; i < t1->health() * 35 / 255; i++)
                g_Screen.scale2x(68, 122 - i, 3, 1, data);
        } else {
            for (int i = 0; i < t1->health() * 35 / 255; i++)
                g_Screen.scale2x(68, 122 - i, 3, 1, datag);
        }
    }
    if (t2) {
        if (t2->isActive()) {
            g_App.menuSprites().drawSpriteXYZ(Sprite::MSPR_SELECT_2, 82, 84, 0, false, true);
            for (int i = 0; i < t2->health() * 35 / 255; i++)
                g_Screen.scale2x(132, 122 - i, 3, 1, data);
        } else {
            for (int i = 0; i < t2->health() * 35 / 255; i++)
                g_Screen.scale2x(132, 122 - i, 3, 1, datag);
        }
    }
    if (t3) {
        if (t3->isActive()) {
            g_App.menuSprites().drawSpriteXYZ(Sprite::MSPR_SELECT_3, 20, 162, 0, false, true);
            for (int i = 0; i < t3->health() * 35 / 255; i++)
                g_Screen.scale2x(68, 200 - i, 3, 1, data);
        } else {
            for (int i = 0; i < t3->health() * 35 / 255; i++)
                g_Screen.scale2x(68, 200 - i, 3, 1, datag);
        }
    }
    if (t4) {
        if (t4->isActive()) {
            g_App.menuSprites().drawSpriteXYZ(Sprite::MSPR_SELECT_4, 82, 162, 0, false, true);
            for (int i = 0; i < t4->health() * 35 / 255; i++)
                g_Screen.scale2x(132, 200 - i, 3, 1, data);
        } else {
            for (int i = 0; i < t4->health() * 35 / 255; i++)
                g_Screen.scale2x(132, 200 - i, 3, 1, datag);
        }
    }
    if (sel_all_) {
        g_App.menuSprites().drawSpriteXYZ(77, 20, 152, 0, false, true);
    }

    // Draw the selector around the selected agent
    switch (cur_agent_) {
    case 0:
        drawAgentSelector(20, 84);
        break;
    case 1:
        drawAgentSelector(82, 84);
        break;
    case 2:
        drawAgentSelector(20, 162);
        break;
    case 3:
        drawAgentSelector(82, 162);
        break;
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
            Agent *selected = g_Session.teamMember(cur_agent_);
            if (selected)
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
        g_App.fonts().drawText(504, 106, "CRYO CHAMBER", FontManager::SIZE_1, false);
        uint8 ldata[49];
        memset(ldata, 252, sizeof(ldata));
        g_Screen.scale2x(504, 120, sizeof(ldata), 1, ldata);
        for (int i = 0; i < AgentManager::MAX_AGENT; i++) {
            if (g_App.agents().agent(i)) {
                for (int j = 0; j < 4; j++)
                    if (g_Session.teamMember(j) == g_App.agents().agent(i)) {
                        sprintf(tmp, "%d", j + 1);
                        g_App.fonts().drawText(504, 124 + i * 12, tmp, FontManager::SIZE_1,
                                               true);
                }
            } else {
                g_App.fonts().drawText(520, 124 + i * 12, "EMPTY", 0, true);
            }
        }
    }
}

void SelectMenu::handleLeave() {
    g_System.hideCursor();
}

void SelectMenu::toggleAgent(int n)
{
    int nactive = 0;
    for (int i = 0; i < 4; i++)
        if (g_Session.teamMember(i) && g_Session.teamMember(i)->isActive())
            nactive++;
    Agent *a = g_Session.teamMember(n);
    if (a) {
        if (a->isActive() && nactive == 1)
            return;
        a->setActive(!a->isActive());
    }
}

void SelectMenu::handleMouseDown(int x, int y, int button, const int modKeys)
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
            needRendering();
        }
        if (y > 150 && y < 162) {
            sel_all_ = !sel_all_;
            needRendering();
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
            needRendering();
        }
    }

    Agent *selected = g_Session.teamMember(cur_agent_);
    if (selected) {
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
                    needRendering();
                }
    }
}

void SelectMenu::handleAction(const int actionId, void *ctx, const int modKeys)
{
    if (actionId == teamButId_) {
        sel_weapon_ = sel_mod_ = sel_weapon_inst_ = 0;
        tab_ = 0;
        hideOption(KEY_F7);
        hideOption(KEY_F8);
        hideOption(KEY_F9);
        showTeamList();
        hideModsList();
        hideEquipList();
        needRendering();
    }
    if (actionId == modsButId_) {
        sel_weapon_ = sel_mod_ = sel_weapon_inst_ = 0;
        tab_ = 1;
        hideOption(KEY_F7);
        hideOption(KEY_F8);
        hideOption(KEY_F9);
        hideTeamList();
        showModsList();
        hideEquipList();
        needRendering();
    }
    if (actionId == equipButId_) {
        sel_weapon_ = sel_mod_ = sel_weapon_inst_ = 0;
        tab_ = 2;
        hideOption(KEY_F7);
        hideOption(KEY_F8);
        hideOption(KEY_F9);
        hideTeamList();
        hideModsList();
        showEquipList();
        needRendering();
    }
    if (actionId >= agent0Id_ && (actionId < agent0Id_ + AgentManager::MAX_AGENT)) {
        int i = actionId - agent0Id_;
        Agent *n = g_App.agents().agent(i);
        if (n) {
            bool found = false;
            int j;
            for (j = 0; j < 4; j++)
                if (g_Session.teamMember(j) == n) {
                    found = true;
                    break;
                }
            if (!found) {
                g_Session.setTeamMember(cur_agent_, n);
            } else if (j == cur_agent_){
                g_Session.setTeamMember(j, NULL);
            }
            needRendering();
        }
    }
    if (actionId >= mod0Id_ && actionId < (mod0Id_ + g_App.numAvailableMods())) {
        int i = actionId - mod0Id_ + 1;
        sel_mod_ = i;
        showOption(KEY_F7);
        showOption(KEY_F8);
        hideModsList();
        needRendering();
    }
    if (actionId >= equip0Id_
        && actionId <= equip0Id_ + g_App.numAvailableWeapons()) {
        int i = actionId - equip0Id_ + 1;
        sel_weapon_ = i;
        showOption(KEY_F7);
        showOption(KEY_F8);
        hideEquipList();
        needRendering();
    }
    if (actionId == cancelButId_) {
        sel_weapon_ = sel_mod_ = sel_weapon_inst_ = 0;
        hideOption(KEY_F7);
        hideOption(KEY_F8);
        hideOption(KEY_F9);
        if (tab_ == 1)
            showModsList();
        else
            showEquipList();
        needRendering();
    }
    if (actionId == purchaseButId_ && sel_weapon_) {
        Weapon *w = g_App.availableWeapon(sel_weapon_ - 1);
        if (sel_all_) {
            for (int n = 0; n < 4; n++) {
                Agent *selected = g_Session.teamMember(n);
                if (selected && selected->numWeapons() < 8
                    && g_App.getGameSession().getMoney() >= w->cost()) {
                    g_App.getGameSession().setMoney(g_App.getGameSession().getMoney() - w->cost());
                    selected->addWeapon(w->createInstance());
                }
            }
        } else {
            Agent *selected = g_Session.teamMember(cur_agent_);
            if (selected && selected->numWeapons() < 8
                && g_App.getGameSession().getMoney() >= w->cost()) {
                g_App.getGameSession().setMoney(g_App.getGameSession().getMoney() - w->cost());
                selected->addWeapon(w->createInstance());
            }
        }
        needRendering();
    }
    if (actionId == purchaseButId_ && sel_mod_) {
        Mod *m = g_App.availableMod(sel_mod_ - 1);
        if (sel_all_) {
            for (int n = 0; n < 4; n++) {
                Agent *selected = g_Session.teamMember(n);
                if (selected && (selected->slot(m->slot()) == NULL
                     || selected->slot(m->slot())->cost() < m->cost())
                    && g_App.getGameSession().getMoney() >= m->cost()) {
                    selected->setSlot(m->slot(), m);
                    g_App.getGameSession().setMoney(g_App.getGameSession().getMoney() - m->cost());
                }
            }
        } else {
            Agent *selected = g_Session.teamMember(cur_agent_);
            if (selected && (selected->slot(m->slot()) == NULL
                 || selected->slot(m->slot())->cost() < m->cost())
                && g_App.getGameSession().getMoney() >= m->cost()) {
                selected->setSlot(m->slot(), m);
                g_App.getGameSession().setMoney(g_App.getGameSession().getMoney() - m->cost());
            }
        }
        sel_mod_ = 0;
        hideOption(KEY_F7);
        hideOption(KEY_F8);
        showModsList();
        needRendering();
    }
    if (actionId == sellButId_ && sel_weapon_inst_) {
        Agent *selected = g_Session.teamMember(cur_agent_);
        WeaponInstance *w = selected->removeWeapon(sel_weapon_inst_ - 1);
        g_Session.setMoney(g_Session.getMoney() + w->cost());
        delete w;
        sel_weapon_inst_ = 0;
        hideOption(KEY_F7);
        hideOption(KEY_F9);
        showEquipList();
        needRendering();
    }
}
