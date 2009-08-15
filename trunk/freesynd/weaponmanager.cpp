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
#include "weaponmanager.h"

WeaponManager::WeaponManager() {
    weapons_.reserve(14);
}

WeaponManager::~WeaponManager() {
    for (unsigned int i = 0; i != weapons_.size(); ++i)
        delete weapons_[i];
}

void WeaponManager::loadWeapons() {
    weapons_.push_back(new Weapon("PERSUADERTRON", 14, 64, 5000, -1, 256, -1,
            -1, 367, Ped::Unarmed, Sound::PERSUADE));
    weapons_.push_back(new Weapon("PISTOL", 15, 65, 0, 13, 1280, 0, 1, 368,
            Ped::Pistol, Sound::PISTOL));
    weapons_.push_back(new Weapon("GAUSS GUN", 16, 66, 50000, 3, 5120, 15000, 0,
            369, Ped::Gauss, Sound::GAUSSGUN));
    weapons_.push_back(new Weapon("SHOTGUN", 17, 67, 250, 12, 1024, 2, 370, 2,
            Ped::Shotgun, Sound::SHOTGUN));
    weapons_.push_back(new Weapon("UZI", 18, 68, 750, 50, 1792, 2, 5, 371,
            Ped::Uzi, Sound::UZI));
    weapons_.push_back(new Weapon("MINI-GUN", 19, 69, 10000, 500, 2304, 10, 6,
            372, Ped::Minigun, Sound::MINIGUN));
    weapons_.push_back(new Weapon("LASER", 20, 70, 35000, 5, 4096, 2000, 373, 7,
            Ped::Laser, Sound::LASER));
    weapons_.push_back(new Weapon("FLAMER", 21, 71, 1500, 1000, 512, 1, 4, 374,
            Ped::Flamer, Sound::FLAME));
    weapons_.push_back(new Weapon("LONG RANGE", 22, 72, 1000, 30, 6144, 2, 3,
            375, Ped::LongRange, Sound::LONGRANGE));
    weapons_.push_back(new Weapon("SCANNER", 23, 73, 500, -1, 4096, -1, -1, 376,
            Ped::Unarmed, Sound::NO_SOUND));
    weapons_.push_back(new Weapon("MEDIKIT", 24, 74, 500, 1, 256, 1, -1, 377,
            Ped::MedKit, Sound::NO_SOUND));
    weapons_.push_back(new Weapon("TIME BOMB", 25, 75, 25000, -1, 1000, -1, -1,
            378, Ped::Unarmed, Sound::TIMEBOMB));
    weapons_.push_back(new Weapon("ACCESS CARD", 26, 76, 1000, -1, 256, -1, -1,
            379, Ped::Unarmed, Sound::NO_SOUND));
    /* NOTE: small icon 27 exists and looks like an N with an arrow above it.
       the corresponding large icon is actually the "all" button on the
       select menu.  It would appear Bullfrog was going to have another
       weapon here but it got scrapped early on and they used the large
       icon space to implement the all button.
     */
    weapons_.push_back(new Weapon("ENERGY SHIELD", 28, 78, 8000, 200, 768, 15,
            -1, 381, Ped::EnergyShield, Sound::NO_SOUND));
}

Weapon *WeaponManager::findWeapon(const char *name) {
    std::string n = name;

    for (unsigned int i = 0; i < weapons_.size(); i++) {
        if (n == weapons_[i]->name())
            return weapons_[i];
    }

    return NULL;
}

Weapon *WeaponManager::findWeapon(Ped::WeaponIndex idx) {
    for (unsigned int i = 0; i < weapons_.size(); i++) {
        if (idx == weapons_[i]->index())
            return weapons_[i];
    }

    return NULL;
}
