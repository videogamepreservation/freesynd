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
#include "confmenu.h"

class CommonConfSubMenu : public Menu {
public:
    CommonConfSubMenu(MenuManager *m, const char *menu_name, uint8 *bkg,
            ConfMenu *confMenu) : Menu(m, menu_name, "", ""), bkg_(bkg) {
        setParentMenu("conf");
    }

    virtual void handleShow() {
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
    }

protected:
    uint8 *bkg_;
};

int g_Colours[8] = { 6, 7, 14, 3, 11, 12, 13, 15 };

class ChangeLogoMenu : public CommonConfSubMenu {
public:
    ChangeLogoMenu(MenuManager *m, uint8 *bkg, ConfMenu *confMenu) :
    CommonConfSubMenu(m, "changeLogo", bkg, confMenu), logo_(g_App.logo()),
    colour_(0) {
        setClearArea(bkg_, 283, 28, 328, 120);
        addStatic(299, 32, "CHANGE COLOUR AND LOGO", 1, false);
        addOption(405, 65, "\002", 1, KEY_F1, NULL);
        addOption(435, 65, "\003", 1, KEY_F2, NULL);
        addStatic(475, 65, "COLOUR", 1, false);
        addOption(405, 95, "\002", 1, KEY_F3, NULL);
        addOption(435, 95, "\003", 1, KEY_F4, NULL);
        addStatic(475, 95, "LOGO", 1, false);
        addOption(340, 127, "OK", 1, KEY_F5, "conf");
        addOption(501, 127, "CANCEL", 1, KEY_F6, "conf");

        for (unsigned int i = 0; i < sizeof(g_Colours) / sizeof(int); i++) {
            if (g_Colours[i] == g_App.logoColour())
                colour_ = i;
        }
    }

    virtual void handleShow() {
        CommonConfSubMenu::handleShow();
        g_Screen.drawLogo(336, 55, logo_, g_Colours[colour_]);
    }

    virtual void handleOption(Key key) {
        if (key == KEY_F1) {
            if (colour_ > 0)
                colour_--;
            else
                colour_ = sizeof(g_Colours) / sizeof(int) - 1;

            g_Screen.drawLogo(336, 55, logo_, g_Colours[colour_]);
        }

        if (key == KEY_F2) {
            if ((unsigned int) colour_ < sizeof(g_Colours) / sizeof(int) - 1)
                colour_++;
            else
                colour_ = 0;

            g_Screen.drawLogo(336, 55, logo_, g_Colours[colour_]);
        }

        if (key == KEY_F3) {
            if (logo_ > 0)
                logo_--;
            else
                logo_ = g_Screen.numLogos() - 1;

            g_Screen.drawLogo(336, 55, logo_, g_Colours[colour_]);
        }

        if (key == KEY_F4) {
            if (logo_ < g_Screen.numLogos() - 1)
                logo_++;
            else
                logo_ = 0;

            g_Screen.drawLogo(336, 55, logo_, g_Colours[colour_]);
        }

        if (key == KEY_F5) {
            g_App.setLogo(logo_);
            g_App.setLogoColour(g_Colours[colour_]);
        }
    }

protected:
    int logo_, colour_;
};

class ChangeNameMenu : public CommonConfSubMenu {
public:
    ChangeNameMenu(MenuManager *m, const char *menu_name, uint8 *bkg,
            ConfMenu *confMenu) :
    CommonConfSubMenu(m, menu_name, bkg, confMenu), tick_count_(0),
    light_last_tick_(false) {}

    void drawCaret(bool light = false) {
        uint8 data[5];
        memset(data, light ? 252 : 16, 5);
        g_Screen.scale2x(315 +
                         g_App.fonts().textWidth(name_value_.c_str(), 1),
                         95, 5, 1, data);
    }

    virtual void handleTick() {
        tick_count_++;

        if ((tick_count_ % 50) == 0) {
            light_last_tick_ = !light_last_tick_;
            drawCaret(light_last_tick_);
        }
    }

    virtual void handleShow() {
        CommonConfSubMenu::handleShow();
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
        g_App.fonts().drawText(313, 81, name_value_.c_str(), 1, false);
        drawCaret();
    }

    virtual void handleUnknownKey(Key key, KeyMod mod, bool pressed) {
        if (key == KEY_BACKSPACE)
            name_value_ = name_value_.substr(0, name_value_.size() - 1);

        if (name_value_.size() < 16) {
            if (key >= KEY_a && key <= KEY_z)
                name_value_ += key - KEY_a + 'A';

            if (key >= KEY_0 && key <= KEY_9)
                name_value_ += key - KEY_0 + '0';

            if (key == KEY_SPACE)
                name_value_ += ' ';
        }

        g_Screen.scale2x(310, 80, 120, 9, bkg_ + 155 + 40 * 320, 320);
        g_App.fonts().drawText(313, 81, name_value_.c_str(), 1, false);
        drawCaret();
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
        addStatic(320, 32, "CHANGE COMPANY NAME", 1, false);
        addOption(340, 127, "OK", 1, KEY_F5, "conf");
        addOption(501, 127, "CANCEL", 1, KEY_F6, "conf");
        name_value_ = g_App.companyName();
    }

    virtual void handleOption(Key key) {
        if (key == KEY_F5)
            g_App.setCompanyName(name_value_.c_str());
    }
};

class ChangeYourNameMenu : public ChangeNameMenu {
public:
    ChangeYourNameMenu(MenuManager *m, uint8 *bkg, ConfMenu *confMenu) :
    ChangeNameMenu(m, "changeName", bkg, confMenu) {
        setClearArea(bkg, 283, 28, 328, 120);
        addStatic(350, 32, "ENTER YOUR NAME", 1, false);
        addOption(340, 127, "OK", 1, KEY_F5, "conf");
        addOption(501, 127, "CANCEL", 1, KEY_F6, "conf");
        name_value_ = g_App.yourName();
    }

    virtual void handleOption(Key key) {
        if (key == KEY_F5)
            g_App.setYourName(name_value_.c_str());
    }
};

ConfMenu::ConfMenu(MenuManager *m) :
Menu(m, "conf", "mconfup.dat", "mconfout.dat") {
    int size;
    bkg_ = File::loadFile("mconscr.dat", size);
    submenu_logo_ = new ChangeLogoMenu(m, bkg_, this);
    submenu_company_name_ = new ChangeCompanyNameMenu(m, bkg_, this);
    submenu_name_ = new ChangeYourNameMenu(m, bkg_, this);
    addStatic(299, 32, "SELECT DETAIL TO CHANGE", 1, false);
    addOption(325, 65, "\001COLOUR AND LOGO", 1, KEY_F1, "changeLogo");
    addOption(325, 90, "\001COMPANY NAME", 1, KEY_F2, "changeCompany");
    addOption(325, 115, "\001YOUR NAME", 1, KEY_F3, "changeName");
    addOption(43, 352, "ACCEPT", 1, KEY_F4, "main");
    addOption(535, 352, "MENU", 1, KEY_F5, "main");
    setParentMenu("main");
}

ConfMenu::~ConfMenu() {
    delete[] bkg_;
    delete submenu_logo_;
    delete submenu_company_name_;
    delete submenu_name_;
}

void ConfMenu::handleShow() {
    g_Screen.drawLogo(28, 22, g_App.logo(), g_App.logoColour());

    if (*g_App.companyName()) {
        g_Screen.scale2x(28, 90, 120, 10, bkg_ + 14 + 45 * 320, 320);
        g_App.fonts().drawText(28, 90, g_App.companyName(), 1, false);
    }

    if (*g_App.yourName()) {
        g_Screen.scale2x(28, 112, 120, 10, bkg_ + 14 + 56 * 320, 320);
        g_App.fonts().drawText(28, 112, g_App.yourName(), 1, false);
    }
}
