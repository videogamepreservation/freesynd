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
        ad_NoAnimation,
        ad_HitAnim,
        ad_DieAnim,
        ad_DeadAnim,
        ad_DeadAgentAnim,
        ad_PickupAnim,
        ad_PutdownAnim,
        ad_WalkAnim,
        ad_StandAnim,
        ad_WalkFireAnim,
        ad_StandFireAnim,
        ad_VaporizeAnim,
        ad_SinkAnim,
        ad_StandBurnAnim,
        ad_WalkBurnAnim,
        ad_DieBurnAnim,
        ad_SmokeBurnAnim,
        ad_DeadBurnAnim,
        ad_PersuadedAnim,
    }AnimationDrawn;

    typedef enum {
        m_tpPedestrian = 0x01,
        m_tpAgent = 0x02,
        m_tpPolice = 0x04,
        m_tpGuard = 0x08,
        m_tpCriminal = 0x10,
    }mainPedType;

    void draw(int x, int y);

    void setFrame(int f) { frame_ = f; }

    void setSightRange(int new_sight_range) { sight_range_ = new_sight_range; }

    int sightRange() { return sight_range_; }

    void showPath(int scrollX, int scrollY);

    void kill();
    bool animate(int elapsed, Mission *mission);
    void drawSelectorAnim(int x, int y);

    ShootableMapObject *target() { return target_; }
    void setTarget(ShootableMapObject *t, PathNode *pn = NULL) {
        target_ = t;
        target_pos_ = pn;
    }

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

    void setAgentIs(ped_enum set_agent_as) { agent_is_ = set_agent_as; }
    ped_enum agentIs() { return agent_is_; }

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

    bool handleDamage(ShootableMapObject::DamageInflictType *d);
    void destroyAllWeapons();

    void setActionStateMasks(unsigned int action_state) {
        action_state_ = action_state;
    }
    unsigned int actionStateMasks() { return action_state_; }

    void setDescStateMasks(unsigned int desc_state) {
        desc_state_ = desc_state;
    }
    unsigned int descStateMasks() { return desc_state_; }

    void setHostileDesc(unsigned int hostile_desc) {
        hostile_desc_ = hostile_desc;
    }
    unsigned int hostileDesc() { return hostile_desc_; }

    void setObjGroupDef(unsigned int obj_group_def) {
        obj_group_def_ = obj_group_def;
    }
    unsigned int objGroupDef() { return obj_group_def_; }

    void addEnemyGroupDef(unsigned int enemy_group_def) {
        enemy_group_defs_.insert(enemy_group_def);
    }
    void rmEnemyGroupDef(unsigned int enemy_group_def) {
        enemy_group_defs_.erase(enemy_group_def);
    }
    bool isInEnemyGroupDef(unsigned int enemy_group_def) {
        return enemy_group_defs_.find(enemy_group_def)
            != enemy_group_defs_.end();
    }

    void addEmulatedGroupDef(unsigned int emulated_group_def) {
        emulated_group_defs_.insert(emulated_group_def);
    }
    void rmEmulatedGroupDef(unsigned int emulated_group_def) {
        emulated_group_defs_.erase(emulated_group_def);
    }
    bool isInEmulatedGroupDef(std::set <unsigned int> &r_egd,
        unsigned int emulated_group_def = 0);
    bool emulatedGroupDefsEmpty() { return emulated_group_defs_.size() == 0; }

    void addHostilesFound(ShootableMapObject * hostile_found) {
        hostiles_found_.insert(hostile_found);
    }
    void rmHostilesFound(ShootableMapObject * hostile_found) {
        hostiles_found_.erase(hostile_found);
    }
    bool isInHostilesFound(ShootableMapObject * hostile_found) {
        return hostiles_found_.find(hostile_found)
            != hostiles_found_.end();
    }
    void verifyHostilesFound();

    bool checkHostileIs(ShootableMapObject *obj,
        unsigned int hostile_desc_alt = 0);
    bool checkFriendIs(PedInstance *p) {
        return (p->objGroupDef() & 0xFF) == (obj_group_def_ & 0xFF);
    }

    typedef enum {
        og_dmUndefined = 0x0,
        og_dmFriend = 0x0001,
        og_dmEnemy = 0x0002,
        og_dmNeutral = 0x0004,
        // nainPedType << 8
        og_dmPedestrian = 0x0100,
        og_dmCivilian = 0x0100,
        og_dmAgent = 0x0200,
        og_dmPolice = 0x0400,
        og_dmGuard = 0x0800,
        og_dmCriminal = 0x1000
    } objGroupDefMasks;

    typedef enum {
        pd_smUndefined = 0x0,
        pd_smControlled = 0x0001,
        pd_smArmed = 0x0002,
        // no active action should be done, ex. persuaded ped will shoot target
        // of persuader only if persuader shoots at it
        pd_smSupporter = 0x0004,
        pd_smEnemyInSight = 0x0008,
        pd_smDead = 0x0010,
        // only if all weapon has no ammunition, persuadatron excludes this
        // shoul not be used for hostile_desc_
        pd_smNoAmmunition = 0x0020,
        // all non-player controllled peds should have this set
        pd_smAutoAction = 0x0040,
        // target can be position or object
        pd_smHasShootTarget = 0x0080,
        // target can be position or object
        pd_smHasReachTarget = 0x0100
    } pedDescStateMasks;

    typedef enum {
        pa_smNone = 0x0,
        pa_smStanding = 0x0001,
        pa_smWalking = 0x0002,
        pa_smHit = 0x0004,
        pa_smFiring = 0x0008,
        pa_smFollowing = 0x0010,
        pa_smPickUp = 0x0020,
        pa_smPutDown = 0x0040,
        pa_smBurning = 0x0080,
        pa_smGetInCar = 0x0100,
        // only driver can have this set after finished
        pa_smUsingCar = 0x0200,
        // passenger only
        pa_smInCar = 0x0400,
        pa_smLeaveCar = 0x0800,
    } pedActionStateMasks;

    typedef struct {
        Mission::ObjectiveType ot_execute;
        PathNode t_pn;
        ShootableMapObject *t_smo;
        // 0 - not started, 1 - executing, 2 - finished, 3 - failed,
        // 4 - suspended
        uint8 state;
        union {
            struct {
                // (objGroupDefMasks)
                uint32 t_ogd;
                // (pedDescStateMasks)
                uint32 t_hostile_desc;
            } enemy_var;
            struct {
                int32 elapsed;
                // = -1 forever
                int32 time_total;
            } time_var;
            struct {
                int32 dir;
                int32 dist;
            } dist_var;
        } multi_var;
        uint32 condition;
    } actionQueueType;

    typedef struct {
        // action state
        uint32 as;
        std::vector <actionQueueType> actions;
        uint16 indx_last;
    } actionQueueGroupType;

    void setActQInQueue(actionQueueGroupType &as);
    bool addActQToQueue(actionQueueGroupType &as);

    void createActQStanding(actionQueueGroupType &as);
    void createActQWalking(actionQueueGroupType &as, PathNode *tpn,
        int32 dir = -1);
    void createActQHit(actionQueueGroupType &as, PathNode *tpn,
        int32 dir = -1);
    void createActQFiring(actionQueueGroupType &as, PathNode &tpn,
        ShootableMapObject *tsmo);
    void createActQFollowing(actionQueueGroupType &as,
        ShootableMapObject *tsmo);

    void createActQPickUp(actionQueueGroupType &as,
        ShootableMapObject *tsmo);
    void createActQPutDown(actionQueueGroupType &as,
        ShootableMapObject *tsmo);

    void createActQBurning(actionQueueGroupType &as);

    void createActQGetInCar(actionQueueGroupType &as,
        ShootableMapObject *tsmo);
    void createActQUsingCar(actionQueueGroupType &as, PathNode *tpn,
        ShootableMapObject *tsmo, uint32 condition);
    void createActQInCar(actionQueueGroupType &as, PathNode *tpn,
        ShootableMapObject *tsmo, uint32 condition);
    void createActQLeaveCar(actionQueueGroupType &as);

protected:
    Ped *ped_;

    enum {
        Firing_Not,
        Firing_Fire,
        Firing_Reload,
        Firing_Stop
    } firing_;

    std::vector <actionQueueGroupType> action_queue_;
    // (pedActionStateMasks)
    unsigned int action_state_;
    // (pedDescStateMasks)
    unsigned int desc_state_;
    // this inherits definition from desc_state_
    // ((target checked)desc_state_ & hostile_desc_) != 0 kill him
    unsigned int hostile_desc_;
    std::set <unsigned int> enemy_group_defs_;
    // if object is not hostile here enemy_group_defs_ and hostile_desc_ check
    // is skipped
    std::set <unsigned int> emulated_group_defs_;
    //std::set <unsigned int> emulated_failed_groups_;
    // dicovered hostiles are set here, check at end for hostile
    std::set <ShootableMapObject *> hostiles_found_;
    // defines group obj belongs to (objGroupDefMasks)
    unsigned int obj_group_def_;
    unsigned int old_obj_group_def_;

    // not used, within a group identification number
    // NOTE: this will be used in A.L., guards group can be as enemy agents group,
    // but what if we would like to make guards attack only territory intruders or
    // someone who has took something on controlled surface, or attacked one of
    // guards or etc.
    unsigned int obj_group_def_id_;
    unsigned int old_obj_group_def_id_;
    // not used

    AnimationDrawn drawn_anim_;

    // target*, if in range movement should stop
    ShootableMapObject *target_;
    PathNode *target_pos_;
    // reach*, wiil only stop when at same or desired distance(undefined for now)
    ShootableMapObject *reach_obj_;
    PathNode *reach_pos_;

    int sight_range_;
    bool is_hostile_;
    int reload_count_;
    int selected_weapon_;
    WeaponInstance *pickup_weapon_, *putdown_weapon_;
    VehicleInstance *in_vehicle_;
    ped_enum agent_is_;
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
