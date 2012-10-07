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
 *   Copyright (C) 2011  Joey Parrish  <joey.parrish@gmail.com>         *
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

WeaponManager::WeaponManager() {
    all_game_weapons_.reserve(18);
}

void WeaponManager::destroy() {
    preFetch_.clear();
    availableWeapons_.clear();
}

WeaponManager::~WeaponManager() {
    destroy();
    for (std::vector<Weapon *>::iterator it = all_game_weapons_.begin();
        it != all_game_weapons_.end();it++)
    {
        delete (*it);
    }
    all_game_weapons_.clear();
        
}

void WeaponManager::reset() {
    destroy();

    all_game_weapons_.clear();
    all_game_weapons_.push_back(loadWeapon(Weapon::Persuadatron));
    all_game_weapons_.push_back(loadWeapon(Weapon::Pistol));
    all_game_weapons_.push_back(loadWeapon(Weapon::Shotgun));
    all_game_weapons_.push_back(loadWeapon(Weapon::Uzi));
    all_game_weapons_.push_back(loadWeapon(Weapon::Scanner));
    all_game_weapons_.push_back(loadWeapon(Weapon::MediKit));
    all_game_weapons_.push_back(loadWeapon(Weapon::Minigun));
    all_game_weapons_.push_back(loadWeapon(Weapon::Flamer));
    all_game_weapons_.push_back(loadWeapon(Weapon::LongRange));
    all_game_weapons_.push_back(loadWeapon(Weapon::EnergyShield));
    all_game_weapons_.push_back(loadWeapon(Weapon::Laser));
    all_game_weapons_.push_back(loadWeapon(Weapon::GaussGun));
    all_game_weapons_.push_back(loadWeapon(Weapon::AccessCard));
    all_game_weapons_.push_back(loadWeapon(Weapon::TimeBomb));

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
    Weapon *pWeapon = NULL;
    
    for (std::vector<Weapon *>::iterator it = all_game_weapons_.begin();
        it != all_game_weapons_.end();it++)
    {
        if ((*it)->getWeaponType() == wt) {
            pWeapon = *it;
            break;
        }
    }
    assert(pWeapon);
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
    // TODO:calibrate weapon time for shot and reload, angle, accuracy
    Weapon *pWeapon = NULL;
    switch(wt) {
        case Weapon::Persuadatron:
            pWeapon = new Weapon(g_App.menus().getMessage("WEAPON_PERSUADERTRON"),
        14, 64, 5000, -1, 256, -1,
        -1, 367, Weapon::Unarmed_Anim, snd::PERSUADE, Weapon::Persuadatron,
        MapObject::dmg_Mental, 1, 150, 25, Weapon::wspt_Persuadatron,
        SFXObject::sfxt_Unknown, SFXObject::sfxt_Unknown,
        SFXObject::sfxt_Unknown, SFXObject::sfxt_Unknown, 0, 1.0, 1.0);
            break;
        case Weapon::Pistol:
            pWeapon = new Weapon(g_App.menus().getMessage("WEAPON_PISTOL"),
        15, 65, 0, 13, 1280, 1, 1, 368,
        Weapon::Pistol_Anim, snd::PISTOL, Weapon::Pistol,
        MapObject::dmg_Bullet, 1, 200, 600, Weapon::wspt_Pistol,
        SFXObject::sfxt_BulletHit, SFXObject::sfxt_BulletHit,
        SFXObject::sfxt_Unknown, SFXObject::sfxt_Unknown, 0, 5.0, 0.9);
            break;
        case Weapon::Minigun:
            pWeapon = new Weapon(g_App.menus().getMessage("WEAPON_MINI_GUN"),
        19, 69, 10000, 500, 2304, 10, 6,
        372, Weapon::Minigun_Anim, snd::MINIGUN, Weapon::Minigun,
        MapObject::dmg_Bullet, 5, 100, 75, Weapon::wspt_Minigun,
        SFXObject::sfxt_BulletHit, SFXObject::sfxt_BulletHit,
        SFXObject::sfxt_Unknown, SFXObject::sfxt_Unknown, 0, 45.0, 0.5, 0, 4);
            break;
        case Weapon::Flamer:
            // NOTE: range changed from 512 to 1152
            pWeapon = new Weapon(g_App.menus().getMessage("WEAPON_FLAMER"),
        21, 71, 1500, 1000, 1152, 1, 4, 374,
        Weapon::Flamer_Anim, snd::FLAME, Weapon::Flamer, MapObject::dmg_Burn,
        10, 70, 50, Weapon::wspt_Flamer, SFXObject::sfxt_FlamerFire,
        SFXObject::sfxt_FlamerFire, SFXObject::sfxt_FlamerFire,
        SFXObject::sfxt_Unknown, 0, 2.0, 0.9, 2200);
            break;
        case Weapon::LongRange:
            pWeapon = new Weapon(g_App.menus().getMessage("WEAPON_LONG_RANGE"),
        22, 72, 1000, 30, 6144, 2, 3,
        375, Weapon::LongRange_Anim, snd::LONGRANGE, Weapon::LongRange,
        MapObject::dmg_Bullet, 1, 900, 400, Weapon::wspt_LongRange,
        SFXObject::sfxt_BulletHit, SFXObject::sfxt_BulletHit,
        SFXObject::sfxt_Unknown, SFXObject::sfxt_Unknown, 0, 1.0, 1.0, 0, 2);
            break;
        case Weapon::EnergyShield:
            pWeapon = new Weapon(g_App.menus().getMessage("WEAPON_ENERGY_SHIELD"),
        28, 78, 8000, 200, 768, 15,
        -1, 381, Weapon::EnergyShield_Anim, snd::NO_SOUND,
        Weapon::EnergyShield, MapObject::dmg_None, 1, 75, 50,
        Weapon::wspt_EnergyShield, SFXObject::sfxt_Unknown,
        SFXObject::sfxt_Unknown, SFXObject::sfxt_Unknown,
        SFXObject::sfxt_Unknown, 0, 0.0, 0.0);
            break;
        case Weapon::Uzi:
            pWeapon = new Weapon(g_App.menus().getMessage("WEAPON_UZI"),
        18, 68, 750, 50, 1792, 2, 5, 371,
        Weapon::Uzi_Anim, snd::UZI, Weapon::Uzi, MapObject::dmg_Bullet, 1,
        150, 100, Weapon::wspt_Uzi, SFXObject::sfxt_BulletHit,
        SFXObject::sfxt_BulletHit, SFXObject::sfxt_Unknown,
        SFXObject::sfxt_Unknown, 0, 20.0, 0.7, 0, 2);
            break;
        case Weapon::Laser:
            pWeapon = new Weapon(g_App.menus().getMessage("WEAPON_LASER"),
        20, 70, 35000, 5, 4096, 2000, 7, 373,
        Weapon::Laser_Anim, snd::LASER, Weapon::Laser, MapObject::dmg_Laser,
        1, 300, 200, Weapon::wspt_Laser, SFXObject::sfxt_Fire_LongSmoke,
        SFXObject::sfxt_Unknown, SFXObject::sfxt_Unknown,
        SFXObject::sfxt_Unknown, 512, 5.0, 0.99, 0, 32);
            break;
        case Weapon::GaussGun:
            pWeapon = new Weapon(g_App.menus().getMessage("WEAPON_GAUSS_GUN"),
        16, 66, 50000, 3, 5120, 15000, 0,
        369, Weapon::Gauss_Anim, snd::GAUSSGUN, Weapon::GaussGun,
        MapObject::dmg_Explosion, 1, 350, 1500, Weapon::wspt_GaussGun,
        SFXObject::sfxt_ExplosionFire,
        SFXObject::sfxt_ExplosionBall, SFXObject::sfxt_Smoke,
        SFXObject::sfxt_LargeFire, 512, 5.0, 0.99, 6000, 64);
            break;
        case Weapon::Shotgun:
            pWeapon = new Weapon(g_App.menus().getMessage("WEAPON_SHOTGUN"),
        17, 67, 250, 12, 1024, 2, 2, 370,
        Weapon::Shotgun_Anim, snd::SHOTGUN, Weapon::Shotgun,
        MapObject::dmg_Bullet, 2, 250, 200, Weapon::wspt_Shotgun,
        SFXObject::sfxt_BulletHit, SFXObject::sfxt_BulletHit,
        SFXObject::sfxt_Unknown, SFXObject::sfxt_Unknown, 0, 60.0, 0.7, 0, 2);
            break;
        case Weapon::MediKit:
            pWeapon = new Weapon(g_App.menus().getMessage("WEAPON_MEDIKIT"),
        24, 74, 500, 1, 256, 1, -1, 377,
        Weapon::Unarmed_Anim, snd::NO_SOUND, Weapon::MediKit,
        MapObject::dmg_Heal, 1, 1, 1, Weapon::wspt_MediKit,
        SFXObject::sfxt_Unknown, SFXObject::sfxt_Unknown,
        SFXObject::sfxt_Unknown, SFXObject::sfxt_Unknown, 0, 0.0, 0.0);
            break;
        case Weapon::Scanner:
            pWeapon = new Weapon(g_App.menus().getMessage("WEAPON_SCANNER"),
        23, 73, 500, -1, 4096, -1, -1, 376,
        Weapon::Unarmed_Anim, snd::NO_SOUND, Weapon::Scanner,
        MapObject::dmg_None, 1, 1, 1, Weapon::wspt_Scanner,
        SFXObject::sfxt_Unknown, SFXObject::sfxt_Unknown,
        SFXObject::sfxt_Unknown, SFXObject::sfxt_Unknown, 0, 0.0, 0.0);
            break;
        case Weapon::AccessCard:
            pWeapon = new Weapon(g_App.menus().getMessage("WEAPON_ACCESS_CARD"),
        26, 76, 1000, -1, 256, -1, -1,
        379, Weapon::Unarmed_Anim, snd::NO_SOUND, Weapon::AccessCard,
        MapObject::dmg_None, 1, 1, 1, Weapon::wspt_AccessCard,
        SFXObject::sfxt_Unknown, SFXObject::sfxt_Unknown,
        SFXObject::sfxt_Unknown, SFXObject::sfxt_Unknown, 0, 0.0, 0.0);
            break;
        case Weapon::TimeBomb:
        // NOTE: what is real number for "shot"? for now = 7500
            pWeapon = new Weapon(g_App.menus().getMessage("WEAPON_TIME_BOMB"),
        25, 75, 25000, -1, 1000, 7500, -1,
        378, Weapon::Unarmed_Anim, snd::EXPLOSION, Weapon::TimeBomb,
        MapObject::dmg_Explosion, 1, 15000, 1, Weapon::wspt_TimeBomb,
        SFXObject::sfxt_ExplosionFire, SFXObject::sfxt_ExplosionBall,
        SFXObject::sfxt_Unknown, SFXObject::sfxt_ExplosionFire, 512, 0.0, 0.0,
        0, 64);
            break;
#if _DEBUG
        default:
            printf("unknown weapon loaded(%i), NULL passed", wt);
            break;
#endif
    }

    return pWeapon;
}

bool WeaponManager::saveToFile(PortableFile &file) {
    file.write32(availableWeapons_.size());

    for(unsigned int i=0; i<availableWeapons_.size(); i++) {
        Weapon *pWeapon = availableWeapons_.get(i);
        file.write32(pWeapon->getWeaponType());
    }

    return true;
}

bool WeaponManager::loadFromFile(PortableFile &infile, const FormatVersion& v) {
    int nbWeap = infile.read32();

    for (int i = 0; i < nbWeap; i++) {
        int type = infile.read32();
        Weapon::WeaponType wt = Weapon::Unknown;
        switch (type) {
            case Weapon::Persuadatron:
                wt = Weapon::Persuadatron;
                break;
            case Weapon::Pistol:
                wt = Weapon::Pistol;
                break;
            case Weapon::GaussGun:
                wt = Weapon::GaussGun;
                break;
            case Weapon::Shotgun:
                wt = Weapon::Shotgun;
                break;
            case Weapon::Uzi:
                wt = Weapon::Uzi;
                break;
            case Weapon::Minigun:
                wt = Weapon::Minigun;
                break;
            case Weapon::Laser:
                wt = Weapon::Laser;
                break;
            case Weapon::Flamer:
                wt = Weapon::Flamer;
                break;
            case Weapon::LongRange:
                wt = Weapon::LongRange;
                break;
            case Weapon::Scanner:
                wt = Weapon::Scanner;
                break;
            case Weapon::MediKit:
                wt = Weapon::MediKit;
                break;
            case Weapon::TimeBomb:
                wt = Weapon::TimeBomb;
                break;
            case Weapon::AccessCard:
                wt = Weapon::AccessCard;
                break;
            case Weapon::EnergyShield:
                wt = Weapon::EnergyShield;
                break;
            default:
                wt = Weapon::Unknown;
        }
        // enable weapon
        enableWeapon(wt);
    }

    return true;
}

bool WeaponManager::checkDmgTypeCanShootStrict(MapObject::DamageType dmg, bool &can_shoot)
{
    bool found = false;
    can_shoot = false;

    for (std::vector<Weapon *>::iterator it = all_game_weapons_.begin();
        it != all_game_weapons_.end();it++)
    {
        if ((*it)->dmgType() == dmg) {
            found = true;
            if (((*it)->shotProperty() & Weapon::spe_CanShoot) != 0)
                can_shoot = true;
        }
    }

    return found;
}

bool WeaponManager::checkDmgTypeCanShootNonStrict(MapObject::DamageType dmg, bool &can_shoot)
{
    bool found = false;
    can_shoot = false;

    for (std::vector<Weapon *>::iterator it = all_game_weapons_.begin();
        it != all_game_weapons_.end();it++)
    {
        if (((*it)->dmgType() & dmg) != 0) {
            found = true;
            if ((*it)->canShoot())
                can_shoot = true;
        }
    }

    return found;

}

