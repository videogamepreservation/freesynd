/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
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
#include <assert.h>
#include "app.h"

VehicleManager::VehicleManager()
{
}

VehicleManager::~VehicleManager()
{
    /*
    for (unsigned int i = 0; i < vehicles_.size(); i++)
        delete vehicles_[i];
        */
}

void VehicleManager::setVehicle(Vehicle *vehicleanim, unsigned short baseAnim)
{
    vehicleanim->setAnims(baseAnim);
    vehicleanim->setAnimsBurning(baseAnim + 8);
    vehicleanim->setAnimsBurnt(baseAnim + 12);
}

VehicleInstance *VehicleManager::loadInstance(uint8 * data, int map)
{
    Mission::LEVELDATA_CARS * gamdata = (Mission::LEVELDATA_CARS *) data;

    int hp = READ_LE_INT16(gamdata->health);
    int dir = gamdata->orientation >> 5;

    Vehicle *vehicleanim = new Vehicle();
    setVehicle(vehicleanim,
        READ_LE_UINT32(gamdata->index_current_anim) - dir);
    VehicleInstance *newvehicle = vehicleanim->createInstance(map);
    int z = READ_LE_UINT16(gamdata->mapposz) >> 7;

    // TODO: the size should be adjusted on orientation/direction change
    // and it should be different per vehicle type
    newvehicle->setSizeX(256);
    newvehicle->setSizeY(256);
    newvehicle->setSizeZ(192);

    int oz = gamdata->mapposz[0] & 0x7F;
    newvehicle->setPosition(gamdata->mapposx[1], gamdata->mapposy[1],
                            z, gamdata->mapposx[0],
                            gamdata->mapposy[0], oz);
    newvehicle->setHealth(hp);
    newvehicle->setStartHealth(hp);
    newvehicle->setDirection(gamdata->orientation);

    return newvehicle;
}
