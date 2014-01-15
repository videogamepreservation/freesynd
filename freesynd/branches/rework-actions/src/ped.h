/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2010  Bohdan Stelmakh <chamel@users.sourceforge.net> *
 *   Copyright (C) 2013  Benoit Blancard <benblan@users.sourceforge.net>*
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
#include <set>

#include "common.h"
#include "mapobject.h"
#include "modowner.h"
#include "gfx/spritemanager.h"
#include "model/weaponholder.h"
#include "weapon.h"
#include "ipastim.h"
#include "ia/actions.h"

class Agent;
class Mission;
class VehicleInstance;
class Vehicle;

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
class PedInstance : public ShootableMovableMapObject, public WeaponHolder,
    public ModOwner
{
public:
    //! starting health for agents
    static const int kAgentMaxHealth;
    PedInstance(Ped *ped, int m);
    ~PedInstance();
    //! Initialize the ped instance as an agent
    void initAsAgent(Agent *p_agent, unsigned int obj_group_id);
    //! Returns true if the agent is one of us.
    bool isOurAgent() { return is_our_; }
    //! Sets if the agent is one of us or not
    void set_is_our(bool is_our) { is_our_ = is_our; }
    
    

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
        ad_PersuadedAnim
    } AnimationDrawn;

    typedef enum {
        m_tpPedestrian = 0x01,
        m_tpAgent = 0x02,
        m_tpPolice = 0x04,
        m_tpGuard = 0x08,
        m_tpCriminal = 0x10
    } mainPedType;

    //! MapObject::state_
    enum pedActionStateMasks {
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
        //! When a ped is hit by a laser
        pa_smHitByLaser = 0x4000,
        pa_smCheckExcluded = pa_smDead | pa_smUnavailable,
        pa_smAll = 0xFFFF
    };

    void draw(int x, int y);

    void setFrame(int f) { frame_ = f; }

    void setSightRange(int new_sight_range) { sight_range_ = new_sight_range; }
    int sightRange() { return sight_range_; }

    void setPersuasionPoints(int points) { persuasion_points_ = points; }
    int persuasionPoints() {return persuasion_points_; }
    bool canPersuade(int points);
    void addPersuaded(PedInstance *p);
    void rmvPersuaded(PedInstance *p);

    void showPath(int scrollX, int scrollY);

    bool switchActionStateTo(uint32 as);
    bool switchActionStateFrom(uint32 as);
    void setActionStateToDrawnAnim(void);
    bool animate(int elapsed, Mission *mission);
    //! Temporary version of animate()
    bool animate2(int elapsed, Mission *mission);
    void drawSelectorAnim(int x, int y);
    //! Update frame to render
    bool updateAnimation(int elapsed);
    //! Set state for ped (replace switchActionStateTo)
    void goToState(uint32 as);
    //! Quit state for ped (replace switchActionStateFrom)
    void leaveState(uint32 as);

    //*************************************
    // Action management
    //*************************************
    //! Adds the given action to the list of actions
    void addMovementAction(fs_actions::MovementAction *pAction, bool appendAction);
    //! Removes all ped's actions
    void destroyAllActions();
    //! Removes ped's action of using weapon
    void destroyUseWeaponAction();
    //! Execute the current action if any
    bool executeAction(int elapsed, Mission *pMission);
    //! Execute a weapon action if any
    bool executeUseWeaponAction(int elapsed, Mission *pMission);
    
    //! Adds action to walk to a given destination
    void addActionWalk(const PathNode &tpn, fs_actions::CreatOrigin origin, bool appendAction);
    //! Adds action to follow a ped
    void addActionFollowPed(PedInstance *pPed);
    //! Adds action to put down weapon on the ground
    void addActionPutdown(uint8 weaponIndex, bool appendAction);
    //! Adds action to pick up weapon from the ground
    void addActionPickup(WeaponInstance *pWeapon, bool appendAction);
    //! Adds action to enter a given vehicle
    void addActionEnterVehicle(Vehicle *pVehicle, bool appendAction);
    //! Adds action to drive vehicle to destination
    void addActionDriveVehicle(fs_actions::CreatOrigin origin, 
           VehicleInstance *pVehicle, PathNode &destination, bool appendAction);
    //! Return true if ped can use a weapon
    bool canAddUseWeaponAction(WeaponInstance *pWeapon = NULL);
    //! Adds action to shoot somewhere
    void addActionShootAt(const PathNode &aimedPt);
    //! Adds action to use medikit
    void addActionUseMedikit();

    //*************************************
    // Movement management
    //*************************************
    //! Set the destination to reach at given speed (todo : replace setDestinationP())
    bool setDestination(Mission *m, PathNode &node, int newSpeed = -1);

    void setDestinationP(Mission *m, int x, int y, int z,
        int ox = 128, int oy = 128);

    bool movementP(Mission *m, int elapsed);

    //*************************************
    // Weapon management
    //*************************************
    void dropWeapon(int n);
    void dropWeapon(WeaponInstance *w);
    void dropAllWeapons();
    void destroyAllWeapons();
    bool wePickupWeapon();

    //*************************************
    // Shoot & Damage management
    //*************************************
    //! Return true if ped is currently using a weapon (ie there's an active action)
    bool isUsingWeapon() { return pUseWeaponAction_ != NULL; }
    //! Make the ped stop using weapon (mainly for automatic weapon)
    void stopUsingWeapon();
    //! Update the ped's shooting direction and target
    void updateShootingDirection(Mission *pMission, ShootableMapObject *pTarget, PathNode &shootPt);
    //! Adjust aimed point with user accuracy and weapon max range
    void adjustAimedPtWithRangeAndAccuracy(Weapon *pWeaponClass, PathNode &aimedPt);
    //! Gets the time before a ped can shoot again
    int getTimeBetweenShoots(WeaponInstance *pWeapon);

    //! Forces agent to kill himself
    void commitSuicide();
    //! Indicates the agent is commiting suicide
    bool is_suiciding() { return is_suiciding_; }
    void set_is_suiciding(bool flag) { is_suiciding_ = flag; }

    //! Method called when object is hit by a weapon shot.
    void handleHit(ShootableMapObject::DamageInflictType &d);
    //! Method called to check if ped has died
    bool handleDeath(ShootableMapObject::DamageInflictType &d);
    bool handleDamage(ShootableMapObject::DamageInflictType *d);

    bool inSightRange(MapObject *t);
    VehicleInstance *inVehicle();

    void putInVehicle(VehicleInstance *v, pedActionStateMasks add_state);
    void leaveVehicle();
    bool setVehicleIgnore(bool ignored);

    int map();
    AnimationDrawn drawnAnim(void);
    void setDrawnAnim(AnimationDrawn drawn_anim);
    bool handleDrawnAnim(int elapsed);

    typedef struct {
        int32 dir_orig;
        int32 dir_last;
        int32 dir_closest;
        int32 dir_closer;
        int32 dir_modifier;
        int32 modifier_value;
        // directional movement only
        // to decide whether to continue movement by changing
        // direction or not
        bool bounce;
        // walkn only on "safe" tiles
        bool safe_walk;
        // when closest is used and first movement is successful
        bool on_new_tile;
        void clear() {
            dir_last = -1;
            dir_modifier = 0;
            modifier_value = 0;
            bounce = false;
            on_new_tile = false;
            safe_walk = true;
        }
    } dirMoveType;

    uint8 moveToDir(Mission *m, int elapsed, dirMoveType &dir_move,
        int dir = -1, int t_posx = -1, int t_posy = -1, int *dist = NULL,
        bool set_dist = false);

    void setAllAdrenaLevels(uint8 amount, uint8 depend, uint8 effect) {
        adrenaline_->setLevels256(amount, depend, effect);
    }

    void setAllInteliLevels(uint8 amount, uint8 depend, uint8 effect) {
        intelligence_->setLevels256(amount, depend, effect);
    }

    void setAllPercepLevels(uint8 amount, uint8 depend, uint8 effect) {
        perception_->setLevels256(amount, depend, effect);
    }
    void updtIPATime(int elapsed) {
        adrenaline_->processTicks(elapsed);
        perception_->processTicks(elapsed);
        intelligence_->processTicks(elapsed);
    }

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

    void setTimeBeforeCheck(int32 tm) { tm_before_check_ = tm; }
    void setBaseModAcc(double mod_acc) { base_mod_acc_ = mod_acc; }

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
                    ++it;
                } while (it->first == first && it != this->end());
                this->erase(its, it);
            } else {
                do {
                    if (it->second == second) {
                        this->erase(it);
                        break;
                    }
                    ++it;
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
                        ++it;
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
                        ++it;
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
                    ++it_this;
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
                        it != rng.second; ++it)
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
                        ++it_this;
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
    //! Verify hostility between this Ped and the object
    bool isHostileTo(ShootableMapObject *obj,
        unsigned int hostile_desc_alt = 0);
    //! Verify if this ped is friend with the given ped
    bool isFriendWith(PedInstance *p);

    typedef enum {
        og_dmUndefined = 0x0,
        og_dmPedestrian = 0x01,
        og_dmCivilian = 0x01,
        og_dmAgent = 0x02,
        og_dmPolice = 0x04,
        og_dmGuard = 0x08,
        og_dmCriminal = 0x10
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
        pd_smAll = 0xFFFF
    } pedDescStateMasks;

    typedef enum {
        ai_aNone = 0x0,
        // Setup control over object where possible to lose this control
        ai_aAquireControl = 0x0001,
        // Leave control over object where possible to lose this control
        ai_aLoseControl = 0x0002,
        // this event should not be used as seperate action only
        ai_aWaitToStart = 0x0004,
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
        // Should wait some time to end or works as delay for other action
        // to have animation drawn, or as simple wait
        ai_aWait = 0x0200,
        ai_aAttackLocation = 0x0400,
        // in range of current weapon or inrange of other friendly units:
        // will execute ai_aReachLocation
        ai_aFindEnemy = 0x0800,
        // in range of current weapon
        ai_aFindNonFriend = 0x1000,
        ai_aDeselectCurWeapon = 0x2000,
        ai_aTrigger = 0x4000,
        // checking persuador to mimic his actions and to follow him
        ai_aCheckOwner = 0x8000,
        // action should be executed within this time limit, if it will
        // not complete (any result) in time it will be marked as failed
        ai_aTimeExecute = 0x10000,
        ai_aFindWeapon = 0x20000,
        ai_aResetActionQueueQueue = 0x40000000,
        ai_aNonFinishable = 0x80000000,
        ai_aAll = 0xFFFFFFFF
    } AiAction;

    typedef enum {
        gd_mNone = 0,
        gd_mStandWalk = 0x0001,
        gd_mFire = 0x0002,
        gd_mThink = 0x0004,
        gd_mExclusive = 0x8000,
        gd_mAll = (gd_mStandWalk | gd_mFire | gd_mThink)
    } groupDescMasks;

    typedef struct {
        PathNode t_pn;
        ShootableMapObject *t_smo;
        toDefineXYZ t_xyz;
        // 0 - unset, 1 - use t_pn, 2 - use t_smo, 3 - both are set
        // 4 - t_xyz used
        uint8 desc;
    } targetDescType;

    typedef struct {
        // action state (pedActionStateMasks)
        uint32 as;
        // PedInstance::AiAction
        uint32 act_exec;
        targetDescType target;
        // 0b(1) - not started, 1b(2) - executing, 2b(4) - finished, 3b(8) - failed,
        // 4b(16) - suspended, 5b(32) - waiting to complete (ai_aWait),
        // 6b(64) - not ready (empty, should be skipped), 7b(128) - waiting (ai_aWait
        // | ai_aWaitToStart), 8b(256) - waiting to start
        uint16 state;
        // 0 - not set, 0b - stand/walking group(has walking action or
        // action requires ped to stand), 1b - firing only(no walking action),
        // (0b+1b) - walking + firing, 2b - decision making(thinking),
        // 3b - exclusive, no other action can be executed in parallel with it
        // and all before it should be completed
        // NOTE: group with same type as previous cannot be executed until
        // previous is completed or failed
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
                WeaponSelectCriteria pw_to_use;
                // upon reaching this value action is complete;
                // health dropped to this value( or persuade resistance points?)
                int32 value;
            } enemy_var;
            struct {
                int32 elapsed;
                // = -1 forever(ai_aWait)
                int32 time_wait;
                // = 0 || t > 0 (ai_aWaitToStart)
                int32 time_to_start;
                // time for execution of action(ai_aTimeExecute)
                int32 exec_time;
                // time action was executing(ai_aTimeExecute)
                int32 exec_elapsed;
                // wait event, 0 - standalone - time(milliseconds), 1 - waits
                // till animation finished, 2 - attached to process time
                uint8 desc;
            } time_var;
            struct {
                // move into this direction, -1 is unset
                int32 dir;
                // dist to target pos/object, 0 is unset/not used
                int32 dist;
                // already walked distance
                int32 dist_walked;
                // radius for distance checking
                // dist + rd > cur_dist > dist - rd
                // 0 or > 0
                int32 rd;
                // -1 is unset
                int32 speed;
                dirMoveType dir_move;
            } dist_var;
        } multi_var;
        /* for ai_aReachLocation 0 - go to location, 1 - go to direction,
         * 2 - go to position of object; if dist is set, on reaching
         * this value action complete, 3 - pathnode coords = x, y are used to
         * calculate direction, 4 - objects coords = x, y to calculate direction
         * NOTE: coords x,y : tile_x_ * 256 + off_x_, tile_y_ * 256 + off_y_
         * for ai_aFollowObject 0 - until dist is reached, 1 - in range
         * of shot view
         * for ai_aAquireControl, for car, 0 - become passenger(or driver),
         * 1 - become driver only if else failed
         * for ai_aAquireControl, for ped, 0 - controled or not is ok,
         * 1 - controlled if else failed
         * for ai_aFindEnemy; 0 - scout - will not attack, 1 - search and
         * destroy
         * for ai_aUseObject, 0 - use following vehicle to get to location
         * (can be passenger); 1 - drive following vehicle to location
         * (only driver can do this)
         */
        uint32 condition;
    } actionQueueType;

    typedef struct {
        std::vector <actionQueueType> actions;
        // index refers to action, state of which defines state of group
        uint32 main_act;
        // same as in actionQueueType, groupDescMasks
        // NOTE: this value is never set in createActQ* functions, should be
        // always set before adding to one of actions queues
        uint32 group_desc;
        // same as in actionQueueType.state
        uint16 state;
        uint32 group_id;
        //! Which process created this action group
        fs_actions::CreatOrigin origin_desc;
    } actionQueueGroupType;

    // act_find (PedInstance::AiAction)
    std::vector <actionQueueType>::iterator findActInQueue(uint32 act_find,
        actionQueueGroupType &as, std::vector <actionQueueType>::iterator search_from);

    std::vector <actionQueueGroupType>::iterator findQueueInActQueue(uint32 id);
    void setActQInQueue(actionQueueGroupType &as,
        uint32 * id = NULL);
    bool addActQToQueue(actionQueueGroupType &as,
        uint32 * id = NULL);
    /*!
     * Add the action to the list of actions.
     * \param as The action to add
     * \param append True means the action is added after existing actions. False
     *   means the action is the only one executed.
     * \param id
     */
    void addAction(actionQueueGroupType &as, bool append = false, uint32 * id = NULL) {
        if (append) {
            addActQToQueue(as, id);
        } else {
            setActQInQueue(as, id);
        }
    }
    void clearActQ() { actions_queue_.clear(); }

    void createDefQueue();
    void addDefActsToActions(uint32 groups_used = 0);
    void clearDefActs() { default_actions_.clear(); }

    void dropActQ();

    void createActQStanding(actionQueueGroupType &as);
    void createActQWalking(actionQueueGroupType &as, PathNode *tpn,
        ShootableMapObject *tsmo, int32 dir = -1, int32 dist = 0,
        bool bounce = true);
    void createActQHit(actionQueueGroupType &as, PathNode *tpn,
        int32 dir = -1);
    bool createActQFiring(actionQueueGroupType &as, PathNode *tpn,
        ShootableMapObject *tsmo = NULL, bool forced_shot = false,
        uint32 make_shots = 0, WeaponSelectCriteria *pw_to_use = NULL,
        int32 value = -1);
    void createActQFollowing(actionQueueGroupType &as,
        ShootableMapObject *tsmo, uint32 condition, int32 dist = 128, int32 rd = 0);

    void createActQPickUp(actionQueueGroupType &as,
        ShootableMapObject *tsmo);
    void createActQPutDown(actionQueueGroupType &as,
        ShootableMapObject *tsmo);

    void createActQBurning(actionQueueGroupType &as);

    void createActQGetInCar(actionQueueGroupType &as,
        ShootableMapObject *tsmo, int32 dir = -1);
    void createActQUsingCar(actionQueueGroupType &as, PathNode *tpn,
        ShootableMapObject *tsmo);
    void createActQLeaveCar(actionQueueGroupType &as,
        ShootableMapObject *tsmo);
    void createActQWait(actionQueueGroupType& as, int tm_wait,
        uint8 desc = 0);

    bool createActQFindEnemy(actionQueueGroupType &as);
    bool createActQFindNonFriend(actionQueueGroupType &as);

    void createActQResetActionQueue(actionQueueGroupType &as);
    void createActQDeselectCurWeapon(actionQueueGroupType &as);
    void createActQTrigger(actionQueueGroupType &as, PathNode *tpn, int32 range);

    void createActQFindWeapon(actionQueueGroupType &as,
        WeaponSelectCriteria *pw_to_use = NULL, int dist = -1);

    void createActQCheckOwner(actionQueueGroupType &as);
    
    void discardActG(uint32 id);
    void discardActG(std::vector <actionQueueGroupType>::iterator it_a);

    void updtActGFiringShots(uint32 id, uint32 make_shots);
    void updtActGFiring(uint32 id, PathNode *tpn,
        ShootableMapObject *tsmo = NULL);
    bool checkActGCompleted(fs_actions::CreatOrigin origin);
    void pauseAllInActG(actionQueueGroupType &as, uint32 start_pos = 1);

    //! Returns ped's speed under normal conditions
    int getDefaultSpeed();
    int getSpeedOwnerBoost();

    void getAccuracy(double &base_acc);
    bool hasAccessCard();
    bool isPersuaded();

    void cpyEnemyDefs(Mmuu32_t &eg_defs) { eg_defs = enemy_group_defs_; }
    bool isArmed() { return (desc_state_ & pd_smArmed) != 0; }
    bool isExcluded() { return (state_ & pa_smCheckExcluded) != 0; }
    targetDescType * lastFiringTarget() { return &last_firing_target_; }
    
    IPAStim *adrenaline_;
    IPAStim *perception_;
    IPAStim *intelligence_;
protected:
    /*!
     * Called before a weapon is selected to check if weapon can be selected.
     * \param wi The weapon to select
     */
    bool canSelectWeapon(WeaponInstance *pNewWeapon);
    /*!
     * Called when a weapon has been deselected.
     * \param wi The deselected weapon
     */
    void handleWeaponDeselected(WeaponInstance * wi);
    /*!
     * Called when a weapon has been selected.
     * \param wi The selected weapon
     */
    void handleWeaponSelected(WeaponInstance * wi);

    //! Creates and insert a HitAction for the ped
    void insertHitAction(DamageInflictType &d);
protected:
    Ped *ped_;
    //! If this flag is true, all actions should be dropped
    bool drop_actions_;
    std::vector <actionQueueGroupType> actions_queue_;
    std::vector <actionQueueGroupType> default_actions_;

    /*! Ped's behaviour.*/
    fs_actions::Behaviour *pBehaviour_;
    /*! Current action*/
    fs_actions::MovementAction *currentAction_;
    /*! Current action of using a weapon.*/
    fs_actions::UseWeaponAction *pUseWeaponAction_;


    uint32 action_grp_id_;
    // (pedDescStateMasks)
    uint32 desc_state_;
    // this inherits definition from desc_state_
    // ((target checked)desc_state_ & hostile_desc_) != 0 kill him
    uint32 hostile_desc_;
    Mmuu32_t enemy_group_defs_;
    // if object is not hostile here, enemy_group_defs_ check
    // is skipped, but not hostiles_found_ or desc_state_
    Mmuu32_t emulated_group_defs_;
    // not set anywhere but used
    Mmuu32_t friend_group_defs_;
    //! dicovered hostiles are set here, only within sight range
    Msmod_t hostiles_found_;
    //! used by police officers, for now friends forever mode
    std::set <ShootableMapObject *> friends_found_;
    //! from this list they will move to friends_found_ if in sight
    std::set <ShootableMapObject *> friends_not_seen_;
    //! defines group obj belongs to (objGroupDefMasks), not unique
    uint32 obj_group_def_;
    uint32 old_obj_group_def_;

    //! a unique group identification number, 0 - all group IDs
    uint32 obj_group_id_;
    uint32 old_obj_group_id_;

    //! time wait before checking enviroment (enemies, friends etc)
    int32 tm_before_check_;

    //! base value that influences accuracy during fire
    double base_mod_acc_;

    AnimationDrawn drawn_anim_;

    int sight_range_;
    VehicleInstance *in_vehicle_;
    //! This flag tells if this is our agent, assuming it's an agent.
    bool is_our_;
    //! controller of ped - for persuaded
    ShootableMapObject *owner_;
    targetDescType last_firing_target_;
    //! points needed to persuade ped
    int persuasion_points_;
    std::set <PedInstance *> persuaded_group_;
    //! Flag to mark suiciding agent
    bool is_suiciding_;

    bool walkable(int x, int y, int z) { return true; }

private:
    inline int getClosestDirs(int dir, int& closest, int& closer);
};

#endif
