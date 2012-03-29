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
#include "core/missionbriefing.h"

const int BriefMenu::MINIMAP_X = 504;
const int BriefMenu::MINIMAP_Y = 220;
const int BriefMenu::MINIMAP_WIDTH = 120;
const int BriefMenu::MINIMAP_HEIGHT = 120;

#if 0
#define EXECUTION_SPEED_TIME
#endif
BriefMenu::BriefMenu(MenuManager * m)
    : Menu(m, MENU_BRIEF, MENU_MAP, "mbrief.dat", "mbrieout.dat"),
        start_line_(0), mm_renderer_() {
    addStatic(85, 35, 545, "#BRIEF_TITLE", FontManager::SIZE_4, false);
    txtTimeId_ = addStatic(500, 9, "", FontManager::SIZE_2, true);       // Time

    // Briefing scroll button
    nextButId_ = addImageOption(461, 316, Sprite::MSPR_RIGHT_ARROW2_D, Sprite::MSPR_RIGHT_ARROW2_L);
    registerHotKey(KFC_RIGHT, nextButId_);
    prevButId_ = addImageOption(427, 316, Sprite::MSPR_LEFT_ARROW2_D, Sprite::MSPR_LEFT_ARROW2_L, false);
    registerHotKey(KFC_LEFT, prevButId_);

    // Accept button
    addOption(17, 347, 128, 25, "#MENU_ACC_BUT", FontManager::SIZE_2, MENU_SELECT);
    // Map button
    addOption(148, 347, 99, 25, "#BRIEF_MAP", FontManager::SIZE_2, MENU_MAP);
    // Main menu button
    addOption(500, 347,  128, 25, "#MENU_MAIN_BUT", FontManager::SIZE_2, MENU_MAIN);

    // Money
    txtMoneyId_ = addStatic(500, 87, 127, "0", FontManager::SIZE_2, true);     // textfield
    // Info
    infosButId_ = addOption(500, 118, 127, 10, "#BRIEF_INFO", FontManager::SIZE_2); // info button
    txtInfoId_ = addStatic(500, 140, 127, "0", FontManager::SIZE_2, true);
    // Enhancement
    enhButId_ = addOption(500, 169, 127, 10, "#BRIEF_ENH", FontManager::SIZE_2);
    txtEnhId_ = addStatic(500, 195, 127, "0", FontManager::SIZE_2, true);
}

BriefMenu::~BriefMenu() {
}

void BriefMenu::handleTick(int elapsed)
{
    if (g_Session.updateTime(elapsed)) {
        updateClock();
    }
    
    if (mm_renderer_.handleTick(elapsed)) {
        redrawMiniMap();
    }
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

	// Loads mission briefing
	p_briefing_ = g_App.missions().loadBriefing(cur_miss);
    
    start_line_ = 0;

    // reset minimap renderer with current mission
    uint8 enh_lvl = g_Session.getSelectedBlock().enhanceLevel;
    MinimapRenderer::EZoom zoom;
    if (enh_lvl == 0) {
        zoom = MinimapRenderer::ZOOM_X4;
    } else if (enh_lvl == 1) {
        zoom = MinimapRenderer::ZOOM_X3;
    } else if (enh_lvl == 2) {
        zoom = MinimapRenderer::ZOOM_X2;
    } else {
        zoom = MinimapRenderer::ZOOM_X1;
    }
    bool drawEnemies = 
        g_Session.getSelectedBlock().enhanceLevel == p_briefing_->nb_enhts();

    mm_renderer_.init(pMission, zoom, drawEnemies);

    updateClock();

    // Initialize the informations label with current cost
    if (g_Session.getSelectedBlock().infoLevel < p_briefing_->nb_infos()) {
        getStatic(txtInfoId_)->setTextFormated("%d",
            p_briefing_->infoCost(g_Session.getSelectedBlock().infoLevel));
        getOption(infosButId_)->setenabled(true);
    } else {
        getStatic(txtInfoId_)->setText("");
        getOption(infosButId_)->setenabled(false);
    }

    // Initialize the enhancements label with current cost
    if (g_Session.getSelectedBlock().enhanceLevel < p_briefing_->nb_enhts()) {
        getStatic(txtEnhId_)->setTextFormated("%d",
            p_briefing_->enhanceCost(g_Session.getSelectedBlock().enhanceLevel));
        getOption(enhButId_)->setenabled(true);
    } else {
        getStatic(txtEnhId_)->setText("");
        getOption(enhButId_)->setenabled(false);
    }

    g_System.showCursor();
}

/*!
 * Helpfull method to order minimap redraw by inserting a dirty rect.
 */
void BriefMenu::redrawMiniMap() {
    addDirtyRect(MINIMAP_X, MINIMAP_Y, MINIMAP_WIDTH, MINIMAP_HEIGHT);
}

void BriefMenu::handleRender(DirtyList &dirtyList) {

    g_Screen.drawLogo(18, 14, g_Session.getLogo(), g_Session.getLogoColour());

    // write briefing
#ifdef EXECUTION_SPEED_TIME
    printf("---------------------------");
    printf("start time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif
    if (p_briefing_) {
        int sizeStr = strlen(p_briefing_->briefing()) + 2;
        char *mbriefing = (char *)malloc(sizeStr);
        assert(mbriefing != NULL);
        strcpy(mbriefing, p_briefing_->briefing());
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
                } while (getMenuFont(FontManager::SIZE_2)->textWidth(tmp, true) > 470);

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

                    getMenuFont(FontManager::SIZE_2)->drawTextCp437(24,
                                           88 + (line_count - start_line_) * 16, 
										   miss, true);
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
    // 4 - 2px(1); x = 502(251), y = 218(109), 124x124(62x62)
    // enemy peds are at maximum enhance lvl
    // 640x400(320x200), (504, 220) = (252, 110)
    // g_Screen.scale2x(10, 100, pMission->mmax_x_, pMission->mmax_y_,
    //     pMission->minimap_overlay_,0, false);

    // draw minimap
    mm_renderer_.render();
}

void BriefMenu::handleLeave() {
    delete p_briefing_;
    g_System.hideCursor();
}

void BriefMenu::handleAction(const int actionId, void *ctx, const int modKeys) {
    if (actionId == infosButId_) {
        // Buy some informations
        if (g_Session.getSelectedBlock().infoLevel < p_briefing_->nb_infos()) {
            g_Session.setMoney(g_Session.getMoney() - p_briefing_->infoCost(g_Session.getSelectedBlock().infoLevel));
            g_Session.getSelectedBlock().infoLevel += 1;
            
            getStatic(txtMoneyId_)->setTextFormated("%d", g_Session.getMoney());
            if (g_Session.getSelectedBlock().infoLevel < p_briefing_->nb_infos()) {
                getStatic(txtInfoId_)->setTextFormated("%d",
                    p_briefing_->infoCost(g_Session.getSelectedBlock().infoLevel));
            } else
                getStatic(txtInfoId_)->setText("");
        }

        getOption(nextButId_)->setVisible(true);
    }

    if (actionId == enhButId_) {
        // Buy some map enhancement
        if (g_Session.getSelectedBlock().enhanceLevel < p_briefing_->nb_enhts()) {
            g_Session.setMoney(g_Session.getMoney() - 
                p_briefing_->enhanceCost(g_Session.getSelectedBlock().enhanceLevel));
            g_Session.getSelectedBlock().enhanceLevel += 1;
            // zoom in the minimap
            mm_renderer_.zoomOut();
            if (g_Session.getSelectedBlock().enhanceLevel == p_briefing_->nb_enhts()) {
                mm_renderer_.setDrawEnemies(true);
                getOption(enhButId_)->setenabled(false);
            }
            redrawMiniMap();
            
            getStatic(txtMoneyId_)->setTextFormated("%d", g_Session.getMoney());
            if (g_Session.getSelectedBlock().enhanceLevel < p_briefing_->nb_enhts()) {
                getStatic(txtEnhId_)->setTextFormated("%d",
                    p_briefing_->enhanceCost(g_Session.getSelectedBlock().enhanceLevel));
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

/*!
 * Handles the mouse click.
 * If the user clicks on the minimap with the left button, the minimap scrolls
 * in the corresponding direction.
 * \return True if the user clicked on the minimap so event is consumed
 */
bool BriefMenu::handleMouseDown(int x, int y, int button, const int modKeys) {
    if (button == 1 && x >= MINIMAP_X && x < (MINIMAP_X + MINIMAP_WIDTH)
        && y >= MINIMAP_Y && y < (MINIMAP_Y + MINIMAP_HEIGHT)) {
        if (x >= MINIMAP_X && x < 544) {
            mm_renderer_.scrollLeft();
        } else if (x >= 584 && x < 624) {
             mm_renderer_.scrollRight();
        }

        if (y >= MINIMAP_Y && y < 260) {
             mm_renderer_.scrollUp();
        } else if (y >= 300 && y < 340) {
             mm_renderer_.scrollDown();
        }
        // Redraw map
        needRendering();
        return true;
    }

    return false;
}
