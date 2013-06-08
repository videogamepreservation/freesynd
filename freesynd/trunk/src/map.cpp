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
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "map.h"
#include "utils/log.h"
#include "gfx/tilemanager.h"
#include "gfx/screen.h"

#if 0
#define EXECUTION_SPEED_TIME
#endif

Map::Map(TileManager * tileManager, uint16 i_id) : tile_manager_(tileManager)
{
    i_id_ = i_id;
    a_tiles_ = NULL;
}

Map::~Map()
{
    delete[] a_tiles_;
}

bool Map::loadMap(uint8 * mapData)
{
    LOG(Log::k_FLG_GFX, "Map", "loadMap", ("Loading Map %d.", i_id_));
    max_x_ = READ_LE_UINT32(mapData + 0);
    max_y_ = READ_LE_UINT32(mapData + 4);
    max_z_ = READ_LE_UINT32(mapData + 8);

    LOG(Log::k_FLG_GFX, "Map", "loadMap", 
        ("Map size in tiles: max_x = %d, max_y = %d, max_z = %d.", max_x_, max_y_, max_z_));

    uint32 *lookup = new uint32[max_x_ * max_y_];
    // NOTE : increased map height by 1 to enable range check on higher tiles
    a_tiles_ = new Tile*[max_x_ * max_y_ * (max_z_ + 1)];
    
    for (int i = 0; i < max_x_ * max_y_; i++)
        lookup[i] = READ_LE_UINT32(mapData + 12 + i * 4);
    for (int h = 0, z_real = max_z_ + 1; h < max_y_; h++)
        for (int w = 0; w < max_x_; w++) {
            int idx = h * max_x_ + w;

            for (int z = 0; z < max_z_; z++) {
                uint8 tileNum = *(mapData + 12 + lookup[idx] + z);
                a_tiles_[idx * z_real + z] = tile_manager_->getTile(tileNum);
                
            }
        }
    delete[] lookup;

    max_z_++;
    for (int h = 0, z = max_z_ - 1; h < max_y_; h++) {
        for (int w = 0; w < max_x_; w++) {
            a_tiles_[(h * max_x_ + w) * max_z_ + z] = tile_manager_->getTile(0);
        }
    }

    map_width_ = (max_x_ + max_y_) * (TILE_WIDTH / 2);
    map_height_ = (max_x_ + max_y_ + max_z_) * TILE_HEIGHT / 3;
    LOG(Log::k_FLG_GFX, "Map", "loadMap", 
        ("Map size in pixels: width = %d, height = %d.", map_width_, map_height_));

    LOG(Log::k_FLG_GFX, "Map", "loadMap", ("Loading finished"));

    return true;
}

void Map::mapDimensions(int *x, int *y, int *z)
{
    *x = maxX();
    *y = maxY();
    *z = maxZ();
}

float scalexPx = 256.0f;
float scalexPy = 256.0f;
float scaleyPx = 256.0f;
float scaleyPy = 256.0f;

int Map::tileToScreenX(int x, int y, int z, int pX, int pY)
{
    float fx = x + pX / scalexPx;
    float fy = y + pY / scalexPy;
    return (int) ((max_x_ * TILE_WIDTH / 2) + (fx - fy) * TILE_WIDTH / 2
                  + TILE_WIDTH / 2);
}

int Map::tileToScreenY(int x, int y, int z, int pX, int pY)
{
    float fx = x + pX / scaleyPx;
    float fy = y + pY / scaleyPy;
    return (int) ((max_z_ + 1) * TILE_HEIGHT / 3 + (fx + fy) * TILE_HEIGHT / 3);
}

MapScreenPoint Map::tileToScreenPoint(int x, int y, int z, int pX, int pY)
{
    float fx = x + pX / scalexPx;
    float fy = y + pY / scalexPy;

    MapScreenPoint msp;
    msp.x = (int) ((max_x_ * TILE_WIDTH / 2) + (fx - fy) * TILE_WIDTH / 2
                  + TILE_WIDTH / 2);

    msp.y = (int) ((max_z_ + 1) * TILE_HEIGHT / 3 + (fx + fy) * TILE_HEIGHT / 3);

    return msp;
}

/*!
 * Returns the tile position corresponding to the screen position.
 * Usually after the user clicked on the map, this method is used
 * to find what tile has been clicked.
 * \param x The x position on the screen (in pixel).
 * \param y The y position on the screen (in pixel).
 */
MapTilePoint Map::screenToTilePoint(int x, int y)
{
    MapTilePoint mtp;

    x -= (max_x_ + 1) * (TILE_WIDTH / 2);
    // x now equals fx * TILE_WIDTH / 2 - fy * TILE_WIDTH / 2
    // which equals TILE_WIDTH/2 * (fx - fy)
    y -= (max_z_ + 1) * (TILE_HEIGHT / 3);
    // y now equals (fx + fy) * TILE_HEIGHT / 3
    float dx = (float) x / (TILE_WIDTH / 2);
    float dy = (float) y / (TILE_HEIGHT / 3);

    // dx equals fx - fy
    // dy equals fx + fy
    float f_tx = (dx + dy) / 2;
    mtp.tx = (int) f_tx;
    mtp.ox = (int) ((f_tx - mtp.tx) * 256.0f);

    float f_ty = (dy - dx) / 2;
    mtp.ty = (int) f_ty;
    mtp.oy = (int) ((f_ty - mtp.ty) * 256.0f);

    return mtp;
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

Tile * Map::getTileAt(int x, int y, int z)
{
    if (x < 0 || x >= max_x_ || y < 0 || y >= max_y_) {
        return tile_manager_->getTile(z < 2 ? 6 : 0);
    }
    
    if (z < 0 || z >= max_z_) {
        return tile_manager_->getTile(0);
    }

    return a_tiles_[(y * max_x_ + x) * max_z_ + z];
}

int Map::tileAt(int x, int y, int z)
{
    if (x < 0 || x >= max_x_)
        return z < 2 ? 6 : 0;
    if (y < 0 || y >= max_y_)
        return z < 2 ? 6 : 0;
    if (z < 0 || z >= max_z_)
        return 0;

    return a_tiles_[(y * max_x_ + x) * max_z_ + z]->id();
}

void Map::patchMap(int x, int y, int z, uint8 tileNum)
{
    assert((x >= 0 && x < max_x_)
        && (y >= 0 && y < max_y_)
        && (z >= 0 && z < max_z_));
    a_tiles_[(y * max_x_ + x) * max_z_ + z] = tile_manager_->getTile(tileNum);
}


int topz = -1;

void Map::draw(int scrollX, int scrollY, MapHelper * helper)
{
    // TODO: after a lot of attempts to fix this, map drawing remains buggy
    if (scrollX + (g_Screen.gameScreenWidth() - g_Screen.gameScreenLeftMargin()) >= map_width_)
        scrollX = map_width_ - (g_Screen.gameScreenWidth() - g_Screen.gameScreenLeftMargin());
    if (scrollY + g_Screen.gameScreenHeight() >= map_height_)
        scrollY = map_height_ - g_Screen.gameScreenHeight();

    MapTilePoint mtp = screenToTilePoint(scrollX, scrollY);
    int sw = mtp.tx;
    int chk = g_Screen.gameScreenWidth() / (TILE_WIDTH / 2) + 2
        + g_Screen.gameScreenHeight() / (TILE_HEIGHT / 3) + max_z_ * 2;
    int swm = sw + chk;
    int sh = mtp.ty - 8;
#if 0
    // HACK for map dump
    sh = 0;
#endif
    int shm = sh + chk;

    if (topz == -1)
        topz = max_z_;

#ifdef EXECUTION_SPEED_TIME
    printf("---------------------------");
    int measure_ticks = SDL_GetTicks();
    printf("start time %i.%i\n", measure_ticks/1000, measure_ticks%1000);
#endif

    helper->createFastKeys(sw, sh, swm, shm);
    int cmw = scrollX + g_Screen.gameScreenWidth() - g_Screen.gameScreenLeftMargin() + 128;
    int cmh = scrollY + g_Screen.gameScreenHeight() + 128;
    int cmx = scrollX - g_Screen.gameScreenLeftMargin();
     //  z = 0 - is minimap data and mapdata
    int chky = sh < 0 ? 0 : sh;
    int zr = shm + max_z_ + 1;
    for (int inc = 0; inc < zr; ++inc) {
        int ye = sh + inc;
        int ys = ye - max_z_ - 2;
        int z = max_z_ + 1;
        for (int yb = ys; yb < ye; ++yb) {
            if (yb < 0 || yb < sh || yb >= shm) {
                --z;
                continue;
            }
            int h = yb;
            for (int w = sw; h >= chky && w < max_x_; ++w) {
                if (w < 0 || h >= max_y_) {
                    --h;
                    continue;
                }
                int screen_w = (max_x_ + (w - h)) * (TILE_WIDTH / 2);
                //int screen_h = (max_z_ + w + h) * (TILE_HEIGHT / 3);
                int coord_h = ((max_z_ + w + h) - (z - 1)) * (TILE_HEIGHT / 3);
                if (screen_w >= scrollX - TILE_WIDTH * 2
                    && screen_w + TILE_WIDTH * 2 < cmw
                    && coord_h >= scrollY - TILE_HEIGHT * 2
                    && coord_h + TILE_HEIGHT * 2 < cmh)
                {
#if 0
                    if (z > 2)
                        continue;
#endif
                    if (z < max_z_) {
                        Tile *p_tile = a_tiles_[(h * max_x_ + w) * max_z_ + z];
                        if (p_tile->notTransparent()) {
                            int dx = 0, dy = 0;
                            if (screen_w - scrollX < 0)
                                dx = -(screen_w - scrollX);
                            if (coord_h - scrollY < 0)
                                dy = -(coord_h - scrollY);
                            if (dx < TILE_WIDTH && dy < TILE_HEIGHT) {
                                p_tile->drawTo((uint8*)g_Screen.pixels(),
                                    g_Screen.gameScreenWidth(),
                                    g_Screen.gameScreenHeight(),
                                    screen_w - cmx,
                                    coord_h - scrollY);
                            }
                        }
                    }
                    if (z - 1 >= 0) {
                        helper->drawAt(w, h, z - 1,
                            screen_w - cmx + TILE_WIDTH / 2,
                            coord_h - scrollY + TILE_HEIGHT / 3 * 2);
                    }
                }
                --h;
            }
            --z;
        }
    }

#ifdef EXECUTION_SPEED_TIME
    printf("+++++++++++++++++++++++++++");
    int measure_ticks_end = SDL_GetTicks();
    printf("end time %i.%i,%i\n", measure_ticks_end/1000, measure_ticks_end%1000,
        measure_ticks_end - measure_ticks);
#endif

}

/*!
 * Construct the minimap from the given map.
 * \param p_map
 */
MiniMap::MiniMap(Map *p_map) {
    // walkdata based colours
    uint8 minimap_colours_[] = {
        8,  7,  7,  7,
        7,  7, 10, 10,
       10, 10,  0, 10,
       15, 15, 10, 10,
       0,   0,  0,  0,
    };

    mmax_x_ = p_map->maxX();
    mmax_y_ = p_map->maxY();

    a_minimap_ = (uint8 *)( malloc(mmax_x_ * mmax_y_) );
    if(a_minimap_ == NULL) {
        FSERR(Log::k_FLG_MEM, "MiniMap", "MiniMap", ("memory allocation failed"));
        return;
    }
    for (unsigned short y = 0; y < mmax_y_; y++) {
        unsigned short yadd = y * mmax_x_;
        for (unsigned short x = 0; x < mmax_x_; x++) {
            Tile::EType type = p_map->getTileAt(x, y, 0)->type();
            a_minimap_[x + yadd] = minimap_colours_[type];
        }
    }
}

MiniMap::~MiniMap() {
    if (a_minimap_) {
        free(a_minimap_);
        a_minimap_ = NULL;
    }
}

/*!
 * Return the color at the given point.
 * \param x
 * \param y
 */
uint8 MiniMap::getColourAt(int x, int y) {

    if (x < 0 || y < 0 || x >= mmax_x_ || y >= mmax_y_)
        return 0;

    if (a_minimap_ != 0)
        return a_minimap_[x + y * mmax_x_];
    return 0;
}

//! Defines a source on the minimap for the signal
void MiniMap::setTarget(MapObject *pTarget) {
    p_target_ = pTarget;
}
//! Clear the target source
void MiniMap::clearTarget() {
    p_target_ = NULL;
}
//! Defines the evacuation point on the minimap
void MiniMap::setEvacuationPoint(const toDefineXYZ &evacPt) {
    evacPt_.x = evacPt.x;
    evacPt_.y = evacPt.y;
    evacPt_.z = evacPt.z;
}

//! Return the evacuation position
void MiniMap::evacuationPoint(toDefineXYZ &evacPt) {
    evacPt.x = evacPt_.x;
    evacPt.y = evacPt_.y;
    evacPt.z = evacPt_.z;
}

//! Return the target position
MapTilePoint MiniMap::targetPosition() {
    MapTilePoint mtp;
    if (p_target_) {
        // TODO MapObject should already have a MapTilePoint
        mtp.tx = p_target_->tileX();
        mtp.ty = p_target_->tileY();
        mtp.tz = p_target_->tileZ();
        mtp.ox = p_target_->offX();
        mtp.oy = p_target_->offY();
    }
    return mtp; 
}
