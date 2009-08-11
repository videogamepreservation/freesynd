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

PedManager::PedManager()
{
}

PedManager::~PedManager()
{
    for (unsigned int i = 0; i != peds_.size(); ++i)
        delete peds_[i];
}

void PedManager::loadPeds()
{
    Ped *agent = new Ped();
    agent->setStandAnim(Ped::Unarmed, 1);
    agent->setWalkAnim(Ped::Unarmed, 9);
    agent->setStandAnim(Ped::EnergyShield, 17);
    agent->setStandAnim(Ped::Pistol, 25);
    agent->setStandAnim(Ped::Shotgun, 25);
    agent->setStandAnim(Ped::Minigun, 33);
    agent->setStandAnim(Ped::Flamer, 41);
    agent->setStandAnim(Ped::LongRange, 49);
    agent->setWalkAnim(Ped::EnergyShield, 57);
    agent->setWalkAnim(Ped::Pistol, 65);
    agent->setWalkAnim(Ped::Shotgun, 65);
    agent->setWalkAnim(Ped::Minigun, 73);
    agent->setWalkAnim(Ped::Flamer, 81);
    agent->setWalkAnim(Ped::LongRange, 89);
    // I think 97 was the original pistol shoot anim, but it wasn't sexy enough.
    //agent->setStandFireAnim(Ped::Laser, 97);
    agent->setStandFireAnim(Ped::Pistol, 105);
    agent->setStandFireAnim(Ped::Uzi, 105);
    agent->setStandFireAnim(Ped::Shotgun, 105);
    agent->setStandFireAnim(Ped::Gauss, 105);
    agent->setStandFireAnim(Ped::Minigun, 113);
    agent->setStandFireAnim(Ped::Laser, 121);
    agent->setStandFireAnim(Ped::Flamer, 129);
    agent->setStandFireAnim(Ped::LongRange, 137);
    agent->setWalkFireAnim(Ped::Pistol, 145);
    agent->setWalkFireAnim(Ped::Uzi, 145);
    agent->setWalkFireAnim(Ped::Shotgun, 145);
    agent->setWalkFireAnim(Ped::Gauss, 145);
    agent->setWalkFireAnim(Ped::Minigun, 153);
    agent->setWalkFireAnim(Ped::Laser, 161);
    agent->setWalkFireAnim(Ped::Flamer, 169);
    agent->setWalkFireAnim(Ped::LongRange, 177);
    agent->setPickupAnim(193);
    agent->setHitAnim(194);
    // 198 vapourized
    // 202 sinking
    agent->setDieAnim(204);
    agent->setDeadAnim(205);
    // 206 die rotated
    // 207 dead rotated
    // 208 on fire
    // 209 walking on fire
    // 210 die burning
    // 211 dead and smoking
    peds_.push_back(agent);

    Ped *guard = new Ped();
    guard->setStandAnim(Ped::Unarmed, 1744);
    guard->setWalkAnim(Ped::Unarmed, 1752);
    guard->setStandAnim(Ped::Pistol, 1768);
    guard->setWalkAnim(Ped::Pistol, 1808);
    guard->setStandFireAnim(Ped::Pistol, 1848);
    guard->setWalkFireAnim(Ped::Pistol, 1896);
    guard->setHitAnim(1937);
    guard->setDieAnim(1949);
    guard->setDeadAnim(1950);
    guard->setPickupAnim(1936);
    /* 1941-1944 guard vapourized
       1945 guard sinking
     */
    peds_.push_back(guard);

    Ped *general = new Ped();
    general->setStandAnim(Ped::Unarmed, 1537);
    general->setWalkAnim(Ped::Unarmed, 1545);
    general->setStandAnim(Ped::Pistol, 1561);
    general->setWalkAnim(Ped::Pistol, 1601);
    general->setStandFireAnim(Ped::Pistol, 1641);
    general->setWalkFireAnim(Ped::Pistol, 1689);
    general->setPickupAnim(1729);
    general->setHitAnim(1730);
    // 1734 general vapourized
    // 1738 sinking
    general->setDieAnim(1742);
    general->setDeadAnim(1743);
    peds_.push_back(general);
}

PedInstance *PedManager::loadInstance(uint8 * data, int map)
{
    Mission::LEVELDATA_PEOPLE * gamdata =
        (Mission::LEVELDATA_PEOPLE *) data;

#if 0
    if (gamdata->currvistype[1] == 1) {
        uint8 *d = (uint8 *) gamdata;
        for (unsigned int i = 0; i < sizeof(*gamdata); i++) {
            printf("%02X ", d[i]);
            if ((i % 16) == 15)
                printf("\n");
        }
        printf("\n");
    }
#endif
#if 0
    printf("currvistype: %i %i\n", gamdata->currvistype[0],
           gamdata->currvistype[1]);
    printf("objtype: %i\n", gamdata->objtype);
    printf("vistype: %i %i\n", gamdata->vistype[0], gamdata->vistype[1]);
    printf("mapposx: %i %i\n", gamdata->mapposx[0], gamdata->mapposx[1]);
    printf("mapposy: %i %i\n", gamdata->mapposy[0], gamdata->mapposy[1]);
    printf("mapposz: %i %i\n", gamdata->mapposz[0], gamdata->mapposz[1]);
    printf("\n");
#endif

    int kind = 0;
    int dir = 0;
    int hp = 255;
    int sr = 0;
    switch (gamdata->currvistype[1]) {
    case 0:
        hp = 1;
        kind = 2;               // general
        sr = 7;
        dir = ((gamdata->currvistype[0] - 248) / 3 + 6) % 8;
        break;
    case 1:
        kind = 0;               // agent
        hp = 10;
        dir = ((gamdata->currvistype[0] - 126) / 3 + 6) % 8;
        break;
    case 30:
        hp = 2;
        kind = 1;               // guard
        sr = 7;
        switch (gamdata->currvistype[0]) {
        case 35:
            dir = 0;
            break;
        case 37:
            dir = 3;
            break;
        case 39:
            dir = 1;
            break;
        case 43:
            dir = 2;
            break;
        case 49:
            break;
        case 50:
            break;
        case 51:
            break;
        case 52:
            break;
        case 53:
            break;
        case 54:
            break;
        case 55:
            dir = 5;
            break;
        case 56:
            break;
        case 57:
            break;
        case 58:
            break;
        case 59:
            break;
        case 63:
            break;
        }
        break;
        // TODO: other kinds of peds
    default:
        kind = 0;
    }

    PedInstance *newped = peds_[kind]->createInstance(map);
    newped->setStartHealth(hp);
    newped->setHealth(hp);
    //printf("%i %i %i %i %i %i\n", gamdata->mapposx[1], gamdata->mapposy[1], gamdata->mapposz[1], gamdata->mapposx[0], gamdata->mapposy[0], gamdata->mapposz[0]);
    newped->setPosition(gamdata->mapposx[1], gamdata->mapposy[1],
                        gamdata->mapposz[1], gamdata->mapposx[0],
                        gamdata->mapposy[0], gamdata->mapposz[0]);
    //newped->setPosition(gamdata->mapposx[1], gamdata->mapposy[1], gamdata->mapposz[1], 0, 0);
    newped->setDirection(dir);
    if (sr) {
        newped->setHostile(true);
        newped->setSightRange(sr);
    }

    return newped;
}
