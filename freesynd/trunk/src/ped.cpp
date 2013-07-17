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

#include "common.h"
#include "app.h"
#include "vehicle.h"
#include "mission.h"
#include "core/squad.h"

Ped::Ped() {
    memset(stand_anims_, 0, sizeof(stand_anims_));
    memset(walk_anims_, 0, sizeof(walk_anims_));
    memset(stand_fire_anims_, 0, sizeof(stand_fire_anims_));
    memset(walk_fire_anims_, 0, sizeof(walk_fire_anims_));
}

bool Ped::drawStandFrame(int x, int y, int dir, int frame,
                         Weapon::WeaponAnimIndex weapon)
{
    assert(weapon < NUM_ANIMS);
    return g_App.gameSprites().drawFrame(
            stand_anims_[weapon] + dir, frame, x, y);
}

int Ped::lastStandFrame(int dir, Weapon::WeaponAnimIndex weapon) {
    assert(weapon != 0 && weapon < NUM_ANIMS);
    return g_App.gameSprites().lastFrame(stand_anims_[weapon] + dir);
}

bool Ped::drawWalkFrame(int x, int y, int dir, int frame,
                        Weapon::WeaponAnimIndex weapon)
{
    assert(weapon < NUM_ANIMS);
    return g_App.gameSprites().drawFrame(
            walk_anims_[weapon] + dir, frame, x, y);
}

int Ped::lastWalkFrame(int dir, Weapon::WeaponAnimIndex weapon) {
    assert(weapon != 0 && weapon < NUM_ANIMS);
    return g_App.gameSprites().lastFrame(walk_anims_[weapon] + dir);
}

bool Ped::drawStandFireFrame(int x, int y, int dir, int frame,
        Weapon::WeaponAnimIndex weapon) {
    assert(weapon != 0 && weapon < NUM_ANIMS);
    return g_App.gameSprites().drawFrame(
            stand_fire_anims_[weapon] + dir, frame, x, y);
}

int Ped::lastStandFireFrame(int dir, Weapon::WeaponAnimIndex weapon)
{
    assert(weapon != 0 && weapon < NUM_ANIMS);
    return g_App.gameSprites().lastFrame(
            stand_fire_anims_[weapon] + dir);
}

bool Ped::drawWalkFireFrame(int x, int y, int dir, int frame,
        Weapon::WeaponAnimIndex weapon) {
    assert(weapon != 0 && weapon < NUM_ANIMS);
    return g_App.gameSprites().drawFrame(
            walk_fire_anims_[weapon] + dir, frame, x, y);
}

int Ped::lastWalkFireFrame(int dir, Weapon::WeaponAnimIndex weapon)
{
    assert(weapon != 0 && weapon < NUM_ANIMS);
    return g_App.gameSprites().lastFrame(walk_fire_anims_[weapon] + dir);
}

bool Ped::drawDieFrame(int x, int y, int frame) {
    return g_App.gameSprites().drawFrame(die_anim_, frame, x, y);
}

int Ped::lastDieFrame() {
    return g_App.gameSprites().lastFrame(die_anim_);
}

void Ped::drawDeadFrame(int x, int y, int frame) {
    g_App.gameSprites().drawFrame(dead_anim_, frame, x, y);
}

void Ped::drawDeadAgentFrame(int x, int y, int frame) {
    g_App.gameSprites().drawFrame(dead_agent_anim_, frame, x, y);
}

void Ped::drawHitFrame(int x, int y, int dir, int frame) {
    g_App.gameSprites().drawFrame(hit_anim_ + dir / 2, frame, x, y);
}

int Ped::lastHitFrame(int dir) {
    return g_App.gameSprites().lastFrame(hit_anim_ + dir / 2);
}

void Ped::drawPickupFrame(int x, int y, int frame) {
    g_App.gameSprites().drawFrame(pickup_anim_, frame, x, y);
}

int Ped::lastPickupFrame() {
    // same for putdown weapon
    return g_App.gameSprites().lastFrame(pickup_anim_);
}

void Ped::drawVaporizeFrame(int x, int y, int dir, int frame) {
    g_App.gameSprites().drawFrame(vaporize_anim_ + dir / 2, frame, x, y);
}

int Ped::lastVaporizeFrame(int dir) {
    return g_App.gameSprites().lastFrame(vaporize_anim_ + dir / 2);
}

void Ped::drawSinkFrame(int x, int y, int frame) {
    g_App.gameSprites().drawFrame(sink_anim_, frame, x, y);
}

int Ped::lastSinkFrame() {
    return g_App.gameSprites().lastFrame(sink_anim_);
}

void Ped::drawStandBurnFrame(int x, int y, int frame) {
    g_App.gameSprites().drawFrame(stand_burn_anim_, frame, x, y);
}

void Ped::drawWalkBurnFrame(int x, int y, int frame) {
    g_App.gameSprites().drawFrame(walk_burn_anim_, frame, x, y);
}

void Ped::drawDieBurnFrame(int x, int y, int frame) {
    g_App.gameSprites().drawFrame(die_burn_anim_, frame, x, y);
}

int Ped::lastDieBurnFrame() {
    return g_App.gameSprites().lastFrame(die_burn_anim_);
}

void Ped::drawSmokeBurnFrame(int x, int y, int frame) {
    g_App.gameSprites().drawFrame(smoke_burn_anim_, frame, x, y);
}

void Ped::drawDeadBurnFrame(int x, int y, int frame) {
    g_App.gameSprites().drawFrame(dead_burn_anim_, frame, x, y);
}

void Ped::drawPersuadeFrame(int x, int y, int frame) {
    g_App.gameSprites().drawFrame(persuade_anim_, frame, x, y);
}

int Ped::lastPersuadeFrame() {
    return g_App.gameSprites().lastFrame(persuade_anim_);
}

void PedInstance::switchActionStateTo(uint32 as) {
    switch(as) {
        case pa_smNone:
            //printf("Ped has undefined state");
            break;
        case pa_smStanding:
            state_ &= (pa_smAll ^(pa_smFollowing
                | pa_smUsingCar | pa_smInCar));
            state_ |= pa_smStanding;
            break;
        case pa_smWalking:
            state_ &= (pa_smAll ^(pa_smFollowing
                | pa_smUsingCar | pa_smInCar));
            state_ |= pa_smWalking;
            break;
        case pa_smHit:
            state_ = pa_smHit;
            break;
        case pa_smFiring:
            state_ |= pa_smFiring;
            break;
        case pa_smFollowing:
            state_ &= (pa_smAll ^ (pa_smStanding | pa_smWalking));
            state_ |= pa_smFollowing;
            break;
        case pa_smPickUp:
            state_ = pa_smPickUp;
            break;
        case pa_smPutDown:
            state_ = pa_smPutDown;
            break;
        case pa_smBurning:
            state_ = pa_smBurning;
            break;
        case pa_smGetInCar:
            state_ = pa_smStanding | pa_smGetInCar;
            break;
        case pa_smUsingCar:
            state_ = pa_smStanding | pa_smUsingCar;
            break;
        case pa_smInCar:
            state_ = pa_smStanding | pa_smInCar;
            break;
        case pa_smLeaveCar:
            state_ = pa_smStanding | pa_smLeaveCar;
            break;
        case pa_smDead:
            state_ = pa_smDead;
            break;
        case pa_smUnavailable:
            state_ = pa_smUnavailable;
            break;
    }
}

void PedInstance::switchActionStateFrom(uint32 as) {
    switch(as) {
        case pa_smNone:
            //printf("Ped has undefined state");
            break;
        case pa_smStanding:
            state_ &= pa_smAll ^ pa_smStanding;
            break;
        case pa_smWalking:
            state_ &= pa_smAll ^ pa_smWalking;
            state_ |= pa_smStanding;
            break;
        case pa_smHit:
            state_ &= pa_smAll ^ pa_smHit;
            break;
        case pa_smFiring:
            state_ &= pa_smAll ^ pa_smFiring;
            break;
        case pa_smFollowing:
            state_ &= pa_smAll ^ (pa_smFollowing | pa_smWalking);
            state_ |= pa_smStanding;
            break;
        case pa_smPickUp:
        case pa_smPutDown:
            state_ = pa_smStanding;
            break;
        case pa_smBurning:
            state_ &= pa_smAll ^ pa_smBurning;
            break;
        case pa_smGetInCar:
            state_ &= pa_smAll ^ (pa_smStanding | pa_smGetInCar);
            break;
        case pa_smUsingCar:
            state_ &= pa_smAll ^ (pa_smStanding | pa_smUsingCar);
            break;
        case pa_smInCar:
            state_ &= pa_smAll ^ (pa_smStanding | pa_smInCar);
            break;
        case pa_smLeaveCar:
            state_ = pa_smStanding;
            break;
        case pa_smDead:
            state_ = pa_smDead;
#ifdef _DEBUG
            printf("It's alive!\n");
#endif
            break;
        case pa_smUnavailable:
            state_ = pa_smUnavailable;
            break;
    }
}

void PedInstance::setActionStateToDrawnAnim(void) {
    // TODO: complete
    if ((state_ & pa_smUnavailable) != 0) {
        setDrawnAnim(PedInstance::ad_NoAnimation);
    } else if ((state_ & pa_smDead) != 0) {
        //setDrawnAnim(PedInstance::ad_DeadAnim);
    } else if ((state_ & (pa_smWalking | pa_smFollowing)) != 0) {
        if ((state_ & pa_smFiring) != 0)
            setDrawnAnim(PedInstance::ad_WalkFireAnim);
        else
            setDrawnAnim(PedInstance::ad_WalkAnim);
    } else if ((state_ & pa_smStanding) != 0) {
        if ((state_ & pa_smFiring) != 0)
            setDrawnAnim(PedInstance::ad_StandFireAnim);
        else
            setDrawnAnim(PedInstance::ad_StandAnim);
    } else if ((state_ & pa_smPickUp) != 0) {
        setDrawnAnim(PedInstance::ad_PickupAnim);
    } else if ((state_ & pa_smPutDown) != 0) {
        setDrawnAnim(PedInstance::ad_PutdownAnim);
    } else if ((state_ & (pa_smUsingCar | pa_smInCar)) != 0) {
        setDrawnAnim(PedInstance::ad_StandAnim);
    }
#ifdef _DEBUG
    if (state_ ==  pa_smNone)
        printf("undefined state_\n");
#endif
}

bool PedInstance::animate(int elapsed, Mission *mission) {
    // TODO: weapon selection action, and use only deselectweapon
    // to disarm

    bool updated = false;
    if (actions_property_ == 1) {
        dropActQ();
        actions_property_ = 0;
    }

    if (!actions_queue_.empty()) {
        for (uint32 indx = 0; indx < actions_queue_.size(); ++indx) {
            std::vector <actionQueueGroupType>::iterator it =
                actions_queue_.begin() + indx;
            if ((it->state & 128) == 0 && (it->state & 12) != 0
                // do not remove scripted actions
                && (it->origin_desc != fs_actions::kOrigScript))
            {
                actions_queue_.erase(it);
            }
        }
    }
#if 0
#ifdef _DEBUG
    if ((desc_state_ & pd_smControlled) != 0) {
        for (uint32 indx = 0; indx < actions_queue_.size(); ++indx) {
            std::vector <actionQueueGroupType>::iterator it =
                actions_queue_.begin() + indx;
            printf("act = %x , stt = %i\n", it->actions[0].act_exec, it->actions[0].state);
        }
        printf("=====end\n");
    }
#endif
#endif

    last_firing_target_.desc = 0;
    // NOTE: some actions have remaining time, it is lost for now
    if (actions_queue_.empty()) {
        if ((state_ & pa_smCheckExcluded) == 0) {
            addDefActsToActions();
        }
    } else {
        friends_not_seen_.clear();
        // TODO: xor finished and failed, should all actions
        // have execution time set?
        uint32 groups_processed = 0;
        for (std::vector <actionQueueGroupType>::iterator it =
            actions_queue_.begin(); it != actions_queue_.end()
            && actions_property_ == 0; ++it)
        {
            if ((it->state & 128) == 0 && (it->state & 76) != 0)
                continue;
            if ((it->group_desc & groups_processed) != 0)
                break;
            if ((it->group_desc & PedInstance::gd_mExclusive) != 0
                && groups_processed != 0)
            {
                break;
            }

            uint32 acts_g_prcssd = 0;
            updtPreferedWeapon();
            for (uint32 indx = 0; indx < it->actions.size(); ++indx)
            {
                std::vector <actionQueueType>::iterator aqt = it->actions.begin() + indx;
                if ((acts_g_prcssd & aqt->group_desc) != 0
                    || (acts_g_prcssd & groups_processed) != 0)
                {
                    break;
                }
                if ((aqt->state & 128) == 0 && (aqt->state & 76) != 0)
                    continue;
                if ((aqt->group_desc & PedInstance::gd_mExclusive) != 0
                    && acts_g_prcssd != 0)
                {
                    break;
                }
#ifdef _DEBUG
                if ((aqt->act_exec & PedInstance::ai_aNone) != 0)
                {
                    printf("obj_None");
                }
#endif
                if ((aqt->act_exec & PedInstance::ai_aTrigger) != 0)
                {
                    toDefineXYZ xyz;
                    aqt->target.t_pn.convertPosToXYZ(&xyz);
                    aqt->state |= 2;
                    for (uint8 i = 0; i < AgentManager::kMaxSlot; ++i) {
                        PedInstance *p_agent = mission->getSquad()->member(i);
                        if(p_agent && (int32)(p_agent->distanceToPosXYZ(&xyz))
                           < aqt->multi_var.dist_var.dist)
                        {
                            for (std::vector <actionQueueType>::iterator it_a
                                 = aqt + 1; it_a != it->actions.end(); ++it_a)
                            {
                                it_a->state ^= 64;
                            }
                            aqt->state |= 4;
                            it->main_act = it->actions.size() - 1;
                            break;
                        }
                    }
                }
                if ((aqt->act_exec & PedInstance::ai_aWaitToStart) != 0)
                {
                    aqt->state |= 384;
                    //if (aqt->multi_var.time_var.desc == 0) {
                        aqt->multi_var.time_var.elapsed += elapsed;
                        if (aqt->multi_var.time_var.elapsed >=
                            aqt->multi_var.time_var.time_to_start)
                        {
                            aqt->state ^= 384;
                            aqt->act_exec ^= PedInstance::ai_aWaitToStart;
                            elapsed = aqt->multi_var.time_var.elapsed
                                - aqt->multi_var.time_var.time_to_start;
                            aqt->multi_var.time_var.elapsed = 0;
                        }
                    //}
                }
                if ((aqt->act_exec & PedInstance::ai_aUseObject) != 0)
                {
                    if (aqt->target.t_smo->isDead()
                        || (state_ & (PedInstance::pa_smInCar
                        | PedInstance::pa_smUsingCar)) == 0
                        || !((VehicleInstance *)aqt->target.t_smo)->isInsideVehicle(this))
                    {
                        aqt->state |= 8;
                    } else {
                        VehicleInstance *v = (VehicleInstance *)aqt->target.t_smo;
                        if (aqt->state == 1) {
                            if (aqt->condition == 0) {
                                if (checkCurrPosTileOnly(aqt->target.t_pn))
                                    aqt->state |= 4;
                                else {
                                    if (v->getDriver() == NULL) {
                                        v->forceSetDriver(this);
                                        if (v->isDriver(this)) {
                                            state_ = (state_ ^ PedInstance::pa_smInCar)
                                                | PedInstance::pa_smUsingCar;
                                        }
                                    }
                                    if ((state_ & PedInstance::pa_smUsingCar) != 0) {
                                        v->setDestinationV(mission, aqt->target.t_pn.tileX(),
                                            aqt->target.t_pn.tileY(), aqt->target.t_pn.tileZ(),
                                            aqt->target.t_pn.offX(), aqt->target.t_pn.offY(), 1024);
                                        if (v->isMoving()) {
                                            aqt->state |= 2;
                                            // TODO: create way to properly set offsets
                                            // in t_pn, without this checkCurrPos fails
                                        } else
                                            aqt->state |= 8;
                                    } else
                                        aqt->state |= 2;
                                }
                            } else if (aqt->condition == 1) {
                                if ((state_ & PedInstance::pa_smUsingCar) == 0)
                                    aqt->state |= 8;
                                else {
                                    if (checkCurrPosTileOnly(aqt->target.t_pn))
                                        aqt->state |= 4;
                                    else {
                                        v->setDestinationV(mission, aqt->target.t_pn.tileX(),
                                            aqt->target.t_pn.tileY(), aqt->target.t_pn.tileZ(),
                                            aqt->target.t_pn.offX(), aqt->target.t_pn.offY(), 1024);
                                        if (v->isMoving()) {
                                            aqt->state |= 2;
                                            // TODO: create way to properly set offsets
                                            // in t_pn, without this checkCurrPos fails
                                        } else
                                            aqt->state |= 8;
                                    }
                                }
                            }
                        } else if (aqt->state == 3) {
                            if (checkCurrPosTileOnly(aqt->target.t_pn))
                                aqt->state |= 4;
                        }
                    }
                }
                if ((aqt->act_exec & PedInstance::ai_aAquireControl) != 0)
                {
                    if (aqt->target.t_smo->isAlive()) {
                        if (aqt->target.t_smo->majorType() == MapObject::mjt_Ped) {
                            if (selectRequiredWeapon(&aqt->multi_var.enemy_var.pw_to_use))
                            {
                                WeaponInstance *wi = selectedWeapon();
                                int tm_left = elapsed;
                                uint16 answ = wi->inflictDamage(aqt->target.t_smo, NULL, &tm_left);
                                if (answ == 0) {
                                    if (checkFriendIs((PedInstance *)aqt->target.t_smo))
                                        aqt->state |= 4;
                                } else if (answ == 2) {
                                    aqt->state |= 2;
                                } else
                                    aqt->state |= 8;
                            } else
                                aqt->state |= 8;
                        } else if (aqt->target.t_smo->majorType()
                                    == MapObject::mjt_Vehicle)
                        {
                            VehicleInstance *v = (VehicleInstance *)aqt->target.t_smo;
                            if (v->isAlive() && (state_
                                & (PedInstance::pa_smInCar
                                | PedInstance::pa_smUsingCar)) == 0
                                && samePosition(v))
                            {
                                if (aqt->condition == 0) {
                                    v->setDriver(this);
                                    if (v->isInsideVehicle(this)) {
                                        aqt->state |= 4;
                                        if (v->isDriver(this))
                                            putInVehicle(v, pa_smUsingCar);
                                        else
                                            putInVehicle(v, pa_smInCar);
                                    } else
                                        aqt->state |= 8;
                                } else if (aqt->condition == 1) {
                                    if (v->hasDriver()) {
                                        if (v->isDriver(this))
                                            aqt->state |= 4;
                                        else
                                            aqt->state |= 8;
                                    } else {
                                        v->setDriver(this);
                                        aqt->state |= 4;
                                        putInVehicle(v, pa_smUsingCar);
                                    }
                                }
                            } else
                                aqt->state |= 8;
                        } else
                            // type cannot be aquired
                            aqt->state |= 8;
                    } else
                        aqt->state |= 8;
                }
                if ((aqt->act_exec & PedInstance::ai_aLoseControl) != 0)
                {
                    if (aqt->state == 1) {
                        if (aqt->target.t_smo->majorType() == MapObject::mjt_Ped) {
                            // TODO: but not now
                        } else if (aqt->target.t_smo->majorType()
                                   == MapObject::mjt_Vehicle)
                        {
                            VehicleInstance *v = (VehicleInstance *)aqt->target.t_smo;
                            if (v->isInsideVehicle(this)) {
                                v->removeDriver(this);
                                aqt->state |= 4;
                                leaveVehicle();
                            } else
                                aqt->state |= 8;
                        }
                    }
                }
                if ((aqt->act_exec & PedInstance::ai_aPickUpObject) != 0)
                {
                    if (aqt->state == 1) {
                        //TODO: better state checking
                        if ((state_ & (PedInstance::pa_smInCar
                            | PedInstance::pa_smUsingCar
                            | PedInstance::pa_smHit)) != 0)
                        {
                            aqt->state |= 8;
                        }
                        WeaponInstance *wi = (WeaponInstance *)aqt->target.t_smo;
                        if (wi->hasOwner() || weapons_.size() == 8 || wi->isDead()
                            || !samePosition(wi))
                            aqt->state |= 8;
                        else {
                            wi->setOwner(this);
                            wi->setMap(-1);
                            wi->setIsIgnored(true);
                            wi->deactivate();
                            weapons_.push_back(wi);
                            aqt->state |= 36;
                        }
                    }
                }
                if ((aqt->act_exec & PedInstance::ai_aDestroyObject) != 0)
                {
                    if ((aqt->state & 128) == 0) {
                        if (aqt->target.t_smo->isDead()) {
                            // not object did it as such he failed,
                            // but goal reached
                            // TODO: projectile shot?
                            aqt->state |= 12;
                        } else if ((aqt->target.t_smo->majorType() == MapObject::mjt_Ped
                            ? aqt->multi_var.enemy_var.forced_shot
                            || !checkFriendIs((PedInstance *)aqt->target.t_smo) : true)
                            && selectRequiredWeapon(&aqt->multi_var.enemy_var.pw_to_use))
                        {
                            // TODO: if object can't see target and none of
                            // friendly units can see it we should fail,
                            // use check from findenemy?
                            last_firing_target_ = aqt->target;
                            WeaponInstance *wi = selectedWeapon();
                            // trust, but check always
                            if (wi && wi->ammoRemaining() > 0 && wi->canShoot()
                                && wi->doesPhysicalDmg())
                            {
                                int tm_left = elapsed;
                                uint32 make_shots = aqt->multi_var.enemy_var.make_shots;
                                uint32 shots_done = make_shots
                                    - aqt->multi_var.enemy_var.shots_done;
                                uint16 answ = wi->inflictDamage(aqt->target.t_smo, NULL, &tm_left,
                                    aqt->multi_var.enemy_var.forced_shot, &shots_done);
                                // TODO: handle correctly, use info returned from
                                // inflictDamage, needs use of condition
                                if (answ == 0 || answ == 2) {
                                    aqt->state |= 2;
                                    if (shots_done != 0) {
                                        // enabling animation completion
                                        aqt->state |= 32;
                                        if (make_shots == 0 && aqt->target.t_smo->isDead())
                                            aqt->state |= 4;
                                        else {
                                            aqt->multi_var.enemy_var.shots_done += shots_done;
                                            if (make_shots != 0 && make_shots
                                                <= shots_done
                                                + aqt->multi_var.enemy_var.shots_done)
                                            {
                                                aqt->state |= 4;
                                            }
                                        }
                                    }
                                    wi = selectedWeapon();
                                    if (wi && wi->ammoRemaining() == 0) {
                                        // hiding weapon without ammo
                                        createActQDeselectCurWeapon(*it);
                                        it->main_act = it->actions.size() - 1;
                                        aqt = it->actions.begin() + indx;
                                    }
                                } else
                                    aqt->state |= 8;
                            } else
                                aqt->state |= 8;
                        } else
                            aqt->state |= 8;
                    } else if ((aqt->act_exec & PedInstance::ai_aWaitToStart) != 0)
                    {
                        //if (aqt->multi_var.time_var.desc == 0) {
                            if ((aqt->target.t_smo->majorType() == MapObject::mjt_Ped
                                && !checkHostileIs((PedInstance *)aqt->target.t_smo))
                                || aqt->target.t_smo->isDead())
                            {
                                aqt->act_exec &= PedInstance::ai_aAll
                                    ^ (PedInstance::ai_aWaitToStart
                                    | PedInstance::ai_aWait);
                                aqt->state ^= 384;
                                // failed target is friendly or dead
                                aqt->state |= 8;
                            }
                        //}
                    }

                    // hiding weapon for police officers for completed/failed state
                    if ((aqt->state & 12) != 0 && obj_group_def_ == og_dmPolice) {
                        createActQDeselectCurWeapon(*it);
                        it->main_act = it->actions.size() - 1;
                        aqt = it->actions.begin() + indx;
                    }
                }
                if ((aqt->act_exec & PedInstance::ai_aPutDownObject) != 0)
                {
                    if (aqt->state == 1) {
                        //TODO: better state checking
                        if ((state_ & (PedInstance::pa_smInCar
                            | PedInstance::pa_smUsingCar
                            | PedInstance::pa_smHit)) != 0)
                            aqt->state |= 8;
                        WeaponInstance *wi = (WeaponInstance *)aqt->target.t_smo;
                        if (wi->getOwner() != this)
                            aqt->state |= 8;
                        else {
                            dropWeapon(wi);
                            aqt->state |= 36;
                        }
                    }
                }
                if ((aqt->act_exec & PedInstance::ai_aReachLocation) != 0)
                {
                    //TODO: better state checking
                    if ((state_ & (PedInstance::pa_smInCar
                        | PedInstance::pa_smUsingCar)) != 0)
                    {
                        aqt->state |= 8;
                    }
                    if (aqt->state == 1) {
                        speed_ = aqt->multi_var.dist_var.speed != -1
                            ? aqt->multi_var.dist_var.speed: getSpeed();
                        if (aqt->condition == 0) {
                            bool set_new_dest = true;
                            dist_to_pos_ = aqt->multi_var.dist_var.dist;
                            if (dist_to_pos_ != 0) {
                                toDefineXYZ xyz;
                                int dist_is = -1;
                                aqt->target.t_pn.convertPosToXYZ(&xyz);
                                dist_is = (int)distanceToPosXYZ(&xyz);
                                if (dist_is <= dist_to_pos_)
                                    set_new_dest = false;
                            }
                            if (set_new_dest) {
                                aqt->state |= 2;
                                setDestinationP(mission, aqt->target.t_pn.tileX(),
                                    aqt->target.t_pn.tileY(), aqt->target.t_pn.tileZ(),
                                    aqt->target.t_pn.offX(), aqt->target.t_pn.offY());
                                if (dest_path_.empty()) {
                                    aqt->state |= 8;
                                    speed_ = 0;
                                }
                            } else {
                                aqt->state |= 4;
                                speed_ = 0;
                            }
                        } else if (aqt->condition == 1) {
                            int dist = aqt->multi_var.dist_var.dist;
                            moveToDir(mission, elapsed,
                                aqt->multi_var.dist_var.dir_move,
                                aqt->multi_var.dist_var.dir, -1, -1,
                                &dist, true);
                            if (aqt->multi_var.dist_var.dist != 0
                                && dist >= aqt->multi_var.dist_var.dist)
                            {
                                aqt->state |= 4;
                                speed_ = 0;
                            } else {
                                aqt->state |= 2;
                                aqt->multi_var.dist_var.dist_walked += dist;
                            }
                        } else if (aqt->condition == 2) {
                            bool set_new_dest = true;
                            dist_to_pos_ = aqt->multi_var.dist_var.dist;
                            if (dist_to_pos_ != 0) {
                                int dist_is = -1;
                                dist_is = (int)distanceTo(
                                    (MapObject *)aqt->target.t_smo);
                                if (dist_is <= dist_to_pos_)
                                    set_new_dest = false;
                            } else {
                                if (this->samePosition(aqt->target.t_smo))
                                    set_new_dest = false;
                            }
                            if (set_new_dest) {
                                aqt->state |= 2;
                                setDestinationP(mission,
                                    aqt->target.t_smo->tileX(), aqt->target.t_smo->tileY(),
                                    aqt->target.t_smo->tileZ(), aqt->target.t_smo->offX(),
                                    aqt->target.t_smo->offY());
                                if (dest_path_.empty()) {
                                    aqt->state |= 8;
                                    speed_ = 0;
                                }
                            } else {
                                aqt->state |= 4;
                                speed_ = 0;
                            }
                        } else if (aqt->condition == 3) {
                            // TODO: check already at location
                            aqt->target.t_pn.convertPosToXY(&aqt->target.t_xyz.x,
                                &aqt->target.t_xyz.y);
                            int diffx = aqt->target.t_xyz.x - tile_x_ * 256 - off_x_;
                            int diffy = aqt->target.t_xyz.y - tile_y_ * 256 - off_y_;

                            int dist = aqt->multi_var.dist_var.dist;
                            dist_to_pos_ = (int)sqrt((double)
                                (diffx * diffx + diffy * diffy));
                            moveToDir(mission, elapsed,
                                aqt->multi_var.dist_var.dir_move,
                                -1, aqt->target.t_xyz.x, aqt->target.t_xyz.y,
                                &dist);
                            aqt->state |= 2;
                        } else if (aqt->condition == 4) {
                            // TODO: check already at location
                            int pos_x, pos_y;
                            aqt->target.t_smo->convertPosToXY(&pos_x, &pos_y);
                            dist_to_pos_ = this->distanceTo(aqt->target.t_smo);
                            int dist = aqt->multi_var.dist_var.dist;
                            moveToDir(mission, elapsed,
                                aqt->multi_var.dist_var.dir_move,
                                -1, pos_x, pos_y, &dist);
                            aqt->state |= 2;
                        }
                    }
                    if ((aqt->state & 15) == 3) {
                        speed_ = aqt->multi_var.dist_var.speed != -1
                            ? aqt->multi_var.dist_var.speed: getSpeed();
                        if (aqt->condition == 0 || aqt->condition == 2) {
                            updated = movementP(mission, elapsed);
                            if (speed_ == 0)
                                aqt->state |= 4;
                        } else if (aqt->condition == 1) {
                            int dist = aqt->multi_var.dist_var.dist;
                            moveToDir(mission, elapsed,
                                aqt->multi_var.dist_var.dir_move,
                                aqt->multi_var.dist_var.dir, -1, -1,
                                &dist, true);
                            aqt->multi_var.dist_var.dir = dir_;
                            aqt->multi_var.dist_var.dist_walked += dist;
                            if (aqt->multi_var.dist_var.dist != 0
                                && aqt->multi_var.dist_var.dist_walked
                                >= aqt->multi_var.dist_var.dist)
                            {
                                aqt->state |= 4;
                                speed_ = 0;
                            }
                        } else if (aqt->condition == 3) {
                            int diffx = aqt->target.t_xyz.x - tile_x_ * 256 - off_x_;
                            int diffy = aqt->target.t_xyz.y - tile_y_ * 256 - off_y_;
                            dist_to_pos_ = (int)sqrt((double)
                                (diffx * diffx + diffy * diffy));
                            int dist = aqt->multi_var.dist_var.dist;
                            if (dist_to_pos_ > 0) {
                                moveToDir(mission, elapsed,
                                    aqt->multi_var.dist_var.dir_move,
                                    -1, aqt->target.t_xyz.x, aqt->target.t_xyz.y,
                                    &dist);
                                if (tile_x_ == aqt->target.t_pn.tileX()
                                    && tile_y_ == aqt->target.t_pn.tileY()
                                    // TODO: add correct z or ignore it?
                                    //&& tile_z_ == aqt->target.t_pn.tileZ()
                                    && off_x_ == aqt->target.t_pn.offX()
                                    && off_y_ == aqt->target.t_pn.offY())
                                {
                                    aqt->state |= 4;
                                    speed_ = 0;
                                }
                            } else {
                                aqt->state |= 4;
                                speed_ = 0;
                            }
                        } else if (aqt->condition == 4) {
                            int pos_x, pos_y;
                            aqt->target.t_smo->convertPosToXY(&pos_x, &pos_y);
                            int diffx = pos_x - tile_x_ * 256 - off_x_;
                            int diffy = pos_y - tile_y_ * 256 - off_y_;
                            int dist = aqt->multi_var.dist_var.dist;
                            dist_to_pos_ = (int)sqrt((double)
                                (diffx * diffx + diffy * diffy));
                            if (dist_to_pos_ > 0) {
                                moveToDir(mission, elapsed,
                                    aqt->multi_var.dist_var.dir_move,
                                    -1, pos_x, pos_y, &dist);
                                if (samePosition(aqt->target.t_smo)) {
                                    aqt->state |= 4;
                                    speed_ = 0;
                                }
                            } else {
                                aqt->state |= 4;
                                speed_ = 0;
                            }
                        }
                    }
                }
                if ((aqt->act_exec & PedInstance::ai_aFollowObject) != 0)
                {
                    if (aqt->target.t_smo == NULL) {
                        aqt->state |= 8;
                        speed_ = 0;
                    // TODO: review, reduce code(, in sight when no weapon?)
                    } else if (aqt->state == 1 || aqt->state == 17) {
                        speed_ = aqt->multi_var.dist_var.speed != -1
                            ? aqt->multi_var.dist_var.speed: getSpeed();
                        if (aqt->condition == 0) {
                            dist_to_pos_ = aqt->multi_var.dist_var.dist;
                            int dist_is = -1;
                            dist_is = (int)distanceTo(
                                (MapObject *)aqt->target.t_smo);
                            if (dist_is > dist_to_pos_ + aqt->multi_var.dist_var.rd
                                || dist_is <= dist_to_pos_ - aqt->multi_var.dist_var.rd)
                            {
                                aqt->state |= 2;
                                setDestinationP(mission,
                                    aqt->target.t_smo->tileX(), aqt->target.t_smo->tileY(),
                                    aqt->target.t_smo->tileZ(), aqt->target.t_smo->offX(),
                                    aqt->target.t_smo->offY());
                                if (dest_path_.empty()) {
                                    aqt->state |= 8;
                                    speed_ = 0;
                                }
                                if ((aqt->state & 16) != 0)
                                    aqt->state ^= 16;
                            } else {
                                aqt->state |= 16;
                                speed_ = 0;
                            }
                        } else if (aqt->condition == 1) {
                            WeaponInstance *wi = selectedWeapon();
                            if (wi) {
                                bool set_new_dest = true;
                                // TODO: set checktile value, based
                                // on mode
                                if (wi->inRangeNoCP(&aqt->target.t_smo, NULL,
                                    false, false) == 1)
                                {
                                    set_new_dest = false;
                                    // object will turn to target
                                    int posx, posy;
                                    aqt->target.t_smo->convertPosToXY(&posx, &posy);
                                    setDirection(posx - tile_x_ * 256 - off_x_,
                                        posy - tile_y_ * 256 - off_y_);
                                }
                                if (set_new_dest) {
                                    aqt->state |= 2;
                                    setDestinationP(mission,
                                        aqt->target.t_smo->tileX(), aqt->target.t_smo->tileY(),
                                        aqt->target.t_smo->tileZ(), aqt->target.t_smo->offX(),
                                        aqt->target.t_smo->offY());
                                    if (dest_path_.empty()) {
                                        aqt->state |= 8;
                                        speed_ = 0;
                                    }
                                    if ((aqt->state & 16) != 0)
                                        aqt->state ^= 16;
                                } else {
                                    aqt->state |= 16;
                                    speed_ = 0;
                                }
                            } else {
                                aqt->state |= 8;
                                speed_ = 0;
                            }
                        }
                    } else if ((aqt->state & 30) == 2) {
                        speed_ = aqt->multi_var.dist_var.speed != -1
                            ? aqt->multi_var.dist_var.speed: getSpeed();
                        if (aqt->condition == 0) {
                            updated = movementP(mission, elapsed);
                            if (speed_ == 0) {
                                aqt->state ^= 2;
                                aqt->state |= 16;
                            } else {
                                PathNode &rp = dest_path_.back();
                                if (!((ShootableMovableMapObject*)aqt->target.t_smo)
                                    ->checkCurrPos(rp))
                                {
                                    // resetting target position
                                    setDestinationP(mission,
                                        aqt->target.t_smo->tileX(), aqt->target.t_smo->tileY(),
                                        aqt->target.t_smo->tileZ(), aqt->target.t_smo->offX(),
                                        aqt->target.t_smo->offY());
                                    if (dest_path_.empty()) {
                                        aqt->state |= 8;
                                        speed_ = 0;
                                    }
                                }
                            }
                        } else if (aqt->condition == 1) {
                            // TODO: view range if no weapon?
                            WeaponInstance *wi = selectedWeapon();
                            if (wi) {
                                updated = movementP(mission, elapsed);
                                if (wi->inRangeNoCP(&aqt->target.t_smo) == 1) {
                                    aqt->state ^= 2;
                                    aqt->state |= 16;
                                    dest_path_.clear();
                                    speed_ = 0;
                                    // object will turn to target
                                    int posx, posy;
                                    aqt->target.t_smo->convertPosToXY(&posx, &posy);
                                    setDirection(posx - tile_x_ * 256 - off_x_,
                                        posy - tile_y_ * 256 - off_y_);
                                } else {
                                    PathNode &rp = dest_path_.back();
                                    if (!((ShootableMovableMapObject*)aqt->target.t_smo)
                                        ->checkCurrPos(rp))
                                    {
                                        // resetting target position
                                        setDestinationP(mission,
                                            aqt->target.t_smo->tileX(), aqt->target.t_smo->tileY(),
                                            aqt->target.t_smo->tileZ(), aqt->target.t_smo->offX(),
                                            aqt->target.t_smo->offY());
                                        if (dest_path_.empty()) {
                                            aqt->state |= 8;
                                            speed_ = 0;
                                        }
                                    }
                                }
                            } else {
                                aqt->state |= 8;
                                speed_ = 0;
                            }
                        }
                    }
                }
                if ((aqt->act_exec & PedInstance::ai_aAttackLocation) != 0
                    && (aqt->state & 128) == 0)
                {
                    // TODO: additional conditions
                    if (selectRequiredWeapon(&aqt->multi_var.enemy_var.pw_to_use))
                    {
                        WeaponInstance *wi = selectedWeapon();
                        if (wi && wi->ammoRemaining() > 0 && wi->canShoot()
                            && wi->doesPhysicalDmg())
                        {
                            last_firing_target_ = aqt->target;
                            int tm_left = elapsed;
                            uint32 make_shots = aqt->multi_var.enemy_var.make_shots;
                            uint32 shots_done = make_shots
                                - aqt->multi_var.enemy_var.shots_done;
                            uint16 answ = wi->inflictDamage(NULL, &aqt->target.t_pn, &tm_left,
                                aqt->multi_var.enemy_var.forced_shot, &shots_done);
                            if (answ == 0 || answ == 2) {
                                aqt->state |= 2;
                                if (shots_done != 0) {
                                    // enable animation compeletion
                                    aqt->state |= 32;
                                    aqt->multi_var.enemy_var.shots_done += shots_done;
                                    if (make_shots != 0 && make_shots
                                        <= shots_done
                                        + aqt->multi_var.enemy_var.shots_done)
                                    {
                                        aqt->state |= 4;
                                    }
                                }
                            } else
                                aqt->state |= 8;
                        } else
                            aqt->state |= 8;
                    } else
                        aqt->state |= 8;
                }
                if ((aqt->act_exec & PedInstance::ai_aFindEnemy) != 0
                    && (aqt->state & 128) == 0)
                {
                    bool selfState = is_ignored_;
                    is_ignored_ = true;
                    bool vehicleState;
                    if ((state_ & (PedInstance::pa_smInCar
                        | PedInstance::pa_smUsingCar)) != 0)
                    {
                        vehicleState = in_vehicle_->isIgnored();
                        in_vehicle_->setIsIgnored(true);
                    }
                    if (!hostiles_found_.empty())
                        verifyHostilesFound(mission);

                    Msmod_t smo_dist;
                    // TODO: reduce inrange calls, later
                    if (hostiles_found_.empty()) {
                        WeaponInstance *wi = selectedWeapon();
                        // TODO: check for weapons, set the largest shooting
                        // range? not only selected, all?
                        int shot_rng = 0;
                        if (wi && wi->doesPhysicalDmg() && wi->canShoot())
                            shot_rng = wi->range();
                        int view_rng = sight_range_;
                        toDefineXYZ cur_xyz;
                        convertPosToXYZ(&cur_xyz);
                        cur_xyz.z += (size_z_ >> 1);
                        // agents, police, guards will exchange info of hostiles
                        // permitting to target non in sight range
                        // (NOTE: if target is set here should it remain locked?
                        // or rechecked whether it is seen by one of same id+def
                        // in ai_aDestroyObject)
                        if ((obj_group_def_ & (og_dmAgent
                            | og_dmPolice | og_dmGuard)) != 0)
                        {
                            int num_peds = mission->numPeds();
                            for (int i = 0; i < num_peds; ++i) {
                                PedInstance *p = mission->ped(i);
                                if ((p->state_ & pa_smCheckExcluded) != 0
                                    || p == this || hostiles_found_.find(p)
                                    != hostiles_found_.end()
                                    || smo_dist.find(p)
                                    != smo_dist.end())
                                {
                                    continue;
                                }
                                if (p->objGroupDef() == obj_group_def_
                                    && checkFriendIs(p))
                                {
                                    Msmod_t::iterator it_s, it_e;
                                    if (p->getHostilesFoundIt(it_s, it_e)) {
                                        do {
                                            double distTo = 0;
                                            ShootableMapObject *smo = it_s->first;
                                            // needs this checking as list might
                                            // become invalidated by other
                                            // peds actions
                                            if ((!smo->isExcluded())
                                                && checkHostileIs(smo))
                                            {
                                                // TODO: set ignoreblocker based on Ai
                                                if (mission->inRangeCPos(
                                                    &cur_xyz,
                                                    &smo, NULL, false, false,
                                                    shot_rng > 0 ? shot_rng
                                                    : view_rng, &distTo) == 1)
                                                {
                                                    hostiles_found_.insert(
                                                        Pairsmod_t(smo, distTo));
                                                }
                                            }
                                            ++it_s;
                                        } while (it_s != it_e);
                                    }
                                } else if (checkHostileIs(p) ) {
                                    // TODO: hostile_desc_alt to checkHostileIs?
                                    double distTo = 0;
                                    // TODO: set ignoreblocker based on Ai
                                    if (mission->inRangeCPos(&cur_xyz,
                                        (ShootableMapObject **)(&p),
                                        NULL, false, false,
                                        view_rng, &distTo) == 1)
                                    {
                                        hostiles_found_.insert(Pairsmod_t(
                                            (ShootableMapObject *)p, distTo));
                                    }
                                }
                            }
                        } else {
                            int num_peds = mission->numPeds();
                            for (int i = 0; i < num_peds; ++i) {
                                PedInstance *p = mission->ped(i);
                                if ((p->state_ & pa_smCheckExcluded) != 0
                                    || p == this || hostiles_found_.find(p)
                                    != hostiles_found_.end())
                                {
                                    continue;
                                }
                                if (checkHostileIs(p) ) {
                                    // TODO: hostile_desc_alt to checkHostileIs?
                                    double distTo = 0;
                                    // TODO: set ignoreblocker based on Ai
                                    if (mission->inRangeCPos(&cur_xyz,
                                        (ShootableMapObject **)(&p),
                                        NULL, false, false,
                                        view_rng, &distTo) == 1)
                                    {
                                        hostiles_found_.insert(Pairsmod_t(
                                            (ShootableMapObject *)p, distTo));
                                    }
                                }
                            }
                        }
                        int num_vehicles = mission->numVehicles();
                        for (int i = 0; i < num_vehicles; ++i) {
                            VehicleInstance *v = mission->vehicle(i);
                            if ((!v->isExcluded()) && v->checkHostilesInside(this, 0))
                            {
                                double distTo = 0;
                                // TODO: set ignoreblocker based on Ai
                                if (mission->inRangeCPos(&cur_xyz,
                                    (ShootableMapObject **)(&v), NULL, false,
                                    false, view_rng, &distTo) == 1)
                                {
                                    hostiles_found_.insert(Pairsmod_t(
                                        (ShootableMapObject *)v, distTo));
                                }
                            }
                        }
                    }

                    is_ignored_ = selfState;
                    if (((state_ & (PedInstance::pa_smInCar
                        | PedInstance::pa_smUsingCar)) != 0))
                    {
                        in_vehicle_->setIsIgnored(vehicleState);
                    }

                    // NOTE: possible check for most dangerous weapon
                    // or object, if is one of objectives to destroy, for
                    // now only distance check
                    bool set_nxt_act = false;
                    if (hostiles_found_.empty()) {
                        if (smo_dist.empty())
                            aqt->state |= 8;
                        else {
                            Msmod_t::iterator it_msmod = smo_dist.begin();
                            Pairsmod_t closest = *it_msmod;
                            ++it_msmod;
                            while (it_msmod != smo_dist.end()) {
                                if (it_msmod->second < closest.second) {
                                    closest = *it_msmod;
                                }
                                ++it_msmod;
                            }
                            aqt->target.t_smo = closest.first;
                            aqt->state |= 4;
                            set_nxt_act = true;
                        }
                    } else {
                        Msmod_t::iterator it_msmod = hostiles_found_.begin();
                        Pairsmod_t closest = *it_msmod;
                        ++it_msmod;
                        while (it_msmod != hostiles_found_.end()) {
                            if (it_msmod->second < closest.second) {
                                closest = *it_msmod;
                            }
                            ++it_msmod;
                        }
                        aqt->target.t_smo = closest.first;
                        aqt->state |= 4;
                        set_nxt_act = true;
                    }

                    // agents with inteligence and perception boosted
                    // will be shooting, or with mod brain lvl 3
                    if (set_nxt_act && obj_group_def_ == PedInstance::og_dmAgent
                        && isOurAgent())
                    {
                        Mod *pMod = slots_[Mod::MOD_BRAIN];
                        set_nxt_act = (perception_->getMultiplier() > 1.0
                            && intelligence_->getMultiplier() > 1.0)
                            || (pMod && pMod->getVersion() == Mod::MOD_V3)
                            ? true: false;
                    }

                    if (set_nxt_act) {
                        std::vector <actionQueueType>::iterator searched =
                            findActInQueue(PedInstance::ai_aDestroyObject,
                                *it, aqt);

                        if ((!weapons_.empty()) && searched != it->actions.end())
                        {
                            actionQueueType & aqt_attack = *searched;
                            aqt_attack.target.t_smo = aqt->target.t_smo;
                            // action is ready, removing not ready flag
                            // enabling destroyobject action
                            aqt_attack.state ^= 64;
                            it->main_act = std::distance(it->actions.begin(), searched);
                            if (obj_group_def_ == og_dmPolice
                                // only non controlled will follow and wait
                                && (desc_state_ & pd_smControlled) == 0)
                            {
                                // forcing showing a gun
                                selectRequiredWeapon();
                                g_App.gameSounds().play(snd::PUTDOWN_WEAPON);
                                aqt_attack.act_exec |= PedInstance::ai_aWaitToStart;
                                aqt_attack.multi_var.time_var.time_to_start = 5000 - tm_before_check_;
                                aqt_attack.multi_var.time_var.desc = 1;
                                // enabling following behavior
                                searched = findActInQueue(PedInstance::ai_aFollowObject,
                                    *it, searched);
                                if (searched != it->actions.end()) {
                                    actionQueueType & aqt_follow = *searched;
                                    aqt_follow.target.t_smo = aqt->target.t_smo;
                                    aqt_follow.state ^= 64;
                                }
                            }
                        }
                        Mod *pMod = slots_[Mod::MOD_EYES];
                        int32 tm_wait = tm_before_check_;
                        if (obj_group_def_ == PedInstance::og_dmAgent) {
                            if (pMod)
                                tm_wait -= 25 * (pMod->getVersion() + 2);
                            tm_wait = (double)tm_wait * (1.0 / intelligence_->getMultiplier());
                        } else if (obj_group_def_ == PedInstance::og_dmCivilian
                            && (rcv_damage_def_ & MapObject::dmg_Panic) != 0)
                        {
                            searched = findActInQueue(PedInstance::ai_aReachLocation,
                                *it, aqt);
                            if (searched != it->actions.end()) {
                                // enabling panic
                                actionQueueType & aqt_panic = *searched;
                                aqt_panic.state ^= 64;
                                // setting opposite direction for movement
                                int tx, ty, dirmove;
                                aqt->target.t_smo->convertPosToXY(&tx, &ty);
                                setDirection(tx - (tile_x_ * 256 + off_x_),
                                    ty - (tile_y_ * 256 + off_y_), &dirmove);
                                aqt_panic.multi_var.dist_var.dir = (dirmove + 128) % 256;
                                it->main_act = std::distance(it->actions.begin(), searched);
                            }
                        }
                        aqt->act_exec |= PedInstance::ai_aWait;
                        aqt->multi_var.time_var.time_wait = tm_wait;
                        aqt->multi_var.time_var.desc = 2;
                        aqt->multi_var.time_var.elapsed = 0;
                    }
                }
                if ((aqt->act_exec & PedInstance::ai_aFindNonFriend) != 0
                    && (aqt->state & 128) == 0)
                {
                    WeaponInstance *wi = selectedWeapon();
                    if (wi && wi->getWeaponType() == Weapon::Persuadatron) {
                        bool selfState = is_ignored_;
                        is_ignored_ = true;
                        // NOTE : can be done as in ai_aFindEnemy with objects
                        // passing within same group + def
                        Msmod_t nf_dist;
                        toDefineXYZ cur_xyz;
                        convertPosToXYZ(&cur_xyz);
                        cur_xyz.z += (size_z_ >> 1);
                        int num_peds = mission->numPeds();

                        // NOTE: this range should be always the same valuse as
                        // persuadatron max range
                        int view_rng = wi->range();
                        for (int i = 0; i < num_peds; ++i) {
                            PedInstance *p = mission->ped(i);
                            if ((p->state_ & pa_smCheckExcluded) != 0
                                || p->isIgnored() || p == this)
                            {
                                continue;
                            }
                            if (!checkFriendIs(p) ) {
                                double distTo = 0;
                                if (mission->inRangeCPos(&cur_xyz,
                                    (ShootableMapObject **)(&p), NULL, false, false,
                                    view_rng, &distTo) == 1)
                                {
                                    nf_dist.insert(Pairsmod_t(
                                        (ShootableMapObject *)p, distTo));
                                }
                            }
                        }
                        is_ignored_ = selfState;
                        if (nf_dist.empty())
                            aqt->state |= 8;
                        else {
                            Msmod_t::iterator it_msmod = nf_dist.begin();
                            Pairsmod_t closest = *it_msmod;
                            ++it_msmod;
                            while (it_msmod != nf_dist.end()) {
                                if (it_msmod->second < closest.second)
                                    closest = *it_msmod;
                                ++it_msmod;
                            }
                            std::vector <actionQueueType>::iterator searched =
                                findActInQueue(PedInstance::ai_aAquireControl,
                                *it, aqt);

                            if (searched != it->actions.end()) {
                                actionQueueType & aqt_aq_cont = *searched;
                                aqt_aq_cont.target.t_smo = closest.first;
                                aqt_aq_cont.state ^= 64;
                                it->main_act = std::distance(
                                    it->actions.begin(), searched);
                            }
                            aqt->state |= 4;
                        }
                    } else
                        aqt->state |= 8;
                }
                if ((aqt->act_exec & PedInstance::ai_aFindWeapon) != 0
                        && (aqt->state & 128) == 0)
                {
                    if (weapons_.size() < 8) {
                        Msmod_t wpns_dist;
                        int view_rng = aqt->multi_var.dist_var.dist != -1
                            ? aqt->multi_var.dist_var.dist : sight_range_;
                        toDefineXYZ cur_xyz;
                        convertPosToXYZ(&cur_xyz);
                        cur_xyz.z += (size_z_ >> 1);
                        int numweapons = mission->numWeapons();
                        for (int32 i = 0; i < numweapons; ++i) {
                            WeaponInstance *w = mission->weapon(i);
                            if (w->isIgnored())
                                continue;
                            if (w->canShoot() && w->usesAmmo()
                                && w->ammoRemaining() > 0)
                            {
                                double distTo = 0;
                                if (mission->inRangeCPos(&cur_xyz,
                                    (ShootableMapObject **)(&w), NULL, false, true,
                                    view_rng, &distTo) == 1)
                                {
                                    wpns_dist.insert(Pairsmod_t(
                                        (ShootableMapObject *)w, distTo));
                                }
                            }
                        }
                        if (wpns_dist.empty())
                            aqt->state |= 8;
                        else {
                            Msmod_t::iterator it_msmod = wpns_dist.begin();
                            Pairsmod_t closest = *it_msmod;
                            ++it_msmod;
                            while (it_msmod != wpns_dist.end()) {
                                if (it_msmod->second < closest.second)
                                    closest = *it_msmod;
                                ++it_msmod;
                            }
                            std::vector <actionQueueType>::iterator searched =
                                findActInQueue(PedInstance::ai_aReachLocation,
                                *it, aqt);

                            if (searched != it->actions.end()) {
                                actionQueueType & aqt_gotoweap = *searched;
                                aqt_gotoweap.target.t_smo = closest.first;
                                searched = findActInQueue(PedInstance::ai_aPickUpObject,
                                    *it, searched);
                                if (searched != it->actions.end()) {
                                    actionQueueType & aqt_pick = *searched;
                                    aqt_pick.target.t_smo = closest.first;
                                    aqt_pick.state ^= 64;
                                    // enabling here because if for some
                                    // reasons pickup will not be availiable
                                    // we don't need to go to weapon
                                    aqt_gotoweap.state ^= 64;
                                    it->main_act = std::distance(
                                        it->actions.begin(), searched);
                                }
                            }
                            aqt->state |= 4;
                        }
                    } else
                        aqt->state |= 8;
                }
                if ((aqt->act_exec & PedInstance::ai_aCheckOwner) != 0
                    && (aqt->state & 128) == 0)
                {
                    if (owner_) {
                        bool disable_walking = false;
                        if (owner_->isDead()){
                            // agent is dead choose another agent from alive
                            owner_ = NULL;
                            Squad *sq = mission->getSquad();
                            for (uint8 i = 0; i < AgentManager::kMaxSlot; ++i) {
                                PedInstance *p = sq->member(i);
                                if (p->isAlive()) {
                                    owner_ = p;
                                    break;
                                }
                            }
                            std::vector <actionQueueGroupType>::iterator it_walk =
                                findQueueInActQueue(it->group_id + 1);
                            if (it_walk != actions_queue_.end()) {
                                std::vector <actionQueueType>::iterator searched =
                                    findActInQueue(PedInstance::ai_aFollowObject,
                                    *it_walk, it_walk->actions.begin());
                                if (searched != it_walk->actions.end()) {
                                    actionQueueType & aqt_follow = *searched;
                                    aqt_follow.target.t_smo = owner_;
                                }
                            }
                        }
                        if (owner_ && owner_->isAlive()) {
                            if (((PedInstance *)owner_)->isArmed()) {
                                if (weapons_.empty()) {
                                    // enable find and pickup weapon
                                    std::vector <actionQueueType>::iterator searched =
                                        findActInQueue(PedInstance::ai_aFindWeapon,
                                        *it, aqt);
                                    if (searched != it->actions.end()) {
                                        actionQueueType & aqt_findweap = *searched;
                                        aqt_findweap.state ^= 64;
                                        it->main_act = std::distance(
                                            it->actions.begin(), searched);
                                    }
                                } else {
                                    std::vector <actionQueueType>::iterator searched =
                                        findActInQueue(PedInstance::ai_aDestroyObject,
                                        *it, aqt);
                                    if (searched != it->actions.end()) {
                                        actionQueueType & aqt_attack = *searched;
                                        targetDescType *t_fire =
                                            ((PedInstance *)owner_)->lastFiringTarget();
                                        if (t_fire->desc == 1) {
                                            // changing action to execute
                                            aqt_attack.act_exec = PedInstance::ai_aAttackLocation;
                                            aqt_attack.target = *t_fire;
                                            aqt_attack.state ^= 64;
                                        } else if (t_fire->desc == 2) {
                                            // already ai_aDestroyObject, but to be sure set again
                                            aqt_attack.act_exec = PedInstance::ai_aDestroyObject;
                                            aqt_attack.target = *t_fire;
                                            aqt_attack.state ^= 64;
                                        }
                                        it->main_act = std::distance(
                                            it->actions.begin(), searched);
                                    } else
                                        aqt->state |= 8;
                                }
                            } else {
                                createActQDeselectCurWeapon(*it);
                                it->main_act = it->actions.size() - 1;
                                aqt = it->actions.begin() + indx;
                            }
                            aqt->state |= 4;
                        } else {
                            createActQDeselectCurWeapon(*it);
                            it->main_act = it->actions.size() - 1;
                            aqt = it->actions.begin() + indx;
                            aqt->state |= 8;
                            disable_walking = true;
                        }
                        if (disable_walking) {
                            std::vector <actionQueueGroupType>::iterator it_walk =
                                findQueueInActQueue(it->group_id + 1);
                            if (it_walk != actions_queue_.end()) {
                                it_walk->state |= 64;
                            }
                        } else {
                            std::vector <actionQueueGroupType>::iterator it_walk =
                                findQueueInActQueue(it->group_id + 1);
                            if (it_walk != actions_queue_.end()) {
                                it_walk->state &= (0xFFFF ^ 64);
                                std::vector <actionQueueType>::iterator searched =
                                    findActInQueue(PedInstance::ai_aFollowObject,
                                    *it_walk, it_walk->actions.begin());
                                if (searched != it_walk->actions.end()) {
                                    searched = findActInQueue(
                                        PedInstance::ai_aReachLocation, *it_walk, searched);
                                    if (searched != it_walk->actions.end()) {
                                        actionQueueType & aqt_rnd = *searched;
                                        // resetting random walking
                                        if ((aqt_rnd.state & 4) != 0) {
                                            aqt_rnd.state ^= 4;
                                            aqt_rnd.multi_var.dist_var.dist_walked = 0;
                                            aqt_rnd.multi_var.dist_var.dir = rand() % 256;
                                        }
                                    }
                                }
                            }
                        }
                    } else
                        aqt->state |= 8;
                }
                if ((aqt->act_exec & (PedInstance::ai_aWait
                    | PedInstance::ai_aWaitToStart)) == PedInstance::ai_aWait)
                {
                    if (aqt->multi_var.time_var.desc == 0) {
                        if ((aqt->state & 32) != 0) {
                            aqt->state |= 130;
                            aqt->multi_var.time_var.elapsed += elapsed;
                            if (aqt->multi_var.time_var.elapsed
                                >= aqt->multi_var.time_var.time_wait)
                            {
                                aqt->state &= (65535 ^ 128);
                                aqt->state |= 4;
                            }
                        } else
                            aqt->state &= (65535 ^ 32);
                    } else if (aqt->multi_var.time_var.desc == 1) {
                        if ((aqt->state & 32) != 0 && (aqt->state & 12) != 0) {
                            // firing animation if set, has usable frame_
                            // to process it we set now "waiting" flag
                            if (drawn_anim_ == PedInstance::ad_WalkFireAnim
                                || drawn_anim_ == PedInstance::ad_StandFireAnim)
                            {
                                aqt->state |= 128;
                            }
                            if ((aqt->state & 128) != 0) {
                                int frame_bckp = frame_;
                                int elapsed_carry_bckp = elapsed_carry_;
                                if (is_frame_drawn_)
                                    MapObject::animate(elapsed);
                                // after one time animation drawn this
                                // action is completed
                                if (handleDrawnAnim(elapsed))
                                    aqt->state &= (65535 ^ 160);
                                frame_ = frame_bckp;
                                elapsed_carry_ = elapsed_carry_bckp;
                            } else
                                aqt->state |= 128;
                        } else if ((aqt->state & 2) == 0)
                            aqt->state &= (65535 ^ 32);
                    } else if (aqt->multi_var.time_var.desc == 2) {
                        // no failed or suspended will have "wait" action
                        if ((aqt->state & 60) == 36) {
                            if ((aqt->state & 128) != 0) {
                                aqt->multi_var.time_var.elapsed += elapsed;
                                if (aqt->multi_var.time_var.elapsed
                                    >= aqt->multi_var.time_var.time_wait)
                                {
                                    aqt->state &= (65535 ^ 160);
                                }
                            } else
                                aqt->state |= 130;
                        } else
                            aqt->state &= (65535 ^ 32);
                    }
                }
                if ((aqt->act_exec & PedInstance::ai_aTimeExecute) != 0)
                {
                    aqt->multi_var.time_var.exec_elapsed += elapsed;
                    if (aqt->multi_var.time_var.exec_elapsed
                        >= aqt->multi_var.time_var.exec_time)
                    {
                        aqt->multi_var.time_var.exec_elapsed = 0;
                        if ((aqt->state & 12) == 0) {
                            aqt->state &= 3;
                            aqt->state |= 8;
                        }
                    }
                }
                if ((aqt->act_exec & PedInstance::ai_aResetActionQueueQueue) != 0)
                {
                    for (uint32 indx_act = 0, sz = it->actions.size() - 1;
                         indx_act < sz; ++indx_act)
                    {
                        actionQueueType &aqt_reset = it->actions[indx_act];
                        if ((aqt_reset.act_exec & PedInstance::ai_aTrigger) != 0)
                            continue;
                        aqt_reset.state &= ((65535 ^ 14));
                        aqt_reset.multi_var.enemy_var.shots_done = 0;
                        aqt_reset.multi_var.time_var.elapsed = 0;
                        aqt_reset.multi_var.time_var.exec_elapsed = 0;
                        aqt_reset.multi_var.dist_var.dist_walked = 0;
                        if ((aqt_reset.act_exec & PedInstance::ai_aWait) != 0
                            && aqt_reset.multi_var.time_var.desc == 1)
                        {
                            aqt_reset.state |= 32;
                        }
                    }
                    aqt->state |= 2;
                }
                if ((aqt->act_exec & PedInstance::ai_aDeselectCurWeapon) != 0)
                {
                    setSelectedWeapon(-1);
                    aqt->state |= 4;
                }
                if ((aqt->act_exec & PedInstance::ai_aNonFinishable) != 0)
                {
                    if ((aqt->state & 12) != 0)
                        aqt->state &= ((65535 ^ 14));
                }
                if (actions_property_ != 0)
                    break;

                if ((aqt->state & 256) != 0) {
                    acts_g_prcssd |= aqt->group_desc;
                } else if ((aqt->state & 32) != 0) {
                    switchActionStateTo(aqt->as);
                    acts_g_prcssd |= aqt->group_desc;
                } else if ((aqt->state & 16) != 0) {
                    switchActionStateFrom(aqt->as);
                } else if ((aqt->state & 8) != 0) {
                    if ((aqt->state & 2) != 0)
                        switchActionStateFrom(aqt->as);
                    acts_g_prcssd |= aqt->group_desc;
                } else if ((aqt->state & 4) != 0) {
                    switchActionStateFrom(aqt->as);
                    acts_g_prcssd |= aqt->group_desc;
                } else if ((aqt->state & 2) != 0) {
                    if (!((aqt->act_exec & (PedInstance::ai_aWait
                        | PedInstance::ai_aWaitToStart)) == PedInstance::ai_aWait
                        && aqt->multi_var.time_var.desc == 1))
                    {
                        switchActionStateTo(aqt->as);
                    }
                    acts_g_prcssd |= aqt->group_desc;
                }
#ifdef _DEBUG
                else if ((aqt->state & 1) != 0) {
                    printf("should not get here, aqt->state = 1\n");
                }
#endif

                if ((aqt->group_desc & PedInstance::gd_mExclusive) != 0
                    && ((aqt->state & 12) == 0 || (aqt->state & 128) != 0))
                {
                    break;
                }
                if (acts_g_prcssd == PedInstance::gd_mAll)
                    break;
            }
            it->state = it->actions[it->main_act].state;

            if ((it->group_desc & PedInstance::gd_mExclusive) != 0
                && ((it->state & 12) == 0 || (it->state & 128) != 0))
            {
                break;
            }
            // TODO : remove exclusive state,
            // it should influence group or all groups?
            // add exclusive state from group?
            groups_processed |= acts_g_prcssd;

            if (groups_processed == PedInstance::gd_mAll
                || (groups_processed & PedInstance::gd_mExclusive))
            {
                break;
            }
        }

        if ((groups_processed & PedInstance::gd_mExclusive) == 0
            && (state_ & pa_smDead) == 0)
        {
            // checking for action groups availiable in queue
            // will add from default_actions_ if not availiable
            uint32 groups_used = 0;
            for (std::vector <actionQueueGroupType>::iterator it =
                 actions_queue_.begin(); it != actions_queue_.end(); ++it)
            {
                if ((it->state & 128) == 0 && (it->state & 76) != 0)
                    continue;

                groups_used |= it->group_desc;
            }

            addDefActsToActions(groups_used);
        }
    }

    if (!friends_not_seen_.empty()) {
        toDefineXYZ cur_xyz;
        convertPosToXYZ(&cur_xyz);
        cur_xyz.z += (size_z_ >> 1);
        bool selfState = is_ignored_;
        is_ignored_ = true;
        for (std::set<ShootableMapObject *>::iterator it
            = friends_not_seen_.begin(); it != friends_not_seen_.end(); ++it)
        {
            ShootableMapObject *smo = *it;
            if (mission->inRangeCPos(&cur_xyz, (ShootableMapObject **)(&smo),
                NULL, false, false, sight_range_) == 1)
                friends_found_.insert(smo);
        }
        is_ignored_ = selfState;
    }
    if (is_frame_drawn_) {
        updated = MapObject::animate(elapsed);
    } else
        is_frame_drawn_ = true;
    if (handleDrawnAnim(elapsed))
        setActionStateToDrawnAnim();
    return updated;
}

PedInstance *Ped::createInstance(int map) {
    return new PedInstance(this, map);
}

/*!
 * Forces an agent to commit suicide.
 * If he's equiped with the good version of Mod Chest, he will
 * explodes causing damage on nearby Peds but all his weapons will
 * be destroyed.
 * Else he dies alone leaving his wepons on the ground.
 */
void PedInstance::commit_suicide() {
    if (health_ > 0) {
        health_ = -1;
        actions_property_ = 1;
        switchActionStateTo(PedInstance::pa_smDead);
        is_ignored_ = true;
        setDrawnAnim(PedInstance::ad_DieAnim);
        if (hasMinimumVersionOfMod(Mod::MOD_CHEST, Mod::MOD_V2)) {
            destroyAllWeapons();
            ShotClass explosion;
            explosion.createExplosion(this, 512.0);
            setDrawnAnim(PedInstance::ad_StandBurnAnim);
            setTimeShowAnim(4000);
        } else {
            if (!weapons_.empty())
                dropAllWeapons();
        }
    }
}

bool isOnScreen(int scrollX, int scrollY, int x, int y) {
    return x >= scrollX && y >= scrollY
            && x < scrollX + GAME_SCREEN_WIDTH - 129
            && y < scrollY + GAME_SCREEN_HEIGHT;
}

bool getOnScreen(int scrollX, int scrollY, int &x, int &y, int tx, int ty) {
    bool off = false;

    // get x, y on screen
    while (!isOnScreen(scrollX, scrollY, x, y)) {
        if (abs(tx - x) != 0)
            x += (tx - x) / abs(tx - x);

        if (abs(ty - y) != 0)
            y += (ty - y) / abs(ty - y);

        off = true;
    }

    return off;
}

void PedInstance::showPath(int scrollX, int scrollY) {
    int px = screenX();
    int py = screenY() - tile_z_ * TILE_HEIGHT/3 + TILE_HEIGHT/3;

    for (std::list<PathNode>::iterator it = dest_path_.begin();
            it != dest_path_.end(); ++it) {
        PathNode & d = *it;
        MapScreenPoint msPt = g_App.maps().map(map())->tileToScreenPoint(d.tileX(), d.tileY(),
                                       d.tileZ(), d.offX(), d.offY());
        int x = msPt.x;
        int y = msPt.y - d.tileZ() * TILE_HEIGHT/3 + TILE_HEIGHT/3;

        int ox = x;
        int oy = y;
        if (isOnScreen(scrollX, scrollY, x, y))
            getOnScreen(scrollX, scrollY, px, py, x, y);
        else if (isOnScreen(scrollX, scrollY, px, py))
            getOnScreen(scrollX, scrollY, x, y, px, py);
        else {
            px = x;
            py = y;
            continue;
        }

        int cl = 11;
        g_Screen.drawLine(px - scrollX + 129, py - scrollY,
                x - scrollX + 129, y - scrollY, cl);
        g_Screen.drawLine(px - scrollX + 129 - 1, py - scrollY,
                x - scrollX + 129 - 1, y - scrollY, cl);
        g_Screen.drawLine(px - scrollX + 129, py - scrollY - 1,
                x - scrollX + 129, y - scrollY - 1, cl);
        g_Screen.drawLine(px - scrollX + 129 - 1, py - scrollY - 1,
                x - scrollX + 129 - 1, y - scrollY - 1, cl);

        px = ox;
        py = oy;
    }
}

PedInstance::PedInstance(Ped *ped, int m) : ShootableMovableMapObject(m),
    ped_(ped), action_grp_id_(1),
    desc_state_(PedInstance::pd_smUndefined),
    hostile_desc_(PedInstance::pd_smUndefined),
    obj_group_def_(PedInstance::og_dmUndefined),
    old_obj_group_def_(PedInstance::og_dmUndefined),
    obj_group_id_(0), old_obj_group_id_(0),
    drawn_anim_(PedInstance::ad_StandAnim),
    sight_range_(0), selected_weapon_(-1), in_vehicle_(NULL),
    owner_(NULL)
{
    hold_on_.wayFree = 0;
    rcv_damage_def_ = MapObject::ddmg_Ped;
    major_type_ = MapObject::mjt_Ped;
    state_ = PedInstance::pa_smNone;
    actions_property_ = 0;
    is_our_ = false;
    
    adrenaline_  = new IPAStim(IPAStim::Adrenaline);
    perception_  = new IPAStim(IPAStim::Perception);
    intelligence_ = new IPAStim(IPAStim::Intelligence);

    tm_before_check_ = 1000;
    base_mod_acc_ = 0.1;
    last_firing_target_.desc = 0;
}

PedInstance::~PedInstance()
{
    delete ped_;
    ped_ = NULL;
    
    delete adrenaline_;
    adrenaline_ = NULL;
    delete perception_;
    perception_ = NULL;
    delete intelligence_;
    intelligence_ = NULL;
}

/*!
 * Initialize the ped instance as one of our agent.
 * \param p_agent The agent reference
 * \param obj_group_id Id of the agent's group.
 */
void PedInstance::initAsAgent(Agent *p_agent, unsigned int obj_group_id) {
    // not in all missions our agents health is 16, this fixes it
    setHealth(16);
    setStartHealth(16);
    while (p_agent->numWeapons()) {
        WeaponInstance *wi = p_agent->removeWeapon(0);
        addWeapon(wi);
        wi->setOwner(this);
        wi->setIsIgnored(true);
    }
    set_is_our(true);
    *((ModOwner *)this) = *((ModOwner *)p_agent);

    setObjGroupID(obj_group_id);
    setObjGroupDef(PedInstance::og_dmAgent);
    addEnemyGroupDef(2);
    addEnemyGroupDef(3);
    setHostileDesc(PedInstance::pd_smArmed);
    setSightRange(7 * 256);
    setBaseSpeed(256);
    setTimeBeforeCheck(400);
    setBaseModAcc(0.5);
}

void PedInstance::draw(int x, int y) {

    // ensure on map
    if (x < 110 || y < 0 || map_ == -1)
        return;

    Weapon::WeaponAnimIndex weapon_idx =
        selectedWeapon() ? selectedWeapon()->index() : Weapon::Unarmed_Anim;
    addOffs(x, y);


    switch(drawnAnim()){
        case PedInstance::ad_HitAnim:
            ped_->drawHitFrame(x, y, getDirection(), frame_);
            break;
        case PedInstance::ad_DieAnim:
            ped_->drawDieFrame(x, y, frame_);
            break;
        case PedInstance::ad_DeadAnim:
            ped_->drawDeadFrame(x, y, frame_);
            break;
        case PedInstance::ad_DeadAgentAnim:
            ped_->drawDeadAgentFrame(x, y, frame_);
            break;
        case PedInstance::ad_PickupAnim:
            ped_->drawPickupFrame(x, y, frame_);
            break;
        case PedInstance::ad_PutdownAnim:
            ped_->drawPickupFrame(x, y, frame_);
            break;
        case PedInstance::ad_WalkAnim:
            ped_->drawWalkFrame(x, y, getDirection(), frame_, weapon_idx);
            break;
        case PedInstance::ad_StandAnim:
            ped_->drawStandFrame(x, y, getDirection(), frame_, weapon_idx);
            break;
        case PedInstance::ad_WalkFireAnim:
            ped_->drawWalkFireFrame(x, y, getDirection(), frame_, weapon_idx);
            break;
        case PedInstance::ad_StandFireAnim:
            ped_->drawStandFireFrame(x, y, getDirection(), frame_, weapon_idx);
            break;
        case PedInstance::ad_VaporizeAnim:
            ped_->drawVaporizeFrame(x, y, getDirection(), frame_);
            break;
        case PedInstance::ad_SinkAnim:
            ped_->drawSinkFrame(x, y, frame_);
            break;
        case PedInstance::ad_StandBurnAnim:
            ped_->drawStandBurnFrame(x, y, frame_);
            break;
        case PedInstance::ad_WalkBurnAnim:
            ped_->drawWalkBurnFrame(x, y, frame_);
            break;
        case PedInstance::ad_DieBurnAnim:
            ped_->drawDieBurnFrame(x, y, frame_);
            break;
        case PedInstance::ad_SmokeBurnAnim:
            ped_->drawSmokeBurnFrame(x, y, frame_);
            break;
        case PedInstance::ad_DeadBurnAnim:
            ped_->drawDeadBurnFrame(x, y, frame_);
            break;
        case PedInstance::ad_PersuadedAnim:
            ped_->drawPersuadeFrame(x, y, frame_);
            break;
        case PedInstance::ad_NoAnimation:
            break;
    }
}

void PedInstance::drawSelectorAnim(int x, int y) {

    Weapon::WeaponAnimIndex weapon_idx =
        selectedWeapon() ? selectedWeapon()->index() : Weapon::Unarmed_Anim;

    switch(drawnAnim()) {
        case PedInstance::ad_HitAnim:
            ped_->drawHitFrame(x, y, getDirection(), frame_);
            break;
        case PedInstance::ad_DieAnim:
            ped_->drawDieFrame(x, y, frame_);
            break;
        case PedInstance::ad_DeadAnim:
            ped_->drawDeadFrame(x, y, frame_);
            break;
        case PedInstance::ad_DeadAgentAnim:
            ped_->drawDeadAgentFrame(x, y, frame_);
            break;
        case PedInstance::ad_PickupAnim:
            ped_->drawPickupFrame(x, y, frame_);
            break;
        case PedInstance::ad_PutdownAnim:
            ped_->drawPickupFrame(x, y, frame_);
            break;
        case PedInstance::ad_WalkAnim:
            ped_->drawWalkFrame(x, y, getDirection(), frame_, weapon_idx);
            break;
        case PedInstance::ad_StandAnim:
            ped_->drawStandFrame(x, y, getDirection(), frame_, weapon_idx);
            break;
        case PedInstance::ad_WalkFireAnim:
            ped_->drawWalkFireFrame(x, y, getDirection(), frame_, weapon_idx);
            break;
        case PedInstance::ad_StandFireAnim:
            ped_->drawStandFireFrame(x, y, getDirection(), frame_, weapon_idx);
            break;
        case PedInstance::ad_VaporizeAnim:
            ped_->drawVaporizeFrame(x, y, getDirection(), frame_);
            break;
        case PedInstance::ad_SinkAnim:
            ped_->drawSinkFrame(x, y, frame_);
            break;
        case PedInstance::ad_StandBurnAnim:
            ped_->drawStandBurnFrame(x, y, frame_);
            break;
        case PedInstance::ad_WalkBurnAnim:
            ped_->drawWalkBurnFrame(x, y, frame_);
            break;
        case PedInstance::ad_DieBurnAnim:
            ped_->drawDieBurnFrame(x, y, frame_);
            break;
        case PedInstance::ad_SmokeBurnAnim:
            ped_->drawSmokeBurnFrame(x, y, frame_);
            break;
        case PedInstance::ad_DeadBurnAnim:
            ped_->drawDeadBurnFrame(x, y, frame_);
            break;
        case PedInstance::ad_PersuadedAnim:
            ped_->drawPersuadeFrame(x, y, frame_);
            break;
        case PedInstance::ad_NoAnimation:
#ifdef _DEBUG
            printf("hmm ad_NoAnimation\n");
#endif
            break;
    }
}

bool PedInstance::inSightRange(MapObject *t) {

    return this->distanceTo(t) < sight_range_;
}

void PedInstance::setSelectedWeapon(int n) {

    if (selected_weapon_ != -1) {
        assert((size_t)selected_weapon_ < weapons_.size());
        WeaponInstance *wi = weapons_[selected_weapon_];
        if (wi->getWeaponType() == Weapon::EnergyShield) {
            setRcvDamageDef(MapObject::ddmg_Ped);
            wi->deactivate();
        }
        if (wi->getWeaponType() == Weapon::AccessCard)
            rmEmulatedGroupDef(4, og_dmPolice);
        desc_state_ &= (pd_smAll ^ (pd_smArmed | pd_smNoAmmunition));
    }

    selected_weapon_ = n;
    if (n != -1) {
        assert((size_t)selected_weapon_ < weapons_.size());
        WeaponInstance *wi = weapons_[selected_weapon_];

        if (wi->usesAmmo()) {
            if (wi->ammoRemaining() == 0) {
                desc_state_ |= pd_smNoAmmunition;
                return;
            } else {
                desc_state_ &= pd_smAll ^ pd_smNoAmmunition;
            }
        } else {
            desc_state_ &= pd_smAll ^ pd_smNoAmmunition;
        }

        if (wi->doesPhysicalDmg())
            desc_state_ |= pd_smArmed;
        else
            desc_state_ &= pd_smAll ^ pd_smArmed;

        if (wi->getWeaponType() == Weapon::EnergyShield) {
            setRcvDamageDef(MapObject::ddmg_PedWithEnergyShield);
            wi->activate();
        }
        if (wi->getWeaponType() == Weapon::AccessCard)
            addEmulatedGroupDef(4, og_dmPolice);
    }
}

bool PedInstance::selectRequiredWeapon(pedWeaponToUse *pw_to_use) {
    WeaponInstance *wi = selectedWeapon();
    // pair <rank, indx>
    std::vector < std::pair<int, int> > found_weapons;
    uint8 sz;

    if (!pw_to_use) {
        pw_to_use = &prefered_weapon_;
    } else if (prefered_weapon_.desc != 5) {
        // overriding selection to respect users choice
        pw_to_use = &prefered_weapon_;
    }

    bool found = false;
    switch (pw_to_use->desc) {
        case 2:
            if (pw_to_use->wpn.wi == wi) {
                found = true;
            } else {
                sz = weapons_.size();
                for (uint8 i = 0; i < sz; ++i) {
                    if (weapon(i) == wi) {
                        setSelectedWeapon(i);
                        found = true;
                        break;
                    }
                }
            }
            break;
        case 3:
            sz = weapons_.size();
            for (uint8 i = 0; i < sz; ++i) {
                WeaponInstance *pWI = weapons_[i];
                if (pWI->getWeaponType() == pw_to_use->wpn.wpn_type) {
                    if (pWI->usesAmmo()) {
                        if (pWI->ammoRemaining()) {
                            found = true;
                            setSelectedWeapon(i);
                            break;
                        }
                    } else {
                        found = true;
                        setSelectedWeapon(i);
                        break;
                    }
                }
            }
            break;
        case 4:
            sz = weapons_.size();
            for (uint8 i = 0; i < sz; ++i) {
                WeaponInstance *pWI = weapons_[i];
                if (pWI->canShoot()
                    && pWI->doesDmgStrict(pw_to_use->wpn.dmg_type))
                {
                    if (pWI->usesAmmo()) {
                        if (pWI->ammoRemaining()) {
                            found = true;
                            found_weapons.push_back(std::make_pair(pWI->rank(),
                                i));
                        }
                    } else {
                        found = true;
                        found_weapons.push_back(std::make_pair(pWI->rank(), i));
                    }
                }
            }
            break;
        case 5:
            sz = weapons_.size();
            for (uint8 i = 0; i < sz; ++i) {
                WeaponInstance *pWI = weapons_[i];
                if (pWI->canShoot()
                    && pWI->doesDmgNonStrict(pw_to_use->wpn.dmg_type))
                {
                    if (pWI->usesAmmo()) {
                        if (pWI->ammoRemaining()) {
                            found = true;
                            found_weapons.push_back(std::make_pair(pWI->rank(), i));
                        }
                    } else {
                        found = true;
                        found_weapons.push_back(std::make_pair(pWI->rank(), i));
                    }
                }
            }
            break;
    }

    if (!found_weapons.empty()) {
        int best_rank = -1;
        int indx = -1;
        if (pw_to_use->use_ranks) {
            sz = found_weapons.size();
            for (uint8 i = 0; i < sz; ++i) {
                if (best_rank < found_weapons[i].first) {
                    best_rank = found_weapons[i].first;
                    indx = found_weapons[i].second;
                }
            }
            setSelectedWeapon(indx);
        } else
            setSelectedWeapon(found_weapons[0].second);
    }
    return found;
}

void PedInstance::selectNextWeapon() {
    if (selected_weapon_ != -1) {
        int nextWeapon = -1;
        WeaponInstance *cur_sel_weapon = (WeaponInstance *)weapon(selected_weapon_);

        if (cur_sel_weapon) {
            for (int i = 0; i < numWeapons(); ++i) {
                WeaponInstance * wi = weapons_[i];
                if (i != selected_weapon_ && wi->usesAmmo() && wi->ammoRemaining()
                        && wi->getWeaponType() == cur_sel_weapon->getWeaponType())
                {
                    if (nextWeapon == -1)
                        nextWeapon = i;
                    else {
                        if (wi->ammoRemaining()
                            < weapon(nextWeapon)->ammoRemaining())
                        {
                            nextWeapon = i;
                        }
                    }
                }
            }
            if (nextWeapon == -1) {
                for (int i = 0; i < numWeapons(); ++i) {
                    WeaponInstance * wi = weapons_[i];
                    if (i != selected_weapon_ && wi->usesAmmo() && wi->ammoRemaining()
                            && wi->doesDmgNonStrict(cur_sel_weapon->dmgType()))
                    {
                        if (nextWeapon == -1)
                            nextWeapon = i;
                        else {
                            if (wi->ammoRemaining()
                                < weapon(nextWeapon)->ammoRemaining())
                            {
                                nextWeapon = i;
                            }
                        }
                    }
                }
            }
            if (nextWeapon != -1)
                setSelectedWeapon(nextWeapon);
        }

        if (nextWeapon == -1)
            selectRequiredWeapon();
    } else
        selectRequiredWeapon();
}

void PedInstance::dropWeapon(int n) {
    assert(n >= 0 && n < (int) weapons_.size());

    dropWeapon(weapons_[n]);
}

void PedInstance::dropWeapon(WeaponInstance *wi) {
    bool upd_selected = selected_weapon_ != -1;

    if (selectedWeapon() == wi) {
        setSelectedWeapon(-1);
        upd_selected = false;
    }
    for (int i = 0; i < (int)weapons_.size(); ++i)
    {
        std::vector <WeaponInstance *>::iterator it = weapons_.begin() + i;
        if ((*it) == wi) {
            weapons_.erase(it);
            if (upd_selected && selected_weapon_ > i)
                --selected_weapon_;
            break;
        }
    }
    wi->setOwner(NULL);
    wi->setMap(map_);
    wi->setIsIgnored();
    wi->setPosition(tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
    if (wi->getMainType() == Weapon::TimeBomb)
        wi->activate();
    else
        wi->deactivate();
}

void PedInstance::dropAllWeapons() {

    setRcvDamageDef(MapObject::ddmg_Ped);
    setSelectedWeapon(-1);
    Mission *m = g_Session.getMission();
    uint8 twd = m->mtsurfaces_[tile_x_ + m->mmax_x_ * tile_y_
        + m->mmax_m_xy * tile_z_].twd;

    for (std::vector < WeaponInstance * >::iterator it
        = weapons_.begin(); it != weapons_.end(); ++it)
    {
        WeaponInstance *w = *it;
        w->setMap(map());
        // randomizing location for drop
        int ox = rand() % 256;
        int oy = rand() % 256;
        w->setPosition(tile_x_, tile_y_, tile_z_, ox, oy);
        w->offzOnStairs(twd);
        w->setOwner(NULL);
        w->setIsIgnored();
        if (w->getMainType() == Weapon::TimeBomb)
            w->activate();
        else
            w->deactivate();
    }
    weapons_.clear();
}

bool PedInstance::wePickupWeapon() {
    return (state_ & pa_smPickUp) != 0;
}

VehicleInstance *PedInstance::inVehicle()
{
    return (state_ & (PedInstance::pa_smInCar
        | PedInstance::pa_smUsingCar)) != 0 ? in_vehicle_ : NULL;
}

void PedInstance::putInVehicle(VehicleInstance * v,
    pedActionStateMasks add_state)
{
    map_ = -1;
    in_vehicle_ = v;
    is_ignored_ = true;
    switchActionStateTo((uint32)add_state);
}

void PedInstance::leaveVehicle() {
    assert(map_ == -1 && in_vehicle_);
    map_ = in_vehicle_->map();
    in_vehicle_ = NULL;
    is_ignored_ = false;
    switchActionStateFrom(state_ & (PedInstance::pa_smInCar
        | PedInstance::pa_smUsingCar));
}


bool PedInstance::setVehicleIgnore(bool ignored) {
    if ((state_ & (PedInstance::pa_smInCar
        | PedInstance::pa_smUsingCar)) != 0)
    {
        in_vehicle_->setIsIgnored(ignored);
        return true;
    }
    return false;
}

int PedInstance::map() {

    return map_;
}

PedInstance::AnimationDrawn PedInstance::drawnAnim(void) {
    return drawn_anim_;
}

void PedInstance::setDrawnAnim(PedInstance::AnimationDrawn drawn_anim) {
    if (drawn_anim_ == drawn_anim)
        return;

    drawn_anim_ = drawn_anim;
    frame_ = 0;
    is_frame_drawn_ = false;
    switch (drawn_anim_) {
        case PedInstance::ad_HitAnim:
            setFramesPerSec(6);
            break;
        case PedInstance::ad_DieAnim:
            setFramesPerSec(8);
            break;
        case PedInstance::ad_DeadAnim:
            setFramesPerSec(2);
            break;
        case PedInstance::ad_DeadAgentAnim:
            setFramesPerSec(2);
            break;
        case PedInstance::ad_PickupAnim:
            setFramesPerSec(8);
            break;
        case PedInstance::ad_PutdownAnim:
            setFramesPerSec(8);
            break;
        case PedInstance::ad_WalkAnim:
            setFramesPerSec(8);
            break;
        case PedInstance::ad_StandAnim:
            setFramesPerSec(8);
            break;
        case PedInstance::ad_WalkFireAnim:
            setFramesPerSec(8);
            break;
        case PedInstance::ad_StandFireAnim:
            setFramesPerSec(8);
            break;
        case PedInstance::ad_VaporizeAnim:
            setFramesPerSec(6);
            break;
        case PedInstance::ad_SinkAnim:
            setFramesPerSec(8);
            break;
        case PedInstance::ad_StandBurnAnim:
            setFramesPerSec(8);
            break;
        case PedInstance::ad_WalkBurnAnim:
            setFramesPerSec(8);
            break;
        case PedInstance::ad_DieBurnAnim:
            setFramesPerSec(6);
            break;
        case PedInstance::ad_SmokeBurnAnim:
            setFramesPerSec(2);
            break;
        case PedInstance::ad_DeadBurnAnim:
            setFramesPerSec(2);
            break;
        case PedInstance::ad_PersuadedAnim:
            setFramesPerSec(8);
            break;
        case PedInstance::ad_NoAnimation:
            break;
    }
}

bool PedInstance::handleDrawnAnim(int elapsed) {
    Weapon::WeaponAnimIndex weapon_idx =
        selectedWeapon() ? selectedWeapon()->index() : Weapon::Unarmed_Anim;

    PedInstance::AnimationDrawn curanim = drawnAnim();
    // TODO: resolve switch selected weapon and current drawing
    // FIXME: quick fix. to remove
    if (weapon_idx == Weapon::Unarmed_Anim
        && (curanim == PedInstance::ad_StandFireAnim
        || curanim == PedInstance::ad_WalkFireAnim))
    {
        return true;
    }
    bool answer = true;
    switch (curanim) {
        case PedInstance::ad_HitAnim:
            if (frame_ < ped_->lastHitFrame(getDirection()))
                answer = false;
            else if (health_ <= 0)
                setDrawnAnim(PedInstance::ad_DieAnim);
            break;
        case PedInstance::ad_DieAnim:
            if (frame_ < ped_->lastDieFrame()) {
                answer = false;
                break;
            }
            setDrawnAnim(PedInstance::ad_DeadAnim);
            break;
        case PedInstance::ad_DeadAnim:
            break;
        case PedInstance::ad_DeadAgentAnim:
            break;
        case PedInstance::ad_PickupAnim:
        case PedInstance::ad_PutdownAnim:
            if (frame_ < ped_->lastPickupFrame())
                answer = false;
            break;
        case PedInstance::ad_WalkAnim:
        case PedInstance::ad_StandAnim:
            break;
        case PedInstance::ad_WalkFireAnim:
            if(frame_ < ped_->lastWalkFireFrame(getDirection(), weapon_idx))
                answer = false;
            break;
        case PedInstance::ad_StandFireAnim:
            if(frame_ < ped_->lastStandFireFrame(getDirection(), weapon_idx))
                answer = false;
            break;
        case PedInstance::ad_VaporizeAnim:
            if (frame_ >= ped_->lastVaporizeFrame(getDirection())) {
                if (is_our_) {
                    setDrawnAnim(PedInstance::ad_DeadAgentAnim);
                } else {
                    setDrawnAnim(PedInstance::ad_NoAnimation);
                }
                map_ = -1;
            } else
                answer = false;
            break;
        case PedInstance::ad_SinkAnim:
            // TODO: use this in future
            break;
        case PedInstance::ad_WalkBurnAnim:
        case PedInstance::ad_StandBurnAnim:
            if (leftTimeShowAnim(elapsed)) {
                answer = false;
                break;
            }
            setDrawnAnim(PedInstance::ad_DieBurnAnim);
            break;
        case PedInstance::ad_DieBurnAnim:
            if (frame_ >= ped_->lastDieBurnFrame()) {
                setDrawnAnim(PedInstance::ad_SmokeBurnAnim);
                setTimeShowAnim(7000);
            } else
                answer = false;
            break;
        case PedInstance::ad_SmokeBurnAnim:
            if (leftTimeShowAnim(elapsed)) {
                answer = false;
                break;
            }
            setDrawnAnim(PedInstance::ad_DeadBurnAnim);
            break;
        case PedInstance::ad_DeadBurnAnim:
            break;
        case PedInstance::ad_PersuadedAnim:
            if (frame_ < ped_->lastPersuadeFrame())
                answer = false;
            break;
        case PedInstance::ad_NoAnimation:
            break;
    }
    return answer;
}

bool PedInstance::handleDamage(ShootableMapObject::DamageInflictType *d) {
    if (health_ <= 0 || rcv_damage_def_ == MapObject::ddmg_Invulnerable
        || (d->dtype & rcv_damage_def_) == 0)
        return false;

    if ((d->dtype & MapObject::dmg_Physical) != 0)
        health_ -= d->dvalue;
    else if (d->dtype == MapObject::dmg_Persuasion) {
        // TODO: check for required number of persuade points before applying
        // they should be influenced by mod brain
        setObjGroupID(((PedInstance *)d->d_owner)->objGroupID());
        ((PedInstance *)d->d_owner)->cpyEnemyDefs(enemy_group_defs_);
        dropActQ();
        assert(d->d_owner != NULL);
        owner_ = d->d_owner;
        desc_state_ |= pd_smControlled;
        friends_found_.clear();
        hostiles_found_.clear();
        hostile_desc_ = ((PedInstance *)d->d_owner)->hostileDesc();

        // adding following behavior
        PedInstance::actionQueueGroupType as;
        as.group_id = 0;
        as.group_desc = PedInstance::gd_mExclusive;
        createActQWait(as, 2000);
        as.main_act = as.actions.size() - 1;
        as.origin_desc = fs_actions::kOrigEvent;
        actions_queue_.push_back(as);

        default_actions_.clear();
        createDefQueue();

        addDefActsToActions();

        setDrawnAnim(PedInstance::ad_PersuadedAnim);
        setRcvDamageDef(MapObject::ddmg_PedPanicImmune);
        return true;
    }

    if (d->ddir != -1)
        dir_ = (d->ddir + 128) % 256;

    if (health_ <= 0) {
        health_ = 0;
        actions_property_ = 1;
        switchActionStateTo(PedInstance::pa_smDead);

        switch ((unsigned int)d->dtype) {
            case MapObject::dmg_Bullet:
                setDrawnAnim(PedInstance::ad_DieAnim);
                dropAllWeapons();
                break;
            case MapObject::dmg_Laser:
                setDrawnAnim(PedInstance::ad_VaporizeAnim);
                destroyAllWeapons();
                break;
            case MapObject::dmg_Burn:
                // TODO: sometime we will walk burning
                setDrawnAnim(PedInstance::ad_StandBurnAnim);
                destroyAllWeapons();
                setTimeShowAnim(4000);
                break;
            case MapObject::dmg_Explosion:
                // TODO: sometime we will walk burning
                setDrawnAnim(PedInstance::ad_StandBurnAnim);
                destroyAllWeapons();
                setTimeShowAnim(4000);
                break;
            case MapObject::dmg_Hit:
                setDrawnAnim(PedInstance::ad_HitAnim);
                dropAllWeapons();
                break;
        }
        is_ignored_ = true;
        // send an event to alert agent died
        if (isOurAgent()) {
            GameEvent evt;
            evt.stream = GameEvent::kMission;
            evt.type = GameEvent::kAgentDied;
            evt.pCtxt = this;
            g_gameCtrl.fireGameEvent(evt);
        }
    } else {
        // TODO: agent sometime can survive explosion, they need to walk burning?
        setDrawnAnim(PedInstance::ad_HitAnim);
    }
    return true;
}

void PedInstance::destroyAllWeapons() {
    setSelectedWeapon(-1);
    while (!weapons_.empty()) {
        WeaponInstance * w = removeWeapon(0);
        w->setMap(-1);
        w->setPosition(tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
        w->setOwner(NULL);
        w->setIsIgnored(true);
        w->deactivate();
    }
}

void PedInstance::addEnemyGroupDef(uint32 eg_id, uint32 eg_def) {
    enemy_group_defs_.add(eg_id, eg_def);
}

void PedInstance::rmEnemyGroupDef(uint32 eg_id, uint32 eg_def) {
    enemy_group_defs_.rm(eg_id, eg_def);
}

bool PedInstance::isInEnemyGroupDef(uint32 eg_id, uint32 eg_def) {
    return enemy_group_defs_.isIn(eg_id, eg_def);
}

void PedInstance::addEmulatedGroupDef(uint32 eg_id, uint32 eg_def) {
    emulated_group_defs_.add(eg_id, eg_def);
}
void PedInstance::rmEmulatedGroupDef(uint32 eg_id, uint32 eg_def) {
    emulated_group_defs_.rm(eg_id, eg_def);
}

bool PedInstance::isInEmulatedGroupDef(uint32 eg_id, uint32 eg_def) {
    return emulated_group_defs_.isIn(eg_id, eg_def);
}

bool PedInstance::isInEmulatedGroupDef(PedInstance::Mmuu32_t &r_egd,
        bool id_only)
{
    if (id_only) {
        return emulated_group_defs_.isIn_KeyOnly(r_egd);
    }
    return emulated_group_defs_.isIn_All(r_egd);
}

bool PedInstance::checkHostileIs(ShootableMapObject *obj,
    unsigned int hostile_desc_alt)
{
    bool hostile_rsp = false;
    bool friend_is = false;

    if (obj->majorType() == MapObject::mjt_Vehicle) {
        hostile_rsp = ((VehicleInstance *)obj)->checkHostilesInside(
            this, hostile_desc_alt);
    } else if (obj->majorType() == MapObject::mjt_Ped) {
        if (checkFriendIs((PedInstance *)obj))
            friend_is = true;
        if (((PedInstance *)obj)->emulatedGroupDefsEmpty()) {
            hostile_rsp =
                isInEnemyGroupDef(((PedInstance *)obj)->objGroupID(),
                ((PedInstance *)obj)->objGroupDef());
        } else {
            hostile_rsp =
                ((PedInstance *)obj)->isInEmulatedGroupDef(enemy_group_defs_);
        }
        if (!(hostile_rsp || friend_is)) {
            if (hostile_desc_alt == PedInstance::pd_smUndefined)
                hostile_desc_alt = hostile_desc_;
            hostile_rsp = (((PedInstance *)obj)->descStateMasks()
                & hostile_desc_alt) != 0;
        }
    }

    return hostile_rsp;
}

bool PedInstance::checkFriendIs(PedInstance *p) {
    // Friend can be neutral to be sure that object is hostile use
    // checkHostileIs and check hostiles_found_(isInHostilesFound)
    if (friends_found_.find(p) != friends_found_.end())
        return true;
    if (p->isInEmulatedGroupDef(obj_group_id_, obj_group_def_)) {
        if (obj_group_def_ == og_dmPolice
            && (desc_state_ & pd_smControlled) == 0)
        {
            friends_not_seen_.insert(p);
        }
        return true;
    }
    if (friend_group_defs_.find(p->objGroupDef()) != friend_group_defs_.end())
        return true;
    return (p->objGroupID() == obj_group_id_);
}

void PedInstance::verifyHostilesFound(Mission *m) {
    std::vector <ShootableMapObject *> rm_set;
    toDefineXYZ cur_xyz;
    convertPosToXYZ(&cur_xyz);
    int check_rng = sight_range_;

    WeaponInstance *wi = selectedWeapon();
    if (wi && wi->doesPhysicalDmg() && wi->canShoot() && wi->range() > check_rng)
        check_rng = wi->range();

    // removing destroyed, friends, objects out of shot/sight range
    for (Msmod_t::iterator it = hostiles_found_.begin();
        it != hostiles_found_.end(); ++it)
    {
        ShootableMapObject *smo = it->first;
        double distTo = 0;
        if (smo->isDead() || (smo->majorType() == MapObject::mjt_Ped
            && checkFriendIs((PedInstance *)(smo)))
            || (smo->majorType() == MapObject::mjt_Vehicle
            && ((VehicleInstance *)smo)->checkHostilesInside(this, hostile_desc_))
            || (m->inRangeCPos(&cur_xyz, &smo, NULL, false, false,
            check_rng, &distTo) != 1))
        {
            rm_set.push_back(smo);
        }
    }
    while (!rm_set.empty()) {
        hostiles_found_.erase(hostiles_found_.find(rm_set.back()));
        rm_set.pop_back();
    }
}

int PedInstance::getSpeed()
{
    int speed_new = base_speed_;

    int weight_max = 0;
    Mod *pMod = slots_[Mod::MOD_LEGS];
    if (pMod) {
        weight_max += 5 << (pMod->getVersion() + 1);
        speed_new *= (pMod->getVersion() + 5);
        speed_new >>= 2;
    } else
        weight_max = 5;
    pMod = slots_[Mod::MOD_ARMS];
    if (pMod)
        weight_max += 5 << (pMod->getVersion() + 1);
    else
        weight_max += 5;

    int weight_inv = 0;
    for (uint8 i = 0; i < weapons_.size(); ++i)
        weight_inv += weapons_[i]->getWeight();

    if (weight_inv > weight_max) {
        if ((weight_inv / weight_max) > 1)
            speed_new = 64;
        else
            speed_new /= 2;
    }

    if (obj_group_def_ == PedInstance::og_dmAgent)
    {
        // See the comments in the IPAStim class for details on the multiplier
        // algorithm for adrenaline
        speed_new = (int)((float)speed_new * adrenaline_->getMultiplier());
    }

    if (desc_state_ == PedInstance::pd_smControlled) {
        speed_new *= ((PedInstance *)owner_)->getSpeedOwnerBoost();
        speed_new >>= 1;
    }

    return speed_new;
}

// NOTE: returned value is *2, it should be should be corrected
// during calculations with /2
int PedInstance::getSpeedOwnerBoost()
{
    if (obj_group_def_ == PedInstance::og_dmAgent)
    {
        float ipa_adr = adrenaline_->getMultiplier();
        if (ipa_adr > 1.0)
            return 4;
        else if (ipa_adr < 1.0)
            return 1;
    }

    return 2;
}


void PedInstance::getAccuracy(double &base_acc)
{
    double base_mod = base_mod_acc_;

    if (obj_group_def_ == PedInstance::og_dmAgent)
    {
        Mod *pMod = slots_[Mod::MOD_EYES];
        if (pMod) {
            base_mod += 0.006 * (pMod->getVersion() + 1);
        }
        pMod = slots_[Mod::MOD_BRAIN];
       if (pMod) {
            base_mod += 0.006 * (pMod->getVersion() + 1);
        }
        pMod = slots_[Mod::MOD_ARMS];
        if (pMod) {
            base_mod += 0.006 * (pMod->getVersion() + 1);
        }
        pMod = slots_[Mod::MOD_HEART];
        if (pMod) {
            base_mod += 0.006 * (pMod->getVersion() + 1);
        }
        pMod = slots_[Mod::MOD_LEGS];
        if (pMod) {
            base_mod += 0.006 * (pMod->getVersion() + 1);
        }
        // 0.59 max from here

        base_mod -= 0.4;
        base_mod += 0.4 * (2.0 - adrenaline_->getMultiplier());
        // 0.99 max after adrenaline
    }

    // NOTE :(1.0 - base_acc) is randomized and not dependent on anything
    // should be added back
    // Ex. weapon accuracy 70%, random value 30%, then
    // 0.7 * (1.0 - base_mod(here 0.5) + (1.0 - 0.7) = 0.65
    // the shot will be randomized at 65% of weapons max angle
    base_acc = base_acc * (1.0 - base_mod) + (1.0 - base_acc);
}

bool PedInstance::hasAccessCard()
{
    WeaponInstance * wi = selectedWeapon();
    Mod *pMod = slots_[Mod::MOD_BRAIN];
    return wi && pMod && wi->getWeaponType() == Weapon::AccessCard
        && pMod->getVersion() == Mod::MOD_V3 ? true : false;
}

bool PedInstance::isPersuaded()
{
    return (desc_state_ & pd_smControlled) != 0
        && obj_group_id_ == g_Session.getMission()->playersGroupID();
}

void PedInstance::updtPreferedWeapon() {
    if (selected_weapon_ != -1) {
        WeaponInstance *wi = weapons_[selected_weapon_];
        prefered_weapon_.desc = 2;
        prefered_weapon_.wpn.wi = wi;
    } else {
        prefered_weapon_.desc = 5;
        prefered_weapon_.wpn.dmg_type = MapObject::dmg_Physical;
    }
}

