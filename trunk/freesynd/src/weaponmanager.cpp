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
#include "weaponmanager.h"

WeaponManager::WeaponManager() {
    weapons_.reserve(14);
}

WeaponManager::~WeaponManager() {
    for (unsigned int i = 0; i != weapons_.size(); ++i)
        delete weapons_[i];
}

void WeaponManager::loadWeapons() {
    weapons_.push_back(new Weapon("PERSUADERTRON", 14, 64, 5000, -1, 256, 0,
        -1, 367, Weapon::Unarmed_Anim, snd::PERSUADE, Weapon::Persuadatron,
        MapObject::dmg_Mental, 1, 1));
    weapons_.push_back(new Weapon("PISTOL", 15, 65, 0, 13, 1280, 1, 1, 368,
        Weapon::Pistol_Anim, snd::PISTOL, Weapon::Pistol,
        MapObject::dmg_Bullet, 1, 1));
    weapons_.push_back(new Weapon("GAUSS GUN", 16, 66, 50000, 3, 5120, 15000, 0,
        369, Weapon::Gauss_Anim, snd::GAUSSGUN, Weapon::GaussGun,
        MapObject::dmg_Explosion, 1, 1));
    weapons_.push_back(new Weapon("SHOTGUN", 17, 67, 250, 12, 1024, 2, 2, 370,
        Weapon::Shotgun_Anim, snd::SHOTGUN, Weapon::Shotgun,
        MapObject::dmg_Bullet, 1, 1));
    weapons_.push_back(new Weapon("UZI", 18, 68, 750, 50, 1792, 2, 5, 371,
        Weapon::Uzi_Anim, snd::UZI, Weapon::Uzi, MapObject::dmg_Bullet, 1, 1));
    weapons_.push_back(new Weapon("MINI-GUN", 19, 69, 10000, 500, 2304, 10, 6,
        372, Weapon::Minigun_Anim, snd::MINIGUN, Weapon::Minigun,
        MapObject::dmg_Bullet, 1, 1));
    weapons_.push_back(new Weapon("LASER", 20, 70, 35000, 5, 4096, 2000, 7, 373,
        Weapon::Laser_Anim, snd::LASER, Weapon::Laser, MapObject::dmg_Laser,
        1, 1));
    weapons_.push_back(new Weapon("FLAMER", 21, 71, 1500, 1000, 512, 1, 4, 374,
        Weapon::Flamer_Anim, snd::FLAME, Weapon::Flamer, MapObject::dmg_Fire,
        1, 1));
    weapons_.push_back(new Weapon("LONG RANGE", 22, 72, 1000, 30, 6144, 2, 3,
        375, Weapon::LongRange_Anim, snd::LONGRANGE, Weapon::LongRange,
        MapObject::dmg_Bullet, 1, 1));
    weapons_.push_back(new Weapon("SCANNER", 23, 73, 500, -1, 4096, 0, -1, 376,
        Weapon::Unarmed_Anim, snd::NO_SOUND, Weapon::Scanner,
        MapObject::dmg_None, 1, 1));
    weapons_.push_back(new Weapon("MEDIKIT", 24, 74, 500, 1, 256, 0, -1, 377,
        Weapon::Unarmed_Anim, snd::NO_SOUND, Weapon::MediKit,
        MapObject::dmg_Heal, 1, 1));
    weapons_.push_back(new Weapon("TIME BOMB", 25, 75, 25000, -1, 1000, 0, -1,
        378, Weapon::Unarmed_Anim, snd::TIMEBOMB, Weapon::TimeBomb,
        MapObject::dmg_Explosion, 1, 1));
    weapons_.push_back(new Weapon("ACCESS CARD", 26, 76, 1000, -1, 256, 0, -1,
        379, Weapon::Unarmed_Anim, snd::NO_SOUND, Weapon::AccessCard,
        MapObject::dmg_None, 1, 1));
    /* NOTE: small icon 27 exists and looks like an N with an arrow above it.
       the corresponding large icon is actually the "all" button on the
       select menu.  It would appear Bullfrog was going to have another
       weapon here but it got scrapped early on and they used the large
       icon space to implement the all button.
     */
    weapons_.push_back(new Weapon("ENERGY SHIELD", 28, 78, 8000, 200, 768, 15,
        -1, 381, Weapon::EnergyShield_Anim, snd::NO_SOUND,
        Weapon::EnergyShield, MapObject::dmg_None, 1, 1));
}

Weapon *WeaponManager::findWeapon(const char *name) {
    std::string n = name;

    for (unsigned int i = 0; i < weapons_.size(); i++) {
        if (n == weapons_[i]->name())
            return weapons_[i];
    }

    return NULL;
}

Weapon *WeaponManager::findWeapon(Weapon::WeaponType wt) {
    for (unsigned int i = 0; i < weapons_.size(); i++) {
        if (wt == weapons_[i]->getWeaponType())
            return weapons_[i];
    }

    return NULL;
}

WeaponInstance *WeaponManager::loadInstance(uint8 * data, int map)
{
    Mission::LEVELDATA_WEAPONS * gamdata =
        (Mission::LEVELDATA_WEAPONS *) data;
    Weapon *w = 0;

    switch (gamdata->sub_type) {
        case 0x01:
            w = findWeapon(Weapon::Persuadatron);
            break;
        case 0x02:
            w = findWeapon(Weapon::Pistol);
            break;
        case 0x03:
            w = findWeapon(Weapon::GaussGun);
            break;
        case 0x04:
            w = findWeapon(Weapon::Shotgun);
            break;
        case 0x05:
            w = findWeapon(Weapon::Uzi);
            break;
        case 0x06:
            w = findWeapon(Weapon::Minigun);
            break;
        case 0x07:
            w = findWeapon(Weapon::Laser);
            break;
        case 0x08:
            w = findWeapon(Weapon::Flamer);
            break;
        case 0x09:
            w = findWeapon(Weapon::LongRange);
            break;
        case 0x0A:
            w = findWeapon(Weapon::Scanner);
            break;
        case 0x0B:
            w = findWeapon(Weapon::MediKit);
            break;
        case 0x0C:
            w = findWeapon(Weapon::TimeBomb);
            break;
        case 0x0D:
            w = findWeapon(Weapon::AccessCard);
            break;
        case 0x11:
            w = findWeapon(Weapon::EnergyShield);
            break;
        default:
            break;
    }
    if (w) {
        WeaponInstance *wi = w->createInstance();
        wi->setAmmoRemaining(wi->ammo());
        int z = READ_LE_UINT16(gamdata->mapposz) >> 7;
        z--;
        int oz = gamdata->mapposz[0] & 0x7F;
        wi->setVisZ(z);
        if (oz > 0)
            z++;
        //printf("x %i y %i z %i ox %i oy %i oz %i\n", gamdata->mapposx[1], gamdata->mapposy[1], z, gamdata->mapposx[0], gamdata->mapposy[0], oz);
        wi->setPosition(gamdata->mapposx[1], gamdata->mapposy[1],
                            z, gamdata->mapposx[0],
                            gamdata->mapposy[0], oz);
        return wi;
    }

    return NULL;
}
