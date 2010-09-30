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

PedManager::PedManager()
{
}

void PedManager::setPed(Ped *pedanim, unsigned short baseAnim)
{
    pedanim->setStandAnim(Ped::Unarmed, 0 + baseAnim);
    pedanim->setWalkAnim(Ped::Unarmed, 8 + baseAnim);
    pedanim->setStandAnim(Ped::EnergyShield, 16 + baseAnim);
    pedanim->setStandAnim(Ped::Pistol, 24 + baseAnim);
    pedanim->setStandAnim(Ped::Shotgun, 24 + baseAnim);
    pedanim->setStandAnim(Ped::Minigun, 32 + baseAnim);
    pedanim->setStandAnim(Ped::Flamer, 40 + baseAnim);
    pedanim->setStandAnim(Ped::LongRange, 48 + baseAnim);
    pedanim->setWalkAnim(Ped::EnergyShield, 56 + baseAnim);
    pedanim->setWalkAnim(Ped::Pistol, 64 + baseAnim);
    pedanim->setWalkAnim(Ped::Shotgun, 64 + baseAnim);
    pedanim->setWalkAnim(Ped::Minigun, 72 + baseAnim);
    pedanim->setWalkAnim(Ped::Flamer, 80 + baseAnim);
    pedanim->setWalkAnim(Ped::LongRange, 88 + baseAnim);
    // I think 97 was the original pistol shoot anim, but it wasn't sexy enough.
    //pedanim->setStandFireAnim(Ped::Laser, 96 + baseAnim);
    pedanim->setStandFireAnim(Ped::Pistol, 104 + baseAnim);
    pedanim->setStandFireAnim(Ped::Uzi, 104 + baseAnim);
    pedanim->setStandFireAnim(Ped::Shotgun, 104 + baseAnim);
    pedanim->setStandFireAnim(Ped::Gauss, 104 + baseAnim);
    pedanim->setStandFireAnim(Ped::Minigun, 112 + baseAnim);
    pedanim->setStandFireAnim(Ped::Laser, 120 + baseAnim);
    pedanim->setStandFireAnim(Ped::Flamer, 128 + baseAnim);
    pedanim->setStandFireAnim(Ped::LongRange, 136 + baseAnim);
    pedanim->setWalkFireAnim(Ped::Pistol, 144 + baseAnim);
    pedanim->setWalkFireAnim(Ped::Uzi, 144 + baseAnim);
    pedanim->setWalkFireAnim(Ped::Shotgun, 144 + baseAnim);
    pedanim->setWalkFireAnim(Ped::Gauss, 144 + baseAnim);
    pedanim->setWalkFireAnim(Ped::Minigun, 152 + baseAnim);
    pedanim->setWalkFireAnim(Ped::Laser, 160 + baseAnim);
    pedanim->setWalkFireAnim(Ped::Flamer, 168 + baseAnim);
    pedanim->setWalkFireAnim(Ped::LongRange, 176 + baseAnim);
    pedanim->setPickupAnim(192 + baseAnim);
    pedanim->setHitAnim(193 + baseAnim);
    // 197 - 200 vapourized
    // 201 sinking
    // 203 - die, 204 - dead, agent only(?)
    pedanim->setDieAgentAnim(203 + baseAnim);
    pedanim->setDeadAgentAnim(204 + baseAnim);
    pedanim->setDieAnim(205 + baseAnim);
    pedanim->setDeadAnim(206 + baseAnim);
    // 207 on fire
    // 208 walking on fire
    // 209 die burning
    // 210 dead and smoking
}

PedInstance *PedManager::loadInstance(uint8 * data, int map)
{
    Mission::LEVELDATA_PEOPLE * gamdata =
        (Mission::LEVELDATA_PEOPLE *) data;

    if (gamdata->type == 0)
        return NULL;

#if 0
    printf("mapposx: %i %i\n", gamdata->mapposx[0], gamdata->mapposx[1]);
    printf("mapposy: %i %i\n", gamdata->mapposy[0], gamdata->mapposy[1]);
    printf("mapposz: %i %i\n", gamdata->mapposz[0], gamdata->mapposz[1]);
    printf("\n");
#endif
    int dir = gamdata->orientation >> 5;
    int hp = 2;

    Ped *pedanim = new Ped();
    setPed(pedanim, READ_LE_UINT32(gamdata->index_base_anim));
    PedInstance *newped = pedanim->createInstance(map);
    newped->setStartHealth(hp);
    newped->setHealth(hp);
    int z = READ_LE_UINT16(gamdata->mapposz) >> 7;
    z--;
    int oz = gamdata->mapposz[0] & 0x7F;
    newped->setVisZ(z);
    if (oz > 0)
        z++;
    //printf("%i %i %i %i %i %i\n", gamdata->mapposx[1], gamdata->mapposy[1], gamdata->mapposz[1], gamdata->mapposx[0], gamdata->mapposy[0], gamdata->mapposz[0]);
    newped->setPosition(gamdata->mapposx[1], gamdata->mapposy[1],
                        z, gamdata->mapposx[0],
                        gamdata->mapposy[0], oz);
    //newped->setPosition(gamdata->mapposx[1], gamdata->mapposy[1], gamdata->mapposz[1], 0, 0);
    newped->setDirection(dir);

    return newped;
}
