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
#include "mapmenu.h"
#include "briefmenu.h"

int g_MissionNumbers[50] = {
    17, 39, 8, 16, 20, 18, 22, 12, 21, 1,
    15, 10, 9, 3, 2, 42, 5, 23, 34, 29,
    28, 35, 6, 4, 50, 32, 24, 37, 41, 33, 38,
    7, 48, 26, 44, 45, 27, 40, 14, 36, 46,
    31, 13, 11, 19, 43, 49, 30, 47, 25
};

BriefMenu::BriefMenu(MenuManager * m, MapMenu * mapMenu) :
Menu(m, "brief", "mbrief.dat", "mbrieout.dat"), map_menu_(mapMenu),
orig_pixels_(0), cur_miss_(0), start_line_(0), info_level_(1),
enhance_level_(0), mission_(0) {
    addStatic(148, 35, "MISSION BRIEF", 3, true);
    addOption(538, 118, "INFO", 1, KEY_F1, NULL);
    addOption(518, 169, "ENHANCE", 1, KEY_F2, NULL);
    addOption(461, 320, "\003", 1, KEY_F6, NULL);
    addOption(427, 320, "\002", 1, KEY_F7, NULL, false);
    addOption(43, 352, "ACCEPT", 1, KEY_F3, "select");
    addOption(173, 352, "MAP", 1, KEY_F4, "map");
    addOption(535, 352, "MENU", 1, KEY_F5, "main");
    setParentMenu("map");
}

BriefMenu::~BriefMenu() {
    if (orig_pixels_)
        delete[] orig_pixels_;

    if (mission_)
        delete mission_;
}

void BriefMenu::handleShow() {
    if (orig_pixels_ == 0) {
        orig_pixels_ = new uint8[GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT];
        memcpy(orig_pixels_, g_Screen.pixels(),
               GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT);
    }
    else {
        g_Screen.blit(0, 0, 500, 340, orig_pixels_, false,
                      GAME_SCREEN_WIDTH);
    }

    g_Screen.drawLogo(18, 14, g_App.logo(), g_App.logoColour());

    // grab mission info
    cur_miss_ = g_MissionNumbers[map_menu_->currentBlk()];
    mission_ = g_App.missions().loadMission(cur_miss_);

    // write briefing
    if (mission_->briefing()) {
        char *mbriefing = strdup(mission_->briefing());
        char *miss = mbriefing;
        char *nextline = miss - 1;

        do
            nextline = strchr(nextline + 1, '\x0a');
        while (nextline && (nextline[0] != '\x0a' || nextline[1] != '\x0a'));

        int line_count = 0;
        int lvls = 0;

        do {
            if (nextline) {
                char *tmp = new char[nextline - miss + 1];

                do {
                    memcpy(tmp, miss, nextline - miss);
                    tmp[nextline - miss] = 0;
                    nextline--;
                } while (g_App.fonts().textWidth(tmp, 1) > 470);

                delete[] tmp;
                nextline++;

                while (nextline[0] != '\x0a' && nextline[0] != ' '
                       && nextline[0] != 0) {
                    nextline--;
                }
            }

            if (*miss == '|') {
                lvls++;

                if (lvls > info_level_) {
                    hideOption(KEY_F6);
                    break;
                }

                nextline = miss + 1;
            }
            else {
                if (line_count >= start_line_) {
                    char tmp = 0;

                    if (nextline) {
                        tmp = *nextline;
                        *nextline = 0;
                    }

                    g_App.fonts().drawText(24,
                                           88 + (line_count -
                                                 start_line_) * 16, miss,
                                           1, false);
                    if (nextline)
                        *nextline = tmp;
                }

                line_count++;
            }
            if (nextline && nextline[0] == '\x0a') {
                miss = nextline + 2;

                if (line_count != 14)
                    line_count++;
            }
            else
                miss = nextline;

            if (miss && *miss) {
                nextline = miss - 1;

                do
                    nextline = strchr(nextline + 1, '\x0a');
                while (nextline
                        && (nextline[0] != '\x0a' || nextline[1] != '\x0a'));

                if (*miss == ' ')
                    miss++;

                if (nextline == NULL)
                    nextline = miss + strlen(miss);

                while (strchr(miss, '\x0a')
                       && strchr(miss, '\x0a') < nextline) {
                    *strchr(miss, '\x0a') = ' ';
                }
            }
            else
                hideOption(KEY_F6);
        } while (miss && *miss && line_count < start_line_ + 14);

        free(mbriefing);        // using free because we strdup()'d to get this
    }
    // TODO: draw briefing minimap

    // write money
    char tmp[100];
    g_Screen.blit(502, 87, 125, 30,
                  orig_pixels_ + 502 + 87 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);
    sprintf(tmp, "%d", g_App.money());
    g_App.fonts().drawText(560 - g_App.fonts().textWidth(tmp, 1) / 2, 87,
                           tmp, 1, false);

    // write cost for more info
    g_Screen.blit(538, 140, 100, 30,
                  orig_pixels_ + 538 + 140 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);

    if (info_level_ < 3) {
        sprintf(tmp, "%d", mission_->infoCost(info_level_));
        g_App.fonts().drawText(560 - g_App.fonts().textWidth(tmp, 1) / 2,
                               140, tmp, 1, false);
    }

    // write cost for more enhance
    g_Screen.blit(538, 195, 100, 30,
                  orig_pixels_ + 538 + 195 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);

    if (enhance_level_ < 3) {
        sprintf(tmp, "%d", mission_->enhanceCost(enhance_level_));
        g_App.fonts().drawText(560 - g_App.fonts().textWidth(tmp, 1) / 2,
                               195, tmp, 1, false);
    }
}

void BriefMenu::handleOption(Key key) {
    if (key == KEY_F1) {
        if (info_level_ < 3) {
            g_App.setMoney(g_App.money() - mission_->infoCost(info_level_));
            info_level_++;
        }

        showOption(KEY_F6);
        show(false);
    }

    if (key == KEY_F2) {
        if (enhance_level_ < 3) {
            g_App.setMoney(g_App.money() -
                           mission_->enhanceCost(enhance_level_));
            enhance_level_++;
        }
        show(false);
    }

    if (key == KEY_F6) {
        start_line_ += 14;
        showOption(KEY_F7);
        show(false);
    }

    if (key == KEY_F7) {
        start_line_ -= 14;

        if (start_line_ <= 0) {
            start_line_ = 0;
            hideOption(KEY_F7);
        }
        showOption(KEY_F6);
        show(false);
    }
}
