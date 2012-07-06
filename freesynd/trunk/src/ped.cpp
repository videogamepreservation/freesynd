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
#include <math.h>

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
    // TODO: findout whether frame really changes?
    g_App.gameSprites().drawFrame(dead_anim_, frame, x, y);
}

void Ped::drawDeadAgentFrame(int x, int y, int frame) {
    // TODO: findout whether frame really changes?
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
    // TODO: findout whether frame really changes?
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
            state_ &= (pa_smAll ^ pa_smStanding);
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
            state_ &= pa_smAll ^ pa_smFollowing;
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
            printf("It's alive!\n");
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
    }
#ifdef _DEBUG
    if (state_ ==  pa_smNone)
        printf("undefined state_\n");
#endif
}

bool PedInstance::animate(int elapsed, Mission *mission) {
    // TODO: proper handling for exclusive states, switching;
    // TODO: animation and state binding, finished animation=state complete?

    if (agent_is_ == PedInstance::Agent_Non_Active)
        return false;

    bool updated = false;
    //Weapon::WeaponAnimIndex weapon_idx =
        //selectedWeapon() ? selectedWeapon()->index() : Weapon::Unarmed_Anim;
    if (actions_property_ == 1) {
        dropActQ();
        actions_property_ = 0;
    }

    if (actions_queue_.size() != 0) {
        for (uint32 indx = 0; indx < actions_queue_.size();) {
            std::vector <actionQueueGroupType>::iterator it =
                actions_queue_.begin() + indx;
            if ((it->state & 128) == 0 && (it->state & 12) != 0) {
                actions_queue_.erase(it);
                continue;
            }
            indx++;
        }
    }

    // NOTE: some actions have remaining time, it is lost for now
    if (actions_queue_.empty()) {
        // TODO: use default_actions_ to fill it up
#if 1
        if ((state_ & pa_smDead) == 0) {
            actionQueueGroupType as;
            createActQFindEnemy(as);
            as.main_act = 0;
            as.group_desc = PedInstance::gd_mThink | PedInstance::gd_mFire;
            actions_queue_.push_back(as);
        }
#endif
    } else {
        friends_not_seen_.clear();
        // TODO: xor finished and failed, should all actions will
        // have execution time set?
        uint32 groups_processed = 0;
        for (std::vector <actionQueueGroupType>::iterator it =
            actions_queue_.begin(); it != actions_queue_.end()
            && actions_property_ == 0; it++)
        {
            if ((it->state & 128) == 0 && (it->state & 76) != 0)
                continue;
            if ((it->group_desc & groups_processed) != 0)
                break;
            if ((it->group_desc & PedInstance::gd_mExclusive) != 0
                && groups_processed != 0)
                break;

            uint32 acts_g_prcssd = 0;
            for (uint32 indx = 0; indx < it->actions.size(); indx++)
            {
                actionQueueType & aqt = it->actions[indx];
                if ((acts_g_prcssd & aqt.group_desc) != 0
                    || (acts_g_prcssd & groups_processed) != 0)
                    break;
                if ((aqt.state & 128) == 0 && (aqt.state & 76) != 0)
                    continue;
                if ((aqt.group_desc & PedInstance::gd_mExclusive) != 0
                    && acts_g_prcssd != 0)
                    break;
                if ((aqt.ot_execute & PedInstance::ai_aNone) != 0)
                {
                    printf("obj_None");
                }
                if ((aqt.ot_execute & PedInstance::ai_aWaitToStart) != 0)
                {
                    aqt.state |= 384;
                    aqt.multi_var.time_var.elapsed += elapsed;
                    if (aqt.multi_var.time_var.elapsed >=
                        aqt.multi_var.time_var.time_before_start)
                    {
                        aqt.state ^= 384;
                        aqt.ot_execute ^= PedInstance::ai_aWaitToStart;
                        elapsed = aqt.multi_var.time_var.elapsed
                            - aqt.multi_var.time_var.time_before_start;
                        aqt.multi_var.time_var.elapsed = 0;
                    }
                }
                if ((aqt.ot_execute & PedInstance::ai_aAquireControl) != 0)
                {
                    if (aqt.t_smo->majorType() == MapObject::mjt_Ped) {
                        // TODO: make it properly, check selected weapon
                        // if not the one in weapon.desc select it,
                        // check owner
                        // NOTE: don't put weapon on the ground
                        WeaponInstance *wi = aqt.multi_var.enemy_var.weapon.wpn.wi;
                        if (wi->getMainType()
                            == Weapon::Persuadatron)
                        {
                            // TODO: proper handling for time, add condition
                            // of failure to inflict damage
                            if (aqt.state == 1)
                                aqt.state |= 2;
                            int tm_left = elapsed;
                            if (wi->inflictDamage(aqt.t_smo, NULL, &tm_left) == 0)
                                if (checkFriendIs((PedInstance *)aqt.t_smo))
                                    aqt.state |= 4;
                        } else
                            aqt.state |= 8;
                    } else if (aqt.t_smo->majorType()
                                == MapObject::mjt_Vehicle)
                    {
                        VehicleInstance *v = (VehicleInstance *)aqt.t_smo;
                        if (v->health() > 0 && (state_
                            & (PedInstance::pa_smInCar
                            | PedInstance::pa_smUsingCar)) == 0
                            && samePosition(v))
                        {
                            if (aqt.condition == 0) {
                                v->setDriver(this);
                                if (v->isInsideVehicle(this)) {
                                    in_vehicle_ = v;
                                    aqt.state |= 4;
                                    is_ignored_ = true;
                                    map_ = -1;
                                    state_ |= pa_smInCar;
                                } else
                                    aqt.state |= 8;
                            } else if (aqt.condition == 1) {
                                if (v->hasDriver()) {
                                    if (v->isDriver(this))
                                        aqt.state |= 4;
                                    else
                                        aqt.state |= 8;
                                } else {
                                    v->setDriver(this);
                                    in_vehicle_ = v;
                                    aqt.state |= 4;
                                    is_ignored_ = true;
                                    map_ = -1;
                                    state_ |= pa_smUsingCar;
                                }
                            }
                        } else
                            aqt.state |= 8;
                    } else
                        // type cannot be aquired
                        aqt.state |= 8;
                }
                if ((aqt.ot_execute & PedInstance::ai_aLoseControl) != 0)
                {
                    if (aqt.state == 1) {
                        if (aqt.t_smo->majorType() == MapObject::mjt_Ped) {
                            // TODO: but not now
                        } else if (aqt.t_smo->majorType()
                                   == MapObject::mjt_Vehicle)
                        {
                            VehicleInstance *v = (VehicleInstance *)aqt.t_smo;
                            if (v->isInsideVehicle(this)) {
                                v->removeDriver(this);
                                map_ = v->map();
                                is_ignored_ = false;
                                in_vehicle_ = NULL;
                                aqt.state |= 4;
                            } else
                                aqt.state |= 8;
                        }
                    }
                }
                if ((aqt.ot_execute & PedInstance::ai_aPickUpObject) != 0)
                {
                    if (aqt.state == 1) {
                        //TODO: better state checking
                        if ((state_ & (PedInstance::pa_smInCar
                            | PedInstance::pa_smUsingCar)) != 0)
                            aqt.state |= 8;
                        WeaponInstance *wi = (WeaponInstance *)aqt.t_smo;
                        if (wi->hasOwner() || weapons_.size() == 8)
                            aqt.state |= 8;
                        else {
                            wi->setOwner(this);
                            wi->setMap(-1);
                            wi->setIsIgnored(true);
                            wi->deactivate();
                            weapons_.push_back(wi);
                            aqt.state |= 4 + 32;
                        }
                    }
                }
                if ((aqt.ot_execute & PedInstance::ai_aDestroyObject) != 0)
                {
                    if ((aqt.state & 128) == 0) {
                        if (aqt.t_smo->health() <= 0) {
                            // not object did it as such he failed,
                            // but goal reached
                            aqt.state |= 12;
                        } else {
                            // TODO: check friend
                            // TODO: make it properly, check selected weapon
                            // if not the one in weapon.desc select it,
                            // check owner
                            WeaponInstance *wi = selectedWeapon();
                            if (!wi)
                                selectBestWeapon();
                            wi = selectedWeapon();
                            if (wi && wi->ammoRemaining() > 0)
                            {
                                int tm_left = elapsed;
                                uint32 make_shots = aqt.multi_var.enemy_var.make_shots;
                                uint32 shots_done = make_shots
                                    - aqt.multi_var.enemy_var.shots_done;
                                uint16 answ = wi->inflictDamage(aqt.t_smo, NULL, &tm_left,
                                    aqt.multi_var.enemy_var.forced_shot, &shots_done);
                                // TODO: handle correctly, use info returned from
                                // inflictDamage, needs use of condition
                                if (answ == 0 || answ == 2) {
                                    if ((aqt.ot_execute & PedInstance::ai_aWait) != 0)
                                        aqt.state |= 2 + 32;
                                    else
                                        aqt.state |= 2;
                                    if (make_shots == 0 && aqt.t_smo->health() <= 0)
                                        aqt.state |= 4;
                                    else if (shots_done != 0) {
                                        aqt.multi_var.enemy_var.shots_done += shots_done;
                                        if (make_shots != 0 && make_shots
                                            <= shots_done
                                            + aqt.multi_var.enemy_var.shots_done)
                                        {
                                            aqt.state |= 4;
                                        }
                                    }
                                } else
                                    aqt.state |= 8;
                            } else
                                aqt.state |= 8;
                        }
                    } else if ((aqt.ot_execute & PedInstance::ai_aWaitToStart) != 0)
                    {
                        if (aqt.t_smo->majorType() == MapObject::mjt_Ped
                            && ((obj_group_def_ == og_dmPolice
                            && checkHostileIs((PedInstance *)aqt.t_smo))
                            || aqt.t_smo->health() <= 0))
                        {
                            aqt.ot_execute &= PedInstance::ai_aAll
                                ^ (PedInstance::ai_aWaitToStart
                                | PedInstance::ai_aWait);
                            aqt.state ^= 384;
                            // failed target is friendly or dead
                            aqt.state |= 8;
                            if (obj_group_def_ == og_dmPolice)
                                setSelectedWeapon(-1);
                        }
                    }
                }
                if ((aqt.ot_execute & PedInstance::ai_aUseObject) != 0)
                {
                }
                if ((aqt.ot_execute & PedInstance::ai_aPutDownObject) != 0)
                {
                    if (aqt.state == 1) {
                        //TODO: better state checking
                        if ((state_ & (PedInstance::pa_smInCar
                            | PedInstance::pa_smUsingCar)) != 0)
                            aqt.state |= 8;
                        WeaponInstance *wi = (WeaponInstance *)aqt.t_smo;
                        if (wi->getOwner() != this)
                            aqt.state |= 8;
                        else {
                            dropWeapon(wi);
                            aqt.state |= 4 + 32;
                        }
                    }
                }
                if ((aqt.ot_execute & PedInstance::ai_aReachLocation) != 0)
                {
                    //TODO: for now we fail while trying to walk
                    // in car
                    //TODO: better state checking
                    if ((state_ & (PedInstance::pa_smInCar
                        | PedInstance::pa_smUsingCar)) != 0)
                        aqt.state |= 8;
                    if (aqt.state == 1) {
                        //TODO: IPA + mods
                        int speed_set = 1280;// aqt.multi_var.dist_var.speed
                        if (aqt.condition == 0) {
                            bool set_new_dest = true;
                            dist_to_pos_ = aqt.multi_var.dist_var.dist;
                            if (dist_to_pos_ != 0) {
                                toDefineXYZ xyz;
                                int dist_is = -1;
                                aqt.t_pn.convertPosToXYZ(&xyz);
                                dist_is = (int)distanceToPosXYZ(&xyz);
                                if (dist_is <= dist_to_pos_)
                                    set_new_dest = false;
                            }
                            if (set_new_dest) {
                                aqt.state |= 2;
                                setDestinationP(mission, aqt.t_pn.tileX(),
                                    aqt.t_pn.tileY(), aqt.t_pn.tileZ(),
                                    aqt.t_pn.offX(), aqt.t_pn.offY());
                                if (dest_path_.empty()) {
                                    aqt.state |= 8;
                                    speed_ = 0;
                                } else
                                    speed_ = speed_set;
                            } else {
                                aqt.state |= 4;
                            }
                        } else if (aqt.condition == 1) {
                            // TODO: directional movement
                        } else if (aqt.condition == 2) {
                            bool set_new_dest = true;
                            dist_to_pos_ = aqt.multi_var.dist_var.dist;
                            if (dist_to_pos_ != 0) {
                                int dist_is = -1;
                                dist_is = (int)distanceTo(
                                    (MapObject *)aqt.t_smo);
                                if (dist_is <= dist_to_pos_)
                                    set_new_dest = false;
                            } else {
                                if (!this->samePosition(aqt.t_smo))
                                    set_new_dest = true;
                            }
                            if (set_new_dest) {
                                aqt.state |= 2;
                                setDestinationP(mission,
                                    aqt.t_smo->tileX(), aqt.t_smo->tileY(),
                                    aqt.t_smo->tileZ(), aqt.t_smo->offX(),
                                    aqt.t_smo->offY());
                                if (dest_path_.empty()) {
                                    aqt.state |= 8;
                                    speed_ = 0;
                                } else
                                    speed_ = speed_set;
                            } else
                                aqt.state |= 4;
                        }
                    }
                    if ((aqt.state & 15) == 3) {
                        //TODO: IPA + mods
                        int speed_set = 1280;
                        speed_ = speed_set;
                        if (aqt.condition == 0 || aqt.condition == 2) {
                            updated = movementP(mission, elapsed);
                            if (speed_ == 0)
                                aqt.state |= 4;
                        } else if (aqt.condition == 1) {
                            // TODO: later
                        }
                    }
                }
                if ((aqt.ot_execute & PedInstance::ai_aFollowObject) != 0)
                {
                    if (aqt.state == 1 || aqt.state == 17) {
                        int speed_set = 256;
                        if (aqt.condition == 0) {
                            dist_to_pos_ = aqt.multi_var.dist_var.dist;
                            int dist_is = -1;
                            dist_is = (int)distanceTo(
                                (MapObject *)aqt.t_smo);
                            if (dist_is > dist_to_pos_) {
                                aqt.state |= 2;
                                setDestinationP(mission,
                                    aqt.t_smo->tileX(), aqt.t_smo->tileY(),
                                    aqt.t_smo->tileZ(), aqt.t_smo->offX(),
                                    aqt.t_smo->offY());
                                if (dest_path_.empty()) {
                                    aqt.state |= 8;
                                    speed_ = 0;
                                } else
                                    speed_ = speed_set;
                                if ((aqt.state & 16) != 0)
                                    aqt.state ^= 16;
                            } else
                                aqt.state |= 16;
                        } else if (aqt.condition == 1) {
                            WeaponInstance *wi = selectedWeapon();
                            if (wi) {
                                bool set_new_dest = true;
                                // TODO: set checktile check value based
                                // on mode
                                if (wi->inRangeNoCP(&aqt.t_smo, NULL,
                                    false, false) == 1) {
                                    set_new_dest = false;
                                }
                                if (set_new_dest) {
                                    aqt.state |= 2;
                                    setDestinationP(mission,
                                        aqt.t_smo->tileX(), aqt.t_smo->tileY(),
                                        aqt.t_smo->tileZ(), aqt.t_smo->offX(),
                                        aqt.t_smo->offY());
                                    if (dest_path_.empty()) {
                                        aqt.state |= 8;
                                        speed_ = 0;
                                    } else
                                        speed_ = speed_set;
                                    if ((aqt.state & 16) != 0)
                                        aqt.state ^= 16;
                                } else
                                    aqt.state |= 16;
                            } else
                                aqt.state |= 8;
                        }
                    } else if ((aqt.state & 30) == 2) {
                        int speed_set = 256;
                        speed_ = speed_set;
                        if (aqt.condition == 0) {
                            updated = movementP(mission, elapsed);
                            if (speed_ == 0) {
                                aqt.state ^= 2;
                                aqt.state |= 16;
                            } else {
                                PathNode &rp = dest_path_.back();
                                if (rp.tileX() != aqt.t_smo->tileX()
                                    || rp.tileY() != aqt.t_smo->tileY()
                                    || rp.tileZ() != aqt.t_smo->tileZ()
                                    || rp.offX() != aqt.t_smo->offX()
                                    || rp.offY() != aqt.t_smo->offY()
                                    || rp.offZ() != aqt.t_smo->offZ())
                                {
                                    // resetting target position
                                    setDestinationP(mission,
                                        aqt.t_smo->tileX(), aqt.t_smo->tileY(),
                                        aqt.t_smo->tileZ(), aqt.t_smo->offX(),
                                        aqt.t_smo->offY());
                                    if (dest_path_.empty()) {
                                        aqt.state |= 8;
                                        speed_ = 0;
                                    } else
                                        speed_ = speed_set;
                                }
                            }
                        } else if (aqt.condition == 1) {
                            // TODO: view range if no weapon?
                            WeaponInstance *wi = selectedWeapon();
                            if (wi) {
                                updated = movementP(mission, elapsed);
                                if (wi->inRangeNoCP(&aqt.t_smo) == 0) {
                                    aqt.state ^= 2;
                                    aqt.state |= 16;
                                    dest_path_.clear();
                                    speed_ = 0;
                                } else {
                                    PathNode &rp = dest_path_.back();
                                    if (rp.tileX() != aqt.t_smo->tileX()
                                        || rp.tileY() != aqt.t_smo->tileY()
                                        || rp.tileZ() != aqt.t_smo->tileZ()
                                        || rp.offX() != aqt.t_smo->offX()
                                        || rp.offY() != aqt.t_smo->offY()
                                        || rp.offZ() != aqt.t_smo->offZ())
                                    {
                                        // resetting target position
                                        setDestinationP(mission,
                                            aqt.t_smo->tileX(), aqt.t_smo->tileY(),
                                            aqt.t_smo->tileZ(), aqt.t_smo->offX(),
                                            aqt.t_smo->offY());
                                        if (dest_path_.empty()) {
                                            aqt.state |= 8;
                                            speed_ = 0;
                                        } else
                                            speed_ = speed_set;
                                    }
                                }
                            } else
                                aqt.state |= 8;
                        }
                    }
                }
                if ((aqt.ot_execute & PedInstance::ai_aAttackLocation) != 0
                    && (aqt.state & 128) == 0)
                {
                    // TODO: additional conditions
                    WeaponInstance *wi = selectedWeapon();
                    if (!wi)
                        selectBestWeapon();
                    if (wi //&& (wi->shotProperty()
                        //& Weapon::spe_CanShoot) != 0
                        && wi->ammoRemaining() > 0)
                    {
                        int tm_left = elapsed;
                        uint32 make_shots = aqt.multi_var.enemy_var.make_shots;
                        uint32 shots_done = make_shots
                            - aqt.multi_var.enemy_var.shots_done;
                        uint16 answ = wi->inflictDamage(NULL, &aqt.t_pn, &tm_left,
                            aqt.multi_var.enemy_var.forced_shot, &shots_done);
                        if (answ == 0 || answ == 2) {
                            if ((aqt.ot_execute & PedInstance::ai_aWait) != 0)
                                aqt.state |= 2 + 32;
                            else
                                aqt.state |= 2;
                            if (shots_done != 0) {
                                aqt.multi_var.enemy_var.shots_done += shots_done;
                                if (make_shots != 0 && make_shots
                                    <= shots_done
                                    + aqt.multi_var.enemy_var.shots_done)
                                {
                                    aqt.state |= 4;
                                }
                            }
                        } else
                            aqt.state |= 8;
                    } else
                        aqt.state |= 8;
                }
                if ((aqt.ot_execute & PedInstance::ai_aFindEnemy) != 0)
                {
                    bool selfState = is_ignored_;
                    is_ignored_ = true;
                    // TODO: check inside of vehicles too, is_ignored_?
                    if (!hostiles_found_.empty())
                        verifyHostilesFound(mission);

                    Msmod_t smo_dist;
                    if (hostiles_found_.empty()) {
                        // TODO: check for weapons, set the largest shooting
                        // range? not only selected, all?
                        int shot_rng = 0;
                        WeaponInstance *wi = selectedWeapon();
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
                            for (int i = 0; i < num_peds; i++) {
                                PedInstance *p = mission->ped(i);
                                if ((p->state_ &
                                    pa_smCheckExcluded) != 0
                                    || hostiles_found_.find(p)
                                    != hostiles_found_.end()
                                    || smo_dist.find(p)
                                    != smo_dist.end())
                                    continue;
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
                                            if (checkHostileIs(smo)) {
                                                // TODO: inrange check here might
                                                // reduce speed, check
                                                // TODO: reduce inrange calls, later
                                                if (//inSightRange((MapObject *)(smo))
                                                    //&&
                                                    mission->inRangeCPos(
                                                    &cur_xyz,
                                                    &smo, NULL, false, false,
                                                    view_rng, &distTo)
                                                    == 1)
                                                {
                                                    hostiles_found_.insert(
                                                        Pairsmod_t(smo, distTo));
                                                } else if (shot_rng > 0
                                                    && mission->inRangeCPos(
                                                    &cur_xyz,
                                                    &smo, NULL, false, false,
                                                    shot_rng, &distTo) == 1)
                                                {
                                                    smo_dist.insert(
                                                        Pairsmod_t(smo, distTo));
                                                }
                                            }
                                            it_s++;
                                        } while (it_s != it_e);
                                    }
                                } else if (checkHostileIs(p) ) {
                                    // TODO: hostile_desc_alt to checkHostileIs?
                                    // IPA lvl inteligence?
                                    double distTo = 0;
                                    if (//inSightRange((MapObject *)(p)) &&
                                        // TODO: set ignoreblocker based on AL
                                        mission->inRangeCPos(
                                        &cur_xyz,
                                        (ShootableMapObject **)(&p),
                                        NULL, false, false,
                                        view_rng, &distTo)
                                        == 1)
                                    {
                                        hostiles_found_.insert(
                                            Pairsmod_t(
                                            (ShootableMapObject *)p,
                                            distTo));
                                    }
                                }
                            }
                        } else {
                            int num_peds = mission->numPeds();
                            for (int i = 0; i < num_peds; i++) {
                                PedInstance *p = mission->ped(i);
                                if ((p->state_ & pa_smCheckExcluded) != 0
                                    || hostiles_found_.find(p)
                                    != hostiles_found_.end())
                                    continue;
                                if (checkHostileIs(p) ) {
                                    // TODO: hostile_desc_alt to checkHostileIs?
                                    double distTo = 0;
                                    if (//inSightRange((MapObject *)(p))
                                        //&&
                                        mission->inRangeCPos(
                                        &cur_xyz,
                                        (ShootableMapObject **)(&p),
                                        NULL, false, false,
                                        view_rng, &distTo)
                                        == 1)
                                    {
                                        hostiles_found_.insert(
                                            Pairsmod_t(
                                            (ShootableMapObject *)p,
                                            distTo));
                                    }
                                }
                            }
                        }
                    }
                    is_ignored_ = selfState;
                    // NOTE: possible check for most dangerous weapon
                    // or object, if is one of objectives to destroy, for
                    // now only distance check
                    bool set_nxt_act = false;
                    if (hostiles_found_.empty()) {
                        if (smo_dist.empty())
                            aqt.state |= 8;
                        else {
                            Msmod_t::iterator it_msmod = smo_dist.begin();
                            Pairsmod_t closest = *it_msmod;
                            it_msmod++;
                            while (it_msmod != smo_dist.end()) {
                                if (it_msmod->second < closest.second) {
                                    closest = *it_msmod;
                                }
                                it_msmod++;
                            }
                            aqt.t_smo = closest.first;
                            aqt.state |= 4;
                            set_nxt_act = true;
                        }
                    } else {
                        Msmod_t::iterator it_msmod = hostiles_found_.begin();
                        Pairsmod_t closest = *it_msmod;
                        it_msmod++;
                        while (it_msmod != hostiles_found_.end()) {
                            if (it_msmod->second < closest.second) {
                                closest = *it_msmod;
                            }
                            it_msmod++;
                        }
                        aqt.t_smo = closest.first;
                        aqt.state |= 4;
                        set_nxt_act = true;
                    }
                    if (set_nxt_act) {
                        actionQueueType & aqt_l = it->actions[indx+1];
                        aqt_l.t_smo = aqt.t_smo;
                        aqt_l.state ^= 64;
                        it->main_act++;
                        if (obj_group_def_ == og_dmPolice) {
                            aqt_l.ot_execute |= PedInstance::ai_aWaitToStart;
                            aqt_l.multi_var.time_var.time_before_start = 5000;
                            // showing a gun
                            selectBestWeapon();
                            g_App.gameSounds().play(snd::PUTDOWN_WEAPON);
                            // TODO: create follow if target moves
                        }
                    }
                }
                if ((aqt.ot_execute & PedInstance::ai_aFindNonFriend) != 0)
                {
                    bool selfState = is_ignored_;
                    // TODO : check inside of vehicles, is_ignored_?
                    // NOTE : can be done as ai_aFindEnemy with objects
                    // passing within same group + def
                    Msmod_t nf_dist;
                    toDefineXYZ cur_xyz;
                    convertPosToXYZ(&cur_xyz);
                    cur_xyz.z += (size_z_ >> 1);
                    int num_peds = mission->numPeds();
                    int view_rng = sight_range_;
                    for (int i = 0; i < num_peds; i++) {
                        PedInstance *p = mission->ped(i);
                        if ((p->state_ & pa_smCheckExcluded) != 0)
                            continue;
                        if (!checkFriendIs(p) ) {
                            double distTo = 0;
                            if (//inSightRange((MapObject *)(p))
                                //&&
                                mission->inRangeCPos(
                                &cur_xyz,
                                (ShootableMapObject **)(&p),
                                NULL, false, false,
                                view_rng, &distTo)
                                == 1)
                            {
                                nf_dist.insert(Pairsmod_t(
                                    (ShootableMapObject *)p, distTo));
                            }
                        }
                    }
                    is_ignored_ = selfState;
                    if (nf_dist.empty())
                        aqt.state |= 8;
                    else {
                        Msmod_t::iterator it_msmod = nf_dist.begin();
                        Pairsmod_t closest = *it_msmod;
                        it_msmod++;
                        while (it_msmod != nf_dist.end()) {
                            if (it_msmod->second < closest.second) {
                                closest = *it_msmod;
                            }
                            it_msmod++;
                        }
                        aqt.t_smo = closest.first;
                        aqt.state |= 4;
                    }
                }
                if ((aqt.ot_execute & (PedInstance::ai_aWait
                    | PedInstance::ai_aWaitToStart)) == PedInstance::ai_aWait)
                {
                    // no failed or suspended will have "wait" action
                    if ((aqt.state & 60) == 36) {
                        if ((aqt.state & 128) != 0) {
                            aqt.multi_var.time_var.elapsed += elapsed;
                            if (aqt.multi_var.time_var.elapsed
                                >= aqt.multi_var.time_var.time_total)
                            {
                                aqt.state &= (65535 ^ 160);
                            }
                        } else
                            aqt.state |= 130;
                    } else
                        aqt.state &= (65535 ^ 32);
                }
                if ((aqt.ot_execute & PedInstance::ai_aNonFinishable) != 0)
                {
                }
                if (actions_property_ != 0)
                    break;

                if ((aqt.state & 256) != 0) {
                    if ((aqt.group_desc & PedInstance::gd_mExclusive) != 0)
                        break;
                    continue;
                } else if ((aqt.state & 32) != 0) {
                    if ((aqt.group_desc & PedInstance::gd_mExclusive) != 0)
                        it->state |= 2;
                    switchActionStateTo(aqt.as);
                    acts_g_prcssd |= aqt.group_desc;
                } else if ((aqt.state & 16) != 0) {
                    switchActionStateFrom(aqt.as);
                } else if ((aqt.state & 8) != 0) {
                    if ((aqt.state & 2) != 0)
                        switchActionStateFrom(aqt.as);
                    if ((aqt.group_desc & PedInstance::gd_mExclusive) != 0)
                        it->state |= 8;
                } else if ((aqt.state & 4) != 0) {
                    switchActionStateFrom(aqt.as);
                    acts_g_prcssd |= aqt.group_desc;
                } else if ((aqt.state & 2) != 0) {
                    if ((aqt.group_desc & PedInstance::gd_mExclusive) != 0)
                        it->state |= 2;
                    switchActionStateTo(aqt.as);
                    acts_g_prcssd |= aqt.group_desc;
                }
#ifdef _DEBUG
                else if ((aqt.state & 1) != 0) {
                    printf("should not get here, aqt.state = 1\n");
                }
#endif
                if ((aqt.group_desc & PedInstance::gd_mExclusive) != 0
                    && ((aqt.state & 12) == 0 || (aqt.state & 128) != 0))
                    break;
                if (acts_g_prcssd == PedInstance::gd_mAll)
                    break;
            }
            it->state = it->actions[it->main_act].state;

            if ((it->group_desc & PedInstance::gd_mExclusive) != 0
                && ((it->state & 12) == 0 || (it->state & 128) != 0))
                break;
            groups_processed |= acts_g_prcssd;

            if (groups_processed == PedInstance::gd_mAll
                || (groups_processed & PedInstance::gd_mExclusive))
                break;
        }
    }

    if (friends_not_seen_.size() != 0) {
        toDefineXYZ cur_xyz;
        convertPosToXYZ(&cur_xyz);
        cur_xyz.z += (size_z_ >> 1);
        bool selfState = is_ignored_;
        is_ignored_ = true;
        for (std::set<ShootableMapObject *>::iterator it
            = friends_not_seen_.begin(); it != friends_not_seen_.end(); it++)
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
        // TODO: find better way to bind state and animation, then
        // extending with "wait" (putdown/pickup, others)
    } else
        is_frame_drawn_ = true;
    if (handleDrawnAnim(elapsed))
        setActionStateToDrawnAnim();
    return updated;
}

#if 0
bool PedInstance::animate(int elapsed, Mission *mission) {

    bool updated = false;
    Weapon::WeaponAnimIndex weapon_idx =
        selectedWeapon() ? selectedWeapon()->index() : Weapon::Unarmed_Anim;

    if (health_ > 0 && isHostile()) {
        // find a weapon with ammo
        // TODO: weapon should not be drawn until enemy is in sight
        if (selectedWeapon() == NULL)
            selectNextWeapon();

        target_ = NULL;
        if (selectedWeapon()) {
            // for the moment, assume peds can only be hostile towards the agents
            for (int i = 0; i < 4; i++) {
                PedInstance * pinst = mission->ped(i);
                if (pinst->health() > 0
                    && selectedWeapon()->inRangeNoCP(
                        (ShootableMapObject **)(&pinst)) == 1) {
                    if (pinst->inVehicle())
                        setTarget(pinst->inVehicle());
                    else
                        setTarget(pinst);

                    resetDest_Speed();
                }
            }

            if (target_ == NULL){
                for (int i = 0; i < 4; i++) {
                    PedInstance * pinst = mission->ped(i);
                    if (pinst->health() > 0 && inSightRange(pinst)) {
                        if (dest_path_.size() == 0)
                            setDestinationP(mission, pinst->tileX(),
                                pinst->tileY(), pinst->tileZ());
                        break;
                    }
                }
            }
        } else {
            if (pickup_weapon_ && pickup_weapon_->map() == -1)
                pickup_weapon_ = 0;

            // find the closest weapon with ammo
            double closest = 9999;
            WeaponInstance *closest_w = 0;

            for (int i = 0; i < mission->numWeapons(); i++) {
                if (mission->weapon(i)->map() != -1
                        && mission->weapon(i)->ammoRemaining() > 0)
                {
                    // NOTE: this is too simple calculation for distance
                    // weapon can be close, but path to reach it can be longer
                    double d = distanceTo(mission->weapon(i));

                    if (d < 1024 && d < closest) {
                        closest = d;
                        closest_w = mission->weapon(i);
                    }
                }
            }

            if (pickup_weapon_ != closest_w && closest_w) {
                if (weapons_.size() > 7)
                    dropWeapon(0);

                pickupWeapon(closest_w);
            }
        }

        if (target_ == NULL)
            stopFiring();
    }
    return updated;
}
#endif

PedInstance *Ped::createInstance(int map) {
    return new PedInstance(this, map);
}

void PedInstance::kill() {
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

    if (agent_is_ == PedInstance::Agent_Non_Active)
        return;

    for (std::list<PathNode>::iterator it = dest_path_.begin();
            it != dest_path_.end(); it++) {
        PathNode & d = *it;
        int x =
            g_App.maps().tileToScreenX(map(), d.tileX(), d.tileY(),
                                       d.tileZ(), d.offX(), d.offY());
        int y =
            g_App.maps().tileToScreenY(map(), d.tileX(), d.tileY(),
                                       d.tileZ(), d.offX(), d.offY())
                                       - d.tileZ() * TILE_HEIGHT/3 + TILE_HEIGHT/3;

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
    agent_is_(PedInstance::Not_Agent)
{
    hold_on_.wayFree = 0;
    rcv_damage_def_ = MapObject::ddmg_Ped;
    major_type_ = MapObject::mjt_Ped;
    state_ = PedInstance::pa_smNone;
    actions_property_ = 0;
}

PedInstance::~PedInstance()
{
    delete ped_;
    ped_ = NULL;
}

void PedInstance::draw(int x, int y) {

    if (agent_is_ == PedInstance::Agent_Non_Active)
        return;

    Weapon::WeaponAnimIndex weapon_idx =
        selectedWeapon() ? selectedWeapon()->index() : Weapon::Unarmed_Anim;
    addOffs(x, y);

    // ensure on map
    if (x < 129 || y < 0 || map_ == -1)
        return;

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

    if (agent_is_ == PedInstance::Agent_Non_Active)
        return;

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
        WeaponInstance *wi = weapons_[selected_weapon_];
        if (wi->getMainType() == Weapon::EnergyShield)
            setRcvDamageDef(MapObject::ddmg_Ped);
        if (wi->getMainType() != Weapon::TimeBomb)
            wi->deactivate();
        if (wi->getWeaponType() == Weapon::AccessCard)
            rmEmulatedGroupDef(4, og_dmPolice);
        desc_state_ &= (pd_smAll ^ (pd_smArmed | pd_smNoAmmunition));
    }

    selected_weapon_ = n;
    if (n != -1) {
        WeaponInstance *wi = weapons_[selected_weapon_];
        if (wi->getMainType() == Weapon::EnergyShield)
            setRcvDamageDef(MapObject::ddmg_PedWithEnergyShield);
        if (wi->getMainType() != Weapon::TimeBomb)
            wi->activate();
        if (wi->getWeaponType() == Weapon::AccessCard)
            addEmulatedGroupDef(4, og_dmPolice);
        if (wi->doesPhysicalDmg()) {
            desc_state_ |= pd_smArmed;
            if (wi->ammoRemaining() == 0)
                desc_state_ |= pd_smNoAmmunition;
        }
    }
}

void PedInstance::selectNextWeapon() {

    if (selected_weapon_ != -1) {
        int nextWeapon = -1;
        WeaponInstance *curSelectedWeapon = (WeaponInstance *)weapon(selected_weapon_);

        if (curSelectedWeapon) {
            for (int i = 0; i < numWeapons(); i++) {
                WeaponInstance * wi = weapon(i);
                if (i != selected_weapon_ && wi->ammoRemaining()
                        && wi->getMainType()
                            == curSelectedWeapon->getMainType())
                {
                    if (nextWeapon == -1)
                        nextWeapon = i;
                    else {
                        if (wi->ammoRemaining()
                            < weapon(nextWeapon)->ammoRemaining())
                            nextWeapon = i;
                    }
                }
            }
        }

        if (nextWeapon == -1)
            selectBestWeapon();
        else
            setSelectedWeapon(nextWeapon);
    } else
        selectBestWeapon();
}

void PedInstance::selectBestWeapon() {
    // TODO: select weapon by type and type of damage + rank
    int bestWeapon = -1;
    int bestWeaponRank = -1;

    for (int i = numWeapons() - 1; i >=0; i--) {
        if (weapon(i)->ammoRemaining() && weapon(i)->rank() > bestWeaponRank) {
            bestWeapon = i;
            bestWeaponRank = weapon(i)->rank();
        }
    }

    if(bestWeapon != -1) {
        setSelectedWeapon(bestWeapon);
    }
}

void PedInstance::dropWeapon(int n) {
    assert(n >= 0 && n < (int) weapons_.size());

    dropWeapon(weapons_[n]);
}

void PedInstance::dropWeapon(WeaponInstance *wi) {
    // TODO: auto selection, when dropped weapon is "selected"?
    bool upd_selected = selected_weapon_ != -1;
    if (selectedWeapon() == wi) {
        setSelectedWeapon(-1);
        upd_selected = false;
    }
    for (int i = 0; i < (int)weapons_.size(); i++)
    {
        std::vector <WeaponInstance *>::iterator it = weapons_.begin() + i;
        if ((*it) == wi) {
            weapons_.erase(it);
            if (upd_selected && selected_weapon_ > i)
                selected_weapon_--;
            break;
        }
    }
    wi->setOwner(NULL);
    wi->setMap(map_);
    wi->setIsIgnored();
    wi->setPosition(tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
    wi->setVisZ(vis_z_);
    if (wi->getMainType() == Weapon::TimeBomb)
        wi->activate();
}

void PedInstance::dropAllWeapons() {

    setRcvDamageDef(MapObject::ddmg_Ped);
    setSelectedWeapon(-1);

    for (std::vector < WeaponInstance * >::iterator it
        = weapons_.begin(); it != weapons_.end(); it++)
    {
        WeaponInstance *w = *it;
        w->setMap(map());
        // TODO: drop weapons not on same place, stairs problem
        w->setPosition(tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
        w->setVisZ(vis_z_);
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

void PedInstance::putInVehicle(VehicleInstance * v) {
    assert(map_ != -1);
    in_vehicle_ = v;
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
#ifdef _DEBUG
    if (map_ == -1) {
        assert(in_vehicle_);
    }
#endif

    return map_;
}

bool PedInstance::walkable(int x, int y, int z) {
    return true;
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
    bool answer = true;
    switch (curanim) {
        case PedInstance::ad_HitAnim:
            if (frame_ < ped_->lastHitFrame(getDirection()))
                answer = false;
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
                if (agent_is_ == PedInstance::Agent_Active) {
                    setDrawnAnim(PedInstance::ad_DeadAgentAnim);
                } else {
                    setDrawnAnim(PedInstance::ad_NoAnimation);
                }
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
    else if (d->dtype == MapObject::dmg_Mental) {
        // TODO: check for required number of persuade points before applying
        setObjGroupDef((obj_group_def_ & 0xFFFFFF00) |
            (((PedInstance *)d->d_owner)->objGroupDef() & 0xFF));
        setObjGroupID(((PedInstance *)d->d_owner)->objGroupID());
        // TODO: inherit enemies
        enemy_group_defs_.clear();
        dropActQ();
        assert(d->d_owner != NULL);

        // adding following behavior for persuaded
        PedInstance::actionQueueGroupType as;
        as.group_desc = PedInstance::gd_mExclusive;
        createActQWait(as, 2000);
        as.main_act = as.actions.size() - 1;
        addActQToQueue(as);
        createActQFollowing(as,
            d->d_owner, 0, 192);
        as.main_act = as.actions.size() - 1;
        addActQToQueue(as);
        setDrawnAnim(PedInstance::ad_PersuadedAnim);
        desc_state_ |= pd_smControlled;
        friends_found_.clear();
        hostiles_found_.clear();
        // TODO: set default_actions_ to owners
        return true;
    }
    if (d->ddir != -1) {
        dir_ = (d->ddir + 128) % 256;
    }
    if (health_ <= 0) {
        health_ = -1;
        actions_property_ = 1;
        switchActionStateTo(PedInstance::pa_smDead);

        switch ((unsigned int)d->dtype) {
            case MapObject::dmg_Bullet:
                setDrawnAnim(PedInstance::ad_DieAnim);
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
                break;
        }
        if (numWeapons())
            dropAllWeapons();
        is_ignored_ = true;
    } else {
        // TODO: agent sometime can survive explosion, they need to walk burning?
        setDrawnAnim(PedInstance::ad_HitAnim);
    }
    return true;
}

void PedInstance::destroyAllWeapons() {
    setSelectedWeapon(-1);
    while (weapons_.size()) {
        WeaponInstance * w = removeWeapon(0);
        w->setMap(-1);
        w->setPosition(tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
        w->setVisZ(vis_z_);
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
        // TODO: add this check later, create a list of all in vehicle
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
            friends_not_seen_.insert(p);
        return true;
    }
    if (friend_group_defs_.find(p->objGroupDef()) !=
        friend_group_defs_.end())
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
        it != hostiles_found_.end(); it++)
    {
        ShootableMapObject *smo = it->first;
        double distTo = 0;
        if (smo->health() <= 0 || (smo->majorType() == MapObject::mjt_Ped
            && checkFriendIs((PedInstance *)(smo)))
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

void PedInstance::createActQStanding(actionQueueGroupType &as) {
    as.state = 1;
    as.actions.clear();
    actionQueueType aq;
    aq.ot_execute = PedInstance::ai_aWait;
    aq.as = PedInstance::pa_smStanding;
    aq.state = 1;
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_total = -1;
    aq.group_desc = PedInstance::gd_mStandWalk;
    as.actions.push_back(aq);
}

void PedInstance::createActQWalking(actionQueueGroupType &as, PathNode *tpn,
    ShootableMapObject *tsmo, int32 dir, int32 dist)
{
    as.state = 1;
    as.actions.clear();
    actionQueueType aq;
    aq.as = PedInstance::pa_smWalking;
    aq.ot_execute = PedInstance::ai_aReachLocation;
    aq.group_desc = PedInstance::gd_mStandWalk;
    aq.state = 1;
    aq.multi_var.dist_var.dir = dir;
    aq.multi_var.dist_var.dist = dist;
    if (dir == -1) {
        if (tpn) {
            aq.t_pn = *tpn;
            aq.condition = 0;
        } else {
            aq.t_smo = tsmo;
            aq.condition = 2;
        }
    } else {
        aq.condition = 1;
    }
    as.actions.push_back(aq);
}

void PedInstance::createActQHit(actionQueueGroupType &as, PathNode *tpn,
    int32 dir)
{
    as.state = 1;
    as.actions.clear();
    actionQueueType aq;
    aq.as = PedInstance::pa_smHit;
    aq.ot_execute = PedInstance::ai_aReachLocation;
    aq.group_desc = PedInstance::gd_mExclusive;
    aq.state = 1;
    // TODO: set directional movement to
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
    as.actions.clear();
    actionQueueType aq;
    aq.group_desc = PedInstance::gd_mFire;
    bool can_shoot = false;
    bool does_phys_dmg = false;
    if (pw_to_use) {
        switch (pw_to_use->desc) {
            // indx used
            // TODO: use it later, convert indx to weapon pointer
            case 1:
                break;
            // pointer
            case 2:
                can_shoot = pw_to_use->wpn.wi->canShoot();
                does_phys_dmg = pw_to_use->wpn.wi->doesPhysicalDmg();
                break;
            // weapon type
            // TODO: create a way for checking can_shoot and does_phys_dmg
            case 3:
                break;
            // strict damage type
            case 4:
            // non-strict damage type
            case 5:
                // TODO: check inventory for shootable weapons? or check in
                // action queue processing? for availiable, search the ground?
                can_shoot = true;
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
        if (tsmo) {
            aq.t_smo = tsmo;
            aq.ot_execute = PedInstance::ai_aDestroyObject;
        } else {
            aq.t_pn = *tpn;
            aq.ot_execute = PedInstance::ai_aAttackLocation;
            aq.t_smo = NULL;
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
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_total = 125;
    as.actions.push_back(aq);
    return true;
}

void PedInstance::createActQFollowing(actionQueueGroupType &as,
    ShootableMapObject *tsmo, uint32 condition, int32 dist)
{
    as.state = 1;
    as.actions.clear();
    actionQueueType aq;
    aq.as = PedInstance::pa_smFollowing;
    aq.ot_execute = PedInstance::ai_aFollowObject;
    aq.group_desc = PedInstance::gd_mStandWalk;
    aq.state = 1;
    aq.t_smo = tsmo;
    aq.multi_var.dist_var.dist = dist;
    aq.condition = condition;
    as.actions.push_back(aq);
}

void PedInstance::createActQPickUp(actionQueueGroupType &as,
    ShootableMapObject *tsmo)
{
    as.state = 1;
    as.actions.clear();
    actionQueueType aq;
    aq.as = PedInstance::pa_smWalking;
    aq.ot_execute = PedInstance::ai_aReachLocation;
    aq.group_desc = PedInstance::gd_mStandWalk;
    aq.state = 1;
    aq.t_smo = tsmo;
    aq.multi_var.dist_var.dir = -1;
    aq.multi_var.dist_var.dist = 0;
    aq.condition = 2;
    as.actions.push_back(aq);
    aq.as = PedInstance::pa_smPickUp;
    aq.ot_execute = PedInstance::ai_aPickUpObject;
    aq.ot_execute |= PedInstance::ai_aWait;
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_total = 500;
    aq.group_desc = PedInstance::gd_mExclusive;
    as.actions.push_back(aq);
}

void PedInstance::createActQPutDown(actionQueueGroupType &as,
    ShootableMapObject *tsmo)
{
    as.state = 1;
    as.actions.clear();
    actionQueueType aq;
    aq.as = PedInstance::pa_smPutDown;
    aq.ot_execute = PedInstance::ai_aPutDownObject;
    aq.group_desc = PedInstance::gd_mExclusive;
    aq.state = 1;
    aq.t_smo = tsmo;
    aq.ot_execute |= PedInstance::ai_aWait;
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_total = 500;
    as.actions.push_back(aq);
}

void PedInstance::createActQBurning(actionQueueGroupType &as) {
    as.state = 1;
    as.actions.clear();
    actionQueueType aq;
    aq.as = PedInstance::pa_smBurning;
    aq.ot_execute = PedInstance::ai_aReachLocation;
    aq.multi_var.dist_var.dir = rand() % 256;
    aq.multi_var.dist_var.dist = rand() % 256 + 128;
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
    aq.group_desc = PedInstance::gd_mExclusive;
    as.actions.push_back(aq);
}

void PedInstance::createActQGetInCar(actionQueueGroupType &as,
    ShootableMapObject *tsmo)
{
    as.state = 1;
    as.actions.clear();
    actionQueueType aq;
    aq.as = PedInstance::pa_smWalking;
    aq.ot_execute = PedInstance::ai_aReachLocation;
    aq.state = 1;
    aq.t_smo = tsmo;
    aq.condition = 2;
    aq.multi_var.dist_var.dist = 0;
    aq.multi_var.dist_var.dir = -1;
    aq.group_desc = PedInstance::gd_mStandWalk;
    as.actions.push_back(aq);
    // TODO: change this use enum
    aq.condition = 0;
    aq.as = PedInstance::pa_smGetInCar;
    aq.group_desc = PedInstance::gd_mExclusive;
    aq.ot_execute = PedInstance::ai_aAquireControl;
    as.actions.push_back(aq);
}

void PedInstance::createActQUsingCar(actionQueueGroupType &as, PathNode *tpn,
    ShootableMapObject *tsmo, uint32 condition)
{
}

void PedInstance::createActQInCar(actionQueueGroupType &as, PathNode *tpn,
    ShootableMapObject *tsmo, uint32 condition)
{
}

void PedInstance::createActQLeaveCar(actionQueueGroupType &as,
        ShootableMapObject *tsmo)
{
    as.state = 1;
    as.actions.clear();
    actionQueueType aq;
    aq.as = PedInstance::pa_smLeaveCar;
    aq.ot_execute = PedInstance::ai_aLoseControl;
    aq.state = 1;
    aq.t_smo = tsmo;
    aq.group_desc = PedInstance::gd_mExclusive;
    as.actions.push_back(aq);
}

void PedInstance::createActQWait(actionQueueGroupType &as, int tm_wait)
{
    as.state = 1;
    as.actions.clear();
    actionQueueType aq;
    aq.as = PedInstance::pa_smNone;
    aq.group_desc = PedInstance::gd_mExclusive;
    aq.state = 1 | 4 | 32 | 128;
    aq.ot_execute = PedInstance::ai_aWait;
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_total = tm_wait;
    as.actions.push_back(aq);
}

void PedInstance::createActQFindEnemy(actionQueueGroupType &as) {
    as.state = 1;
    as.actions.clear();
    actionQueueType aq;
    aq.as = PedInstance::pa_smNone;
    aq.group_desc = PedInstance::gd_mThink;
    aq.state = 1;
    aq.ot_execute = PedInstance::ai_aFindEnemy;
    as.actions.push_back(aq);
    aq.as = PedInstance::pa_smFiring;
    aq.group_desc = PedInstance::gd_mFire;
    aq.state = 65;
    aq.ot_execute = PedInstance::ai_aDestroyObject;
    aq.ot_execute |= PedInstance::ai_aWait;
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_total = 125;
    aq.multi_var.enemy_var.make_shots = 0;
    aq.multi_var.enemy_var.shots_done = 0;
    aq.multi_var.enemy_var.weapon.desc = 4;
    aq.multi_var.enemy_var.weapon.wpn.dmg_type = MapObject::dmg_Physical;
    aq.multi_var.enemy_var.forced_shot = false;
    as.actions.push_back(aq);
}


bool PedInstance::setActQInQueue(actionQueueGroupType &as,
    uint32 * id)
{
    // NOTE: if action is invalidated all remaining actions in queue are
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
                        it_a->ot_execute &= ~(PedInstance::ai_aDestroyObject
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


