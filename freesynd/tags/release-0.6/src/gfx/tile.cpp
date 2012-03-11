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
#include "tile.h"

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


Tile::Tile()
{
}

bool Tile::loadTile(uint8 * tileData, uint32 offset)
{
    memset(tile_Data_, 255, TILE_WIDTH * TILE_HEIGHT);

    for (int i = 0; i < SUBTILES_PERtile__X; ++i) {
        for (int j = 0; j < SUBTILES_PERtile__Y; ++j) {
            uint32 subTileOffset =
                READ_LE_UINT32(tileData + offset +
                               (i * SUBTILES_PERtile__Y + j) * 4);
            loadSubTile(tileData, subTileOffset,
                        (SUBTILES_PERtile__Y - 1 -
                         j) * SUBTILE_HEIGHT * TILE_WIDTH +
                        i * SUBTILE_WIDTH, TILE_WIDTH, tile_Data_);
        }
    }

    all_alpha_ = true;
    for (int h = 0; h < TILE_HEIGHT; h++)
        for (int w = 0; w < TILE_WIDTH; w++)
            if (tile_Data_[h * TILE_HEIGHT + w] != 255) {
                all_alpha_ = false;
                break;
            }

    return true;
}

bool Tile::drawTo(uint8 * screen, int swidth, int sheight, int x, int y,
                  bool clear)
{
    if (all_alpha_)
        return false;

    if (clear)
        memset(screen, 255, swidth * sheight);

    for (int j = 0; j < TILE_HEIGHT; j++) {
        if (y + j < 0)
            continue;
        if (y + j >= sheight)
            break;
        for (int i = 0; i < TILE_WIDTH; i++) {
            if (x + i < 0)
                continue;
            if (x + i >= swidth)
                break;
            uint8 c = tile_Data_[(TILE_HEIGHT - j - 1) * TILE_WIDTH + i];
            if (c != 255)
                screen[(y + j) * swidth + x + i] = c;
        }
    }

    return true;
}
