/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>				*
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

#ifndef MAP_H
#define MAP_H

#include "common.h"

#define NUM_MAPS               59

/*!
 * Tile manager class.
 */
class TileManager;

/*!
 * Map helper class.
 */
class MapHelper {
public:
    virtual ~ MapHelper() {}
    virtual void drawAt(int tilex, int tiley, int tilez, int x, int y,
            int scrollX, int scrollY) = 0;
    virtual void createFastKeys(int tilex, int tiley,
        int maxtilex, int maxtiley) = 0;
};

/*!
 * Map class.
 */
class Map {
public:
    Map(TileManager *tileManager);
    ~Map();

    bool loadMap(uint8 *mapData);
    void draw(int scrollX, int scrollY, MapHelper *helper = 0);

    int width() { return map_width_; }
    int height() { return map_height_; }

    int tileToScreenX(int x, int y, int z, int pX, int pY);
    int tileToScreenY(int x, int y, int z, int pX, int pY);
    int screenToTileX(int x, int y, int &ox);
    int screenToTileY(int x, int y, int &oy);

    int maxX() { return max_x_; }
    int maxY() { return max_y_; }
    int maxZ() { return max_z_; }

    int maxZAt(int x, int y);
    int tileAt(int x, int y, int z);
    void patchMap(int x, int y, int z, uint8 tileNum);
    bool stairsAt(int x, int y, int z);

protected:
    int max_x_, max_y_, max_z_;
    uint8 *map_data_;
    TileManager *tile_manager_;
    int map_width_, map_height_;
};

#endif
