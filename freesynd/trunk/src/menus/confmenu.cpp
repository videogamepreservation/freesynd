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

class CommonConfSubMenu : public Menu {
public:
    CommonConfSubMenu(MenuManager *m, const char *menu_name, uint8 *bkg,
            ConfMenu *confMenu) : Menu(m, menu_name, "", ""), bkg_(bkg) {
        setParentMenu("conf");
    }

    virtual void handleRender() {
        uint8 data[68 * 13];
        memset(data, 255, 68 * 13);

        for (int i = 4; i < 66; i++) {
            data[i + 0 * 68] = 16;
            data[i + 11 * 68] = 16;
        }

        for (int j = 0; j < 12; j++) {
            data[4 + j * 68] = 16;
            data[66 + j * 68] = 16;
        }

        g_Screen.scale2x(283, 122, 68, 13, data);
        g_Screen.scale2x(468, 122, 68, 13, data);

        g_System.showCursor();
    }

protected:
    uint8 *bkg_;
};

int g_Colours[8] = { 6, 7, 14, 3, 11, 12, 13, 15 };

class ChangeLogoMenu : public CommonConfSubMenu {
public:
    ChangeLogoMenu(MenuManager *m, uint8 *bkg, ConfMenu *confMenu) :
    CommonConfSubMenu(m, "changeLogo", bkg, confMenu), logo_(g_App.getGameSession().getLogo()),
    colour_(0) {
        setClearArea(bkg_, 283, 28, 328, 120);
        addStatic(299, 32, "#CONF_COL_LOGO_MSG", FontManager::SIZE_2, false);
        leftColButId_ = addImageOption(405, 58, KEY_F1, Sprite::MSPR_LEFT_ARROW_D, Sprite::MSPR_LEFT_ARROW_L);
        rightColButId_ = addImageOption(435, 58, KEY_F2, Sprite::MSPR_RIGHT_ARROW_D, Sprite::MSPR_RIGHT_ARROW_L);

        addStatic(475, 60, "#CONF_COL_TITLE", FontManager::SIZE_2, false);
        leftLogoButId_ = addImageOption(405, 94, KEY_F3, Sprite::MSPR_LEFT_ARROW_D, Sprite::MSPR_LEFT_ARROW_L);
        rightLogoButId_ = addImageOption(435, 94, KEY_F4, Sprite::MSPR_RIGHT_ARROW_D, Sprite::MSPR_RIGHT_ARROW_L);

        addStatic(475, 96, "#CONF_LOGO_TITLE", FontManager::SIZE_2, false);
        okButId_ = addOption(291, 122, 125, 23, "#CONF_OK_BUT", FontManager::SIZE_2, KEY_F5, "conf");
        addOption(476, 122, 123, 23, "#MENU_CANCEL_BUT", FontManager::SIZE_2, KEY_F6, "conf");

        for (unsigned int i = 0; i < sizeof(g_Colours) / sizeof(int); i++) {
            if (g_Colours[i] == g_App.getGameSession().getLogoColour())
                colour_ = i;
        }
    }

    virtual void handleRender() {
        CommonConfSubMenu::handleRender();
        g_Screen.drawLogo(336, 55, logo_, g_Colours[colour_]);
    }

    virtual void handleAction(const int actionId, void *ctx, const int modKeys) {
        if (actionId == leftColButId_) {
            if (colour_ > 0)
                colour_--;
            else
                colour_ = sizeof(g_Colours) / sizeof(int) - 1;

            g_Screen.drawLogo(336, 55, logo_, g_Colours[colour_]);
        }

        if (actionId == rightColButId_) {
            if ((unsigned int) colour_ < sizeof(g_Colours) / sizeof(int) - 1)
                colour_++;
            else
                colour_ = 0;

            g_Screen.drawLogo(336, 55, logo_, g_Colours[colour_]);
        }

        if (actionId == leftLogoButId_) {
            if (logo_ > 0)
                logo_--;
            else
                logo_ = g_Screen.numLogos() - 1;

            g_Screen.drawLogo(336, 55, logo_, g_Colours[colour_]);
        }

        if (actionId == rightLogoButId_) {
            if (logo_ < g_Screen.numLogos() - 1)
                logo_++;
            else
                logo_ = 0;

            g_Screen.drawLogo(336, 55, logo_, g_Colours[colour_]);
        }

        if (actionId == okButId_) {
            g_App.getGameSession().setLogo(logo_);
            g_App.getGameSession().setLogoColour(g_Colours[colour_]);
        }
    }

protected:
    int logo_, colour_;
    int rightColButId_;
    int leftColButId_;
    int leftLogoButId_;
    int rightLogoButId_;
    int okButId_;
};

class ChangeNameMenu : public CommonConfSubMenu {
public:
    ChangeNameMenu(MenuManager *m, const char *menu_name, uint8 *bkg,
            ConfMenu *confMenu) :
    CommonConfSubMenu(m, menu_name, bkg, confMenu), tick_count_(0),
    light_last_tick_(false) {}

    void drawCaret(bool light = false) {
        g_Screen.drawRect(315 +
                         g_App.fonts().textWidth(name_value_.c_str(), FontManager::SIZE_2),
                         95, 10, 2, light ? 252 : 16);
    }

    virtual void handleTick(int elapsed) {
        tick_count_+= elapsed;
        tick_count_ %= 200;

        if (tick_count_ < 100) {
            if (light_last_tick_) {
                light_last_tick_ = false;
                drawCaret(light_last_tick_);
            }
        } else {
            if (!light_last_tick_) {
                light_last_tick_ = true;
                drawCaret(light_last_tick_);
            }
        }
    }

    virtual void handleRender() {
        CommonConfSubMenu::handleRender();
        uint8 data[136 * 13];
        memset(data, 255, 136 * 13);

        for (int i = 4; i < 134; i++) {
            data[i + 0 * 136] = 16;
            data[i + 11 * 136] = 16;
        }

        for (int j = 0; j < 12; j++) {
            data[4 + j * 136] = 16;
            data[134 + j * 136] = 16;
        }

        g_Screen.scale2x(300, 77, 136, 13, data);
        g_App.fonts().drawText(313, 81, name_value_.c_str(), FontManager::SIZE_2, false);
        drawCaret();
    }

    virtual bool handleUnknownKey(Key key, const int modKeys) {
        bool consumed = false;
        if (key == KEY_BACKSPACE) {
            name_value_ = name_value_.substr(0, name_value_.size() - 1);
            consumed = true;
        }

        if (name_value_.size() < NAME_MAX_SIZE) {
            if (key >= KEY_a && key <= KEY_z) {
                name_value_ += key - KEY_a + 'A';
                consumed = true;
            }

            if (key >= KEY_0 && key <= KEY_9) {
                name_value_ += key - KEY_0 + '0';
                consumed = true;
            }

            if (key == KEY_SPACE) {
                name_value_ += ' ';
                consumed = true;
            }
        }

        g_Screen.scale2x(310, 80, 129, 9, bkg_ + 155 + 40 * 320, 320);
        g_App.fonts().drawText(313, 81, name_value_.c_str(), FontManager::SIZE_2, false);
        drawCaret();

        return consumed;
    }

protected:
    std::string name_value_;
    int tick_count_;
    bool light_last_tick_;
};

class ChangeCompanyNameMenu : public ChangeNameMenu {
public:
    ChangeCompanyNameMenu(MenuManager *m, uint8 *bkg, ConfMenu *confMenu) :
    ChangeNameMenu(m, "changeCompany", bkg, confMenu) {
        setClearArea(bkg, 283, 28, 328, 120);
        addStatic(320, 32, "#CONF_COM_NAME_MSG", FontManager::SIZE_2, false);
        okButId_ = addOption(291, 122, 125, 23, "#CONF_OK_BUT", FontManager::SIZE_2, KEY_F5, "conf");
        addOption(476, 122, 123, 23, "#MENU_CANCEL_BUT", FontManager::SIZE_2, KEY_F6, "conf");
        name_value_ = g_App.getGameSession().getCompanyName();
    }

    virtual void handleAction(const int actionId, void *ctx, const int modKeys) {
        if (actionId == okButId_) {
            g_App.getGameSession().setCompanyName(name_value_.c_str());
            g_App.setCheatCode(name_value_.c_str());
        }
    }

private:
    int okButId_;
};

class ChangeYourNameMenu : public ChangeNameMenu {
public:
    ChangeYourNameMenu(MenuManager *m, uint8 *bkg, ConfMenu *confMenu) :
    ChangeNameMenu(m, "changeName", bkg, confMenu) {
        setClearArea(bkg, 283, 28, 328, 120);
        addStatic(350, 32, "#CONF_YOUR_NAME_MSG", FontManager::SIZE_2, false);
        okButId_ = addOption(291, 122, 125, 23, "#CONF_OK_BUT", FontManager::SIZE_2, KEY_F5, "conf");
        addOption(476, 122, 123, 23, "#MENU_CANCEL_BUT", FontManager::SIZE_2, KEY_F6, "conf");
        name_value_ = g_App.getGameSession().getUserName();
    }

    virtual void handleAction(const int actionId, void *ctx, const int modKeys) {
        if (actionId == okButId_)
            g_App.getGameSession().setUserName(name_value_.c_str());
    }
private:
    int okButId_;
};

ConfMenu::ConfMenu(MenuManager *m) :
Menu(m, "conf", "mconfup.dat", "mconfout.dat") {
    int size;
    bkg_ = File::loadOriginalFile("mconscr.dat", size);
    submenu_logo_ = new ChangeLogoMenu(m, bkg_, this);
    submenu_company_name_ = new ChangeCompanyNameMenu(m, bkg_, this);
    submenu_name_ = new ChangeYourNameMenu(m, bkg_, this);
    setClearArea(bkg_, 283, 28, 328, 120);
    addStatic(299, 32, "#CONF_MAIN_MSG", FontManager::SIZE_2, false);
    addOption(325, 65, 240, 20, "#CONF_COL_LOGO_BUT", FontManager::SIZE_2, KEY_F1, "changeLogo", true, false, Sprite::MSPR_BULLET_D, Sprite::MSPR_BULLET_L);
    addOption(325, 90, 240, 20, "#CONF_COM_NAME_BUT", FontManager::SIZE_2, KEY_F2, "changeCompany", true, false, Sprite::MSPR_BULLET_D, Sprite::MSPR_BULLET_L);
    addOption(325, 115, 240, 20, "#CONF_YOUR_NAME_BUT", FontManager::SIZE_2, KEY_F3, "changeName", true, false, Sprite::MSPR_BULLET_D, Sprite::MSPR_BULLET_L);
    // Accept button
    addOption(17, 347, 128, 25, "#MENU_ACC_BUT", FontManager::SIZE_2, KEY_F4, "main");
    // Main menu button
    addOption(500, 347,  128, 25, "#MENU_MAIN_BUT", FontManager::SIZE_2, KEY_F5, "main");
    setParentMenu("main");
}

ConfMenu::~ConfMenu() {
    delete[] bkg_;
    delete submenu_logo_;
    delete submenu_company_name_;
    delete submenu_name_;
}

void ConfMenu::handleRender() {
    g_Screen.drawLogo(28, 22, g_App.getGameSession().getLogo(), g_App.getGameSession().getLogoColour());

    if (*g_App.getGameSession().getCompanyName()) {
        g_Screen.scale2x(28, 90, 120, 10, bkg_ + 14 + 45 * 320, 320);
        g_App.fonts().drawText(28, 92, g_App.getGameSession().getCompanyName(), FontManager::SIZE_1, false);
    }

    if (*g_App.getGameSession().getUserName()) {
        g_Screen.scale2x(28, 112, 120, 10, bkg_ + 14 + 56 * 320, 320);
        g_App.fonts().drawText(28, 114, g_App.getGameSession().getUserName(), FontManager::SIZE_1, false);
    }

    g_System.showCursor();
}

void ConfMenu::handleLeave() {
    g_System.hideCursor();
}
