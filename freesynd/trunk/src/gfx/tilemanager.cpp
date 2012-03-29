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
#include <assert.h>

#include "tilemanager.h"
#include "resources.h"
#include "utils/log.h"
#include "utils/file.h"

/*!
 *
 */
void unpackBlocks4(const uint8 * data, uint8 * pixels)
{
    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 8; ++i) {
            if (bitSet(data[j], 7 - i)) {
                pixels[j * 8 + i] = 255;        // transparent
            } else {
                pixels[j * 8 + i] =
                    static_cast < uint8 >
                    ((bitValue(data[4 + j], 7 - i) << 0) & 0xff)
                    | static_cast < uint8 >
                    ((bitValue(data[8 + j], 7 - i) << 1) & 0xff)
                    | static_cast < uint8 >
                    ((bitValue(data[12 + j], 7 - i) << 2) & 0xff)
                    | static_cast < uint8 >
                    ((bitValue(data[16 + j], 7 - i) << 3) & 0xff);
            }
        }
    }
}

/*! 
 *
 */
void loadSubTile(const uint8 * data, uint32 offset, uint32 index,
                 uint32 stride, uint8 * pixels)
{
    if (offset < TILE_HEADER_LENGTH)
        return;

    data += offset;

    for (int i = 0; i < SUBTILE_HEIGHT; ++i) {
        unpackBlocks4(data,
                      pixels + index + (SUBTILE_HEIGHT - 1 - i) * stride);
        data += SUBTILE_ROW_LENGTH;
    }
}

/*!
 * Default constructor.
 */
TileManager::TileManager()
{}

/*!
 * Loads a tile from the tile data.
 * \param tileData Data containing all tiles
 * \param id Id of the tile to load
 * \return The loaded tile.
 */
Tile * TileManager::loadTile(uint8 * tileData, uint8 id)
{
    uint32 offset = id * TILE_INDEX_SIZE;
    uint8 a_tile_data[TILE_WIDTH * TILE_HEIGHT];
    memset(a_tile_data, 255, TILE_WIDTH * TILE_HEIGHT);

    for (int i = 0; i < SUBTILES_PERtile__X; ++i) {
        for (int j = 0; j < SUBTILES_PERtile__Y; ++j) {
            uint32 subTileOffset =
                READ_LE_UINT32(tileData + offset +
                               (i * SUBTILES_PERtile__Y + j) * 4);
            loadSubTile(tileData, subTileOffset,
                        (SUBTILES_PERtile__Y - 1 -
                         j) * SUBTILE_HEIGHT * TILE_WIDTH +
                        i * SUBTILE_WIDTH, TILE_WIDTH, a_tile_data);
        }
    }

    // If at least one pixel is not transparent
    // b_all_alpha will be false and so tile will be drawn
    bool b_all_alpha = true;
    for (int h = 0; h < TILE_HEIGHT; h++) {
        for (int w = 0; w < TILE_WIDTH; w++)
            if (a_tile_data[h * TILE_HEIGHT + w] != 255) {
                b_all_alpha = false;
                break;
            }
    }

    Tile *p_tile = new Tile(id, a_tile_data, b_all_alpha);
    return p_tile;
}

/*!
 * Loads all tile from the file.
 */
bool TileManager::loadTiles()
{
    int size;
    uint8 *tileData = File::loadOriginalFile(TILE_SET, size);
  
    if (!tileData) {
        FSERR(Log::k_FLG_IO, "TileManager", "loadTiles", ("Failed to load tiles data\n"));
        return false;
    }

    for (int i = 0; i < NUMtile_S; ++i) {
        a_tiles_[i] = loadTile(tileData, i);
    }

    delete[] tileData;
    return true;
}

/*!
 * Returns the tile with the given index.
 * \param tileNum The tile index in the tile manager.
 * \return The tile or null if no tile is found for the index.
 */
Tile * TileManager::getTile(uint8 tileNum) {
    //make sure the tile is loaded
    if (tileNum >= NUMtile_S) {
        FSERR(Log::k_FLG_GFX, "TileManager", "getTile", ("tile %d not loaded!", tileNum));
        return NULL;
    }

    return a_tiles_[tileNum];
}