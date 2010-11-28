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
    std::string str;
    menu_manager_->getMessage("MAP_BRIEF_BUT", str);
    
    addStatic(148, 35, "#BRIEF_TITLE", 3, true);
    addStatic(500, 9, "", 1, false);       // Time
    menu_manager_->getMessage("BRIEF_INFO", str);
    int butX = 560 - g_App.fonts().textWidth(str.c_str(), FontManager::SIZE_2) / 2;
    addOption(butX, 118, str.c_str(), 1, KEY_F1, NULL);
    menu_manager_->getMessage("BRIEF_ENH", str);
    butX = 560 - g_App.fonts().textWidth(str.c_str(), FontManager::SIZE_2) / 2;
    addOption(butX, 169, str.c_str(), 1, KEY_F2, NULL);
    addOption(461, 320, "", 1, KEY_F6, NULL, true, Sprite::MSPR_RIGHT_ARROW2_D, Sprite::MSPR_RIGHT_ARROW2_L);
    addOption(427, 320, "", 1, KEY_F7, NULL, false, Sprite::MSPR_LEFT_ARROW2_D, Sprite::MSPR_LEFT_ARROW2_L);
    menu_manager_->getMessage("MENU_ACC_BUT", str);
    butX = 80 - g_App.fonts().textWidth(str.c_str(), FontManager::SIZE_2) / 2;
    addOption(butX, 352, str.c_str(), 1, KEY_F3, "select");
    menu_manager_->getMessage("BRIEF_MAP", str);
    butX = 195 - g_App.fonts().textWidth(str.c_str(), FontManager::SIZE_2) / 2;
    addOption(butX, 352, str.c_str(), 1, KEY_F4, "map");
    addOption(535, 352, "#MENU_MAIN_BUT", 1, KEY_F5, "main");

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
    pMission->createMinimap();

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
    // 4 - 2px(1), 5 - enemy peds; x = 502(251), y = 218(109), 124x124(62x62)
    // 640x400(320x200), (504, 220) = (252, 110)
    // g_Screen.drawRect(504, 220, 120, 120);

    minimap_scroll_x_ = 40;
    minimap_scroll_y_ = 40;
    int maxx = g_App.maps().map(pMission->map())->maxX();
    int maxy = g_App.maps().map(pMission->map())->maxY();
    unsigned char clvl = enhance_level_;
    bool addenemies = false;
    if (clvl > 4) {
        clvl = 4;
        addenemies = true;
    }
    unsigned char pixperblock = 10 - (clvl << 1);
    short fullblocks = 120 / pixperblock;
    short halfblocks = fullblocks / 2;
    short modblocks = fullblocks % 2;
    short bxl = minimap_scroll_x_ - halfblocks + 1;
    short bxr = minimap_scroll_x_ + halfblocks + modblocks;
    short byl = minimap_scroll_y_ - halfblocks + 1;
    short byr = minimap_scroll_y_ + halfblocks + modblocks;
    // checking borders for correctness, map will be always on center
    if (bxl < 0) {
        bxl = 0;
        if (bxr >= maxx) {
            bxr = maxx - 1;
        } else {
            bxr = fullblocks >= maxx ? maxx - 1 : (fullblocks) - 1;
        }
    }
    if (bxr >= maxx) {
        bxr = maxx - 1;
        bxl = (maxx - (fullblocks)) < 0 ? 0 : (maxx - (fullblocks));
    }

    if (byl < 0) {
        byl = 0;
        if (byr >= maxy) {
            byr = maxy - 1;
        } else {
            byr = fullblocks >= maxy ? maxy - 1 : (fullblocks) - 1;
        }
    }
    if (byr >= maxy) {
        byr = maxy - 1;
        byl = (maxy - (fullblocks)) < 0 ? 0 : (maxy - (fullblocks));
    }

    short sx = 504;
    short sy = 220;
    if ((bxr - bxl + 1) < (fullblocks)) {
        sx += ((fullblocks - (bxr - bxl + 1)) >> 1) * pixperblock;
    }
    if ((byr - byl + 1) < (fullblocks)) {
        sy += ((fullblocks - (byr - byl + 1)) >> 1) * pixperblock;
    }

    if (addenemies) {
        for (short x = bxl; x <= bxr; x++) {
            short xc = sx + (x - bxl) * pixperblock;
            for (short y = byl; y <= byr; y++) {
                g_Screen.drawRect(xc, sy + (y - byl) * pixperblock, pixperblock,
                    pixperblock, pMission->getMinimapColour(x, y));
            }
        }
    } else {
        for (short x = bxl; x <= bxr; x++) {
            short xc = sx + (x - bxl) * pixperblock;
            for (short y = byl; y <= byr; y++) {
                g_Screen.drawRect(xc, sy + (y - byl) * pixperblock, pixperblock,
                    pixperblock, pMission->getMinimapColour(x, y));
            }
        }
    }


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

void BriefMenu::resetMinimapBaseValues() {
}
