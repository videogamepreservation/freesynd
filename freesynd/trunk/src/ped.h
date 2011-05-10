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

#ifndef PED_H
#define PED_H

#include "common.h"
#include "mapobject.h"
#include "pathsurfaces.h"
#include "gfx/spritemanager.h"
#include "weaponholder.h"
#include "weapon.h"
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

    void setStandAnim(Weapon::WeaponAnimIndex weapon, int anim) {
        assert(weapon < NUM_ANIMS);
        stand_anims_[weapon] = anim;
    }

    void setWalkAnim(Weapon::WeaponAnimIndex weapon, int anim) {
        assert(weapon < NUM_ANIMS);
        walk_anims_[weapon] = anim;
    }

    void setStandFireAnim(Weapon::WeaponAnimIndex weapon, int anim) {
        assert(weapon < NUM_ANIMS);
        stand_fire_anims_[weapon] = anim;
    }

    void setWalkFireAnim(Weapon::WeaponAnimIndex weapon, int anim) {
        assert(weapon < NUM_ANIMS);
        walk_fire_anims_[weapon] = anim;
    }

    void setDieAgentAnim(int anim) { die_agent_anim_ = anim; }
    void setDeadAgentAnim(int anim) { dead_agent_anim_ = anim; }
    void setDieAnim(int anim) { die_anim_ = anim; }
    void setDeadAnim(int anim) { dead_anim_ = anim; }
    void setHitAnim(int anim) { hit_anim_ = anim; }
    void setPickupAnim(int anim) { pickup_anim_ = anim; }

    void setVaporizeAnim(int anim) { vaporize_anim_ = anim; }
    void setSinkAnim(int anim) { sink_anim_ = anim; }
    void setStandBurnAnim(int anim) { stand_burn_anim_ = anim; }
    void setWalkBurnAnim(int anim) { walk_burn_anim_ = anim; }
    void setDieBurnAnim(int anim) { die_burn_anim_ = anim; }
    void setSmokeBurnAnim(int anim) { smoke_burn_anim_ = anim; }
    void setDeadBurnAnim(int anim) { dead_burn_anim_ = anim; }

    void setPersuadeAnim(int anim) { persuade_anim_ = anim; }

    PedInstance *createInstance(int map);

    bool drawStandFrame(int x, int y, int dir, int frame,
            Weapon::WeaponAnimIndex weapon = Weapon::Unarmed_Anim);
    int lastStandFrame(int dir, Weapon::WeaponAnimIndex weapon);
    bool drawWalkFrame(int x, int y, int dir, int frame,
            Weapon::WeaponAnimIndex weapon = Weapon::Unarmed_Anim);
    int lastWalkFrame(int dir, Weapon::WeaponAnimIndex weapon);
    bool drawStandFireFrame(int x, int y, int dir, int frame,
            Weapon::WeaponAnimIndex weapon);
    int lastStandFireFrame(int dir, Weapon::WeaponAnimIndex weapon);
    bool drawWalkFireFrame(int x, int y, int dir, int frame,
            Weapon::WeaponAnimIndex weapon);
    int lastWalkFireFrame(int dir, Weapon::WeaponAnimIndex weapon);
    bool drawDieFrame(int x, int y, int frame);
    int lastDieFrame();

    void drawDeadFrame(int x, int y, int frame);
    void drawDeadAgentFrame(int x, int y, int frame);

    void drawHitFrame(int x, int y, int dir, int frame);
    int lastHitFrame(int dir);
    void drawPickupFrame(int x, int y, int frame);
    int lastPickupFrame();
    void drawVaporizeFrame(int x, int y, int dir, int frame);
    int lastVaporizeFrame(int dir);
    void drawSinkFrame(int x, int y, int frame);
    int lastSinkFrame();
    
    void drawStandBurnFrame(int x, int y, int frame);
    void drawWalkBurnFrame(int x, int y, int frame);
    void drawDieBurnFrame(int x, int y, int frame);
    int lastDieBurnFrame();
    void drawSmokeBurnFrame(int x, int y, int frame);
    void drawDeadBurnFrame(int x, int y, int frame);

    void drawPersuadeFrame(int x, int y, int frame);
    int lastPersuadeFrame();

protected:
    int stand_anims_[NUM_ANIMS];
    int walk_anims_[NUM_ANIMS];
    int stand_fire_anims_[NUM_ANIMS];
    int walk_fire_anims_[NUM_ANIMS];
    int die_agent_anim_;
    int dead_agent_anim_;
    int die_anim_;
    int dead_anim_;
    // has 4 directions
    int hit_anim_;
    int pickup_anim_;

    // has 4 directions
    int vaporize_anim_;
    int sink_anim_;
    int stand_burn_anim_;
    int walk_burn_anim_;
    int die_burn_anim_;
    int smoke_burn_anim_;
    int dead_burn_anim_;

    int persuade_anim_;
};

/*!
 * Pedestrian instance class.
 */
class PedInstance : public ShootableMovableMapObject, public WeaponHolder {
public:
    PedInstance(Ped *ped, int m);
    ~PedInstance();

    typedef enum {
        NoAnimation,
        HitAnim,
        DieAnim,
        DeadAnim,
        DeadAgentAnim,
        PickupAnim,
        PutdownAnim,
        WalkAnim,
        StandAnim,
        WalkFireAnim,
        StandFireAnim,
        VaporizeAnim,
        SinkAnim,
        StandBurnAnim,
        WalkBurnAnim,
        DieBurnAnim,
        SmokeBurnAnim,
        DeadBurnAnim,
    }AnimationDrawn;

    typedef enum {
        m_tpPedestrian = 0x01,
        m_tpAgent = 0x02,
        m_tpPolice = 0x04,
        m_tpGuard = 0x08,
        m_tpCriminal = 0x10,
    }mainPedType;

    void draw(int x, int y, int scrollX, int scrollY);

    void setFrame(int f) { frame_ = f; }

    void setSightRange(int new_sight_range) { sight_range_ = new_sight_range; }

    int sightRange() { return sight_range_; }

    void showPath(int scrollX, int scrollY);

    void kill();
    bool animate(int elapsed, Mission *mission);
    void drawSelectorAnim(int x, int y);

    ShootableMapObject *target() { return target_; }
    void setTarget(ShootableMapObject *t) { target_ = t; }

    void startFiring() { firing_ = PedInstance::Firing_Fire; }
    void stopFiring();

    bool inSightRange(MapObject *t);

    bool isHostile() { return is_hostile_; }
    void setHostile(bool new_hostile) { is_hostile_ = new_hostile; }

    WeaponInstance *selectedWeapon() {
        return selected_weapon_ >= 0
                && selected_weapon_ < (int) weapons_.size()
            ? weapons_[selected_weapon_] : 0;
    }

    void setSelectedWeapon(int n) {
        if (selected_weapon_ != -1)
            weapons_[selected_weapon_]->resetWeaponUsedTime();
        selected_weapon_ = n;
        if (n != -1 && weapons_[selected_weapon_]->getWeaponType() == Weapon::EnergyShield)
            setRcvDamageDef(MapObject::ddmg_PedWithEnergyShield);
        else
            setRcvDamageDef(MapObject::ddmg_Ped);
    }

    void selectNextWeapon();
    void selectBestWeapon();
    void dropWeapon(int n);
    void dropAllWeapons();
    void pickupWeapon(WeaponInstance *w);
    bool wePickupWeapon();

    VehicleInstance *inVehicle() { return map_ == -1 ? in_vehicle_ : 0; }

    void putInVehicle(VehicleInstance *v);
    void leaveVehicle();

    typedef enum {
        Not_Agent,
        Agent_Non_Active,
        Agent_Active
    } ped_enum;

    void setIsAnAgent(ped_enum set_agent_as) { is_an_agent_ = set_agent_as; }
    ped_enum isAnAgent() { return is_an_agent_; }

    int map();
    AnimationDrawn drawnAnim();
    void setDrawnAnim(AnimationDrawn drawn_anim);

    typedef struct {
        toDefineXYZ coords;
        floodPointDesc *p;
    }toSetDesc;
    typedef struct {
        unsigned short indxs;
        unsigned short n;
    }lvlNodesDesc;

    void setDestinationP(Mission *m, int x, int y, int z,
        int ox = 128, int oy = 128, int new_speed = 160);

    void addDestinationP(Mission *m, int x, int y, int z,
        int ox = 128, int oy = 128, int new_speed = 160);

    bool movementP(Mission *m, int elapsed);

    void setAllAdrenaLevels(uint8 amount, uint8 depend, uint8 effect) {
        lvl_percep_amount_ = amount;
        lvl_percep_dependency_ = depend;
        lvl_percep_effect_ = effect;
    }

    void setAllInteliLevels(uint8 amount, uint8 depend, uint8 effect) {
        lvl_percep_amount_ = amount;
        lvl_percep_dependency_ = depend;
        lvl_percep_effect_ = effect;
    }

    void setAllPercepLevels(uint8 amount, uint8 depend, uint8 effect) {
        lvl_percep_amount_ = amount;
        lvl_percep_dependency_ = depend;
        lvl_percep_effect_ = effect;
    }

    bool handleDamage(MapObject::DamageInflictType *d);
    void destroyAllWeapons();

protected:
    Ped *ped_;
    bool dead_;

    enum {
        Firing_Not,
        Firing_Fire,
        Firing_Reload,
        Firing_Stop
    } firing_;

    // 0 - not defined, 1b - stand, 2b - walking, 3b - hit, 4b - firing,
    // 5b - following, 6b - pickup/putdown, 7b - burning, 8b - in car
    unsigned int action_state_;
    // 0 - not defined, 1b - controled(persuaded), 2b - hostile (condition is
    // hostile_desc_ + enemy_groups_), 3b - armed, 4b - supporter (no
    // active action should be done, ex. persuaded ped will shoot target
    // of persuader only if persuader shoots at it), 5b - neutral (all unarmed
    // are nuetral), 6b - enemy in range, 7b - is dead, 8b - not dead, 9b - no
    // ammunition, 10b - emulates some groups, 11b - emulation failed (some
    // actions should remove emulation for group, maybe temporary), 12b - A.L.
    // controled(maybe one day we would like to do remote control on persuaded)
    unsigned int desc_state_;
    // this inherits definition from desc_state_
    // ((target checked)desc_state_ & hostile_desc_) != 0 kill him
    unsigned char hostile_desc_;
    std::set <unsigned char> enemy_groups_;
    std::set <unsigned char> emulated_groups_;
    std::set <unsigned char> emulated_failed_groups_;
    // group ped belongs to
    // 0 - neutral, 1 - our agents group, 2 - enemy agents group,
    // 3 - guards, 4 - police
    // NOTE: this will be used in A.L., guards group can be as enemy agents group,
    // but what if we would like to make guards attack only territory intruders or
    // someone who has took something on controlled surface, or attacked one of
    // guards or etc.
    unsigned char peds_group_;
    AnimationDrawn drawn_anim_;
    ShootableMapObject *target_;
    int sight_range_;
    bool is_hostile_;
    int reload_count_;
    int selected_weapon_;
    WeaponInstance *pickup_weapon_, *putdown_weapon_;
    VehicleInstance *in_vehicle_;
    ped_enum is_an_agent_;
    // IPA levels: white bar level,set level,exhaused level and forced level
    //uint8 lvl_adrena_reserve_;
    uint8 lvl_adrena_amount_;
    uint8 lvl_adrena_dependency_;
    uint8 lvl_adrena_effect_;

    //uint8 lvl_inteli_reserve_;
    uint8 lvl_inteli_amount_;
    uint8 lvl_inteli_dependency_;
    uint8 lvl_inteli_effect_;

    //uint8 lvl_percep_reserve_;
    uint8 lvl_percep_amount_;
    uint8 lvl_percep_dependency_;
    uint8 lvl_percep_effect_;

    bool walkable(int x, int y, int z);
};

#endif
