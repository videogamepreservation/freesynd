/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2010  Bohdan Stelmakh <chamel@users.sourceforge.net> *
 *   Copyright (C) 2011  Benoit Blancard <benblan@users.sourceforge.net>*
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
    
}

void WeaponManager::destroy() {
    // Delete all weapons from the cache
    for (unsigned int i = 0; i != preFetch_.size(); ++i) {
       delete preFetch_[i];
    }

    // Then delete all available weapons
    for (unsigned i = 0; i != availableWeapons_.size(); ++i) {
        delete availableWeapons_.get(i);
    }

    preFetch_.clear();
    availableWeapons_.clear();
}

WeaponManager::~WeaponManager() {
    destroy();
}

void WeaponManager::reset() {
    destroy();

    // Enables default weapons
    enableWeapon(Weapon::Persuadatron);
    enableWeapon(Weapon::Pistol);
    enableWeapon(Weapon::Shotgun);
    enableWeapon(Weapon::Scanner);
    enableWeapon(Weapon::MediKit);
}

void WeaponManager::cheatEnableAllWeapons() {
    enableWeapon(Weapon::Persuadatron);
    enableWeapon(Weapon::Pistol);
    enableWeapon(Weapon::Shotgun);
    enableWeapon(Weapon::Uzi);
    enableWeapon(Weapon::Scanner);
    enableWeapon(Weapon::MediKit);
    enableWeapon(Weapon::Minigun);
    enableWeapon(Weapon::Flamer);
    enableWeapon(Weapon::LongRange);
    enableWeapon(Weapon::EnergyShield);
    enableWeapon(Weapon::Laser);
    enableWeapon(Weapon::GaussGun);
    enableWeapon(Weapon::AccessCard);
    enableWeapon(Weapon::TimeBomb);
}

void WeaponManager::enableWeapon(Weapon::WeaponType wt) {
    // First check if weapon is not already available
    for (unsigned i = 0; i != availableWeapons_.size(); ++i) {
        if (wt == availableWeapons_.get(i)->getWeaponType())
            return;
    }

    // Then get weapon
    Weapon *pWeapon = getWeapon(wt);

    // removes it from cache
    for (std::vector < Weapon * >::iterator it = preFetch_.begin();
         it != preFetch_.end(); it++) {
             if (pWeapon == *it) {
                preFetch_.erase(it);
                break;
             }
    }

    // make it available
    for (unsigned i = 0; i != availableWeapons_.size(); ++i) {
        if (pWeapon->getWeaponType() < availableWeapons_.get(i)->getWeaponType()) {
            // The new weapon is inserted in the order of the WeaponType constants
            availableWeapons_.insertAt(i, pWeapon);
            return;
        }
    }
    // If we're here, it's because the new weapon comes at the end of the list
    availableWeapons_.add(pWeapon);
}

Weapon * WeaponManager::getWeapon(Weapon::WeaponType wt) {
    // Search in prefetched weapons first
    for (unsigned int i = 0; i < preFetch_.size(); i++) {
        if (wt == preFetch_[i]->getWeaponType())
            return preFetch_[i];
    }

    // Then search in available weapons
    for (unsigned i = 0; i != availableWeapons_.size(); ++i) {
        if (wt == availableWeapons_.get(i)->getWeaponType())
            return availableWeapons_.get(i);
    }

    // Weapon was not found so loads it
    Weapon *pWeapon = loadWeapon(wt);
    // Stores it in cache
    preFetch_.push_back(pWeapon);
    
    return pWeapon;
}

bool WeaponManager::isAvailable(Weapon *pWeapon) {
    // search in available weapons
    for (unsigned i = 0; i != availableWeapons_.size(); ++i) {
        if (pWeapon->getWeaponType() == availableWeapons_.get(i)->getWeaponType())
            return true;
    }

    // No weapon of that type has been found
    return false;
}

Weapon * WeaponManager::loadWeapon(Weapon::WeaponType wt) {
    /* NOTE: small icon 27 exists and looks like an N with an arrow above it.
       the corresponding large icon is actually the "all" button on the
       select menu.  It would appear Bullfrog was going to have another
       weapon here but it got scrapped early on and they used the large
       icon space to implement the all button.
     */
    Weapon *pWeapon = NULL;
    switch(wt) {
        case Weapon::Persuadatron:
            pWeapon = new Weapon("PERSUADERTRON", 14, 64, 5000, -1, 256, 0,
        -1, 367, Weapon::Unarmed_Anim, snd::PERSUADE, Weapon::Persuadatron,
        MapObject::dmg_Mental, 1, 1);
            break;
        case Weapon::Pistol:
            pWeapon = new Weapon("PISTOL", 15, 65, 0, 13, 1280, 1, 1, 368,
        Weapon::Pistol_Anim, snd::PISTOL, Weapon::Pistol,
        MapObject::dmg_Bullet, 1, 1);
            break;
        case Weapon::Minigun:
            pWeapon = new Weapon("MINI-GUN", 19, 69, 10000, 500, 2304, 10, 6,
        372, Weapon::Minigun_Anim, snd::MINIGUN, Weapon::Minigun,
        MapObject::dmg_Bullet, 1, 1);
            break;
        case Weapon::Flamer:
            pWeapon = new Weapon("FLAMER", 21, 71, 1500, 1000, 512, 1, 4, 374,
        Weapon::Flamer_Anim, snd::FLAME, Weapon::Flamer, MapObject::dmg_Burn,
        1, 1);
            break;
        case Weapon::LongRange:
            pWeapon = new Weapon("LONG RANGE", 22, 72, 1000, 30, 6144, 2, 3,
        375, Weapon::LongRange_Anim, snd::LONGRANGE, Weapon::LongRange,
        MapObject::dmg_Bullet, 1, 1);
            break;
        case Weapon::EnergyShield:
            pWeapon = new Weapon("ENERGY SHIELD", 28, 78, 8000, 200, 768, 15,
        -1, 381, Weapon::EnergyShield_Anim, snd::NO_SOUND,
        Weapon::EnergyShield, MapObject::dmg_None, 1, 1);
            break;
        case Weapon::Uzi:
            pWeapon = new Weapon("UZI", 18, 68, 750, 50, 1792, 2, 5, 371,
        Weapon::Uzi_Anim, snd::UZI, Weapon::Uzi, MapObject::dmg_Bullet, 1, 1);
            break;
        case Weapon::Laser:
            pWeapon = new Weapon("LASER", 20, 70, 35000, 5, 4096, 2000, 7, 373,
        Weapon::Laser_Anim, snd::LASER, Weapon::Laser, MapObject::dmg_Laser,
        1, 1);
            break;
        case Weapon::GaussGun:
            pWeapon = new Weapon("GAUSS GUN", 16, 66, 50000, 3, 5120, 15000, 0,
        369, Weapon::Gauss_Anim, snd::GAUSSGUN, Weapon::GaussGun,
        MapObject::dmg_Explosion, 1, 1);
            break;
        case Weapon::Shotgun:
            pWeapon = new Weapon("SHOTGUN", 17, 67, 250, 12, 1024, 2, 2, 370,
        Weapon::Shotgun_Anim, snd::SHOTGUN, Weapon::Shotgun,
        MapObject::dmg_Bullet, 1, 1);
            break;
        case Weapon::MediKit:
            pWeapon = new Weapon("MEDIKIT", 24, 74, 500, 1, 256, 1, -1, 377,
        Weapon::Unarmed_Anim, snd::NO_SOUND, Weapon::MediKit,
        MapObject::dmg_Heal, 1, 1);
            break;
        case Weapon::Scanner:
            pWeapon = new Weapon("SCANNER", 23, 73, 500, -1, 4096, 0, -1, 376,
        Weapon::Unarmed_Anim, snd::NO_SOUND, Weapon::Scanner,
        MapObject::dmg_None, 1, 1);
            break;
        case Weapon::AccessCard:
            pWeapon = new Weapon("ACCESS CARD", 26, 76, 1000, -1, 256, 0, -1,
        379, Weapon::Unarmed_Anim, snd::NO_SOUND, Weapon::AccessCard,
        MapObject::dmg_None, 1, 1);
            break;
        case Weapon::TimeBomb:
            pWeapon = new Weapon("TIME BOMB", 25, 75, 25000, -1, 1000, 0, -1,
        378, Weapon::Unarmed_Anim, snd::TIMEBOMB, Weapon::TimeBomb,
        MapObject::dmg_Explosion, 1, 1);
            break;
        default:
            break;
    }

    return pWeapon;
}
