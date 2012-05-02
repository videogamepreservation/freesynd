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

#include <map>
#include <list>

#include "common.h"
#include "mapobject.h"
#include "pathsurfaces.h"
#include "gfx/spritemanager.h"
#include "weaponholder.h"
#include "weapon.h"
#include "mission.h"

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
    void switchActionStateTo(uint32 as);
    void switchActionStateFrom(uint32 as);
    void setActionStateToDrawnAnim(void);
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

    void setSelectedWeapon(int n);

    void selectNextWeapon();
    void selectBestWeapon();
    void dropWeapon(int n);
    void dropWeapon(WeaponInstance *w);
    void dropAllWeapons();
    void pickupWeapon(WeaponInstance *w);
    bool wePickupWeapon();

    VehicleInstance *inVehicle();

    void putInVehicle(VehicleInstance *v);
    void leaveVehicle();
    bool setVehicleIgnore(bool ignored);

    typedef enum {
        Not_Agent,
        Agent_Non_Active,
        Agent_Active
    } agentAndNonEnum;

    void setAgentIs(agentAndNonEnum set_agent_as) { agent_is_ = set_agent_as; }
    agentAndNonEnum agentIs() { return agent_is_; }
    // our agent
    bool isOurAgent() { return agent_is_ == Agent_Active; }

    int map();
    AnimationDrawn drawnAnim(void);
    void setDrawnAnim(AnimationDrawn drawn_anim);
    bool handleDrawnAnim(int elapsed);

    typedef struct {
        toDefineXYZ coords;
        floodPointDesc *p;
    } toSetDesc;
    typedef struct {
        unsigned short indxs;
        unsigned short n;
    } lvlNodesDesc;

    void setDestinationP(Mission *m, int x, int y, int z,
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

    void setObjGroupID(unsigned int obj_group_id) {
        obj_group_id_ = obj_group_id;
    }
    unsigned int objGroupID() { return obj_group_id_; }

    class Mmuu32_t: public std::multimap<uint32, uint32> {
    public:
        Mmuu32_t() {}
        ~Mmuu32_t() {}
        void rm(uint32 first, uint32 second = 0) {
            Mmuu32_t::iterator it = this->find(first);
            if (it == this->end())
                return;
            if (second == 0) {
                // removing all of this id
                Mmuu32_t::iterator its = it;
                do {
                    it++;
                } while (it->first == first && it != this->end());
                this->erase(its, it);
            } else {
                do {
                    if (it->second == second) {
                        this->erase(it);
                        break;
                    }
                    it++;
                } while (it->first == first && it != this->end());
            }
        }
        void add(uint32 first, uint32 second = 0) {
            Mmuu32_t::iterator it = this->find(first);
            if (it != this->end()) {
                if (second == 0) {
                    if (it->second != 0) {
                        // non-zeros should be removed, second value equal zero
                        // should be the only present
                        this->rm(first);
                        this->insert(std::pair<uint32, uint32>(first, second));
                    }
                } else {
                    bool found = false;
                    do {
                        if (it->first != first)
                            break;
                        if (it->second == second) {
                            found = true;
                            break;
                        }
                        it++;
                    } while (it != this->end());
                    if (!found)
                        this->insert(std::pair<uint32, uint32>(first, second));
                }
            } else
                this->insert(std::pair<uint32, uint32>(first, second));
        }
        bool isIn(uint32 first, uint32 second = 0) {
            Mmuu32_t::iterator it = this->find(first);
            bool found = false;
            if (it != this->end()) {
                if (second == 0 || it->second == 0)
                    found = true;
                else {
                    do {
                        if (it->second == second) {
                            found = true;
                            break;
                        }
                        it++;
                    } while (it != this->end() && it->first == first);
                }
            }
            return found;
        }
        bool isIn_KeyOnly(Mmuu32_t &mm) {
            if (mm.empty() || this->empty())
                return false;
            bool found = false;
            Mmuu32_t::iterator it_this = this->begin();
            uint32 first_value = it_this->first;
            do {
                Mmuu32_t::iterator it_mm = mm.find(first_value);
                found = it_mm != mm.end();
                if (found)
                    break;
                if (it_this->first != first_value)
                    first_value = it_this->first;
                else {
                    it_this++;
                    continue;
                }
            } while (it_this != this->end());
            return found;
        }
        bool isIn_All(Mmuu32_t &mm) {
            if (mm.empty() || this->empty())
                return false;
            bool found = false;
            Mmuu32_t::iterator it_this = this->begin();
            uint32 first_value = it_this->first;
            uint32 second_value = it_this->second;
            do {
                Mmuu32_t::iterator it_mm = mm.find(first_value);
                found = it_mm != mm.end();
                if (found) {
                    if (second_value == 0 || it_mm->second == 0)
                        break;
                    std::pair<Mmuu32_t::iterator, Mmuu32_t::iterator> rng =
                        mm.equal_range(first_value);
                    found = false;
                    for (Mmuu32_t::iterator it = rng.first;
                        it != rng.second; it++)
                    {
                        if (it->second == second_value) {
                            found = true;
                            break;
                        }
                    }
                }
                if (!found) {
                    if (it_this->first != first_value) {
                        first_value = it_this->first;
                        second_value = it_this->second;
                    } else {
                        if (it_this->second != second_value)
                            second_value = it_this->second;
                        it_this++;
                        continue;
                    }
                }
            } while (it_this != this->end());
            return found;
        }
    };

    void addEnemyGroupDef(uint32 eg_id, uint32 eg_def = 0);
    void rmEnemyGroupDef(uint32 eg_id, uint32 eg_def = 0);
    bool isInEnemyGroupDef(uint32 eg_id, uint32 eg_def = 0);

    void addEmulatedGroupDef(uint32 eg_id, uint32 eg_def = 0);
    void rmEmulatedGroupDef(uint32 eg_id, uint32 eg_def = 0);
    bool isInEmulatedGroupDef(uint32 eg_id, uint32 eg_def = 0);
    bool isInEmulatedGroupDef(Mmuu32_t &r_egd,
        bool id_only = true);
    bool emulatedGroupDefsEmpty() { return emulated_group_defs_.size() == 0; }

    typedef std::pair<ShootableMapObject *, double> Pairsmod_t;
    typedef std::map <ShootableMapObject *, double> Msmod_t;
    void addHostilesFound(ShootableMapObject * hostile_found) {
        hostiles_found_.insert(Pairsmod_t(hostile_found, -1.0));
    }
    void rmHostilesFound(ShootableMapObject * hostile_found) {
        hostiles_found_.erase(hostile_found);
    }
    bool isInHostilesFound(ShootableMapObject * hostile_found) {
        return hostiles_found_.find(hostile_found)
            != hostiles_found_.end();
    }
    void verifyHostilesFound(Mission *m);
    bool getHostilesFoundIt(Msmod_t::iterator &it_s, Msmod_t::iterator &it_e)
    {
        if (hostiles_found_.empty())
            return false;
        it_s = hostiles_found_.begin();
        it_e = hostiles_found_.end();
        return true;
    }

    bool checkHostileIs(ShootableMapObject *obj,
        unsigned int hostile_desc_alt = 0);
    bool checkFriendIs(PedInstance *p);

    typedef enum {
        og_dmUndefined = 0x0,
        //unused-----
        og_dmFriend = 0x0001,
        og_dmEnemy = 0x0002,
        og_dmNeutral = 0x0004,
        //unsued-----
        // mainPedType << 8
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
        // only if all weapon has no ammunition, persuadatron excludes this
        // should not be used for hostile_desc_
        pd_smNoAmmunition = 0x0010,
        // all non-player controllled peds should have this set
        pd_smAutoAction = 0x0020,
        // target can be position or object
        pd_smHasShootTarget = 0x0040,
        // target can be position or object
        pd_smHasReachTarget = 0x0080
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
        pa_smDead = 0x1000,
        // this object should be ignored in all Ai procedures
        pa_smUnavailable = 0x2000,
        pa_smCheckExcluded = pa_smDead | pa_smUnavailable,
        pa_smAll = 0xFFFF
    } pedActionStateMasks; // MapObject::state_

    typedef struct {
        union {
            // weapon index from weapons_ in mission_
            uint32 indx;
            // use only this weapon for attack
            WeaponInstance *wi;
            // use only this type of weapon
            Weapon::WeaponType wpn_type;
            // use weapon that inflicts this type of damage
            MapObject::DamageType dmg_type;
        } wpn;
        // union description
        // 0 - not used, 1 - indx, 2 - pointer, 3 - weapon type,
        // 4 - damage type strict (type == dmg_type),
        // 5 - damage type non-strict (type & dmg_type != 0)
        uint8 desc;
    } pedWeaponToUse;

    typedef enum {
        ai_aNone = 0x0,
        // Setup control over object where possible to lose this control
        ai_aAquireControl = 0x0001,
        // Leave control over object where possible to lose this control
        ai_aLoseControl = 0x0002,
        // Obtain inventory object
        ai_aPickUpObject = 0x0008,
        // Object of defined subtype (of type) should be destroyed
        // defined by indx
        ai_aDestroyObject = 0x0010,
        // Use of object untill condition is met
        ai_aUseObject = 0x0020,
        ai_aPutDownObject = 0x0040,
        // Objects should be at defined location
        ai_aReachLocation = 0x0080,
        ai_aFollowObject = 0x0100,
        // Should wait some time
        ai_aWait = 0x0200,
        ai_aAttackLocation = 0x0400,
        // in range of current weapon or inrange of other friendly units:
        // will execute ai_aReachLocation
        ai_aFindEnemy = 0x0800,
        // in range of current weapon
        ai_aFindNonFriend = 0x1000,
        ai_aNonFinishable = 0x80000000,
    }AiAction;

    typedef enum {
        gd_mNone = 0,
        gd_mStandWalk = 0x0001,
        gd_mFire = 0x0002,
        gd_mThink = 0x0004,
        gd_mExclusive = 0x8000,
        gd_mAll = (gd_mStandWalk | gd_mFire | gd_mThink)
    } groupDescMasks;

    typedef struct {
        // action state (pedActionStateMasks)
        uint32 as;
        // PedInstance::AiAction
        uint32 ot_execute;
        PathNode t_pn;
        // ai_aFindEnemy sets this value, others use
        ShootableMapObject *t_smo;
        // 0b - not started, 1b - executing, 2b - finished, 3b - failed,
        // 4b - suspended, 5b - waiting for animation to complete,
        // 6b - not set (empty, should be skipped)
        uint8 state;
        // same as in actionQueueGroupType, groupDescMasks
        uint32 group_desc;
        struct {
            struct {
                // (objGroupDefMasks)
                uint32 t_ogd;
                // (pedDescStateMasks)
                uint32 t_hostile_desc;
                // how many shots to do, 0 = shoot until cancelled
                uint32 make_shots;
                uint32 shots_done;
                bool forced_shot;
                pedWeaponToUse weapon;
                // upon reaching this value action is complete;
                // health dropped to this value( or persuade resistance points?)
                int32 value;
            } enemy_var;
            struct {
                int32 elapsed;
                // = -1 forever
                int32 time_total;
            } time_var;
            struct {
                // move into this direction, -1 is unset
                int32 dir;
                // dist to target pos/object
                int32 dist;
                // -1 is unset
                int32 speed;
            } dist_var;
        } multi_var;
        // for ai_aReachLocation 0 - go to location, 1 - go to direction,
        // 2 - go to position of object; if dist is set, on reaching
        // this value action complete
        // for ai_aFollowObject 0 - until dist is reached, 1 - in range
        // of shot view
        // for ai_aAquireControl, for car, 0 - become passenger(or driver),
        // 1 - become driver only if else failed
        // for ai_aAquireControl, for ped, 0 - controled or not is ok,
        // 1 - controlled if else failed
        // for ai_aFindEnemy; 0 - scout - will not attack, 1 - search and
        // destroy
        uint32 condition;
    } actionQueueType;

    typedef struct {
        std::vector <actionQueueType> actions;
        // index refers action state of which defines state of group
        uint32 main_act;
        // 0 - not set, 0b - stand/walking group(has walking action or
        // action requires ped to stand), 1b - firing only(no walking action),
        // (0b+1b) - walking + firing, 2b - decision making(thinking),
        // 3b - exclusive, no other action can be executed in parallel with it
        // and all before it should be completed
        // NOTE: a group should not interfere with actions of other group
        // groupDescMasks
        uint32 group_desc;
        // 0b - not started, 1b - executing, 2b - finished, 3b - failed,
        // 4b - suspended
        uint8 state;
        uint32 group_id;
    } actionQueueGroupType;

    void setActQInQueue(actionQueueGroupType &as,
        bool set_id = false);
    bool addActQToQueue(actionQueueGroupType &as,
        bool set_id = false);
    void clearActQ() { actions_queue_.clear(); }
    bool addDefActsToActions(actionQueueGroupType &as);
    void clearDefActs() { default_actions_.clear(); }
    void dropActQ();

    void createActQStanding(actionQueueGroupType &as);
    void createActQWalking(actionQueueGroupType &as, PathNode *tpn,
        ShootableMapObject *tsmo, int32 dir = -1, int32 dist = 0);
    void createActQHit(actionQueueGroupType &as, PathNode *tpn,
        int32 dir = -1);
    bool createActQFiring(actionQueueGroupType &as, PathNode *tpn,
        ShootableMapObject *tsmo = NULL, bool forced_shot = false,
        int make_shots = 0, pedWeaponToUse *pw_to_use = NULL,
        int32 value = -1);
    void createActQFollowing(actionQueueGroupType &as,
        ShootableMapObject *tsmo, uint32 condition, int32 dist = 128);

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
    void createActQLeaveCar(actionQueueGroupType &as,
        ShootableMapObject *tsmo);
    void createActQWait(actionQueueGroupType &as, int tm_wait);

    void createActQFindEnemy(actionQueueGroupType &as);
    
    void discardActG(uint32 id);
    void discardActG(std::vector <actionQueueGroupType>::iterator it_a);

protected:
    Ped *ped_;

    enum {
        Firing_Not,
        Firing_Fire,
        Firing_Reload,
        Firing_Stop
    } firing_;

    std::vector <actionQueueGroupType> actions_queue_;
    std::vector <actionQueueGroupType> default_actions_;
    uint32 action_grp_id_;
    // (pedDescStateMasks)
    unsigned int desc_state_;
    // this inherits definition from desc_state_
    // ((target checked)desc_state_ & hostile_desc_) != 0 kill him
    unsigned int hostile_desc_;
    Mmuu32_t enemy_group_defs_;
    // if object is not hostile here, enemy_group_defs_ check
    // is skipped, but not hostiles_found_ or desc_state_
    Mmuu32_t emulated_group_defs_;
    Mmuu32_t friend_group_defs_;
    //std::set <unsigned int> emulated_failed_groups_;
    // dicovered hostiles are set here, only within sight range
    Msmod_t hostiles_found_;
    //unused for now
    //std::set <ShootableMapObject *> friends_found_;
    //! defines group obj belongs to (objGroupDefMasks), not unique
    unsigned int obj_group_def_;
    unsigned int old_obj_group_def_;

    //! a unique group identification number, 0 - all group IDs
    unsigned int obj_group_id_;
    unsigned int old_obj_group_id_;

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
    agentAndNonEnum agent_is_;
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
