/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2012  Bohdan Stelmakh <chamel@users.sourceforge.net> *
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

#include "ped.h"
#include "app.h"


void PedInstance::createActQStanding(actionQueueGroupType &as) {
    as.state = 1;
    actionQueueType aq;
    aq.act_exec = PedInstance::ai_aWait;
    aq.as = PedInstance::pa_smStanding;
    aq.state = 1;
    aq.multi_var.time_var.desc = 0;
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_wait = -1;
    aq.group_desc = PedInstance::gd_mStandWalk;
    as.actions.push_back(aq);
}

void PedInstance::createActQWalking(actionQueueGroupType &as, PathNode *tpn,
    ShootableMapObject *tsmo, int32 dir, int32 dist, bool bounce)
{
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smWalking;
    aq.act_exec = PedInstance::ai_aReachLocation;
    aq.group_desc = PedInstance::gd_mStandWalk;
    aq.state = 1;
    aq.multi_var.dist_var.dir = dir;
    aq.multi_var.dist_var.dist = dist;
    aq.multi_var.dist_var.speed = -1;
    aq.multi_var.dist_var.dir_move.clear();
    // TODO: set condition as function parameter and set values based on it?
    if (dir == -1) {
        if (tpn) {
            aq.target.t_pn = *tpn;
            aq.target.desc = 1;
            aq.condition = 0;
        } else {
            aq.target.desc = 2;
            aq.target.t_smo = tsmo;
            aq.condition = 2;
        }
    } else {
        aq.multi_var.dist_var.dir_move.bounce = bounce;
        if (tpn) {
            // direction will be calculated from pathnode position
            aq.target.t_pn = *tpn;
            aq.target.desc = 1;
            aq.condition = 3;
        } else if (tsmo) {
            // direction will be calculated from objects position
            aq.target.t_smo = tsmo;
            aq.target.desc = 2;
            aq.condition = 4;
        } else {
            // directional movement only
            aq.condition = 1;
            aq.target.desc = 0;
        }
    }
    as.actions.push_back(aq);
}

void PedInstance::createActQHit(actionQueueGroupType &as, PathNode *tpn,
    int32 dir)
{
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smHit;
    aq.act_exec = PedInstance::ai_aReachLocation;
    aq.group_desc = PedInstance::gd_mExclusive;
    aq.multi_var.dist_var.speed = -1;
    aq.state = 1;
    // TODO: set directional movement to opposite?
    aq.multi_var.dist_var.dir = dir;
    aq.condition = 1;
    // calculate direction from this point
    if (dir == -1) {
        aq.target.t_pn = *tpn;
        aq.target.desc = 1;
    }
    as.actions.push_back(aq);
}

bool PedInstance::createActQFiring(actionQueueGroupType &as, PathNode *tpn,
    ShootableMapObject *tsmo, bool forced_shot, uint32 make_shots, 
    pedWeaponToUse *pw_to_use, int32 value)
{
    // TODO: review, if no weapon found or selected assume physical damage?
    // should be possible to create without a target
    as.state = 1;
    actionQueueType aq;
    aq.group_desc = PedInstance::gd_mFire;
    bool can_shoot = false;
    bool does_phys_dmg = false;
    Weapon *pWeapon = NULL;
    if (pw_to_use) {
        switch (pw_to_use->desc) {
            case 1:
                pw_to_use->wpn.wi = weapon(pw_to_use->wpn.indx);
                can_shoot = pw_to_use->wpn.wi->canShoot();
                does_phys_dmg = pw_to_use->wpn.wi->doesPhysicalDmg();
                pw_to_use->desc = 2;
                break;
            case 2:
                // pointer
                can_shoot = pw_to_use->wpn.wi->canShoot();
                does_phys_dmg = pw_to_use->wpn.wi->doesPhysicalDmg();
                break;
            case 3:
                // weapon type
                pWeapon = g_App.weapons().getWeapon(pw_to_use->wpn.wpn_type);
                can_shoot = pWeapon->canShoot();
                does_phys_dmg = pWeapon->doesPhysicalDmg();
                break;
            case 4:
                // strict damage type
                if(!g_App.weapons().checkDmgTypeCanShootStrict(
                    pw_to_use->wpn.dmg_type, can_shoot)) {
                    return false;
                }
                does_phys_dmg = (pw_to_use->wpn.dmg_type
                    & MapObject::dmg_Physical) != 0;
                break;
            case 5:
                // non-strict damage type
                if(!g_App.weapons().checkDmgTypeCanShootNonStrict(
                    pw_to_use->wpn.dmg_type, can_shoot)) {
                    return false;
                }
                does_phys_dmg = (pw_to_use->wpn.dmg_type
                    & MapObject::dmg_Physical) != 0;
                break;
        }
        aq.multi_var.enemy_var.pw_to_use = *pw_to_use;
    } else {
        WeaponInstance *wi = selectedWeapon();
        if (wi) {
            can_shoot = wi->canShoot();
            does_phys_dmg = wi->doesPhysicalDmg();
            //TODO: remove?
            /*
            Mod *pMod = slots_[Mod::MOD_BRAIN];
            if (pMod) {
                if (pMod->getVersion() == Mod::MOD_V1) {
                    aq.multi_var.enemy_var.pw_to_use.desc = 3;
                    aq.multi_var.enemy_var.pw_to_use.wpn.wpn_type = wi->getWeaponType();
                } else { // v2, v3
                    aq.multi_var.enemy_var.pw_to_use.desc = 4;
                    aq.multi_var.enemy_var.pw_to_use.wpn.dmg_type = wi->dmgType();
                }
            } else {*/
                aq.multi_var.enemy_var.pw_to_use.desc = 2;
                aq.multi_var.enemy_var.pw_to_use.wpn.wi = wi;
            //}
        } else {
            can_shoot = true;
            does_phys_dmg = true;
            aq.multi_var.enemy_var.pw_to_use.desc = 5;
            aq.multi_var.enemy_var.pw_to_use.wpn.dmg_type = MapObject::dmg_Physical;
        }
    }

    if (!can_shoot)
        return false;

    if (pw_to_use == NULL) {
        if (obj_group_def_ == PedInstance::og_dmAgent) {
            aq.multi_var.enemy_var.pw_to_use.use_ranks =
                slots_[Mod::MOD_BRAIN] ? true: false;
        } else
            aq.multi_var.enemy_var.pw_to_use.use_ranks = false;
    }
    aq.multi_var.enemy_var.value = value;
    aq.multi_var.enemy_var.forced_shot = forced_shot;
    aq.multi_var.enemy_var.shots_done = 0;
    aq.multi_var.enemy_var.make_shots = make_shots;
    if (does_phys_dmg) {
        aq.as = PedInstance::pa_smFiring;
        // TODO: use condition to set more information for action execution
        // continuos shooting until ammo ends(or all weapons ammo),
        // until target destroyed, type of damage that will complete action
        if (tpn) {
            aq.target.t_pn = *tpn;
            aq.target.desc = 1;
            aq.act_exec = PedInstance::ai_aAttackLocation;
            aq.target.t_smo = NULL;
        } else {
            aq.target.t_smo = tsmo;
            aq.target.desc = 2;
            aq.act_exec = PedInstance::ai_aDestroyObject;
        }
    } else {
        aq.as = PedInstance::pa_smNone;
        aq.target.t_smo = tsmo;
        aq.target.desc = 0;
        aq.act_exec = PedInstance::ai_aAquireControl;
    }
    aq.state = 1;
    aq.act_exec |= PedInstance::ai_aWait;
    aq.multi_var.time_var.desc = 1;
    aq.multi_var.time_var.elapsed = 0;
    as.actions.push_back(aq);
    return true;
}

void PedInstance::createActQFollowing(actionQueueGroupType &as,
    ShootableMapObject *tsmo, uint32 condition, int32 dist, int32 rd)
{
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smFollowing;
    aq.act_exec = PedInstance::ai_aFollowObject;
    aq.group_desc = PedInstance::gd_mStandWalk;
    aq.state = 1;
    aq.target.t_smo = tsmo;
    aq.target.desc = 2;
    aq.multi_var.dist_var.dist = dist;
    aq.multi_var.dist_var.speed = -1;
    aq.multi_var.dist_var.rd = rd;
    aq.condition = condition;
    as.actions.push_back(aq);
}

void PedInstance::createActQPickUp(actionQueueGroupType &as,
    ShootableMapObject *tsmo)
{
    as.state = 1;
    createActQWalking(as, NULL, tsmo);
    actionQueueType aq;
    aq.state = 1;
    aq.target.t_smo = tsmo;
    aq.target.desc = 2;
    aq.as = PedInstance::pa_smPickUp;
    aq.act_exec = PedInstance::ai_aPickUpObject;
    aq.act_exec |= PedInstance::ai_aWait;
    aq.multi_var.time_var.desc = 1;
    aq.group_desc = PedInstance::gd_mExclusive;
    as.actions.push_back(aq);
}

void PedInstance::createActQPutDown(actionQueueGroupType &as,
    ShootableMapObject *tsmo)
{
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smPutDown;
    aq.act_exec = PedInstance::ai_aPutDownObject;
    aq.group_desc = PedInstance::gd_mExclusive;
    aq.state = 1;
    aq.target.t_smo = tsmo;
    aq.target.desc = 2;
    aq.act_exec |= PedInstance::ai_aWait;
    aq.multi_var.time_var.desc = 1;
    as.actions.push_back(aq);
}

void PedInstance::createActQBurning(actionQueueGroupType &as) {
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smBurning;
    aq.act_exec = PedInstance::ai_aReachLocation;
    aq.multi_var.dist_var.dir = rand() % 256;
    aq.multi_var.dist_var.dist = rand() % 256 + 128;
    aq.multi_var.dist_var.speed = -1;
    aq.state = 1;
    aq.condition = 1;
    aq.group_desc = PedInstance::gd_mExclusive;
    as.actions.push_back(aq);
    aq.as = PedInstance::pa_smBurning;
    aq.multi_var.dist_var.dir = rand() % 256;
    aq.multi_var.dist_var.dist = rand() % 256 + 128;
    aq.group_desc = PedInstance::gd_mExclusive;
    as.actions.push_back(aq);
    aq.as = PedInstance::pa_smBurning;
    aq.multi_var.dist_var.dir = rand() % 256;
    aq.multi_var.dist_var.dist = rand() % 256 + 128;
    aq.group_desc = PedInstance::gd_mExclusive;
    as.actions.push_back(aq);
    aq.as = PedInstance::pa_smBurning;
    aq.act_exec = PedInstance::ai_aWait;
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_wait = 1000;
    aq.multi_var.time_var.desc = 0;
    aq.group_desc = PedInstance::gd_mExclusive;
    as.actions.push_back(aq);
}

void PedInstance::createActQGetInCar(actionQueueGroupType &as,
    ShootableMapObject *tsmo, int32 dir)
{
    as.state = 1;
    createActQWalking(as, NULL, tsmo, dir);
    actionQueueType aq;
    aq.state = 1;
    aq.target.t_smo = tsmo;
    aq.target.desc = 2;
    // TODO: change this use enum
    aq.condition = 0;
    aq.as = PedInstance::pa_smGetInCar;
    aq.group_desc = PedInstance::gd_mExclusive;
    aq.act_exec = PedInstance::ai_aAquireControl;
    as.actions.push_back(aq);
}

void PedInstance::createActQUsingCar(actionQueueGroupType &as, PathNode *tpn,
    ShootableMapObject *tsmo)
{
    as.state = 1;
    actionQueueType aq;
    aq.state = 1;
    aq.target.desc = 3;
    aq.target.t_smo = tsmo;
    aq.target.t_pn = *tpn;
    aq.condition = 1;
    aq.as = PedInstance::pa_smNone;
    aq.group_desc = PedInstance::gd_mStandWalk;
    aq.act_exec = PedInstance::ai_aUseObject;
    as.actions.push_back(aq);
}

void PedInstance::createActQInCar(actionQueueGroupType &as, PathNode *tpn,
    ShootableMapObject *tsmo)
{
    as.state = 1;
    actionQueueType aq;
    aq.state = 1;
    aq.target.desc = 3;
    aq.target.t_smo = tsmo;
    aq.target.t_pn = *tpn;
    aq.condition = 0;
    aq.as = PedInstance::pa_smNone;
    aq.group_desc = PedInstance::gd_mStandWalk;
    aq.act_exec = PedInstance::ai_aUseObject;
    as.actions.push_back(aq);
}

void PedInstance::createActQLeaveCar(actionQueueGroupType &as,
        ShootableMapObject *tsmo)
{
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smLeaveCar;
    aq.act_exec = PedInstance::ai_aLoseControl;
    aq.state = 1;
    aq.target.desc = 2;
    aq.target.t_smo = tsmo;
    aq.group_desc = PedInstance::gd_mExclusive;
    as.actions.push_back(aq);
}

void PedInstance::createActQWait(actionQueueGroupType &as, int tm_wait, uint8 desc)
{
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smNone;
    aq.group_desc = PedInstance::gd_mExclusive;
    if (desc == 2) {
        aq.state = 1 | 4 | 32 | 128;
    } else if (desc == 1)
        aq.state = 1 | 4;
    else
        aq.state = 1 | 32;
    aq.act_exec = PedInstance::ai_aWait;
    aq.multi_var.time_var.desc = desc;
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_wait = tm_wait;
    as.actions.push_back(aq);
}

bool PedInstance::createActQFindEnemy(actionQueueGroupType &as) {
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smNone;
    aq.group_desc = PedInstance::gd_mThink | PedInstance::gd_mExclusive;
    aq.state = 1;
    aq.act_exec = PedInstance::ai_aFindEnemy | PedInstance::ai_aWaitToStart;
    Mod *pMod = slots_[Mod::MOD_BRAIN];

    pedWeaponToUse pw_to_use;
    pw_to_use.use_ranks = false;

    int32 tm_wait = tm_before_check_;
    if (obj_group_def_ == PedInstance::og_dmAgent) {
        if (pMod) {
            tm_wait -= 25 * (pMod->getVersion() + 2);
            pw_to_use.use_ranks = true;
        }
        tm_wait = (double)tm_wait * (1.0 / perception_->getMultiplier());
    }
    aq.multi_var.time_var.desc = 0;
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_to_start = tm_wait;
    as.actions.push_back(aq);

    if (obj_group_def_ == PedInstance::og_dmAgent) {
        WeaponInstance *wi = selectedWeapon();
        if (wi && wi->usesAmmo() && wi->ammoRemaining() == 0) {
            wi = NULL;
            setSelectedWeapon(-1);
        }
        if (wi && wi->canShoot() && wi->doesPhysicalDmg()) {
            pw_to_use.desc = 2;
            pw_to_use.wpn.wi = wi;
        } else {
            pw_to_use.desc = 5;
            pw_to_use.wpn.dmg_type = MapObject::dmg_Physical;
        }
    } else {
        pw_to_use.desc = 5;
        pw_to_use.wpn.dmg_type = MapObject::dmg_Physical;
    }
    if (createActQFiring(as, NULL, NULL, false, 0, &pw_to_use))
        as.actions.back().state |= 64;
    else
        return false;
    createActQFollowing(as, NULL, 1);
    as.actions.back().state |= 64;
    createActQWalking(as, NULL, NULL, 0);
    as.actions.back().act_exec |= PedInstance::ai_aTimeExecute;
    as.actions.back().multi_var.time_var.exec_elapsed = 0;
    as.actions.back().multi_var.time_var.exec_time = 3000;
    as.actions.back().multi_var.dist_var.speed = 512;
    as.actions.back().state |= 64;
    return true;
}

bool PedInstance::createActQFindNonFriend(actionQueueGroupType &as)
{
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smNone;
    aq.group_desc = PedInstance::gd_mThink;
    aq.state = 1;
    aq.act_exec = PedInstance::ai_aFindNonFriend | PedInstance::ai_aWaitToStart;
    Mod *pMod = slots_[Mod::MOD_BRAIN];
    int32 tm_wait = tm_before_check_;
    if (obj_group_def_ == PedInstance::og_dmAgent) {
        if (pMod)
            tm_wait -= 25 * (pMod->getVersion() + 2);
        tm_wait = (double)tm_wait * (1.0 / perception_->getMultiplier());
    }
    aq.multi_var.time_var.desc = 0;
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_to_start = tm_wait;
    as.actions.push_back(aq);
    pedWeaponToUse pw_to_use;
    pw_to_use.desc = 4;
    pw_to_use.wpn.dmg_type = MapObject::dmg_Persuasion;
    if (createActQFiring(as, NULL, NULL, false, 0, &pw_to_use))
        as.actions.back().state |= 64;
    else
        return false;

    return true;
}

void PedInstance::createActQResetActionQueue(actionQueueGroupType &as) {
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smNone;
    aq.group_desc = PedInstance::gd_mExclusive;
    aq.state = 1;
    aq.act_exec = PedInstance::ai_aResetActionQueueQueue;
    as.actions.push_back(aq);
}

void PedInstance::createActQDeselectCurWeapon(actionQueueGroupType &as) {
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smNone;
    aq.group_desc = PedInstance::gd_mFire;
    aq.state = 1;
    aq.act_exec = PedInstance::ai_aDeselectCurWeapon;
    as.actions.push_back(aq);
}

void PedInstance::createActQTrigger(actionQueueGroupType &as, PathNode *tpn,
    int32 range)
{
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smNone;
    aq.group_desc = PedInstance::gd_mExclusive;
    aq.state = 1;
    aq.act_exec = PedInstance::ai_aTrigger;
    aq.target.t_pn = *tpn;
    aq.multi_var.dist_var.dist = range;
    as.actions.push_back(aq);
}

void PedInstance::createActQFindWeapon(actionQueueGroupType &as,
    pedWeaponToUse *pw_to_use, int dist)
{
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smNone;
    aq.group_desc = PedInstance::gd_mThink | PedInstance::gd_mExclusive;
    aq.state = 1;
    aq.act_exec = PedInstance::ai_aFindWeapon | PedInstance::ai_aWaitToStart;
    aq.multi_var.dist_var.dist = dist;
    aq.multi_var.enemy_var.pw_to_use.use_ranks = false;
    Mod *pMod = slots_[Mod::MOD_BRAIN];
    int32 tm_wait = tm_before_check_;
    if (obj_group_def_ == PedInstance::og_dmAgent) {
        if (pMod) {
            tm_wait -= 25 * (pMod->getVersion() + 2);
            aq.multi_var.enemy_var.pw_to_use.use_ranks = true;
        }
        tm_wait = (double)tm_wait * (1.0 / perception_->getMultiplier());
    }
    aq.multi_var.time_var.desc = 0;
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_to_start = tm_wait;

    if (!pw_to_use)
        aq.multi_var.enemy_var.pw_to_use = prefered_weapon_;
    else
        aq.multi_var.enemy_var.pw_to_use = *pw_to_use;

    as.actions.push_back(aq);

    createActQPickUp(as, NULL);
    std::vector <actionQueueType>::reverse_iterator it = as.actions.rbegin();
    (it++)->state |= 64;
    it->state |= 64;
}

void PedInstance::createActQCheckOwner(actionQueueGroupType &as) {
    as.state = 1;
    actionQueueType aq;
    aq.state = 1;
    aq.as = PedInstance::pa_smNone;
    aq.group_desc = PedInstance::gd_mThink | PedInstance::gd_mExclusive;
    aq.act_exec = PedInstance::ai_aCheckOwner | PedInstance::ai_aWaitToStart;

    Mod *pMod = slots_[Mod::MOD_BRAIN];
    int32 tm_wait = tm_before_check_;
    if (obj_group_def_ == PedInstance::og_dmAgent) {
        if (pMod) {
            tm_wait -= 25 * (pMod->getVersion() + 2);
        }
        tm_wait = (double)tm_wait * (1.0 / perception_->getMultiplier());
    }
    aq.multi_var.time_var.desc = 0;
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_to_start = tm_wait;
    as.actions.push_back(aq);
    if (createActQFiring(as, NULL, NULL, false, 1))
        as.actions.back().state |= 64;
}

void PedInstance::setActQInQueue(actionQueueGroupType &as,
    uint32 * id)
{
    // NOTE: if action is invalidated - all remaining actions in queue are
    // invalid, they should be removed
    if ((as.group_desc & PedInstance::gd_mExclusive) != 0) {
        dropActQ();
        setActionStateToDrawnAnim();
    } else {
        bool discarding = false;
        std::vector <actionQueueGroupType>::iterator it_s
            = actions_queue_.end();
        for (std::vector <actionQueueGroupType>::iterator it =
            actions_queue_.begin(); it != actions_queue_.end(); ++it)
        {
            if (discarding) {
                discardActG(it);
            } else if ((it->group_desc & as.group_desc) != 0
                && (it->state & 12) == 0)
            {
                discarding = true;
                it_s = it;
                discardActG(it);
            }
        }
        if (it_s != actions_queue_.end())
            actions_queue_.erase(it_s, actions_queue_.end());
        setActionStateToDrawnAnim();
    }

    // finally add the action group
    addActQToQueue(as, id);
}

bool PedInstance::addActQToQueue(actionQueueGroupType &as,
    uint32 * id)
{
    if (id != NULL) {
        as.group_id = action_grp_id_++;
        *id = as.group_id;
    } else
        as.group_id = 0;
    actions_queue_.push_back(as);
    return true;
}

void PedInstance::createDefQueue() {
    default_actions_.clear();

    actionQueueGroupType as;
    as.origin_desc = fs_actions::kOrigDefault;
    as.main_act = 0;

    if (createActQFindEnemy(as)) {
        // NOTE: not adding gd_mStandWalk because it will be usually blocked
        // and will not be added from defaults
        as.group_desc = PedInstance::gd_mThink | PedInstance::gd_mFire;
        as.group_id = action_grp_id_++;
        default_actions_.push_back(as);
    }
    as.actions.clear();
    if ((desc_state_ & PedInstance::pd_smControlled) != 0) {
        createActQCheckOwner(as);
        as.group_desc = PedInstance::gd_mThink | PedInstance::gd_mFire;
        as.group_id = action_grp_id_++;
        default_actions_.push_back(as);
        as.actions.clear();

        createActQFindWeapon(as);
        as.group_desc = PedInstance::gd_mThink | PedInstance::gd_mFire;
        as.state |= 64;
        as.group_id = action_grp_id_++;
        default_actions_.push_back(as);
        as.actions.clear();

        as.group_desc = PedInstance::gd_mStandWalk;
        createActQFollowing(as, owner_, 0, 192, 256);
        createActQWalking(as, NULL, NULL, rand() % 256, 128);
        as.state |= 64;
        as.group_id = action_grp_id_++;
        default_actions_.push_back(as);
        as.actions.clear();
    } else {
        if (obj_group_def_ == PedInstance::og_dmAgent) {
            if (isOurAgent()) {
                as.group_desc = PedInstance::gd_mThink | PedInstance::gd_mFire;
                if (createActQFindNonFriend(as)) {
                    as.group_id = action_grp_id_++;
                    default_actions_.insert(default_actions_.begin(), as);
                } else
                    as.actions.clear();
            }
        }/* else if (obj_group_def_ == PedInstance::og_dmCivilian
                   || obj_group_def_ == PedInstance::og_dmCriminal)
        {
            ;
        }*/
    }
}

void PedInstance::addDefActsToActions(uint32 groups_used) {
    for (std::vector <actionQueueGroupType>::iterator it = default_actions_.begin();
        it != default_actions_.end(); ++it)
    {
        if ((groups_used & it->group_desc) == 0
            && findQueueInActQueue(it->group_id) == actions_queue_.end())
        {
            actions_queue_.push_back(*it);
        }
    }
}

void PedInstance::discardActG(uint32 id) {
    for (std::vector <actionQueueGroupType>::iterator it =
        actions_queue_.begin(); it != actions_queue_.end(); ++it)
    {
        if (it->group_id == id) {
            discardActG(it);
            setActionStateToDrawnAnim();
            break;
        }
    }
}

void PedInstance::discardActG(std::vector <actionQueueGroupType>::iterator it_a)
{
    if ((it_a->state & 14) == 2) {
        for (std::vector <actionQueueType>::iterator it =
            it_a->actions.begin(); it != it_a->actions.end(); ++it)
        {
            if ((it->state & 12) == 0 || (it->state & 128) != 0) {
                it->state |= 8;
                it->state &= ~128;
                switchActionStateFrom(it->as);
            }
        }
        it_a->state |= 8;
        it_a->state &= ~128;
    }
}

void PedInstance::dropActQ() {
    for (std::vector <actionQueueGroupType>::iterator it =
        actions_queue_.begin(); it != actions_queue_.end(); ++it)
    {
            discardActG(it);
    }
    actions_queue_.clear();
}

void PedInstance::updtActGFiringShots(uint32 id, uint32 make_shots)
{
    for (std::vector <actionQueueGroupType>::iterator it =
        actions_queue_.begin(); it != actions_queue_.end(); ++it)
    {
            if (it->group_id == id) {
                for (std::vector <actionQueueType>::iterator it_a =
                    it->actions.begin(); it_a != it->actions.end(); ++it)
                {
                    if ((it_a->act_exec & (PedInstance::ai_aDestroyObject
                        | PedInstance::ai_aAttackLocation
                        | PedInstance::ai_aAquireControl)) != 0)
                    {
                        it_a->multi_var.enemy_var.make_shots = make_shots;
                        break;
                    }
                }
                break;
            }
    }
}

void PedInstance::updtActGFiring(uint32 id, PathNode* tpn,
    ShootableMapObject* tsmo)
{
    for (std::vector <actionQueueGroupType>::iterator it =
        actions_queue_.begin(); it != actions_queue_.end(); ++it)
    {
            if (it->group_id == id) {
                for (std::vector <actionQueueType>::iterator it_a =
                    it->actions.begin(); it_a != it->actions.end(); ++it)
                {
                    if ((it_a->act_exec & (PedInstance::ai_aDestroyObject
                        | PedInstance::ai_aAttackLocation)) != 0)
                    {
                        it_a->act_exec &= PedInstance::ai_aAll
                            ^ (PedInstance::ai_aDestroyObject
                            | PedInstance::ai_aAttackLocation);
                        // TODO : update when condition will be used
                        if (tsmo) {
                            it_a->target.t_smo = tsmo;
                            it_a->act_exec |= PedInstance::ai_aDestroyObject;
                        } else {
                            it_a->target.t_pn = *tpn;
                            it_a->act_exec |= PedInstance::ai_aAttackLocation;
                            it_a->target.t_smo = NULL;
                        }
                        break;
                    }
                    if ((it_a->act_exec
                        & (PedInstance::ai_aAquireControl)) != 0)
                    {
                        if (tsmo) {
                            it_a->target.t_smo = tsmo;
                        } else {
                            it_a->target.t_pn = *tpn;
                            it_a->target.t_smo = NULL;
                        }
                        break;
                    }
                }
                break;
            }
    }
}

bool PedInstance::checkActGCompleted(fs_actions::CreatOrigin origin) {
    for (std::vector <actionQueueGroupType>::iterator it =
        actions_queue_.begin(); it != actions_queue_.end(); ++it)
    {
        if ((it->origin_desc) == origin && (it->state & 4) != 0)
            return true;
    }
    return false;
}

void PedInstance::pauseAllInActG(actionQueueGroupType &as, uint32 start_pos) {
    for (std::vector <actionQueueType>::iterator it =
        as.actions.begin() + start_pos; it != as.actions.end(); ++it)
    {
        it->state |= 64;
    }
}

std::vector <PedInstance::actionQueueType>::iterator PedInstance::findActInQueue(uint32 act_find,
    actionQueueGroupType &as, std::vector <actionQueueType>::iterator search_from)
{
    for (std::vector <actionQueueType>::iterator it =
        search_from; it != as.actions.end(); ++it)
    {
        if ((it->act_exec & act_find) != 0) {
            return it;
        }
    }
    return as.actions.end();
}

std::vector <PedInstance::actionQueueGroupType>::iterator PedInstance::findQueueInActQueue(uint32 id)
{
    for (std::vector <actionQueueGroupType>::iterator it =
        actions_queue_.begin(); it != actions_queue_.end(); ++it)
    {
        if ((it->group_id) == id)
            return it;
    }
    return actions_queue_.end();
}


