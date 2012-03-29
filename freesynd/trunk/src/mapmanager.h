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

#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <map>
#include "common.h"
#include "map.h"
#include "gfx/tilemanager.h"

/*!
 * Map manager class.
 */
class MapManager {
public:
    MapManager();
    ~MapManager();

    //! Initialize the manager
    bool initialize();
    Map * loadMap(int mapNum);
    bool drawMap(int mapNum, int scrollx, int scrolly, MapHelper *helper);
    Map *map(int mapNum);
    int mapWidth(int mapNum);
    int mapHeight(int mapNum);
    int tileToScreenX(int mapNum, int x, int y, int z, int pX, int pY);
    int tileToScreenY(int mapNum, int x, int y, int z, int pX, int pY);
    int screenToTileX(int mapNum, int x, int y, int &ox);
    int screenToTileY(int mapNum, int x, int y, int &oy);
    int maxZAt(int mapNum, int x, int y);
    bool mapDimensions(int mapNum, int *x, int *y, int *z);

protected:
    std::map<int, Map *> maps_;
    TileManager tileManager_;
};

#endif
