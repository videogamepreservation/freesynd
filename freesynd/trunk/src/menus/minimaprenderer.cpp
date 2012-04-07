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

void MinimapRenderer::init(Mission *pMission, EZoom zoom, bool draw_enemies)
{
    // Initialize minimap origin by looking for the position
    // of the first found agent on the map
    p_mission_ = pMission;
    zoom_ = zoom;
    scroll_step_ = 30 / (10 - zoom_);
    b_draw_enemies_ = draw_enemies;
    minimap_blink_ticks_ = 0;
    minimap_blink_ = 0;

    bool found = false;
    int maxx = pMission->mmax_x_;
    int maxy = pMission->mmax_y_;

    for (int x = 0; x < maxx && (!found); x++) {
        for (int y = 0; y < maxy && (!found); y++) {
            if (pMission->getMinimapOverlay(x, y) == 1) {
                // We found a tile with an agent on it
                // stop searching and memorize position
                minimap_scroll_x_ = x;
                minimap_scroll_y_ = y;
                found = true;
            }
        }
    }
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

    // scrolling step depends on the level of details of the minimap
    scroll_step_ = 30 / (10 - (zoom_));
}

bool MinimapRenderer::handleTick(int elapsed)
{
    minimap_blink_ticks_ += elapsed;

    if (minimap_blink_ticks_ > 500) {
        minimap_blink_ticks_ = 0;
        minimap_blink_ ^= 1;
        return true;
    }

    return false;
}

void MinimapRenderer::scrollRight() {
    minimap_scroll_x_ += scroll_step_;
    if (minimap_scroll_x_ >= p_mission_->mmax_x_)
        minimap_scroll_x_ = p_mission_->mmax_x_ - 1;
}

void MinimapRenderer::scrollLeft() {
    minimap_scroll_x_ -= scroll_step_;
    if (minimap_scroll_x_ < 0)
        minimap_scroll_x_ = 0;
}

void MinimapRenderer::scrollUp() {
    minimap_scroll_y_ -= scroll_step_;
    if (minimap_scroll_y_ < 0)
        minimap_scroll_y_ = 0;
}

void MinimapRenderer::scrollDown() {
    minimap_scroll_y_ += scroll_step_;
    if (minimap_scroll_y_ >= p_mission_->mmax_y_) {
        minimap_scroll_y_ = p_mission_->mmax_y_ - 1;
    }
}

void MinimapRenderer::render() 
{
    int maxx = p_mission_->mmax_x_;
    int maxy = p_mission_->mmax_y_;

    unsigned char pixperblock = 10 - zoom_;
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

    for (short x = bxl; x <= bxr; x++) {
        short xc = sx + (x - bxl) * pixperblock;
        for (short y = byl; y <= byr; y++) {
            unsigned char c = p_mission_->getMinimapOverlay(x, y);
            switch (c) {
                case 0:
                    c = p_mission_->getMiniMap()->getColourAt(x, y);
                    break;
                case 1:
                    c = minimap_blink_ ? 14 : 12;
                    break;
                case 2:
                    if (b_draw_enemies_)
                        c = minimap_blink_ ? 14 : 5;
                    else
                        c = p_mission_->getMiniMap()->getColourAt(x, y);
            }
            g_Screen.drawRect(xc, sy + (y - byl) * pixperblock, pixperblock,
                pixperblock, c);
        }
    }
}
