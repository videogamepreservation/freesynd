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

#ifndef WEAPON_H
#define WEAPON_H

#include <string>
#include "mapobject.h"
#include "sound/sound.h"

class WeaponInstance;

/*!
 * Weapon class.
 */
class Weapon {
public:

    WeaponInstance *createInstance();
    typedef enum {
        Persuadatron = 0,
        Pistol = 1,
        GaussGun = 2,
        Shotgun = 3,
        Uzi = 4,
        Minigun = 5,
        Laser = 6,
        Flamer = 7,
        LongRange = 8,
        Scanner = 9,
        MediKit = 10,
        TimeBomb = 11,
        AccessCard = 12,
        EnergyShield = 13,
        Unknown = 14
    } WeaponType;

    typedef enum {
        Unarmed_Anim,
        Pistol_Anim,
        Minigun_Anim,
        Flamer_Anim,
        LongRange_Anim,
        EnergyShield_Anim,
        Uzi_Anim,
        Laser_Anim,
        Gauss_Anim,
        Shotgun_Anim,
    } WeaponAnimIndex;

    Weapon(const char *w_name, int smallIcon, int bigIcon, int w_cost,
            int w_ammo, int w_range, int w_shot, int w_rank, int w_anim,
            WeaponAnimIndex w_idx, snd::InGameSample w_sample,
            WeaponType w_type, MapObject::DamageType w_dmg_type,
            int w_ammo_per_shot, int w_time_for_shot, int w_time_reload,
            unsigned int w_shot_property);

    const char *getName() { return name_.c_str(); }

    void drawSmallIcon(int x, int y);
    void drawBigIcon(int x, int y);
    void drawInfo(int x, int y);

    int cost() { return cost_; }
    int ammo() { return ammo_; }
    int range() { return range_; }
    int damagePerShot() { return damage_per_shot_; }
    int rank() { return rank_; }
    int anim() { return anim_; }
    snd::InGameSample getSound() { return sample_; }

    int selector() {
        return small_icon_ == 28 ? 1618 : small_icon_ - 14 + 1602;
    }

    WeaponAnimIndex index() { return idx_; }
    WeaponType getWeaponType() { return type_; }
    MapObject::DamageType dmgType() { return dmg_type_; }

    bool operator==(Weapon weapon) { return this->type_ == weapon.getWeaponType(); }

    int ammoPerShot() { return ammo_per_shot_; }
    int timeForShot() { return time_for_shot_; }
    int timeReload() { return time_reload_; }

    bool wasSubmittedToSearch() { return submittedToSearch_; }
    void submitToSearch() { submittedToSearch_ = true; }

    typedef enum {
        spe_None = 0,
        // can shoot only at owner
        spe_Owner = 1,
        spe_PointToPoint = 2,
        spe_PointToManyPoints = 4,
        spe_TargetReachInstant = 8,
        spe_TargetReachNeedTime = 16,
        spe_CreatesProjectile = 16,
        spe_RangeDamageOnReach = 32,
        // ignore accuracy
        spe_NoTarget = 64,
        spe_UsesAmmo = 128,
        spe_ChangeAttribute = 256,
        spe_SelfDestruction = 512,
    }ShotPropertyEnum;

    typedef enum {
        wspt_None = spe_None,
        wspt_Persuadatron = (spe_PointToPoint | spe_TargetReachInstant
            | spe_NoTarget),
        wspt_Pistol =
            (spe_PointToPoint | spe_TargetReachInstant | spe_UsesAmmo),
        wspt_GaussGun =
            (spe_PointToPoint | spe_TargetReachNeedTime | spe_UsesAmmo
            | spe_RangeDamageOnReach),
        wspt_Shotgun =
            (spe_PointToManyPoints | spe_TargetReachInstant | spe_UsesAmmo),
        wspt_Uzi = (spe_PointToPoint | spe_TargetReachInstant | spe_UsesAmmo),
        wspt_Minigun =
            (spe_PointToManyPoints | spe_TargetReachInstant | spe_UsesAmmo),
        wspt_Laser =
            (spe_PointToPoint | spe_TargetReachInstant
            | spe_RangeDamageOnReach | spe_UsesAmmo),
        wspt_Flamer =
            (spe_PointToPoint | spe_TargetReachInstant | spe_UsesAmmo),
        wspt_LongRange =
            (spe_PointToPoint | spe_TargetReachInstant | spe_UsesAmmo),
        wspt_Scanner = (spe_Owner | spe_ChangeAttribute),
        wspt_MediKit = (spe_Owner | spe_UsesAmmo),
        wspt_TimeBomb = (spe_NoTarget
            | spe_RangeDamageOnReach | spe_SelfDestruction),
        wspt_AccessCard = (spe_Owner | spe_ChangeAttribute),
        wspt_EnergyShield =
            (spe_Owner | spe_ChangeAttribute | spe_UsesAmmo),
    }WeaponShotPropertyType;

    unsigned int shotProperty() { return shot_property_; }

protected:
    std::string name_;
    int small_icon_, big_icon_;
    int cost_, ammo_, range_, damage_per_shot_;
    int anim_;
    int rank_;  //!> weapon rank
    WeaponType type_;
    MapObject::DamageType dmg_type_;
    WeaponAnimIndex idx_;
    snd::InGameSample sample_;
    int ammo_per_shot_;
    // time weapon uses to do a single shot
    int time_for_shot_;
    // time required to make weapon ready to shoot
    int time_reload_;
    /*! True when weapon was found and submit to search manager.*/
    bool submittedToSearch_;
    unsigned int shot_property_;
};

/*!
 * Weapon instance class.
 */
class WeaponInstance : public ShootableMapObject {
public:
    WeaponInstance(Weapon *w);

    int ammoRemaining() { return ammo_remaining_; }
    void setAmmoRemaining(int n) {  ammo_remaining_ = n; }

    bool animate(int elapsed);
    void draw(int x, int y);
    bool inflictDamage(ShootableMapObject * tobj, PathNode * tp,
        int elapsed = -1, bool ignoreBlocker = false);

    void setOwner(ShootableMapObject *owner) { owner_ = owner; }
    ShootableMapObject *getOwner() { return owner_; }

    Weapon *getWeaponClass() { return pWeaponClass_; }

    int range() { return pWeaponClass_->range(); }
    int ammo() { return pWeaponClass_->ammo(); }
    int rank() { return pWeaponClass_->rank(); }
    const char * name() { return pWeaponClass_->getName(); }

    Weapon::WeaponAnimIndex index() { return pWeaponClass_->index(); }
    Weapon::WeaponType getWeaponType() { return pWeaponClass_->getWeaponType(); }

    bool operator==(WeaponInstance wi) {
        return pWeaponClass_->getWeaponType() == wi.getWeaponType();
    }

    //! Plays the weapon's sound.
    void playSound();

    void resetWeaponUsedTime() { weapon_used_time_ = 0; }

    void shotTargetRandomizer(PathNode * cp, PathNode * tp, double angle);

    uint8 inRange(ShootableMapObject ** t, PathNode * pn = NULL,
        bool setBlocker = false, bool checkTileOnly = false);

    int getShots(int elapsed, int tForReload, int tForShot);

protected:
    Weapon *pWeaponClass_;
    int ammo_remaining_;
    ShootableMapObject *owner_;
    // if this value is smaller time_for_shot_ shot cannot be done 
    // if is greater then time_for_shot_ reload is in execution
    // if is greater then time_for_shot_ + time_reload_ then full shot is done
    int weapon_used_time_;
};

#endif
