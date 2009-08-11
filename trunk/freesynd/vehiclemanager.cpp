/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
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
#include "app.h"

VehicleManager::VehicleManager()
{
}

VehicleManager::~VehicleManager()
{
    for (unsigned int i = 0; i < vehicles_.size(); i++)
        delete vehicles_[i];
}

void VehicleManager::loadVehicles()
{
    vehicles_.push_back(new Vehicle("CAR", 446));
}

VehicleInstance *VehicleManager::loadInstance(uint8 * data, int map)
{
    Mission::LEVELDATA_CARS * gamdata = (Mission::LEVELDATA_CARS *) data;

    int kind;
    int hp = 255;
    switch (gamdata->type) {
    case 13:
        kind = 0;
        hp = 5;
        break;
        // TODO: other kinds of vehicles
    default:
        return NULL;
    }

    VehicleInstance *newvehicle = vehicles_[kind]->createInstance(map);
    newvehicle->setPosition(gamdata->mapposx[1], gamdata->mapposy[1],
                            gamdata->mapposz[1], gamdata->mapposx[0],
                            gamdata->mapposy[0], gamdata->mapposz[0]);
    newvehicle->setHealth(hp);
    newvehicle->setStartHealth(hp);
    newvehicle->setDirection(gamdata->currvistype - 147);

    return newvehicle;
}
