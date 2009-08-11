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

#ifndef PED_H
#define PED_H

#include "common.h"
#include "spritemanager.h"
#include "mapobject.h"
#include "weaponholder.h"
#include <list>

class WeaponInstance;
class PedInstance;
class Mission;
class VehicleInstance;

#define NUM_ANIMS 10

/*!
 * Pedestrian class.
 */
class Ped {
  public:
    Ped();
    virtual ~Ped() {}
    typedef enum {
        Unarmed,
        Pistol,
        Minigun,
        Flamer,
        LongRange,
        EnergyShield,
        Uzi,
        Laser,
        Gauss,
        Shotgun,
        MedKit,
    } WeaponIndex;

    void setStandAnim(WeaponIndex weapon, int anim) {
        assert(weapon < NUM_ANIMS);
        stand_anims_[weapon] = anim;
    }

    void setWalkAnim(WeaponIndex weapon, int anim) {
        assert(weapon < NUM_ANIMS);
        walk_anims_[weapon] = anim;
    }

    void setStandFireAnim(WeaponIndex weapon, int anim) {
        assert(weapon < NUM_ANIMS);
        stand_fire_anims_[weapon] = anim;
    }

    void setWalkFireAnim(WeaponIndex weapon, int anim) {
        assert(weapon < NUM_ANIMS);
        walk_fire_anims_[weapon] = anim;
    }

    void setDieAnim(int anim) {
        die_anim_ = anim;
    }

    void setDeadAnim(int anim) {
        dead_anim_ = anim;
    }

    void setHitAnim(int anim) {
        hit_anim_ = anim;
    }

    void setPickupAnim(int anim) {
        pickup_anim_ = anim;
    }

    PedInstance *createInstance(int map);

    bool drawStandFrame(int x, int y, int dir, int frame,
            WeaponIndex weapon = Unarmed);
    bool drawWalkFrame(int x, int y, int dir, int frame,
            WeaponIndex weapon = Unarmed);
    bool drawStandFireFrame(int x, int y, int dir, int frame,
            WeaponIndex weapon);
    bool drawWalkFireFrame(int x, int y, int dir, int frame,
            WeaponIndex weapon);
    bool lastStandFireFrame(int dir, int frame, WeaponIndex weapon);
    bool lastWalkFireFrame(int dir, int frame, WeaponIndex weapon);
    bool drawDieFrame(int x, int y, int frame);
    int lastDieFrame();
    void drawDeadFrame(int x, int y, int frame);
    void drawHitFrame(int x, int y, int dir, int frame);
    int lastHitFrame(int dir);
    void drawPickupFrame(int x, int y, int frame);
    int lastPickupFrame();

  protected:
    int stand_anims_[NUM_ANIMS];
    int walk_anims_[NUM_ANIMS];
    int stand_fire_anims_[NUM_ANIMS];
    int walk_fire_anims_[NUM_ANIMS];
    int die_anim_;
    int dead_anim_;
    int hit_anim_;
    int pickup_anim_;
};

/*!
 * Pedestrian instance class.
 */
class PedInstance : public ShootableMovableMapObject, public WeaponHolder {
  public:
    PedInstance(Ped *ped, int m);
    virtual ~PedInstance() {}

    void draw(int x, int y, int scrollX, int scrollY);

    void setFrame(int f) {
        frame_ = f;
    }

    void setSightRange(int new_sight_range) {
        sight_range_ = new_sight_range;
    }

    int sightRange() {
        return sight_range_;
    }

    void showPath(int scrollX, int scrollY);

    void kill();
    bool animate(int elapsed, Mission *mission);

    ShootableMapObject *target() {
        return target_;
    }

    void setTarget(ShootableMapObject *t) {
        target_ = t;
    }

    void setTarget(int x, int y) {
        target_ = 0;
        target_x_ = x;
        target_y_ = y;
    }

    void stopFiring();

    int hitDamage() {
        return hit_damage_;
    }

    void setHitDamage(int n) {
        hit_damage_ = n;
    }

    void inflictDamage(int d) {
        if (receive_damage_ == 0) {
            receive_damage_ = d;
            frame_ = 0;
        }
    }

    bool inRange(ShootableMapObject *t);
    bool inSightRange(MapObject *t);

    bool isHostile() {
        return is_hostile_;
    }

    void setHostile(bool new_hostile) {
        is_hostile_ = new_hostile;
    }

    WeaponInstance *selectedWeapon() {
        return selected_weapon_ >= 0
                && selected_weapon_ < (int) weapons_.size()
            ? weapons_[selected_weapon_] : 0;
    }

    void setSelectedWeapon(int n) {
        selected_weapon_ = n;
    }

    void selectNextWeapon();
    void selectBestWeapon();
    void dropWeapon(int n);
    void pickupWeapon(WeaponInstance *w);

    VehicleInstance *inVehicle() {
        return map_ == -1 ? in_vehicle_ : 0;
    }

    void putInVehicle(VehicleInstance *v);
    void leaveVehicle();

    int map();

  protected:
    Ped *ped_;
    bool dead_;
    enum {
        Firing_Not,
        Firing_Fire,
        Firing_Reload,
        Firing_Stop
    } firing_;
    ShootableMapObject *target_;
    int target_x_, target_y_;
    int hit_damage_;
    int receive_damage_;
    int sight_range_;
    bool is_hostile_;
    int reload_count_;
    int selected_weapon_;
    WeaponInstance *pickup_weapon_, *putdown_weapon_;
    VehicleInstance *in_vehicle_;

    bool walkable(int x, int y, int z);
};

#endif
