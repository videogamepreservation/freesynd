/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
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
#include <string.h>
#include <assert.h>

#include "app.h"
#include "gfx/screen.h"
#include "map.h"
#include "gfx/tilemanager.h"

#if 0
#define EXECUTION_SPEED_TIME
#endif

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
    map_height_ = (max_x_ + max_y_ + max_z_) * TILE_HEIGHT / 3;

    return true;
}

float scalexPx = 256.0f;
float scalexPy = 256.0f;
float scaleyPx = 256.0f;
float scaleyPy = 256.0f;

int Map::tileToScreenX(int x, int y, int z, int pX, int pY)
{
    float fx = x + pX / scalexPx;
    float fy = y + pY / scalexPy;
    return (int) (map_width_ / 2 + (fx - fy) * TILE_WIDTH / 2
                  + TILE_WIDTH / 2);
}

int Map::tileToScreenY(int x, int y, int z, int pX, int pY)
{
    float fx = x + pX / scaleyPx;
    float fy = y + pY / scaleyPy;
    return (int) (max_z_ * TILE_HEIGHT / 3 + (fx + fy) * TILE_HEIGHT / 3);
}

int Map::screenToTileX(int x, int y, int &ox)
{
    x -= (map_width_ / 2 + TILE_WIDTH / 2);
    // x now equals fx * TILE_WIDTH / 2 - fy * TILE_WIDTH / 2
    // which equals TILE_WIDTH/2 * (fx - fy)
    y -= (max_z_ + 1) * TILE_HEIGHT / 3;
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
    x -= (map_width_ / 2 + TILE_WIDTH / 2);
    y -= (max_z_ + 1) * TILE_HEIGHT / 3;
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
    /*
    int idx = y * max_x_ + x;
    int mz = 0;
    for (int z = 0; z < max_z_; z++) {
        int tile = map_data_[idx * max_z_ + z];
        if (tile > 5) {
            mz = z;
        }
    }
    */
    // TODO: disabling this thing, causes a lot of speed drain
    // find a better for such optimization, not all objects are drawn
    // that is why I disabled it
    return max_z_ - 1;
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

void Map::patchMap(int x, int y, int z, uint8 tileNum)
{
    assert((x >= 0 && x < max_x_)
        && (y >= 0 && y < max_y_)
        && (z >= 0 && z < max_z_));
    map_data_[(y * max_x_ + x) * max_z_ + z] = tileNum;
}

bool Map::stairsAt(int x, int y, int z)
{
    uint8 isStairs = g_App.walkdata_[tileAt(x, y, z)];
    return  isStairs > 0 && isStairs < 5;
}


int topz = -1;

void Map::draw(int scrollX, int scrollY, MapHelper * helper)
{
    // TODO: after a lot of attempts to fix this, map drawing remains buggy
    if (scrollX + (g_Screen.gameScreenWidth() - g_Screen.gameScreenLeftMargin()) >= map_width_)
        scrollX = map_width_ - (g_Screen.gameScreenWidth() - g_Screen.gameScreenLeftMargin());
    if (scrollY + g_Screen.gameScreenHeight() >= map_height_)
        scrollY = map_height_ - g_Screen.gameScreenHeight();

    uint8 buf[TILE_WIDTH * TILE_HEIGHT];

    int ox, oy;
    int sw = screenToTileX(scrollX, scrollY, ox);
    int chk = g_Screen.gameScreenWidth() / (TILE_WIDTH / 2) + 2
        + g_Screen.gameScreenHeight() / (TILE_HEIGHT / 3) + max_z_ * 2;
    int swm = sw + chk;
    int sh = screenToTileY(scrollX, scrollY, oy) - 8;
    int shm = sh + chk;

    if (topz == -1)
        topz = max_z_;

#if 0
    // HACK for map dump
    sw = 0; swm = max_x_;
    sh = 0; shm = max_y_;
#endif

#ifdef EXECUTION_SPEED_TIME
    printf("---------------------------");
    printf("start time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif

    helper->createFastKeys(sw, sh, swm, shm);
    int cmw = scrollX + g_Screen.gameScreenWidth() - g_Screen.gameScreenLeftMargin() + 128;
    int cmh = scrollY + g_Screen.gameScreenHeight() + 128;
    int cmx = scrollX - g_Screen.gameScreenLeftMargin();
     //  z = 0 - is minimap data and mapdata
    int chky = sh < 0 ? 0 : sh;
    int zr = shm + max_z_ + 1;
    for (int inc = 0; inc < zr; inc++) {
        int ye = sh + inc;
        int ys = ye - max_z_ - 2;
        int z = max_z_ + 1;
        for (int yb = ys; yb < ye; yb++) {
            if (yb < 0 || yb < sh || yb >= shm) {
                z--;
                continue;
            }
            int h = yb;
            for (int w = sw; h >= chky && w < max_x_; w++) {
                if (w < 0 || h >= max_y_) {
                    h--;
                    continue;
                }
                int screen_w = (max_x_ + (w - h)) * (TILE_WIDTH / 2);
                int screen_h = (max_z_ + w + h) * (TILE_HEIGHT / 3);
                int coord_h = screen_h - (z - 1) * (TILE_HEIGHT / 3);
                if (screen_w >= scrollX - TILE_WIDTH * 2
                    && screen_w + TILE_WIDTH * 2 < cmw
                    && coord_h >= scrollY - TILE_HEIGHT * 2
                    && coord_h + TILE_HEIGHT * 2 < cmh) {
                    if (z < max_z_) {
                        int tile = map_data_[(h * max_x_ + w) * max_z_ + z];
                        if (tile > 4) {
                        int dx = 0, dy = 0;
                            if (screen_w - scrollX < 0)
                                dx = -(screen_w - scrollX);
                            if (coord_h - scrollY < 0)
                                dy = -(coord_h - scrollY);
                            if (dx < TILE_WIDTH && dy < TILE_HEIGHT) {
                                tile_manager_->drawTileTo(buf, TILE_WIDTH,
                                                     TILE_HEIGHT, tile, 0, 0, true);
                                g_Screen.blit(screen_w - cmx +
                                              dx, coord_h - scrollY + dy,
                                              TILE_WIDTH - dx,
                                              TILE_HEIGHT - dy,
                                              buf + dx + dy * TILE_WIDTH,
                                              false, TILE_WIDTH);
                            }
                        }
                    }
                    if (z - 2 >= 0) {
                        helper->drawAt(w, h, z - 2,
                            screen_w - cmx + TILE_WIDTH / 2,
                            coord_h - scrollY + TILE_HEIGHT / 3 * 2,
                            scrollX, scrollY);
                    }
                }
                h--;
            }
            z--;
        }
    }

#ifdef EXECUTION_SPEED_TIME
    printf("+++++++++++++++++++++++++++");
    printf("end time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif

}
