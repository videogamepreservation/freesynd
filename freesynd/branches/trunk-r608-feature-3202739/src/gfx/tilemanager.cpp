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

TileManager::TileManager():tilesLoaded_(0)
{
}

bool TileManager::loadTiles(uint8 * tileData)
{
    assert(tileData);

    for (int i = 0; i < NUMtile_S; ++i) {
        if (!tile_[i].loadTile(tileData, i * TILE_INDEX_SIZE)) {
            printf("Failed to load tile: %d\n", i);
        }
    }

    tilesLoaded_ = NUMtile_S;
    return true;
}

bool TileManager::drawTileTo(uint8 * screen, int swidth, int sheight,
                             int tileNum, int x, int y, bool clear)
{
    assert(tileNum >= 0 && tileNum < NUMtile_S);

    //make sure the tile is loaded
    if (tileNum >= tilesLoaded_) {
        printf("TileManager::drawTileTo: tile %d not loaded!\n", tileNum);
        return false;
    }

    return tile_[tileNum].drawTo(screen, swidth, sheight, x, y, clear);
}
