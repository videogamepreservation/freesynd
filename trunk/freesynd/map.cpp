/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
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
#include <string.h>
#include <assert.h>

#include "screen.h"
#include "map.h"
#include "tilemanager.h"

Map::Map(TileManager * tileManager):map_data_(NULL),
tile_manager_(tileManager)
{
}

Map::~Map()
{
    delete[] map_data_;
}

bool Map::loadMap(uint8 * mapData)
{

    max_x_ = READ_LE_UINT32(mapData + 0);
    max_y_ = READ_LE_UINT32(mapData + 4);
    max_z_ = READ_LE_UINT32(mapData + 8);

    if (map_data_)
        delete[] map_data_;
    map_data_ = new uint8[max_x_ * max_y_ * max_z_];
    uint32 *lookup = new uint32[max_x_ * max_y_];

    for (int i = 0; i < max_x_ * max_y_; i++)
        lookup[i] = READ_LE_UINT32(mapData + 12 + i * 4);
    for (int h = 0; h < max_y_; h++)
        for (int w = 0; w < max_x_; w++) {
            int idx = h * max_x_ + w;
            memcpy(map_data_ + idx * max_z_, mapData + 12 + lookup[idx],
                   max_z_);
        }
    delete[] lookup;

    map_width_ = max_x_ * TILE_WIDTH;
    map_height_ =
        max_x_ * TILE_HEIGHT / 3 + max_y_ * TILE_HEIGHT / 3 +
        max_z_ * TILE_HEIGHT / 3;

    return true;
}

// TODO: these are hacks to get the units positioned correctly on the map
// I have no idea why they work and they're probably incorrect for other values of z
//int mx = 67, my = 14, mz = 31;
float scalexPx = 256.0f;
float scalexPy = 256.0f;
float scaleyPx = 256.0f;
float scaleyPy = 256.0f;

int Map::tileToScreenX(int x, int y, int z, int pX, int pY)
{
    float fx = x + pX / scalexPx;
    float fy = y + pY / scalexPy;
    return (int) (map_width_ / 2 + fx * TILE_WIDTH / 2 -
                  fy * TILE_WIDTH / 2) + TILE_WIDTH / 2;
}

int Map::tileToScreenY(int x, int y, int z, int pX, int pY)
{
    float fx = x + pX / scaleyPx;
    float fy = y + pY / scaleyPy;
    return (int) (max_z_ * TILE_HEIGHT / 3 + (fx + fy) * TILE_HEIGHT / 3);
}

int Map::screenToTileX(int x, int y, int &ox)
{
    x -= map_width_ / 2;
    // x now equals fx * TILE_WIDTH / 2 - fy * TILE_WIDTH / 2
    // which equals TILE_WIDTH/2 * (fx - fy)
    y -= max_z_ * TILE_HEIGHT / 3;
//    y += mz;
    // y now equals (fx + fy) * TILE_HEIGHT / 3
    float dx = (float) x / (TILE_WIDTH / 2);
    float dy = (float) y / (TILE_HEIGHT / 3);
    // dx equals fx - fy
    // dy equals fx + fy
    float r = (dx + dy) / 2;
    int ri = (int) r;
    ox = (int) ((r - ri) * 256);
    return ri;
}

int Map::screenToTileY(int x, int y, int &oy)
{
    x -= map_width_ / 2;
    y -= max_z_ * TILE_HEIGHT / 3;
//    y += mz;
    float dx = (float) x / (TILE_WIDTH / 2);
    float dy = (float) y / (TILE_HEIGHT / 3);
    float r = (dy - dx) / 2;
    int ri = (int) r;
    oy = (int) ((r - ri) * 256);
    return ri;
}

int Map::maxZAt(int x, int y)
{
    assert(x < max_x_);
    assert(y < max_y_);
    int idx = y * max_x_ + x;
    int mz = 0;
    for (int z = 0; z < max_z_; z++) {
        int tile = map_data_[idx * max_z_ + z];
        if (tile > 5) {
            mz = z;
        }
    }
    return mz;
}

int Map::tileAt(int x, int y, int z)
{
    if (x < 0 || x >= max_x_)
        return z < 2 ? 6 : 0;
    if (y < 0 || y >= max_y_)
        return z < 2 ? 6 : 0;
    if (z < 0 || z >= max_z_)
        return 0;
    return map_data_[(y * max_x_ + x) * max_z_ + z];
}

bool Map::stairsAt(int x, int y, int z)
{
    return tileAt(x, y, z) == 178 || tileAt(x, y, z) == 179;
}


int topz = -1;

void Map::draw(int scrollX, int scrollY, MapHelper * helper)
{
    if (scrollX + (g_Screen.gameScreenWidth() - g_Screen.gameScreenLeftMargin()) >= map_width_)
        scrollX = map_width_ - (g_Screen.gameScreenWidth() - g_Screen.gameScreenLeftMargin());
    if (scrollY + g_Screen.gameScreenHeight() >= map_height_)
        scrollY = map_height_ - g_Screen.gameScreenHeight();

    uint8 buf[TILE_WIDTH * TILE_HEIGHT];

    int ox, oy;
    int sw = screenToTileX(scrollX, scrollY, ox);
    int swm = sw + g_Screen.gameScreenWidth() * 30 / GAME_SCREEN_WIDTH;
    if (swm > max_x_)
        swm = max_x_;
    int sh = screenToTileY(scrollX, scrollY, oy) - 8;
    if (sh < 0)
        sh = 0;
    int shm = sh + g_Screen.gameScreenHeight() * 30 / GAME_SCREEN_HEIGHT;
    if (shm > max_y_)
        shm = max_y_;

    if (topz == -1)
        topz = max_z_;

#if 0 
    // HACK for map dump
    sw = 0; swm = max_x_;
    sh = 0; shm = max_y_;
#endif

#if 1
    // TODO: this is messy, and it doesn't draw sprites in the right order (top to bottom)
    for (int pass = 0; pass < 2; pass++)
        for (int h = sh; h < shm; h++)
            for (int w = sw; w < swm; w++) {
                int idx = h * max_x_ + w;
                int screen_w =
                    map_width_ / 2 + w * TILE_WIDTH / 2 -
                    h * TILE_WIDTH / 2;
                int screen_h =
                    max_z_ * TILE_HEIGHT / 3 + (w + h) * TILE_HEIGHT / 3;
                for (int z = 0;
                     pass == 0 ? z < 2 : (z < max_z_ && z < topz); z++) {
                    int tile = map_data_[idx * max_z_ + z];
                    int coord_h = screen_h - ((z - 1) * TILE_HEIGHT / 3);
                    if (screen_w >= scrollX - 64
                        && screen_w + TILE_WIDTH <
                        scrollX + (g_Screen.gameScreenWidth() - g_Screen.gameScreenLeftMargin() + 128)
                        && coord_h >= scrollY - 64
                        && coord_h + TILE_HEIGHT <
                        scrollY + g_Screen.gameScreenHeight() + 128) {
                        bool helpOnly = (pass == 1 && z <= 1);
                        if (helpOnly
                            || tile_manager_->drawTileTo(buf, TILE_WIDTH,
                                                         TILE_HEIGHT, tile,
                                                         0, 0, true)) {
                            int dx = 0, dy = 0;
                            if (screen_w - scrollX < 0)
                                dx = -(screen_w - scrollX);
                            if (coord_h - scrollY < 0)
                                dy = -(coord_h - scrollY);
                            if (dx < TILE_WIDTH && dy < TILE_HEIGHT
                                && !helpOnly) {
                                g_Screen.blit(screen_w - scrollX + g_Screen.gameScreenLeftMargin() +
                                              dx, coord_h - scrollY + dy,
                                              TILE_WIDTH - dx,
                                              TILE_HEIGHT - dy,
                                              buf + dx + dy * TILE_WIDTH,
                                              false, TILE_WIDTH);
                            }
                            if (helper)
                                helper->drawAt(w, h, z,
                                               screen_w - scrollX + g_Screen.gameScreenLeftMargin() +
                                               TILE_WIDTH / 2,
                                               coord_h - scrollY, scrollX,
                                               scrollY);
                        }
                    }
                }
            }
#endif
}
