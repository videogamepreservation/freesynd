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
#include "utils/file.h"
#include "briefmenu.h"

BriefMenu::BriefMenu(MenuManager * m) :
Menu(m, "brief", "mbrief.dat", "mbrieout.dat"),
start_line_(0), info_level_(0),
enhance_level_(0) {
    addStatic(148, 35, "MISSION BRIEF", 3, true);
    addStatic(500, 9, "", 1, false);       // Time
    addOption(538, 118, "INFO", 1, KEY_F1, NULL);
    addOption(518, 169, "ENHANCE", 1, KEY_F2, NULL);
    addOption(461, 320, "", 1, KEY_F6, NULL, true, Sprite::MSPR_RIGHT_ARROW2_D, Sprite::MSPR_RIGHT_ARROW2_L);
    addOption(427, 320, "", 1, KEY_F7, NULL, false, Sprite::MSPR_LEFT_ARROW2_D, Sprite::MSPR_LEFT_ARROW2_L);
    addOption(43, 352, "ACCEPT", 1, KEY_F3, "select");
    addOption(173, 352, "MAP", 1, KEY_F4, "map");
    addOption(535, 352, "MENU", 1, KEY_F5, "main");

    setParentMenu("map");
}

BriefMenu::~BriefMenu() {
}

void BriefMenu::handleTick(int elapsed)
{
    if (g_Session.updateTime(elapsed)) {
        updateClock();
    }
}

/*! 
 * Update the game time display
 */
void BriefMenu::updateClock() {
    char tmp[100];

    g_Session.getTimeAsStr(tmp);
    setStaticText(1, tmp);

    needRendering();
}

void BriefMenu::handleShow() {

    menu_manager_->saveBackground();

    // grab mission info
    int cur_miss = g_Session.getSelectedBlock().mis_id;
    Mission *pMission = g_App.missions().loadMission(cur_miss);
    assert(pMission != NULL);
    g_Session.setMission(pMission);
    // NOTE: map is required to be loaded here, because minimap is z=0
    pMission->loadMap();

    updateClock();
}

void BriefMenu::handleRender() {

    /*g_Screen.blit(0, 0, GAME_SCREEN_WIDTH, 340, orig_pixels_, false,
                      GAME_SCREEN_WIDTH);*/

    g_Screen.drawLogo(18, 14, g_Session.getLogo(), g_Session.getLogoColour());

    Mission *pMission = g_Session.getMission();
    // write briefing
    if (pMission->briefing()) {
        int sizeStr = strlen(pMission->briefing()) + 1;
        char *mbriefing = (char *)malloc(sizeStr);
        assert(mbriefing != NULL);
        fs_strcpy(mbriefing, sizeStr, pMission->briefing());
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
                } while (g_App.fonts().textWidth(tmp, FontManager::SIZE_2) > 470);

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
                                                 FontManager::SIZE_2, false);
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

        free(mbriefing);        // using free because allocated this
    }
    // TODO: draw briefing minimap
    // NOTE: enhance levels: 0 = 10px(5), 1 = 8px(4), 2 = 6px(3), 3 - 4px(2),
    // 4 - 2px(1) + enemy peds; x = 502(251), y = 218(109), 124x124(62x62)

    // write money
    char tmp[100];
/*    g_Screen.blit(502, 87, 125, 30,
                  orig_pixels_ + 502 + 87 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);*/
    sprintf(tmp, "%d", g_App.getGameSession().getMoney());
    g_App.fonts().drawText(560 - g_App.fonts().textWidth(tmp, FontManager::SIZE_2) / 2, 87,
                           tmp, FontManager::SIZE_2, false);

    // write cost for more info
/*    g_Screen.blit(538, 140, 100, 30,
                  orig_pixels_ + 538 + 140 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);*/

    if (info_level_ < pMission->getMaxInfoLvl()) {
        sprintf(tmp, "%d", pMission->infoCost(info_level_));
        g_App.fonts().drawText(560 - g_App.fonts().textWidth(tmp, FontManager::SIZE_2) / 2,
                               140, tmp, FontManager::SIZE_2, false);
    }

    // write cost for more enhance
/*    g_Screen.blit(538, 195, 100, 30,
                  orig_pixels_ + 538 + 195 * GAME_SCREEN_WIDTH, false,
                  GAME_SCREEN_WIDTH);*/

    if (enhance_level_ < pMission->getMaxEnhanceLvl()) {
        sprintf(tmp, "%d", pMission->enhanceCost(enhance_level_));
        g_App.fonts().drawText(560 - g_App.fonts().textWidth(tmp, FontManager::SIZE_2) / 2,
                               195, tmp, FontManager::SIZE_2, false);
    }

    g_System.showCursor();
}

void BriefMenu::handleLeave() {
    g_System.hideCursor();
}

void BriefMenu::handleOption(Key key, const int modKeys) {
    Mission *pMission = g_Session.getMission();
    if (key == KEY_F1) {
        // Buy some informations
        if (info_level_ < pMission->getMaxInfoLvl()) {
            g_Session.setMoney(g_Session.getMoney() - pMission->infoCost(info_level_));
            info_level_++;
            needRendering();
        }

        showOption(KEY_F6);
    }

    if (key == KEY_F2) {
        // Buy some map enhancement
        if (enhance_level_ < pMission->getMaxEnhanceLvl()) {
            g_Session.setMoney(g_Session.getMoney() -
                           pMission->enhanceCost(enhance_level_));
            enhance_level_++;
            needRendering();
        }
    }

    if (key == KEY_F6) {
        // Next page
        start_line_ += 14;
        showOption(KEY_F7);
        needRendering();
    }

    if (key == KEY_F7) {
        // Previous page
        start_line_ -= 14;

        if (start_line_ <= 0) {
            start_line_ = 0;
            hideOption(KEY_F7);
        }
        showOption(KEY_F6);
        needRendering();
    }
}
