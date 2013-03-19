/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2010  Benoit Blancard <benblan@users.sourceforge.net>*
 *   Copyright (C) 2011  Bohdan Stelmakh <chamel@users.sourceforge.net> *
 *   Copyright (C) 2011  Joey Parrish <joey.parrish@gmail.com>          *
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
#include "utils/file.h"
#include "confmenu.h"

#define NAME_MAX_SIZE 16
#define MAX_COLOUR 8

int g_Colours[MAX_COLOUR] = { 6, 7, 14, 3, 11, 12, 13, 15 };


ConfMenu::ConfMenu(MenuManager *m) :
Menu(m, MENU_CONF, MENU_MAIN, "mconfup.dat", "mconfout.dat") {
\tisCachable_ = false;
\ttoAcceptLogo_ = 0;
\ttoAcceptColourId_ = 0;
\ttempLogo_ = 0;
\ttempColourId_ = 0;
    
\tpanelMsgId_ = addStatic(280, 32, 330, "#CONF_MAIN_MSG", FontManager::SIZE_2, true);
    
\tlogoButId_ = addOption(325, 65, 240, 20, "#CONF_COL_LOGO_BUT", FontManager::SIZE_2, MENU_NO_MENU, true, false, Sprite::MSPR_BULLET_D, Sprite::MSPR_BULLET_L);
    compNameButId_ = addOption(325, 90, 240, 20, "#CONF_COM_NAME_BUT", FontManager::SIZE_2, MENU_NO_MENU, true, false, Sprite::MSPR_BULLET_D, Sprite::MSPR_BULLET_L);
    userNameButId_ = addOption(325, 115, 240, 20, "#CONF_YOUR_NAME_BUT", FontManager::SIZE_2, MENU_NO_MENU, true, false, Sprite::MSPR_BULLET_D, Sprite::MSPR_BULLET_L);
    // Accept button
    acceptButId_ = addOption(17, 347, 128, 25, "#MENU_ACC_BUT", FontManager::SIZE_2, MENU_MAIN);
    // Main menu button
    menuButId_ = addOption(500, 347,  128, 25, "#MENU_MAIN_BUT", FontManager::SIZE_2, MENU_MAIN);

\tcreatePanels();

\t// Sub panel Ok and Cancel buttons
    okButId_ = addOption(291, 122, 125, 23, "#CONF_OK_BUT", FontManager::SIZE_2, MENU_NO_MENU, false);
    cancelButId_ = addOption(476, 122, 123, 23, "#MENU_CANCEL_BUT", FontManager::SIZE_2, MENU_NO_MENU, false);

\ttoAcceptCmpNameTxtId_ = addStatic(32, 93, "", FontManager::SIZE_1, false);
\ttoAcceptUsrNameTxtId_ = addStatic(32, 115, "", FontManager::SIZE_1, false);

\tcurrPanel_ = PNL_MAIN;

\t// Initialize data for the buttons frame
    memset(butFrameData_, 255, 68 * 13);
    for (int i = 4; i < 66; i++) {
        butFrameData_[i + 0 * 68] = 16;
        butFrameData_[i + 11 * 68] = 16;
    }

    for (int j = 0; j < 12; j++) {
        butFrameData_[4 + j * 68] = 16;
        butFrameData_[66 + j * 68] = 16;
    }

\t// Initialize data for the text field frame
\tmemset(tfFrameData_, 255, 136 * 13);

    for (int i = 4; i < 134; i++) {
        tfFrameData_[i + 0 * 136] = 16;
        tfFrameData_[i + 11 * 136] = 16;
    }

    for (int j = 0; j < 12; j++) {
        tfFrameData_[4 + j * 136] = 16;
        tfFrameData_[134 + j * 136] = 16;
    }
}

ConfMenu::~ConfMenu() {
}

void ConfMenu::createPanels() {
\t// Color picker
\tcolStaticId_ = addStatic(475, 60, "#CONF_COL_TITLE", FontManager::SIZE_2, true);
\tgetStatic(colStaticId_)->setVisible(false);
\tleftColButId_ = addImageOption(405, 58, Sprite::MSPR_LEFT_ARROW_D, Sprite::MSPR_LEFT_ARROW_L, false);
    rightColButId_ = addImageOption(435, 58, Sprite::MSPR_RIGHT_ARROW_D, Sprite::MSPR_RIGHT_ARROW_L, false);

\t// Logo picker
    leftLogoButId_ = addImageOption(405, 94, Sprite::MSPR_LEFT_ARROW_D, Sprite::MSPR_LEFT_ARROW_L, false);
    rightLogoButId_ = addImageOption(435, 94, Sprite::MSPR_RIGHT_ARROW_D, Sprite::MSPR_RIGHT_ARROW_L, false);
    logoStaticId_ = addStatic(475, 96, "#CONF_LOGO_TITLE", FontManager::SIZE_2, true);
\tgetStatic(logoStaticId_)->setVisible(false);

\t// Change names textfields
\tpUserNameTF_ = addTextField(312, 79, 255, 21, FontManager::SIZE_2, NAME_MAX_SIZE);
\tpCompNameTF_ = addTextField(312, 79, 255, 21, FontManager::SIZE_2, NAME_MAX_SIZE);
}

void ConfMenu::handleRender(DirtyList &dirtyList) {
\t// Draw the current logo
    g_Screen.drawLogo(28, 22, toAcceptLogo_, g_Colours[toAcceptColourId_]);

\tif (currPanel_ == PNL_LOGO) {
\t\t// Draw the selected logo
\t\tg_Screen.drawLogo(336, 55, tempLogo_, g_Colours[tempColourId_]);
\t} else if (currPanel_ == PNL_CMPNM || currPanel_ == PNL_USRNM) {
\t\t// draw a frame around the textfield
        g_Screen.scale2x(300, 77, 136, 13, tfFrameData_);
\t}

\tif (currPanel_ != PNL_MAIN) {
\t\t// draw frame around ok and cancel buttons
        g_Screen.scale2x(283, 122, 68, 13, butFrameData_);
        g_Screen.scale2x(468, 122, 68, 13, butFrameData_);
\t}
}

void ConfMenu::handleShow() {
\tmenu_manager_->saveBackground();

\ttoAcceptLogo_ = g_Session.getLogo();

\tfor (unsigned int i = 0; i < sizeof(g_Colours) / sizeof(int); i++) {
        if (g_Colours[i] == g_Session.getLogoColour())
            toAcceptColourId_ = i;
    }

\tgetStatic(toAcceptUsrNameTxtId_)->setText(g_Session.getUserName());
\tgetStatic(toAcceptCmpNameTxtId_)->setText(g_Session.getCompanyName());

\tg_System.showCursor();
}

void ConfMenu::handleLeave() {
\tshowMainPanel();

    g_System.hideCursor();
}

void ConfMenu::handleAction(const int actionId, void *ctx, const int modKeys) {
\tif (actionId == logoButId_) {
\t\tshowLogoPanel();
\t} else if (actionId == userNameButId_) {
\t\tshowUserNamePanel();
\t} else if (actionId == compNameButId_) {
\t\tshowCompanyNamePanel();
\t} else if (actionId == leftColButId_) {
\t\ttempColourId_--;
\t\tif (tempColourId_ < 0) {
\t\t\ttempColourId_ = MAX_COLOUR - 1;
\t\t}
\t\tredrawPanel();
\t} else if (actionId == rightColButId_) {
\t\ttempColourId_ = (tempColourId_ + 1) % MAX_COLOUR;
\t\tredrawPanel();
\t} else if (actionId == leftLogoButId_) {
\t\ttempLogo_--;
\t\tif (tempLogo_ < 0 ) {
\t\t\ttempLogo_ = g_Screen.numLogos() - 1;
\t\t}
\t\tredrawPanel();
\t} else if (actionId == rightLogoButId_) {
\t\ttempLogo_ = (tempLogo_ + 1) % g_Screen.numLogos();
\t\tredrawPanel();
\t} else if (actionId == okButId_) {
\t\tif (currPanel_ == PNL_LOGO) {
\t\t\ttoAcceptColourId_ = tempColourId_;
\t\t\ttoAcceptLogo_ = tempLogo_;
\t\t} else if (currPanel_ == PNL_USRNM) {
\t\t\tgetStatic(toAcceptUsrNameTxtId_)->setText(pUserNameTF_->getText().c_str());
\t\t} else {
\t\t\tgetStatic(toAcceptCmpNameTxtId_)->setText(pCompNameTF_->getText().c_str());
\t\t}
\t\tredrawLogo();
\t\tshowMainPanel();
\t} else if (actionId == cancelButId_) {
\t\tshowMainPanel();
\t} else if (actionId == acceptButId_) {
\t\tg_Session.setCompanyName(getStatic(toAcceptCmpNameTxtId_)->getText().c_str());
\t\tg_Session.setUserName(getStatic(toAcceptUsrNameTxtId_)->getText().c_str());
\t\tg_Session.setLogo(toAcceptLogo_);
\t\tg_Session.setLogoColour(g_Colours[toAcceptColourId_]);
\t}
}

bool ConfMenu::handleUnknownKey(Key key, const int modKeys) {
\tif (currPanel_ != PNL_MAIN) {
\t\tif (key.keyFunc == KFC_ESCAPE) {
\t\t\tshowMainPanel();
\t\t\treturn true;
\t\t} else if (key.keyFunc == KFC_RETURN) {
\t\t\tif (currPanel_ == PNL_LOGO) {
\t\t\t\ttoAcceptColourId_ = tempColourId_;
\t\t\t\ttoAcceptLogo_ = tempLogo_;
\t\t\t} else if (currPanel_ == PNL_USRNM) {
\t\t\t\tgetStatic(toAcceptUsrNameTxtId_)->setText(pUserNameTF_->getText().c_str());
\t\t\t} else {
\t\t\t\tgetStatic(toAcceptCmpNameTxtId_)->setText(pCompNameTF_->getText().c_str());
\t\t\t}
\t\t\tredrawLogo();
\t\t\tshowMainPanel();
\t\t\treturn true;
\t\t}
\t}

\treturn false;
}

void ConfMenu::showMainPanel() {
\tgetOption(logoButId_)->setVisible(true);
\tgetOption(userNameButId_)->setVisible(true);
\tgetOption(compNameButId_)->setVisible(true);
\tgetStatic(panelMsgId_)->setText("#CONF_MAIN_MSG");

\tgetOption(okButId_)->setVisible(false);
\tgetOption(cancelButId_)->setVisible(false);
\tgetOption(acceptButId_)->setWidgetEnabled(true);
\tgetOption(menuButId_)->setWidgetEnabled(true);

\tif (currPanel_ == PNL_LOGO) {
\t\tgetOption(leftColButId_)->setVisible(false);
\t\tgetOption(rightColButId_)->setVisible(false);
\t\tgetOption(leftLogoButId_)->setVisible(false);
\t\tgetOption(rightLogoButId_)->setVisible(false);

\t\tgetStatic(colStaticId_)->setVisible(false);
\t\tgetStatic(logoStaticId_)->setVisible(false);
\t} else if (currPanel_ == PNL_USRNM) {
\t\tpUserNameTF_->setVisible(false);
\t\tpUserNameTF_->setText("");
\t\tcaptureInputBy(NULL);
\t} else if (currPanel_ == PNL_CMPNM) {
\t\tpCompNameTF_->setVisible(false);
\t\tpCompNameTF_->setText("");
\t\tcaptureInputBy(NULL);
\t}

\tredrawPanel();
\tcurrPanel_ = PNL_MAIN;
}

void ConfMenu::hideMainPanel() {
\tgetOption(logoButId_)->setVisible(false);
\tgetOption(userNameButId_)->setVisible(false);
\tgetOption(compNameButId_)->setVisible(false);

\tgetOption(acceptButId_)->setWidgetEnabled(false);
\tgetOption(menuButId_)->setWidgetEnabled(false);

\tredrawPanel();
}

void ConfMenu::showLogoPanel() {
\tcurrPanel_ = PNL_LOGO;
\tgetStatic(panelMsgId_)->setText("#CONF_COL_LOGO_MSG");
\tgetOption(leftColButId_)->setVisible(true);
\tgetOption(rightColButId_)->setVisible(true);
\tgetOption(leftLogoButId_)->setVisible(true);
\tgetOption(rightLogoButId_)->setVisible(true);

\tgetOption(okButId_)->setVisible(true);
\tgetOption(cancelButId_)->setVisible(true);

\tgetStatic(colStaticId_)->setVisible(true);
\tgetStatic(logoStaticId_)->setVisible(true);

\ttempColourId_ = toAcceptColourId_;
\ttempLogo_ = toAcceptLogo_;

\thideMainPanel();
}

void ConfMenu::showUserNamePanel() {
\tcurrPanel_ = PNL_USRNM;
\tgetStatic(panelMsgId_)->setText("#CONF_YOUR_NAME_MSG");

\tpUserNameTF_->setText(getStatic(toAcceptUsrNameTxtId_)->getText().c_str());
\tpUserNameTF_->setVisible(true);
\tcaptureInputBy(pUserNameTF_);

\tgetOption(okButId_)->setVisible(true);
\tgetOption(cancelButId_)->setVisible(true);

\thideMainPanel();
}

void ConfMenu::showCompanyNamePanel() {
\tcurrPanel_ = PNL_CMPNM;
\tgetStatic(panelMsgId_)->setText("#CONF_COM_NAME_MSG");

\tpCompNameTF_->setText(getStatic(toAcceptCmpNameTxtId_)->getText().c_str());
\tpCompNameTF_->setVisible(true);
\tcaptureInputBy(pCompNameTF_);

\tgetOption(okButId_)->setVisible(true);
\tgetOption(cancelButId_)->setVisible(true);

\thideMainPanel();
}
