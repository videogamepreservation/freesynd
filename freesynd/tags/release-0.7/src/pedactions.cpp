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

#include "common.h"
#include "app.h"


void PedInstance::createActQStanding(actionQueueGroupType &as) {
    as.state = 1;
    actionQueueType aq;
    aq.ot_execute = PedInstance::ai_aWait;
    aq.as = PedInstance::pa_smStanding;
    aq.state = 1;
    aq.multi_var.time_var.desc = 0;
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_total = -1;
    aq.group_desc = PedInstance::gd_mStandWalk;
    as.actions.push_back(aq);
}

void PedInstance::createActQWalking(actionQueueGroupType &as, PathNode *tpn,
    ShootableMapObject *tsmo, int32 dir, int32 dist, bool bounce)
{
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smWalking;
    aq.ot_execute = PedInstance::ai_aReachLocation;
    aq.group_desc = PedInstance::gd_mStandWalk;
    aq.state = 1;
    aq.multi_var.dist_var.dir = dir;
    aq.multi_var.dist_var.dist = dist;
    aq.multi_var.dist_var.speed = -1;
    if (dir == -1) {
        if (tpn) {
            aq.t_pn = *tpn;
            aq.condition = 0;
        } else {
            aq.t_smo = tsmo;
            aq.condition = 2;
        }
    } else {
        aq.multi_var.dist_var.dir_move.clear();
        // TODO: implement response from directional movement
        aq.multi_var.dist_var.dir_move.bounce = bounce;
        if (tpn) {
            // direction will be calculated from pathnode position
            aq.t_pn = *tpn;
            aq.condition = 3;
        } else if (tsmo) {
            // direction will be calculated from objects position
            aq.t_smo = tsmo;
            aq.condition = 4;
        } else { // directional movement only
            aq.condition = 1;
            aq.multi_var.dist_var.dir_move.bounce = bounce;
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
    aq.ot_execute = PedInstance::ai_aReachLocation;
    aq.group_desc = PedInstance::gd_mExclusive;
    aq.multi_var.dist_var.speed = -1;
    aq.state = 1;
    // TODO: set directional movement to opposite?
    aq.multi_var.dist_var.dir = dir;
    aq.condition = 1;
    // calculate direction from this point
    if (dir == -1)
        aq.t_pn = *tpn;
    as.actions.push_back(aq);
}

bool PedInstance::createActQFiring(actionQueueGroupType &as, PathNode *tpn,
    ShootableMapObject *tsmo, bool forced_shot, uint32 make_shots, 
    pedWeaponToUse *pw_to_use, int32 value)
{
    as.state = 1;
    actionQueueType aq;
    aq.group_desc = PedInstance::gd_mFire;
    bool can_shoot = false;
    bool does_phys_dmg = false;
    Weapon *pWeapon = NULL;
    if (pw_to_use) {
        switch (pw_to_use->desc) {
            // TODO: use it later, convert indx to weapon pointer
            case 1:
                // indx used
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
        aq.multi_var.enemy_var.weapon = *pw_to_use;
    } else {
        WeaponInstance *wi = selectedWeapon();
        if (wi) {
            can_shoot = wi->canShoot();
            does_phys_dmg = wi->doesPhysicalDmg();
            aq.multi_var.enemy_var.weapon.desc = 2;
            aq.multi_var.enemy_var.weapon.wpn.wi = wi;
        } else
            return false;
    }

    if (!can_shoot)
        return false;

    aq.multi_var.enemy_var.value = value;
    aq.multi_var.enemy_var.forced_shot = forced_shot;
    if (does_phys_dmg) {
        aq.as = PedInstance::pa_smFiring;
        // TODO: use condition to set more information for action execution
        // continuos shooting until ammo ends(or all weapons ammo),
        // until target destroyed, type of damage that will complete action
        aq.multi_var.enemy_var.make_shots = make_shots;
        aq.multi_var.enemy_var.shots_done = 0;
        if (tpn) {
            aq.t_pn = *tpn;
            aq.ot_execute = PedInstance::ai_aAttackLocation;
            aq.t_smo = NULL;
        } else {
            aq.t_smo = tsmo;
            aq.ot_execute = PedInstance::ai_aDestroyObject;
        }
    } else {
        if (tsmo && tsmo->majorType() == MapObject::mjt_Ped) {
            aq.as = PedInstance::pa_smNone;
            aq.multi_var.enemy_var.make_shots = make_shots;
            aq.multi_var.enemy_var.shots_done = 0;
            aq.t_smo = tsmo;
            aq.ot_execute = PedInstance::ai_aAquireControl;
        } else
            return false;
    }
    aq.state = 1;
    aq.ot_execute |= PedInstance::ai_aWait;
    aq.multi_var.time_var.desc = 1;
    as.actions.push_back(aq);
    return true;
}

void PedInstance::createActQFollowing(actionQueueGroupType &as,
    ShootableMapObject *tsmo, uint32 condition, int32 dist)
{
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smFollowing;
    aq.ot_execute = PedInstance::ai_aFollowObject;
    aq.group_desc = PedInstance::gd_mStandWalk;
    aq.state = 1;
    aq.t_smo = tsmo;
    aq.multi_var.dist_var.dist = dist;
    aq.multi_var.dist_var.speed = -1;
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
    aq.t_smo = tsmo;
    aq.as = PedInstance::pa_smPickUp;
    aq.ot_execute = PedInstance::ai_aPickUpObject;
    aq.ot_execute |= PedInstance::ai_aWait;
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
    aq.ot_execute = PedInstance::ai_aPutDownObject;
    aq.group_desc = PedInstance::gd_mExclusive;
    aq.state = 1;
    aq.t_smo = tsmo;
    aq.ot_execute |= PedInstance::ai_aWait;
    aq.multi_var.time_var.desc = 1;
    as.actions.push_back(aq);
}

void PedInstance::createActQBurning(actionQueueGroupType &as) {
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smBurning;
    aq.ot_execute = PedInstance::ai_aReachLocation;
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
    aq.ot_execute = PedInstance::ai_aWait;
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_total = 1000;
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
    aq.t_smo = tsmo;
    // TODO: change this use enum
    aq.condition = 0;
    aq.as = PedInstance::pa_smGetInCar;
    aq.group_desc = PedInstance::gd_mExclusive;
    aq.ot_execute = PedInstance::ai_aAquireControl;
    as.actions.push_back(aq);
}

void PedInstance::createActQUsingCar(actionQueueGroupType &as, PathNode *tpn,
    ShootableMapObject *tsmo)
{
    as.state = 1;
    actionQueueType aq;
    aq.state = 1;
    aq.t_smo = tsmo;
    aq.t_pn = *tpn;
    aq.condition = 1;
    aq.as = PedInstance::pa_smNone;
    aq.group_desc = PedInstance::gd_mStandWalk;
    aq.ot_execute = PedInstance::ai_aUseObject;
    as.actions.push_back(aq);
}

void PedInstance::createActQInCar(actionQueueGroupType &as, PathNode *tpn,
    ShootableMapObject *tsmo)
{
    as.state = 1;
    actionQueueType aq;
    aq.state = 1;
    aq.t_smo = tsmo;
    aq.t_pn = *tpn;
    aq.condition = 0;
    aq.as = PedInstance::pa_smNone;
    aq.group_desc = PedInstance::gd_mStandWalk;
    aq.ot_execute = PedInstance::ai_aUseObject;
    as.actions.push_back(aq);
}

void PedInstance::createActQLeaveCar(actionQueueGroupType &as,
        ShootableMapObject *tsmo)
{
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smLeaveCar;
    aq.ot_execute = PedInstance::ai_aLoseControl;
    aq.state = 1;
    aq.t_smo = tsmo;
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
    aq.ot_execute = PedInstance::ai_aWait;
    aq.multi_var.time_var.desc = desc;
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_total = tm_wait;
    as.actions.push_back(aq);
}

void PedInstance::createActQFindEnemy(actionQueueGroupType &as) {
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smNone;
    aq.group_desc = PedInstance::gd_mThink;
    aq.state = 1;
    aq.ot_execute = PedInstance::ai_aFindEnemy;
    as.actions.push_back(aq);
    pedWeaponToUse pw_to_use;
    pw_to_use.desc = 5;
    pw_to_use.wpn.dmg_type = MapObject::dmg_Physical;
    if (createActQFiring(as, NULL, NULL, false, 0, &pw_to_use))
        as.actions.back().state |= 64;
    createActQFollowing(as, NULL, 1);
    as.actions.back().state |= 64;
}

void PedInstance::createActQResetActionQueue(actionQueueGroupType &as) {
    as.state = 1;
    actionQueueType aq;
    aq.as = PedInstance::pa_smNone;
    aq.group_desc = PedInstance::gd_mExclusive;
    aq.state = 1;
    aq.ot_execute = PedInstance::ai_aResetActionQueueQueue;
    as.actions.push_back(aq);
}


bool PedInstance::setActQInQueue(actionQueueGroupType &as,
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
            actions_queue_.begin(); it != actions_queue_.end(); it++)
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
    if (id != NULL) {
        as.group_id = action_grp_id_++;
        *id = as.group_id;
    } else {
        as.group_id = 0;
    }
    actions_queue_.push_back(as);
    return true;
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

bool PedInstance::addDefActsToActions(actionQueueGroupType &as) {
    /*
    for (std::vector <actionQueueGroupType>::iterator it = actions_queue_.begin();
        it != actions_queue_.end(); it++)
    {
    }
    */
    actions_queue_.push_back(as);
    return true;
}

void PedInstance::discardActG(uint32 id) {
    for (std::vector <actionQueueGroupType>::iterator it =
        actions_queue_.begin(); it != actions_queue_.end(); it++)
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
            it_a->actions.begin(); it != it_a->actions.end(); it++
        ) {
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
        actions_queue_.begin(); it != actions_queue_.end(); it++)
    {
            discardActG(it);
    }
    actions_queue_.clear();
}

void PedInstance::updtActGFiringShots(uint32 id, uint32 make_shots)
{
    for (std::vector <actionQueueGroupType>::iterator it =
        actions_queue_.begin(); it != actions_queue_.end(); it++)
    {
            if (it->group_id == id) {
                for (std::vector <actionQueueType>::iterator it_a =
                    it->actions.begin(); it_a != it->actions.end(); it++)
                {
                    if ((it_a->ot_execute & (PedInstance::ai_aDestroyObject
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
        actions_queue_.begin(); it != actions_queue_.end(); it++)
    {
            if (it->group_id == id) {
                for (std::vector <actionQueueType>::iterator it_a =
                    it->actions.begin(); it_a != it->actions.end(); it++)
                {
                    if ((it_a->ot_execute & (PedInstance::ai_aDestroyObject
                        | PedInstance::ai_aAttackLocation)) != 0)
                    {
                        it_a->ot_execute &= PedInstance::ai_aAll
                            ^ (PedInstance::ai_aDestroyObject
                            | PedInstance::ai_aAttackLocation);
                        // TODO : update with when condition will be used
                        if (tsmo) {
                            it_a->t_smo = tsmo;
                            it_a->ot_execute |= PedInstance::ai_aDestroyObject;
                        } else {
                            it_a->t_pn = *tpn;
                            it_a->ot_execute |= PedInstance::ai_aAttackLocation;
                            it_a->t_smo = NULL;
                        }
                        break;
                    }
                    if ((it_a->ot_execute
                        & (PedInstance::ai_aAquireControl)) != 0)
                    {
                        if (tsmo) {
                            it_a->t_smo = tsmo;
                        } else {
                            it_a->t_pn = *tpn;
                            it_a->t_smo = NULL;
                        }
                        break;
                    }
                }
                break;
            }
    }
}
