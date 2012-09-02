/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2012  Benoit Blancard <benblan@users.sourceforge.net>*
 *                                                                      *
 *    This program is free software;  you can redistribute it and / or  *
 *  modify it  under the  terms of the  GNU General  Public License as  *
 *  published by the Free Software Foundation; either version 2 of the  *
 *  License, or (at your option) any later version.                     *
 *                                                                      *
 *    This program is  distributed in the hope that it will be useful,  *
 *  but WITHOUT  ANY WARRANTY;without even  the impliedwarranty of      *
 *  MERCHANTABILITY  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *
 *  General Public License for more details.                            *
 *                                                                      *
 *    You can view the GNU  General Public License, online, at the GNU  *
 *  project'sweb  site;  see <http://www.gnu.org/licenses/gpl.html>.  *
 *  The full text of the license is also included in the file COPYING.  *
 *                                                                      *
 ************************************************************************/

#include "menus/minimaprenderer.h"
#include "mission.h"
#include "gfx/screen.h"
#include "app.h"

MinimapRenderer::MinimapRenderer() {
    scroll_step_ = 0;
}

/*!
 * Init the renderer with a new mission, zoom level and draw_enemies params.
 */
void MinimapRenderer::init(Mission *pMission, EZoom zoom, bool draw_enemies) {
    p_mission_ = pMission;
    zoom_ = zoom;
    b_draw_enemies_ = draw_enemies;
    minimap_blink_ticks_ = 0;
    minimap_blink_ = 0;

    updateRenderingInfos();

    // Initialize minimap origin by looking for the position
    // of the first found agent on the map
    initMinimapLocation();
}

/*!
 * Centers the minimap on the starting position of agents
 */
void MinimapRenderer::initMinimapLocation() {
    bool found = false;
    int maxx = p_mission_->mmax_x_;
    int maxy = p_mission_->mmax_y_;

    for (int x = 0; x < maxx && (!found); x++) {
        for (int y = 0; y < maxy && (!found); y++) {
            if (p_mission_->getMinimapOverlay(x, y) == 1) {
                // We found a tile with an agent on it
                // stop searching and memorize position
                mm_tx_ = x;
                mm_ty_ = y;
                found = true;
            }
        }
    }

    uint16 halftiles = mm_maxtile_ / 2;
    mm_tx_ = mm_tx_ - halftiles + 1;
    mm_ty_ = mm_ty_ - halftiles + 1;

    checkBorders();
}

/*!
 * Checking borders for correctness
 * we assume that maps are always bigger than minimap.
 */
void MinimapRenderer::checkBorders() {
    int maxx = p_mission_->mmax_x_;
    int maxy = p_mission_->mmax_y_;

    if (mm_tx_ < 0) {
        mm_tx_ = 0;
    } else if ((mm_tx_ + mm_maxtile_) >= maxx) {
        mm_tx_ = maxx - mm_maxtile_;
    }

    if (mm_ty_ < 0) {
        mm_ty_ = 0;
    } else if ((mm_ty_ + mm_maxtile_) >= maxy) {
        mm_tx_ = maxy - mm_maxtile_;
    }

    assert(mm_tx_ >= 0);
    assert(mm_ty_ >= 0);
    assert((mm_tx_ + mm_maxtile_) <= maxx);
    assert((mm_ty_ + mm_maxtile_) <= maxy);
}

void MinimapRenderer::updateRenderingInfos() {
    pixpertile_ = 10 - zoom_;
    scroll_step_ = 30 / pixpertile_;
    mm_maxtile_ = 120 / pixpertile_;
}

void MinimapRenderer::zoomOut() {
    switch (zoom_) {
    case ZOOM_X2:
        zoom_ = ZOOM_X1;
        break;
    case ZOOM_X3:
        zoom_ = ZOOM_X2;
        break;
    case ZOOM_X4:
        zoom_ = ZOOM_X3;
        break;
    default:
        break;
    }

    // update scrolling data
    updateRenderingInfos();
    checkBorders();
}

bool MinimapRenderer::handleTick(int elapsed) {
    minimap_blink_ticks_ += elapsed;

    if (minimap_blink_ticks_ > 500) {
        minimap_blink_ticks_ = 0;
        minimap_blink_ ^= 1;
        return true;
    }

    return false;
}

void MinimapRenderer::scrollRight() {
    mm_tx_ += scroll_step_;
    if ((mm_tx_ + mm_maxtile_) >= p_mission_->mmax_x_)
        mm_tx_ = p_mission_->mmax_x_ - mm_maxtile_;
}

void MinimapRenderer::scrollLeft() {
    mm_tx_ -= scroll_step_;
    if (mm_tx_ < 0)
        mm_tx_ = 0;
}

void MinimapRenderer::scrollUp() {
    mm_ty_ -= scroll_step_;
    if (mm_ty_ < 0)
        mm_ty_ = 0;
}

void MinimapRenderer::scrollDown() {
    mm_ty_ += scroll_step_;
    if ((mm_ty_ + mm_maxtile_) >= p_mission_->mmax_y_) {
        mm_ty_ = p_mission_->mmax_y_ - mm_maxtile_;
    }
}

void MinimapRenderer::render() {
    uint16 sx = 504;
    uint16 sy = 220;

    for (uint16 tx = mm_tx_; tx < (mm_tx_ + mm_maxtile_); tx++) {
        uint16 xc = sx + (tx - mm_tx_) * pixpertile_;
        for (uint16 ty = mm_ty_; ty < (mm_ty_ + mm_maxtile_); ty++) {
            unsigned char c = p_mission_->getMinimapOverlay(tx, ty);
            switch (c) {
                case 0:
                    c = p_mission_->getMiniMap()->getColourAt(tx, ty);
                    break;
                case 1:
                    c = minimap_blink_ ? 14 : 12;
                    break;
                case 2:
                    if (b_draw_enemies_)
                        c = minimap_blink_ ? 14 : 5;
                    else
                        c = p_mission_->getMiniMap()->getColourAt(tx, ty);
            }
            g_Screen.drawRect(xc, sy + (ty - mm_ty_) * pixpertile_, pixpertile_,
                pixpertile_, c);
        }
    }
}
