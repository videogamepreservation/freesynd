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

#ifndef TILE_H
#define TILE_H

#include "common.h"

// TODO: Convert these to const int's -- we are using C++, yes? :-)
#define NUMtile_S               256

#define TILE_WIDTH              64
#define TILE_HEIGHT             48

#define SUBTILE_WIDTH           32
#define SUBTILE_HEIGHT          16

#define SUBTILES_PERtile__X     (TILE_WIDTH / SUBTILE_WIDTH)
#define SUBTILES_PERtile__Y     (TILE_HEIGHT / SUBTILE_HEIGHT)
#define SUBTILES_PERtile_       (SUBTILES_PERtile__X * SUBTILES_PERtile__Y)

#define BLOCKS_PER_SUBTILE_ROW  (SUBTILE_WIDTH / PIXELS_PER_BLOCK)
#define SUBTILE_ROW_LENGTH      (BLOCK_LENGTH * BLOCKS_PER_SUBTILE_ROW)

#define TILE_INDEX_SIZE         (SUBTILES_PERtile_ * 4)
#define TILE_HEADER_LENGTH      (NUMtile_S * TILE_INDEX_SIZE)

/*!
 * Tile class.
 */
class Tile {
public:
    Tile(uint8 id, uint8 *tile_Data, bool all_alpha);
    ~Tile();

    //! Returns the tile id 
    uint8 id() { return i_id_; }

    //! Draws the tile to the given surface
    bool drawTo(uint8 *screen, int swidth, int sheight, int x, int y,
            bool clear = false);

protected:
    /*! Each tile has a unique id.*/
    uint8 i_id_;
    /*! The pixels that compose the tile.*/
    uint8 *a_pixels_;
    /*! A quick flag to tell that all pixel are transparent.*/
    bool b_all_alpha_;
};

#endif
