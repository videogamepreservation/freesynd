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

void VehicleManager::setVehicleBaseAnim(Vehicle *vehicleanim, unsigned short baseAnim)
{
    vehicleanim->setAnims(baseAnim);
    vehicleanim->setAnimsBurning(baseAnim + 8);
    vehicleanim->setAnimsBurnt(baseAnim + 12);
}

VehicleInstance *VehicleManager::loadInstance(uint8 * data, int map)
{
    // TODO: check all maps
    // TODO: train, join somehow
    LevelData::Cars * gamdata = (LevelData::Cars *) data;

    int hp = READ_LE_INT16(gamdata->health);
    int dir = gamdata->orientation >> 5;

    Vehicle *vehicleanim = new Vehicle();
    int cur_anim = READ_LE_UINT16(gamdata->index_current_anim) - dir;
    setVehicleBaseAnim(vehicleanim, cur_anim);
    VehicleInstance *vehivle_new = vehicleanim->createInstance(map);
    vehivle_new->setHealth(hp);
    vehivle_new->setStartHealth(hp);
    vehivle_new->setMainType(gamdata->sub_type);
    switch (gamdata->sub_type) {
        case 0x01:
            // large armored
            break;
        case 0x04:
            // large armored damaged
            // it is actually base animation and they have 8 directions
            setVehicleBaseAnim(vehicleanim, cur_anim - 12 + (dir >> 1));
            vehivle_new->setStartHealth(0);
            vehivle_new->setHealth(-1);
            vehivle_new->setIsIgnored(true);
            vehicleanim->setVehicleAnim(Vehicle::BurntAnim);
            break;
        case 0x05:
            // train head
        case 0x09:
            // train body
            break;
        case 0x0D:
            // grey vehicle
            break;
        case 0x11:
            // firefighters vehicle
            break;
        case 0x1C:
            // small armored vehicle
            break;
        case 0x24:
            // police vehicle
            break;
        case 0x28:
            // medical vehicle
            break;
#if _DEBUG
        default:
            printf("uknown vehicle type %02X , %02X, %X\n", gamdata->sub_type,
                gamdata->orientation,
                READ_LE_UINT16(gamdata->index_current_frame));
            printf("x = %i, xoff = %i, ", gamdata->mapposx[1],
                gamdata->mapposx[0]);
            printf("y = %i, yoff = %i, ", gamdata->mapposy[1],
                gamdata->mapposy[0]);
            printf("z = %i, zoff = %i\n", gamdata->mapposz[1],
                gamdata->mapposz[0]);
            break;
#endif
    }
    int z = READ_LE_UINT16(gamdata->mapposz) >> 7;

    // TODO: the size should be adjusted on orientation/direction change
    // and it should be different per vehicle type
    vehivle_new->setSizeX(256);
    vehivle_new->setSizeY(256);
    vehivle_new->setSizeZ(192);

    int oz = gamdata->mapposz[0] & 0x7F;
    vehivle_new->setPosition(gamdata->mapposx[1], gamdata->mapposy[1],
                            z, gamdata->mapposx[0],
                            gamdata->mapposy[0], oz);
    vehivle_new->setDirection(gamdata->orientation);

    return vehivle_new;
}
