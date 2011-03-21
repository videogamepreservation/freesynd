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
#include "core/research.h"
#include "researchmenu.h"

ResearchMenu::ResearchMenu(MenuManager * m):Menu(m, "research", "mresrch.dat", "mresout.dat")
{
    tab_ = TAB_EQUIPS;
    pSelectedWeapon_ = NULL;
    pSelectedMod_ = NULL;
    pSelectedRes_ = NULL;

    addStatic(228, 35, "#RES_TITLE", FontManager::SIZE_4, true);          //Title
    txtTimeId_ = addStatic(500, 9, "", FontManager::SIZE_2, false);     // Time

    modsButId_ = addToggleAction(16, 290, 129, 25, "#RES_MODS_BUT", FontManager::SIZE_2, KEY_F1, tab_ == TAB_MODS);
    equipButId_ = addToggleAction(16, 318, 129, 25,  "#RES_EQUIP_BUT", FontManager::SIZE_2, KEY_F2, tab_ == TAB_EQUIPS);
    addOption(16, 346, 129, 25,  "#MENU_ACC_BUT", FontManager::SIZE_2, KEY_F3, "select");
    addOption(500, 347,  128, 25, "#MENU_MAIN_BUT", FontManager::SIZE_2, KEY_F4, "main");

    pFieldEquipLBox_ = addListBox(20, 84,  122, 120, 4, tab_ == TAB_EQUIPS);
    pFieldModsLBox_ = addListBox(20, 84,  122, 120, 6, tab_ == TAB_MODS);
    synchFieldSearchList(g_App.researchManager().getAvailableModsSearch(), pFieldModsLBox_);
    synchFieldSearchList(g_App.researchManager().getAvailableEquipsSearch(), pFieldEquipLBox_);

    pEquipsLBox_ = addListBox(504, 110,  122, 230, 18, tab_ == TAB_EQUIPS);
    pModsLBox_ = addListBox(504, 110,  122, 230, 18, tab_ == TAB_MODS);
    addWeaponOptions();
    addModOptions();
    // Close Mods/Equips details button
    cancelDescId_ = addOption(500, 320,  127, 22,  "#MENU_CANCEL_BUT", FontManager::SIZE_2, KEY_F5, NULL, false);
    // Start research button
    researchId_ = addOption(16, 158, 129, 25,  "#RES_RES_BUT", FontManager::SIZE_2, KEY_F6, NULL, false);
    // Close search details button
    cancelSearchId_ = addOption(16, 184, 129, 25,  "#MENU_CANCEL_BUT", FontManager::SIZE_2, KEY_F7, NULL, false);

    fieldTxtId_ = addStatic(20, 86, "", FontManager::SIZE_1, false);    // Search name
    fundMinLblId_ = addStatic(20, 105, "#RES_MIN_FUND_LBL", FontManager::SIZE_1, true);    // Funding Minimun label
    getStatic(fundMinLblId_)->setVisible(false);
    fundMinTxtId_ = addStatic(20, 117, "", FontManager::SIZE_1, false);    // Funding Minimun
    fundMaxLblId_ = addStatic(20, 130, "#RES_MAX_FUND_LBL", FontManager::SIZE_1, true);    // Funding max label
    getStatic(fundMaxLblId_)->setVisible(false);
    fundMaxTxtId_ = addStatic(20, 143, "", FontManager::SIZE_1, false);    // Search maximum

    incrFundId_ = addOption(16, 210, 129, 25,  "#RES_INC_FUND_BUT", FontManager::SIZE_2, KEY_F8, NULL, false);
    decrFundId_ = addOption(16, 260, 129, 25,  "#RES_DEC_FUND_BUT", FontManager::SIZE_2, KEY_F9, NULL, false);
    fundCurrLblId_ = addStatic(16, 242, 129, "", FontManager::SIZE_2, false);    // Current Funding label
    searchTitleLblId_ = addStatic(158, 86, "", FontManager::SIZE_2, false);    // Current search title
    setParentMenu("select");
}

/*!
 * Verify that list of research match mods or equips list box
 * \param pList List of available researches
 * \param pListBox Mods or Equips List box
 */
void ResearchMenu::synchFieldSearchList(std::list<Research *> *pList, ListBox *pListBox)
{
    int i = 0;
    // Runs through the research list
    for (std::list < Research * >::iterator it = pList->begin(); it != pList->end(); it++) {
        Research *pResearch = *it;
        // Check if entries match
        if (pListBox->existsAt(i)) {
            // An agent is also on this slot in the list box : checks
            // if two agents are the same
            int id = pListBox->getItemIdAt(i);
            if (pResearch->getId() != id) {
                // Ids are different : replaces the one on the listbox
                // by the one that is on the reference list
                pListBox->setAt(pResearch->getName(), pResearch->getId(), i);
            }
        } else {
            // The list box is empty on this slot so adds a new agent
            pListBox->setAt(pResearch->getName(), pResearch->getId(), i);
        }
        i++;
    }

    // Removes any remaining lines in list box
    if (i < pListBox->getMaxLine()) {
        for (; i < pListBox->getMaxLine(); i++) {
            pListBox->remove(i);
        }
    }
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

    pSelectedRes_ = NULL;
    hideOption(KEY_F6);
    hideOption(KEY_F7);
    getStatic(fieldTxtId_)->setVisible(false);
    getStatic(fundMinTxtId_)->setVisible(false);
    getStatic(fundMaxTxtId_)->setVisible(false);
    getStatic(fundMinLblId_)->setVisible(false);
    getStatic(fundMaxLblId_)->setVisible(false);
}

/*!
 * Hides the search field list box for Equips and Mods.
 */
void ResearchMenu::hideFieldList() {
    pFieldEquipLBox_->setVisible(false);
    pFieldModsLBox_->setVisible(false);
}

void ResearchMenu::showResInfo() {
    if (pSelectedRes_) {
        MenuText * pTxt = getStatic(fundMinTxtId_);
        pTxt->setVisible(true);
        pTxt->setTextFormated("%d", pSelectedRes_->getMinFunding());

        pTxt = getStatic(fundMaxTxtId_);
        pTxt->setVisible(true);
        pTxt->setTextFormated("%d", pSelectedRes_->getMaxFunding());

        pTxt = getStatic(fieldTxtId_);
        pTxt->setVisible(true);
        pTxt->setText(pSelectedRes_->getName().c_str());

        getStatic(fundMinLblId_)->setVisible(true);
        getStatic(fundMaxLblId_)->setVisible(true);

        if (pSelectedRes_->isStarted()) {
            showResGraph();
        } else {
            // Show start research button only if selected research
            // has not started yet
            getOption(researchId_)->setVisible(true);
        }
        // Show cancel button
        getOption(cancelSearchId_)->setVisible(true);
    }
}

void ResearchMenu::showResGraph() {
    getOption(researchId_)->setVisible(false);
    getOption(incrFundId_)->setVisible(true);
    getOption(decrFundId_)->setVisible(true);
    MenuText *pTxt = getStatic(fundCurrLblId_);
    pTxt->setVisible(true);
    pTxt->setTextFormated("%d", pSelectedRes_->getCurrFunding());
    getStatic(searchTitleLblId_)->setTextFormated("#RES_GRAP_TITLE", pSelectedRes_->getName().c_str());
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

    if (pSelectedRes_) {
        uint8 ldata[63];
        memset(ldata, 16, sizeof(ldata));
        g_Screen.scale2x(18, 102, sizeof(ldata), 1, ldata);
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
        int selRes = -1;
        if (actionId == pFieldEquipLBox_->getId()) {
            selRes = pFieldEquipLBox_->getItemIdAt(*id);
            pSelectedRes_ = g_App.researchManager().getEquipsSearch(selRes);
        } else {
            selRes = pFieldModsLBox_->getItemIdAt(*id);
            pSelectedRes_ = g_App.researchManager().getModsSearch(selRes);
        }
        
        // Hide list
        hideFieldList();
        // Show Research and Cancel buttons
        showResInfo();

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
            showFieldList();
            showDetailsList();
        }
    } else if (actionId == equipButId_) {
        if (tab_ != TAB_EQUIPS) {
            tab_ = TAB_EQUIPS;
            showFieldList();
            showDetailsList();
        }
    } else if (actionId == cancelDescId_) {
        showDetailsList();
    } else if (actionId == cancelSearchId_) {
        showFieldList();
    } else if (actionId == researchId_) {
        pSelectedRes_->start();
        showResGraph();
    } else if (actionId == incrFundId_) {
        if (pSelectedRes_->incrFunding()) {
            getStatic(fundCurrLblId_)->setTextFormated("%d", pSelectedRes_->getCurrFunding());
        }
    } else if (actionId == decrFundId_) {
        if (pSelectedRes_->decrFunding()) {
            getStatic(fundCurrLblId_)->setTextFormated("%d", pSelectedRes_->getCurrFunding());
        }
    }
}
