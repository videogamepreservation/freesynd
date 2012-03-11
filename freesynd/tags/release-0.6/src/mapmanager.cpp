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
#include "mapmanager.h"
#include "utils/file.h"

MapManager::MapManager()
{
}

MapManager::~MapManager()
{
    for (unsigned int i = 0; i < maps_.size(); i++)
        delete maps_[i];
}

Map * MapManager::loadMap(int mapNum)
{
    char tmp[100];
    sprintf(tmp, "map%02d.dat", mapNum);
    int size;
    uint8 *mapData = File::loadOriginalFile(tmp, size);

    if (!tileManager_.loaded()) {
        uint8 *data = File::loadOriginalFile("hblk01.dat", size);
        tileManager_.loadTiles(data);
        delete[] data;
    }

    maps_[mapNum] = new Map(&tileManager_);
    maps_[mapNum]->loadMap(mapData);
    // patch for "YUKON" map
    if (mapNum == 0x27) {
        maps_[mapNum]->patchMap(60, 63, 1, 0x27);
        maps_[mapNum]->patchMap(61, 63, 1, 0x27);
        maps_[mapNum]->patchMap(62, 63, 1, 0x27);
        maps_[mapNum]->patchMap(60, 64, 1, 0x27);
        maps_[mapNum]->patchMap(61, 64, 1, 0x27);
        maps_[mapNum]->patchMap(62, 64, 1, 0x27);
        maps_[mapNum]->patchMap(60, 65, 1, 0x27);
        maps_[mapNum]->patchMap(61, 65, 1, 0x27);
        maps_[mapNum]->patchMap(62, 65, 1, 0x27);
        maps_[mapNum]->patchMap(60, 66, 1, 0x27);
        maps_[mapNum]->patchMap(61, 66, 1, 0x27);
        maps_[mapNum]->patchMap(62, 66, 1, 0x27);
        maps_[mapNum]->patchMap(60, 67, 1, 0x27);
        maps_[mapNum]->patchMap(61, 67, 1, 0x27);
        maps_[mapNum]->patchMap(62, 67, 1, 0x27);
    }

    delete[] mapData;

    return maps_[mapNum];
}

bool MapManager::drawMap(int mapNum, int scrollx, int scrolly,
                         MapHelper * helper)
{

    //make sure the map is loaded
    if (maps_.find(mapNum) == maps_.end()) {
        printf("MapManager::drawMap: map %d not loaded!\n", mapNum);
        return false;
    }

    maps_[mapNum]->draw(scrollx, scrolly, helper);

    return true;
}

Map *MapManager::map(int mapNum)
{
    //make sure the map is loaded
    if (maps_.find(mapNum) == maps_.end()) {
        printf("MapManager::map: map %d not loaded!\n", mapNum);
        return false;
    }

    return maps_[mapNum];
}

int MapManager::mapWidth(int mapNum)
{
    //make sure the map is loaded
    if (maps_.find(mapNum) == maps_.end()) {
        printf("MapManager::mapWidth: map %d not loaded!\n", mapNum);
        return false;
    }

    return maps_[mapNum]->width();
}

int MapManager::mapHeight(int mapNum)
{
    //make sure the map is loaded
    if (maps_.find(mapNum) == maps_.end()) {
        printf("MapManager::mapHeight: map %d not loaded!\n", mapNum);
        return false;
    }

    return maps_[mapNum]->height();
}

int MapManager::tileToScreenX(int mapNum, int x, int y, int z, int pX,
                              int pY)
{
    //make sure the map is loaded
    if (maps_.find(mapNum) == maps_.end()) {
        printf("MapManager::tileToScreenX: map %d not loaded!\n", mapNum);
        return 0;
    }

    return maps_[mapNum]->tileToScreenX(x, y, z, pX, pY);
}

int MapManager::tileToScreenY(int mapNum, int x, int y, int z, int pX,
                              int pY)
{
    //make sure the map is loaded
    if (maps_.find(mapNum) == maps_.end()) {
        printf("MapManager::tileToScreenY: map %d not loaded!\n", mapNum);
        return 0;
    }

    return maps_[mapNum]->tileToScreenY(x, y, z, pX, pY);
}

int MapManager::screenToTileX(int mapNum, int x, int y, int &ox)
{
    //make sure the map is loaded
    if (maps_.find(mapNum) == maps_.end()) {
        printf("MapManager::screenToTileX: map %d not loaded!\n", mapNum);
        return 0;
    }

    return maps_[mapNum]->screenToTileX(x, y, ox);
}

int MapManager::screenToTileY(int mapNum, int x, int y, int &oy)
{
    //make sure the map is loaded
    if (maps_.find(mapNum) == maps_.end()) {
        printf("MapManager::screenToTileY: map %d not loaded!\n", mapNum);
        return 0;
    }

    return maps_[mapNum]->screenToTileY(x, y, oy);
}

int MapManager::maxZAt(int mapNum, int x, int y)
{
    return maps_[mapNum]->maxZAt(x, y);
}

bool MapManager::mapDimensions(int mapNum, int *x, int *y, int *z)
{
    //make sure the map is loaded
    if (maps_.find(mapNum) == maps_.end()) {
        printf("MapManager::mapDimensions: map %d not loaded!\n", mapNum);
        return false;
    }

    Map * m = maps_[mapNum];
    *x = m->maxX();
    *y = m->maxY();
    *z = m->maxZ();

    return true;
}
