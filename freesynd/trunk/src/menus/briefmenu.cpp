/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2010  Benoit Blancard <benblan@users.sourceforge.net>*
 *   Copyright (C) 2010  Bohdan Stelmakh <chamel@users.sourceforge.net> *
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

#if 0
#ifdef SYSTEM_SDL
#include "system_sdl.h"
#endif
#define EXECUTION_SPEED_TIME
#endif
BriefMenu::BriefMenu(MenuManager * m) :
Menu(m, MENU_BRIEF, MENU_MAP, "mbrief.dat", "mbrieout.dat"),
start_line_(0) {
    
    addStatic(85, 35, 545, "#BRIEF_TITLE", FontManager::SIZE_4, true);
    txtTimeId_ = addStatic(500, 9, "", FontManager::SIZE_2, false);       // Time

    // Briefing scroll button
    nextButId_ = addImageOption(461, 316, Sprite::MSPR_RIGHT_ARROW2_D, Sprite::MSPR_RIGHT_ARROW2_L);
	registerHotKey(KEY_RIGHT, nextButId_);
    prevButId_ = addImageOption(427, 316, Sprite::MSPR_LEFT_ARROW2_D, Sprite::MSPR_LEFT_ARROW2_L, false);
	registerHotKey(KEY_LEFT, prevButId_);

    // Accept button
    addOption(17, 347, 128, 25, "#MENU_ACC_BUT", FontManager::SIZE_2, MENU_SELECT);
    // Map button
	addOption(148, 347, 99, 25, "#BRIEF_MAP", FontManager::SIZE_2, MENU_MAP);
    // Main menu button
    addOption(500, 347,  128, 25, "#MENU_MAIN_BUT", FontManager::SIZE_2, MENU_MAIN);

    // Money
    txtMoneyId_ = addStatic(500, 87, 127, "0", FontManager::SIZE_2, false);     // textfield
    // Info
    infosButId_ = addOption(500, 118, 127, 10, "#BRIEF_INFO", FontManager::SIZE_2); // info button
    txtInfoId_ = addStatic(500, 140, 127, "0", FontManager::SIZE_2, false);
    // Enhancement
    enhButId_ = addOption(500, 169, 127, 10, "#BRIEF_ENH", FontManager::SIZE_2);
    txtEnhId_ = addStatic(500, 195, 127, "0", FontManager::SIZE_2, false);
}

BriefMenu::~BriefMenu() {
}

void BriefMenu::handleTick(int elapsed)
{
    if (g_Session.updateTime(elapsed)) {
        updateClock();
    }
    drawMinimap(elapsed);
}

/*! 
 * Update the game time display
 */
void BriefMenu::updateClock() {
    char tmp[100];

    g_Session.getTimeAsStr(tmp);
    getStatic(txtTimeId_)->setText(tmp);

    // update money
    getStatic(txtMoneyId_)->setTextFormated("%d", g_Session.getMoney());
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
    
    bool found = false;
    int maxx = pMission->mmax_x_;
    int maxy = pMission->mmax_y_;

    for (int x = 0; x < maxx && (!found); x++) {
        for (int y = 0; y < maxy && (!found); y++) {
            if (pMission->getMinimapOverlay(x, y) == 1) {
                minimap_scroll_x_ = x;
                minimap_scroll_y_ = y;
                found = true;
            }
        }
    }

    minimap_blink_ticks_ = 0;
    minimap_blink_ = 0;
    start_line_ = 0;

    updateClock();

    if (g_Session.getSelectedBlock().infoLevel < pMission->getMaxInfoLvl()) {
        getStatic(txtInfoId_)->setTextFormated("%d",
            pMission->infoCost(g_Session.getSelectedBlock().infoLevel));
    } else
        getStatic(txtInfoId_)->setText("");
    if (g_Session.getSelectedBlock().enhanceLevel < pMission->getMaxEnhanceLvl()) {
        getStatic(txtEnhId_)->setTextFormated("%d",
            pMission->enhanceCost(g_Session.getSelectedBlock().enhanceLevel));
    } else
        getStatic(txtEnhId_)->setText("");

    g_System.showCursor();
}

void BriefMenu::handleRender() {

    g_Screen.drawLogo(18, 14, g_Session.getLogo(), g_Session.getLogoColour());

    Mission *pMission = g_Session.getMission();

    // write briefing
#ifdef EXECUTION_SPEED_TIME
    printf("---------------------------");
    printf("start time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif
    if (pMission->briefing()) {
        int sizeStr = strlen(pMission->briefing()) + 2;
        char *mbriefing = (char *)malloc(sizeStr);
        assert(mbriefing != NULL);
        strcpy(mbriefing, pMission->briefing());
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
                } while (g_App.fonts().textWidth(tmp, true, FontManager::SIZE_2) > 470);

                delete[] tmp;
                /* is this faster?
                char tmp;
                bool found;

                do {
                    tmp = *nextline;
                    *nextline = 0;
                    found = g_App.fonts().textWidth(miss, true, FontManager::SIZE_2) > 470;
                    *nextline = tmp;
                    nextline--;
                } while (found);
                */

                nextline++;

                while (nextline[0] != '\x0a' && nextline[0] != ' '
                       && nextline[0] != 0) {
                    nextline--;
                }
            }

            if (*miss == '|') {
                lvls++;

                if (lvls > g_Session.getSelectedBlock().infoLevel) {
                    getOption(nextButId_)->setVisible(false);
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
                                                 start_line_) * 16, miss, true,
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
                getOption(nextButId_)->setVisible(false);
        } while (miss && *miss && line_count < start_line_ + 14);

        free(mbriefing);        // using free because allocated this
    }
#ifdef EXECUTION_SPEED_TIME
    printf("+++++++++++++++++++++++++++");
    printf("end time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif
    // NOTE: enhance levels: 0 = 10px(5), 1 = 8px(4), 2 = 6px(3), 3 - 4px(2),
    // 4 - 2px(1) + enemy peds; x = 502(251), y = 218(109), 124x124(62x62)
    // 640x400(320x200), (504, 220) = (252, 110)
    // g_Screen.scale2x(10, 100, pMission->mmax_x_, pMission->mmax_y_,
    //     pMission->minimap_overlay_,0, false);

    drawMinimap(0);
}

void BriefMenu::handleLeave() {
    g_System.hideCursor();
}

void BriefMenu::handleAction(const int actionId, void *ctx, const int modKeys) {
    Mission *pMission = g_Session.getMission();
    if (actionId == infosButId_) {
        // Buy some informations
        if (g_Session.getSelectedBlock().infoLevel < pMission->getMaxInfoLvl()) {
            g_Session.setMoney(g_Session.getMoney() - pMission->infoCost(g_Session.getSelectedBlock().infoLevel));
            g_Session.getSelectedBlock().infoLevel += 1;
            
            getStatic(txtMoneyId_)->setTextFormated("%d", g_Session.getMoney());
            if (g_Session.getSelectedBlock().infoLevel < pMission->getMaxInfoLvl()) {
                getStatic(txtInfoId_)->setTextFormated("%d",
                    pMission->infoCost(g_Session.getSelectedBlock().infoLevel));
            } else
                getStatic(txtInfoId_)->setText("");
        }

        getOption(nextButId_)->setVisible(true);
    }

    if (actionId == enhButId_) {
        // Buy some map enhancement
        if (g_Session.getSelectedBlock().enhanceLevel < pMission->getMaxEnhanceLvl()) {
            g_Session.setMoney(g_Session.getMoney() - 
                pMission->enhanceCost(g_Session.getSelectedBlock().enhanceLevel));
            g_Session.getSelectedBlock().enhanceLevel += 1;
            
            getStatic(txtMoneyId_)->setTextFormated("%d", g_Session.getMoney());
            if (g_Session.getSelectedBlock().enhanceLevel < pMission->getMaxEnhanceLvl()) {
                getStatic(txtEnhId_)->setTextFormated("%d",
                    pMission->enhanceCost(g_Session.getSelectedBlock().enhanceLevel));
            } else
                getStatic(txtEnhId_)->setText("");
        }
    }

    if (actionId == nextButId_) {
        // Next page
        start_line_ += 14;
        getOption(prevButId_)->setVisible(true);
        needRendering();
    }

    if (actionId == prevButId_) {
        // Previous page
        start_line_ -= 14;

        if (start_line_ <= 0) {
            start_line_ = 0;
            getOption(prevButId_)->setVisible(false);
        }
        getOption(nextButId_)->setVisible(true);
        needRendering();
    }
}

bool BriefMenu::handleMouseDown(int x, int y, int button, const int modKeys) {

    unsigned char scroll_step = 30 / (10 - (g_Session.getSelectedBlock().enhanceLevel << 1));

    Mission *pMission = g_Session.getMission();
    if (button == 1 && x >= 504 && x < 624
        && y >= 220 && y < 340) {
        if (x >= 504 && x < 544) {
            minimap_scroll_x_ -= scroll_step;
            if (minimap_scroll_x_ < 0)
                minimap_scroll_x_ = 0;
        } else if (x >= 584 && x < 624) {
            minimap_scroll_x_ += scroll_step;
            if (minimap_scroll_x_ > pMission->mmax_x_)
                minimap_scroll_x_ = pMission->mmax_x_ - 1;
        }
        if (y >= 220 && y < 260) {
            minimap_scroll_y_ -= scroll_step;
            if (minimap_scroll_y_ < 0)
                minimap_scroll_y_ = 0;
        } else if (y >= 300 && y < 340) {
            minimap_scroll_y_ += scroll_step;
            if (minimap_scroll_y_ > pMission->mmax_y_)
                minimap_scroll_y_ = pMission->mmax_y_ - 1;
        }
        needRendering();
		return true;
    }

	return false;
}

void BriefMenu::drawMinimap(int elapsed) {

    Mission *pMission = g_Session.getMission();
    int maxx = pMission->mmax_x_;
    int maxy = pMission->mmax_y_;
    //printf("x %i, y %i\n", maxx, maxy);
    unsigned char clvl = g_Session.getSelectedBlock().enhanceLevel;
    bool addenemies = false;

    if (clvl == 4) {
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

    if (elapsed == 0) {
        for (short x = bxl; x <= bxr; x++) {
            short xc = sx + (x - bxl) * pixperblock;
            for (short y = byl; y <= byr; y++) {
                unsigned char c = pMission->getMinimapOverlay(x, y);
                switch (c) {
                    case 0:
                        c = pMission->getMinimapColour(x, y);
                        break;
                    case 1:
                        c = 14;
                        break;
                    case 2:
                        if (addenemies)
                            c = 14;
                        else
                            c = pMission->getMinimapColour(x, y);
                }
                g_Screen.drawRect(xc, sy + (y - byl) * pixperblock, pixperblock,
                    pixperblock, c);
            }
        }
    } else {
        // drawing blinking only
        elapsed += minimap_blink_ticks_;
        int inc = elapsed / 50;
        minimap_blink_ticks_ = elapsed % 50;
        minimap_blink_ += inc;
        unsigned char cour = 14;
        unsigned char cenemy = 14;
        minimap_blink_ %= 20;
        if ((minimap_blink_ % 10)  > 6) {
            cour = 12;
            cenemy = 5;
        }
        for (short x = bxl; x <= bxr; x++) {
            short xc = sx + (x - bxl) * pixperblock;
            for (short y = byl; y <= byr; y++) {
                unsigned char c = pMission->getMinimapOverlay(x, y);
                switch (c) {
                    case 0:
                        break;
                    case 1:
                        g_Screen.drawRect(xc, sy + (y - byl) * pixperblock, pixperblock,
                            pixperblock, cour);
                        break;
                    case 2:
                        if (addenemies)
                            g_Screen.drawRect(xc, sy + (y - byl) * pixperblock, pixperblock,
                                pixperblock, cenemy);
                }
            }
        }
    }
}
