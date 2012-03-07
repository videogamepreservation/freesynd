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
#include "pathsurfaces.h"
#include <math.h>

//#define NEW_ANIMATE_HANDLING

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
            action_state_ &= (pa_smAll ^(pa_smFollowing | pa_smFiring
                | pa_smUsingCar | pa_smInCar));
            action_state_ |= pa_smStanding;
            break;
        case pa_smWalking:
            action_state_ &= (pa_smAll ^(pa_smFollowing | pa_smFiring
                | pa_smUsingCar | pa_smInCar));
            action_state_ |= pa_smWalking;
            break;
        case pa_smHit:
            action_state_ = pa_smHit;
            break;
        case pa_smFiring:
            action_state_ |= pa_smFiring;
            break;
        case pa_smFollowing:
            action_state_ &= (pa_smAll ^(pa_smStanding | pa_smWalking
                | pa_smFiring | pa_smUsingCar | pa_smInCar));
            action_state_ |= pa_smFollowing;
            break;
        case pa_smPickUp:
            action_state_ = pa_smPickUp;
            break;
        case pa_smPutDown:
            action_state_ = pa_smPutDown;
            break;
        case pa_smBurning:
            action_state_ = pa_smBurning;
            break;
        case pa_smGetInCar:
            action_state_ = pa_smStanding | pa_smGetInCar;
            break;
        case pa_smUsingCar:
            action_state_ = pa_smStanding | pa_smUsingCar;
            break;
        case pa_smInCar:
            action_state_ = pa_smStanding | pa_smInCar;
            break;
        case pa_smLeaveCar:
            action_state_ = pa_smStanding | pa_smLeaveCar;
            break;
        case pa_smDead:
            action_state_ = pa_smDead;
            break;
        case pa_smUnavailable:
            action_state_ = pa_smUnavailable;
            break;
    }
}

void PedInstance::switchActionStateFrom(uint32 as) {
    switch(as) {
        case pa_smNone:
            //printf("Ped has undefined state");
            break;
        case pa_smStanding:
            action_state_ &= pa_smAll ^ pa_smStanding;
            break;
        case pa_smWalking:
            action_state_ &= pa_smAll ^ pa_smWalking;
            action_state_ |= pa_smStanding;
            break;
        case pa_smHit:
            action_state_ &= pa_smAll ^ pa_smHit;
            break;
        case pa_smFiring:
            action_state_ &= pa_smAll ^ pa_smFiring;
            break;
        case pa_smFollowing:
            action_state_ &= pa_smAll ^ pa_smFollowing;
            break;
        case pa_smPickUp:
        case pa_smPutDown:
            action_state_ = pa_smStanding;
            break;
        case pa_smBurning:
            action_state_ &= pa_smAll ^ pa_smBurning;
            break;
        case pa_smGetInCar:
            action_state_ &= pa_smAll ^ (pa_smStanding | pa_smGetInCar);
            break;
        case pa_smUsingCar:
            action_state_ &= pa_smAll ^ (pa_smStanding | pa_smUsingCar);
            break;
        case pa_smInCar:
            action_state_ &= pa_smAll ^ (pa_smStanding | pa_smInCar);
            break;
        case pa_smLeaveCar:
            action_state_ = pa_smStanding;
            break;
        case pa_smDead:
            action_state_ = pa_smDead;
            printf("It's alive!");
            break;
        case pa_smUnavailable:
            action_state_ = pa_smUnavailable;
            break;
    }
}

void PedInstance::setActionStateToDrawnAnim(void) {
    // TODO: complete
    if ((action_state_ & pa_smUnavailable) != 0) {
        setDrawnAnim(PedInstance::ad_NoAnimation);
    } else if ((action_state_ & pa_smDead) != 0) {
        //setDrawnAnim(PedInstance::ad_DeadAnim);
    } else if ((action_state_ & pa_smWalking) != 0) {
        setDrawnAnim(PedInstance::ad_WalkAnim);
    } else if ((action_state_ & pa_smStanding) != 0) {
        setDrawnAnim(PedInstance::ad_StandAnim);
    } else if ((action_state_ & pa_smPickUp) != 0) {
        setDrawnAnim(PedInstance::ad_PickupAnim);
    } else if ((action_state_ & pa_smPutDown) != 0) {
        setDrawnAnim(PedInstance::ad_PutdownAnim);
    }
}

#ifdef NEW_ANIMATE_HANDLING
bool PedInstance::animate(int elapsed, Mission *mission) {

    if (agent_is_ == PedInstance::Agent_Non_Active)
        return true;

    bool updated = false;
    Weapon::WeaponAnimIndex weapon_idx =
        selectedWeapon() ? selectedWeapon()->index() : Weapon::Unarmed_Anim;

    // NOTE: some actions have remaining time, it is lost for now
    if (actions_queue_.empty()) {
        // TODO: use default_actions_ to fill it up
    } else {
        // TODO: xor finished and failed
        uint32 groups_processed = 0;
        uint32 groups_max = 7;
        for (std::vector <actionQueueGroupType>::iterator it =
            actions_queue_.begin(); it != actions_queue_.end(); it++)
        {
            if ((it->state & 12) != 0)
                continue;
            if ((it->group_desc & groups_processed) != 0)
                break;
            if ((it->group_desc & PedInstance::gd_mExclusive) != 0
                && groups_processed != 0)
                break;
            
            // TODO: xor finished and failed
            uint32 acts_g_prcssd = 0;
            for (uint32 indx = 0; indx < it->actions.size(); indx++)
            {
                actionQueueType & aqt = it->actions[indx];
                if ((acts_g_prcssd & aqt.group_desc) != 0
                    || (acts_g_prcssd & groups_processed) != 0)
                    break;
                if ((aqt.state & 12) != 0)
                    continue;
                if ((aqt.group_desc & PedInstance::gd_mExclusive) != 0
                    && acts_g_prcssd != 0)
                    break;
                if ((aqt.ot_execute & Mission::objv_None) != 0)
                {
                    printf("obj_None");
                }
                if ((aqt.ot_execute & Mission::objv_AquireControl) != 0)
                {
                    if (aqt.t_smo->majorType() == MapObject::mjt_Ped) {
                        WeaponInstance *wi = selectedWeapon();
                        if (wi && wi->getMainType()
                            == Weapon::Persuadatron)
                        {
                            // TODO: proper handling for time, add condition
                            // of failure to inflict damage
                            if (aqt.state == 1)
                                aqt.state |= 2;
                            int tm_left = elapsed;
                            wi->inflictDamage(aqt.t_smo, NULL, &tm_left);
                            if (checkFriendIs((PedInstance *)aqt.t_smo))
                                aqt.state |= 4;
                        } else {
                            aqt.state |= 8;
                        }
                    } else if (aqt.t_smo->majorType()
                        == MapObject::mjt_Vehicle)
                    {
                        VehicleInstance *v = (VehicleInstance *)aqt.t_smo;
                        if (v->health() > 0 && (action_state_
                            & (PedInstance::pa_smInCar
                            | PedInstance::pa_smUsingCar)) == 0)
                        {
                            if (aqt.condition == 0) {
                                v->setDriver(this);
                                if (v->isInsideVehicle(this)) {
                                    in_vehicle_ = v;
                                    aqt.state |= 4;
                                    is_ignored_ = true;
                                    map_ = -1;
                                    action_state_ |= pa_smInCar;
                                } else {
                                    aqt.state |= 8;
                                }
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
                                    action_state_ |= pa_smUsingCar;
                                }
                            }
                        } else
                            aqt.state |= 8;
                    }
                }
                if ((aqt.ot_execute & Mission::objv_LoseControl) != 0)
                {
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
                if ((aqt.ot_execute & Mission::objv_PickUpObject) != 0)
                {
                    if (aqt.state == 1) {
                        WeaponInstance *wi = (WeaponInstance *)aqt.t_smo;
                        if (wi->hasOwner() || weapons_.size() == 8)
                            aqt.state |= 8;
                        else {
                            wi->setOwner(this);
                            wi->setMap(-1);
                            wi->setIsIgnored(true);
                            wi->deactivate();
                            weapons_.push_back(wi);
                            //aqt.state |= 4;
                        }
                    }
                }
                if ((aqt.ot_execute & Mission::objv_DestroyObject) != 0)
                {
                    if (aqt.t_smo->health() <= 0)
                        // not object did it as such he failed,
                        // but goal reached
                        aqt.state |= 12;
                    else {
                        WeaponInstance *wi = selectedWeapon();
                        if (!wi)
                            selectBestWeapon();
                        wi = selectedWeapon();
                        if (wi && (wi->shotProperty()
                            & Weapon::spe_DamageAll) != 0)
                        {
                            // TODO: set ignoreblocker properly,
                            // more info from weapon
                            int tm_left = elapsed;
                            wi->inflictDamage(aqt.t_smo, NULL, &tm_left);
                            // TODO: handle correctly
                            if (aqt.t_smo->health() <= 0)
                                aqt.state |= 4;
                            else
                                aqt.state |= 2;
                        } else
                            aqt.state |= 8;
                    }
                }
                if ((aqt.ot_execute & Mission::objv_UseObject) != 0)
                {
                }
                if ((aqt.ot_execute & Mission::objv_PutDownObject) != 0)
                {
                    if (aqt.state == 1) {
                        WeaponInstance *wi = (WeaponInstance *)aqt.t_smo;
                        if (wi->getOwner() != this)
                            aqt.state |= 8;
                        else {
                            dropWeapon(wi);
                            //aqt.state |= 4;
                        }
                    }
                }
                if ((aqt.ot_execute & Mission::objv_ReachLocation) != 0)
                {
                    if (aqt.state == 1) {
                        //TODO: IPA + mods
                        int speed_set = 128;// aqt.multi_var.dist_var.speed
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
                                    aqt.t_pn.offX(), aqt.t_pn.offY(),
                                    speed_set);
                                if (dest_path_.empty())
                                    aqt.state |= 8;
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
                            }
                            if (set_new_dest) {
                                aqt.state |= 2;
                                setDestinationP(mission,
                                    aqt.t_smo->tileX(), aqt.t_smo->tileY(),
                                    aqt.t_smo->tileZ(), aqt.t_smo->offX(),
                                    aqt.t_smo->offY(), speed_set);
                                if (dest_path_.empty())
                                    aqt.state |= 8;
                            } else {
                                aqt.state |= 4;
                            }
                        }
                    }
                    if ((aqt.state & 15) == 3) {
                        //TODO: IPA + mods
                        int speed_set = 128;
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
                if ((aqt.ot_execute & Mission::objv_FollowObject) != 0)
                {
                    if (aqt.state == 1 || aqt.state == 17) {
                        int speed_set = 128;
                        if (aqt.condition == 0) {
                            bool set_new_dest = true;
                            dist_to_pos_ = aqt.multi_var.dist_var.dist;
                            int dist_is = -1;
                            dist_is = (int)distanceTo(
                                (MapObject *)aqt.t_smo);
                            if (dist_is <= dist_to_pos_)
                                set_new_dest = false;
                            if (set_new_dest) {
                                aqt.state |= 2;
                                setDestinationP(mission,
                                    aqt.t_smo->tileX(), aqt.t_smo->tileY(),
                                    aqt.t_smo->tileZ(), aqt.t_smo->offX(),
                                    aqt.t_smo->offY(), speed_set);
                                if (dest_path_.empty())
                                    aqt.state |= 8;
                                if ((aqt.state & 16) != 0)
                                    aqt.state ^= 16;
                            } else {
                                aqt.state |= 16;
                            }
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
                                        aqt.t_smo->offY(), speed_set);
                                    if (dest_path_.empty())
                                        aqt.state |= 8;
                                    if ((aqt.state & 16) != 0)
                                        aqt.state ^= 16;
                                } else {
                                    aqt.state |= 16;
                                }
                            } else {
                                aqt.state |= 8;
                            }
                        }
                    }
                    if ((aqt.state & 30) == 2) {
                        int speed_set = 128;
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
                                    dest_path_.clear();
                                    speed_ = 0;
                                    aqt.state ^= 2;
                                }
                            }
                        } else if (aqt.condition == 1) {
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
                                        dest_path_.clear();
                                        speed_ = 0;
                                        aqt.state ^= 2;
                                    }
                                }
                            } else {
                                aqt.state |= 8;
                            }
                        }
                    }
                }
                if ((aqt.ot_execute & Mission::objv_Wait) != 0)
                {
                    if (aqt.state == 1) {
                        aqt.multi_var.time_var.elapsed = 0;
                        aqt.state |= 2;
                    }
                    if (aqt.state == 3) {
                        aqt.multi_var.time_var.elapsed += elapsed;
                        if (aqt.multi_var.time_var.elapsed
                            >= aqt.multi_var.time_var.time_total)
                            aqt.state |= 4;
                    }
                }
                if ((aqt.ot_execute & Mission::objv_AttackLocation) != 0)
                {
                    // TODO: additional conditions, single shot?
                    WeaponInstance *wi = selectedWeapon();
                    if (!wi)
                        selectBestWeapon();
                    if (wi && (wi->shotProperty()
                        & Weapon::spe_DamageAll) != 0)
                    {
                        // TODO: proper handling, ignoreblocker,
                        // more info from weapon needed
                        int tm_left = elapsed;
                        wi->inflictDamage(NULL, &aqt.t_pn, &tm_left);
                        aqt.state |= 2;
                    } else
                        aqt.state |= 8;
                }
                if ((aqt.ot_execute & Mission::objv_FindEnemy) != 0)
                {
                    // TODO: check inside of vehicles too, is_ignored_?
                    if (!hostiles_found_.empty())
                        verifyHostilesFound(mission);
                    Msmod_t smo_dist;
                    if (hostiles_found_.empty()) {
                        // TODO: check for weapons, get the largest shooting
                        // range and put it here
                        int shot_rng = 1;
                        int view_rng = (sight_range_ << 8);
                        toDefineXYZ cur_xyz;
                        convertPosToXYZ(&cur_xyz);
                        cur_xyz.z += (size_z_ >> 1);
                        if (obj_group_def_ == og_dmAgent
                            || obj_group_def_ == og_dmPolice
                            || obj_group_def_ == og_dmGuard)
                        {
                            int num_peds = mission->numPeds();
                            for (int i = 0; i < num_peds; i++) {
                                PedInstance *p = mission->ped(i);
                                if ((actionStateMasks() &
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
                                            // TODO: inrange check here might
                                            // reduce speed, check
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
                                            it_s++;
                                        } while (it_s != it_e);
                                    }
                                } else if (checkHostileIs(p) ) {
                                    // TODO: hostile_desc_alt to checkHostileIs?
                                    double distTo = 0;
                                    if (//inSightRange((MapObject *)(p)) &&
                                        // TODO: set ignoreblocker based
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
                                if ((actionStateMasks() &
                                    pa_smCheckExcluded) != 0
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
                    // NOTE: possible check for most dangerous weapon
                    // or object, if is one of objectives to destroy, for
                    // now only distance check
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
                            }
                            aqt.t_smo = closest.first;
                            aqt.state |= 4;
                        }
                    } else {
                        Msmod_t::iterator it_msmod = hostiles_found_.begin();
                        Pairsmod_t closest = *it_msmod;
                        it_msmod++;
                        while (it_msmod != hostiles_found_.end()) {
                            if (it_msmod->second < closest.second) {
                                closest = *it_msmod;
                            }
                        }
                        aqt.t_smo = closest.first;
                        aqt.state |= 4;
                    }
                }
                if ((aqt.ot_execute & Mission::objv_FindNonFriend) != 0)
                {
                    // TODO : check inside of vehicles, is_ignored_?
                    // NOTE : can be done as objv_FindEnemy with objects
                    // passing within same group + def
                    Msmod_t nf_dist;
                    toDefineXYZ cur_xyz;
                    convertPosToXYZ(&cur_xyz);
                    cur_xyz.z += (size_z_ >> 1);
                    int num_peds = mission->numPeds();
                    int view_rng = (sight_range_ << 8);
                    for (int i = 0; i < num_peds; i++) {
                        PedInstance *p = mission->ped(i);
                        if ((actionStateMasks() &
                            pa_smCheckExcluded) != 0)
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
                        }
                        aqt.t_smo = closest.first;
                        aqt.state |= 4;
                    }
                }
                /*
                if ((aqt.ot_execute & Mission::objv_ExecuteObjective) != 0)
                {
                }
                if ((aqt.ot_execute & Mission::objv_ExecuteObjectiveEnd) != 0)
                {
                }
                */
                if ((aqt.ot_execute & Mission::objv_NonFinishable) != 0)
                {
                }
                if ((aqt.state & 16) != 0) {
                } else if ((aqt.state & 8) != 0) {
                } else if ((aqt.state & 4) != 0) {
                    switchActionStateFrom(aqt.as);
                    acts_g_prcssd |= aqt.group_desc;
                } else if ((aqt.state & 2) != 0) {
                    switchActionStateTo(aqt.as);
                    acts_g_prcssd |= aqt.group_desc;
                } else if ((aqt.state & 1) != 0) {
                    printf("should not get here");
                }
                if ((aqt.group_desc & PedInstance::gd_mExclusive) != 0
                    && (aqt.state & 12) == 0)
                    break;
                if (acts_g_prcssd == groups_max)
                    break;
            }
            it->state = it->actions[it->main_act].state;
            if ((it->group_desc & PedInstance::gd_mExclusive) != 0
                && (it->state & 12) == 0)
                break;
            groups_processed |= acts_g_prcssd;

            if (groups_processed == groups_max)
                break;
        }
    }
    handleDrawnAnim(elapsed);
    setActionStateToDrawnAnim();
    updated = MapObject::animate(elapsed);
    return updated;
}
#endif
#ifndef NEW_ANIMATE_HANDLING
bool PedInstance::animate(int elapsed, Mission *mission) {

    if (agent_is_ == PedInstance::Agent_Non_Active)
        return true;

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

    if (in_vehicle_) {
        if (map_ == -1) {
        } else if (samePosition(in_vehicle_)) {
            map_ = -1;
            in_vehicle_->setDriver(this);
            is_ignored_ = true;
            return true;
        } else {
            if(health_ > 0) {
                if(dest_path_.empty()) {
                    setDestinationP(mission ,in_vehicle_->tileX(),
                        in_vehicle_->tileY(), in_vehicle_->tileZ(),
                        in_vehicle_->offX(), in_vehicle_->offY(), 320);
                } else {
                    PathNode &pn = dest_path_.back();
                    if(pn.tileX() != in_vehicle_->tileX()
                        || pn.tileY() != in_vehicle_->tileY()
                        || pn.tileZ() != in_vehicle_->tileZ()
                        || pn.offX() != in_vehicle_->offX()
                        || pn.offY() != in_vehicle_->offY())
                        setDestinationP(mission ,in_vehicle_->tileX(),
                            in_vehicle_->tileY(), in_vehicle_->tileZ(),
                            in_vehicle_->offX(), in_vehicle_->offY(), 320);
                }
            }
        }
    }

    if (pickup_weapon_ && pickup_weapon_->map() == -1) {
        pickup_weapon_ = NULL;
        resetDest_Speed();
    }

    updated = MapObject::animate(elapsed);
    PedInstance::AnimationDrawn curanim = drawnAnim();
    switch (curanim) {
        case PedInstance::ad_HitAnim:
            if (frame_ > ped_->lastHitFrame(getDirection())) {
                if(speed_) {
                    setDrawnAnim(PedInstance::ad_WalkAnim);
                } else
                    setDrawnAnim(PedInstance::ad_StandAnim);
            } else 
                if (health_ <= 0)
                    return updated;
            if (health_ > 0)
                break;
        case PedInstance::ad_DieAnim:
            if (frame_ <= ped_->lastDieFrame())
                return updated;
            setDrawnAnim(PedInstance::ad_DeadAnim);
            return true;
            break;
        case PedInstance::ad_DeadAnim:
            return false;
            break;
        case PedInstance::ad_DeadAgentAnim:
            return false;
            break;
        case PedInstance::ad_PickupAnim:
        case PedInstance::ad_PutdownAnim:
            if (frame_ > ped_->lastPickupFrame()) {
                if(speed_) {
                    setDrawnAnim(PedInstance::ad_WalkAnim);
                } else
                    setDrawnAnim(PedInstance::ad_StandAnim);
            } else
                return updated;
            break;
        case PedInstance::ad_WalkAnim:
            break;
        case PedInstance::ad_StandAnim:
            break;
        case PedInstance::ad_WalkFireAnim:
            if(frame_ > ped_->lastWalkFireFrame(getDirection(), weapon_idx)) {
                if (speed_) {
                    setDrawnAnim(PedInstance::ad_WalkAnim);
                } else
                    setDrawnAnim(PedInstance::ad_StandAnim);
            }
            break;
        case PedInstance::ad_StandFireAnim:
            if(frame_ > ped_->lastStandFireFrame(getDirection(), weapon_idx)) {
                if (speed_) {
                    setDrawnAnim(PedInstance::ad_WalkAnim);
                } else
                    setDrawnAnim(PedInstance::ad_StandAnim);
            }
            break;
        case PedInstance::ad_VaporizeAnim:
            if (frame_ > ped_->lastVaporizeFrame(getDirection())) {
                if (agent_is_ == PedInstance::Agent_Active) {
                    setDrawnAnim(PedInstance::ad_DeadAgentAnim);
                } else {
                    setDrawnAnim(PedInstance::ad_NoAnimation);
                }
            }
            return updated;
        case PedInstance::ad_SinkAnim:
            // TODO: use this in future
            break;
        case PedInstance::ad_WalkBurnAnim:
            updated |= movementP(mission, elapsed);
        case PedInstance::ad_StandBurnAnim:
            if (leftTimeShowAnim(elapsed))
                return updated;
            setDrawnAnim(PedInstance::ad_DieBurnAnim);
            return updated;
        case PedInstance::ad_DieBurnAnim:
            if (frame_ > ped_->lastDieBurnFrame()) {
                setDrawnAnim(PedInstance::ad_SmokeBurnAnim);
                setTimeShowAnim(7000);
            }
            return updated;
        case PedInstance::ad_SmokeBurnAnim:
            if (leftTimeShowAnim(elapsed))
                return updated;
            setDrawnAnim(PedInstance::ad_DeadBurnAnim);
            return updated;
        case PedInstance::ad_DeadBurnAnim:
            return updated;
        case PedInstance::ad_PersuadedAnim:
            if (frame_ > ped_->lastPersuadeFrame()) {
                setDrawnAnim(PedInstance::ad_StandAnim);
            }
            return updated;
        case PedInstance::ad_NoAnimation:
            return updated;
    }

    if (curanim != PedInstance::ad_HitAnim
        && curanim != PedInstance::ad_PickupAnim
        && curanim != PedInstance::ad_PutdownAnim
        && curanim != PedInstance::ad_StandFireAnim)
    {
        updated |= movementP(mission, elapsed);
    }

    if (putdown_weapon_) {
        WeaponInstance *w = putdown_weapon_;
        w->setMap(map());
        w->setPosition(tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
        w->setVisZ(vis_z_);
        w->setOwner(NULL);
        w->setIsIgnored();
        w->activate();
        putdown_weapon_ = NULL;
        setDrawnAnim(PedInstance::ad_PutdownAnim);
        if(speed() != 0){
            resetDest_Speed();
        }
        return true;
    }

    if (pickup_weapon_) {
        if (samePosition(pickup_weapon_)) {
            weapons_.push_back(pickup_weapon_);
            pickup_weapon_->setMap(-1);
            pickup_weapon_->setOwner(this);
            pickup_weapon_->setIsIgnored(true);
            pickup_weapon_->deactivate();
            pickup_weapon_ = NULL;
            setDrawnAnim(PedInstance::ad_PickupAnim);
            return true;
        } else {
            if(health_ > 0) {
                if(dest_path_.empty()) {
                    setDestinationP(mission, pickup_weapon_->tileX(),
                        pickup_weapon_->tileY(), pickup_weapon_->tileZ(),
                        pickup_weapon_->offX(), pickup_weapon_->offY(), 320);
                } else {
                    PathNode &pn = dest_path_.back();
                    if(pn.tileX() != pickup_weapon_->tileX()
                        || pn.tileY() != pickup_weapon_->tileY()
                        || pn.tileZ() != pickup_weapon_->tileZ()
                        || pn.offX() != pickup_weapon_->offX()
                        || pn.offY() != pickup_weapon_->offY())
                        setDestinationP(mission, pickup_weapon_->tileX(),
                            pickup_weapon_->tileY(), pickup_weapon_->tileZ(),
                            pickup_weapon_->offX(), pickup_weapon_->offY(), 320);
                }
            }
        }
    }

    if (target_ && health_ > 0 && firing_ == PedInstance::Firing_Not) {    
        if (target_->majorType() == MapObject::mjt_Ped
            && checkFriendIs((PedInstance *)target_))
            target_ = NULL;
        if (target_ && target_->health() > 0) {
            if (selectedWeapon()
                && selectedWeapon()->inflictDamage(target_, NULL))
            {
                firing_ = PedInstance::Firing_Fire;
                updated = true;

            } else {
                stopFiring();
            }
        } else {
            target_ = NULL;
        }
    }

    if (weapon_idx == Weapon::Unarmed_Anim) {
        firing_ = PedInstance::Firing_Not;
        if (speed_) {
            setDrawnAnim(PedInstance::ad_WalkAnim);
        } else
            setDrawnAnim(PedInstance::ad_StandAnim);
    }

    if (health_ > 0) {
        updated = true;
        if (firing_ == PedInstance::Firing_Not) {
            if (curanim != PedInstance::ad_HitAnim
                && curanim != PedInstance::ad_StandFireAnim
                && curanim != PedInstance::ad_WalkFireAnim) {
                if (speed_) {
                    setDrawnAnim(PedInstance::ad_WalkAnim);
                } else
                    setDrawnAnim(PedInstance::ad_StandAnim);
            }
        } else if (firing_ == PedInstance::Firing_Fire) {
            if (speed_) {
                setDrawnAnim(PedInstance::ad_WalkFireAnim);
            } else
                setDrawnAnim(PedInstance::ad_StandFireAnim);

            if (!(target_ && target_->health() > 0))
                firing_ = PedInstance::Firing_Stop;

            if (firing_ == PedInstance::Firing_Fire)
                firing_ = PedInstance::Firing_Reload;
        }
        if (firing_ == PedInstance::Firing_Stop) {
            firing_ = PedInstance::Firing_Not;
            target_ = 0;
        }
    }
    if (firing_ == PedInstance::Firing_Reload) {
        reload_count_ += elapsed;
        // miliseconds
        // TODO: this value should be influenced by IPA values
        int required = 1200;
        if (agent_is_ == PedInstance::Agent_Active)
            required = 500;

        if (weapon_idx == Weapon::Pistol_Anim
            || weapon_idx == Weapon::Shotgun_Anim)
        {
            required = 1500;
            if (agent_is_ == PedInstance::Agent_Active)
                required = 750;
        }

        if (reload_count_ >= required) {
            firing_ = PedInstance::Firing_Not;
            reload_count_ = 0;
            updated = true;

            if (selectedWeapon())
                if (selectedWeapon()->ammoRemaining() == 0)
                    selectNextWeapon();
        }
    }

    return updated;
}
#endif

void PedInstance::stopFiring() {
    if (firing_ != PedInstance::Firing_Not)
        firing_ = PedInstance::Firing_Stop;
}

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
    ped_(ped), firing_(PedInstance::Firing_Not),
    action_state_(PedInstance::pa_smNone),
    desc_state_(PedInstance::pd_smUndefined),
    hostile_desc_(PedInstance::pd_smUndefined),
    obj_group_def_(PedInstance::og_dmUndefined),
    old_obj_group_def_(PedInstance::og_dmUndefined),
    obj_group_id_(0), old_obj_group_id_(0),
    drawn_anim_(PedInstance::ad_StandAnim), target_(NULL), target_pos_(NULL),
    reach_obj_(NULL), reach_pos_(NULL), sight_range_(0),
    is_hostile_(false), reload_count_(0), selected_weapon_(-1),
    pickup_weapon_(NULL), putdown_weapon_(NULL), in_vehicle_(NULL),
    agent_is_(PedInstance::Not_Agent)
{
    hold_on_.wayFree = 0;
    rcv_damage_def_ = MapObject::ddmg_Ped;
    major_type_ = MapObject::mjt_Ped;
}

PedInstance::~PedInstance(){
    delete ped_;
    dest_path_.clear();
    if (target_pos_)
        delete target_pos_;
    if (reach_pos_)
        delete reach_pos_;
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

    if (selectedWeapon() == 0) {
        firing_ = PedInstance::Firing_Not;
        target_ = 0;
    }

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
            printf("hmm ad_NoAnimation\n");
            break;
    }
}

bool PedInstance::inSightRange(MapObject *t) {
    float d =
        sqrt((float) (t->tileX() - tileX()) * (t->tileX() - tileX()) +
             (t->tileY() - tileY()) * (t->tileY() - tileY()) +
             (t->tileZ() - tileZ()) * (t->tileZ() - tileZ()));

    return d < sight_range_;
}

void PedInstance::selectNextWeapon() {

    if (selected_weapon_ != -1) {
        int nextWeapon = -1;
        Weapon *curSelectedWeapon = (Weapon *)weapon(selected_weapon_);

        if (curSelectedWeapon) {
            for (int i = 0; i < numWeapons(); i++) {
                WeaponInstance * wi = weapon(i);
                if (i != selected_weapon_ && wi->ammoRemaining()
                        && wi->getWeaponType()
                            == curSelectedWeapon->getWeaponType())
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
            selected_weapon_ = nextWeapon;
    } else
        selectBestWeapon();

    if (selected_weapon_ != -1
        && weapon(selected_weapon_)->getWeaponType() == Weapon::EnergyShield)
            setRcvDamageDef(MapObject::ddmg_PedWithEnergyShield);
    else
        setRcvDamageDef(MapObject::ddmg_Ped);
}

void PedInstance::selectBestWeapon() {
    int bestWeapon = -1;
    int bestWeaponRank = -1;

    for (int i = numWeapons() - 1; i >=0; i--) {
        if (weapon(i)->ammoRemaining() && weapon(i)->rank() > bestWeaponRank) {
            bestWeapon = i;
            bestWeaponRank = weapon(i)->rank();
        }
    }

    if(bestWeapon != -1) {
        selected_weapon_ = bestWeapon;
    }
}

void PedInstance::dropWeapon(int n) {
    assert(n >= 0 && n < (int) weapons_.size());

    if (selected_weapon_ == n) {
        if (weapons_[selected_weapon_]->getWeaponType() == Weapon::EnergyShield)
            setRcvDamageDef(MapObject::ddmg_PedWithEnergyShield);
        else
            setRcvDamageDef(MapObject::ddmg_Ped);
        selected_weapon_ = -1;
    }

    WeaponInstance *w = weapons_[n];
    weapons_.erase(weapons_.begin() + n);
    if (n < selected_weapon_)
        selected_weapon_--;

    putdown_weapon_ = w;
}

void PedInstance::dropWeapon(WeaponInstance *wi) {
    if (selectedWeapon() == wi) {
        selected_weapon_ = -1;
        if (wi->getWeaponType() == Weapon::EnergyShield)
            setRcvDamageDef(MapObject::ddmg_PedWithEnergyShield);
        else
            setRcvDamageDef(MapObject::ddmg_Ped);
    }
    for (std::vector <WeaponInstance *>::iterator it
        = weapons_.begin(); it != weapons_.end(); it++)
    {
        if ((*it) == wi) {
            weapons_.erase(it);
            break;
        }
    }
    wi->setOwner(NULL);
    wi->setMap(map_);
    wi->setIsIgnored();
    wi->setPosition(tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
    wi->setVisZ(vis_z_);
    wi->activate();
}

void PedInstance::dropAllWeapons() {

    setRcvDamageDef(MapObject::ddmg_Ped);
    selected_weapon_ = -1;

    for (std::vector < WeaponInstance * >::iterator it
        = weapons_.begin(); it != weapons_.end(); it++)
    {
        WeaponInstance *w = *it;
        w->setMap(map());
        // TODO: drop weapons not on same place
        w->setPosition(tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
        w->setVisZ(vis_z_);
        w->setOwner(NULL);
        w->setIsIgnored();
        w->activate();
    }
    weapons_.clear();
}

void PedInstance::pickupWeapon(WeaponInstance * w) {
    assert(w->map() == map());

    if (weapons_.size() >= 8)
        return;

    pickup_weapon_ = w;
    frame_ = 0;
}

bool PedInstance::wePickupWeapon() {
    return pickup_weapon_ != NULL;
}

VehicleInstance *PedInstance::inVehicle()
{
#ifdef NEW_ANIMATE_HANDLING
    return (action_state_ & (PedInstance::pa_smInCar
        | PedInstance::pa_smUsingCar)) != 0 ? in_vehicle_ : NULL;
#else
    return map_ == -1 ? in_vehicle_ : NULL;
#endif
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
#ifdef NEW_ANIMATE_HANDLING
    switchActionStateFrom(action_state_ & (PedInstance::pa_smInCar
        | PedInstance::pa_smUsingCar));
#endif
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
        case PedInstance::ad_NoAnimation:
            break;
        case PedInstance::ad_PersuadedAnim:
            setFramesPerSec(8);
            break;
    }
}

void PedInstance::handleDrawnAnim(int elapsed) {
    Weapon::WeaponAnimIndex weapon_idx =
        selectedWeapon() ? selectedWeapon()->index() : Weapon::Unarmed_Anim;

    //TODO: returns remove?
    PedInstance::AnimationDrawn curanim = drawnAnim();
    switch (curanim) {
        case PedInstance::ad_HitAnim:
            if (frame_ > ped_->lastHitFrame(getDirection())) {
                if(speed_) {
                    setDrawnAnim(PedInstance::ad_WalkAnim);
                } else
                    setDrawnAnim(PedInstance::ad_StandAnim);
            } else 
                if (health_ <= 0)
                    return;
            if (health_ > 0)
                break;
        case PedInstance::ad_DieAnim:
            if (frame_ < ped_->lastDieFrame())
                return;
            setDrawnAnim(PedInstance::ad_DeadAnim);
            return;
            break;
        case PedInstance::ad_DeadAnim:
            return;
            break;
        case PedInstance::ad_DeadAgentAnim:
            return;
            break;
        case PedInstance::ad_PickupAnim:
        case PedInstance::ad_PutdownAnim:
            if (frame_ >= ped_->lastPickupFrame()) {
                if(speed_) {
                    setDrawnAnim(PedInstance::ad_WalkAnim);
                } else
                    setDrawnAnim(PedInstance::ad_StandAnim);
            } else
                return;
            break;
        case PedInstance::ad_WalkAnim:
            break;
        case PedInstance::ad_StandAnim:
            break;
        case PedInstance::ad_WalkFireAnim:
            if(frame_ >= ped_->lastWalkFireFrame(getDirection(), weapon_idx)) {
                if (speed_) {
                    setDrawnAnim(PedInstance::ad_WalkAnim);
                } else
                    setDrawnAnim(PedInstance::ad_StandAnim);
            }
            break;
        case PedInstance::ad_StandFireAnim:
            if(frame_ >= ped_->lastStandFireFrame(getDirection(), weapon_idx)) {
                if (speed_) {
                    setDrawnAnim(PedInstance::ad_WalkAnim);
                } else
                    setDrawnAnim(PedInstance::ad_StandAnim);
            }
            break;
        case PedInstance::ad_VaporizeAnim:
            if (frame_ >= ped_->lastVaporizeFrame(getDirection())) {
                if (agent_is_ == PedInstance::Agent_Active) {
                    setDrawnAnim(PedInstance::ad_DeadAgentAnim);
                } else {
                    setDrawnAnim(PedInstance::ad_NoAnimation);
                }
            }
            return;
        case PedInstance::ad_SinkAnim:
            // TODO: use this in future
            break;
        case PedInstance::ad_WalkBurnAnim:
        case PedInstance::ad_StandBurnAnim:
            if (leftTimeShowAnim(elapsed))
                return;
            setDrawnAnim(PedInstance::ad_DieBurnAnim);
            return;
        case PedInstance::ad_DieBurnAnim:
            if (frame_ >= ped_->lastDieBurnFrame()) {
                setDrawnAnim(PedInstance::ad_SmokeBurnAnim);
                setTimeShowAnim(7000);
            }
            return;
        case PedInstance::ad_SmokeBurnAnim:
            if (leftTimeShowAnim(elapsed))
                return;
            setDrawnAnim(PedInstance::ad_DeadBurnAnim);
            return;
        case PedInstance::ad_DeadBurnAnim:
            return;
        case PedInstance::ad_PersuadedAnim:
            if (frame_ >= ped_->lastPersuadeFrame()) {
                setDrawnAnim(PedInstance::ad_StandAnim);
            }
            return;
        case PedInstance::ad_NoAnimation:
            return;
    }
}

bool PedInstance::movementP(Mission *m, int elapsed)
{
    bool updated = false;

    if (!dest_path_.empty()) {
        int nxtTileX = dest_path_.front().tileX();
        int nxtTileY = dest_path_.front().tileY();
        int nxtTileZ = dest_path_.front().tileZ();
        if (hold_on_.wayFree == 1) {
            if (hold_on_.tilex == nxtTileX && hold_on_.tiley == nxtTileY
                && hold_on_.tilez == nxtTileZ)
                return updated;
            else
                hold_on_.wayFree = 0;
        } else if (hold_on_.wayFree == 2) {
            if (hold_on_.xadj || hold_on_.yadj) {
                if(abs(hold_on_.tilex - nxtTileX) <= hold_on_.xadj
                    && abs(hold_on_.tiley - nxtTileY) <= hold_on_.yadj
                    && hold_on_.tilez == nxtTileZ)
                {
                    dest_path_.clear();
                    speed_ = 0;
                    // TODO: "current action drop" function will be
                    // better for this purpose
#ifndef NEW_ANIMATE_HANDLING
                    if (in_vehicle_) {
                        in_vehicle_ = NULL;
                    }
                    if (pickup_weapon_) {
                        pickup_weapon_ = NULL;
                    }
#endif
                    return updated;
                } else
                    hold_on_.wayFree = 0;
            } else {
                if (hold_on_.tilex == nxtTileX && hold_on_.tiley == nxtTileY
                    && hold_on_.tilez == nxtTileZ)
                {
                    dest_path_.clear();
                    speed_ = 0;
                    // TODO: same as above
#ifndef NEW_ANIMATE_HANDLING
                    if (in_vehicle_) {
                        in_vehicle_ = NULL;
                    }
                    if (pickup_weapon_) {
                        pickup_weapon_ = NULL;
                    }
#endif
                    return updated;
                } else
                    hold_on_.wayFree = 0;
            }
        }
        int adx =
             nxtTileX * 256 + dest_path_.front().offX();
        int ady =
             nxtTileY * 256 + dest_path_.front().offY();
        int atx = tile_x_ * 256 + off_x_;
        int aty = tile_y_ * 256 + off_y_;
        int diffx = adx - atx, diffy = ady - aty;

        if (abs(diffx) < 16 && abs(diffy) < 16) {
            off_y_ = dest_path_.front().offY();
            off_x_ = dest_path_.front().offX();
            tile_z_ = nxtTileZ;
            tile_y_ = nxtTileY;
            tile_x_ = nxtTileX;
            dest_path_.pop_front();
            if (dest_path_.empty())
                speed_ = 0;
            updated = true;
        } else {
            setDirection(diffx, diffy, &dir_);

            int dx = 0, dy = 0;
            double d = sqrt((double)(diffx * diffx + diffy * diffy));
#ifdef NEW_ANIMATE_HANDLING
            bool reached_dist = false;
            if (dist_to_pos_ != 0) {
                toDefineXYZ xyz;
                dest_path_.back().convertPosToXYZ(&xyz);
                double dist_cur = distanceToPosXYZ(&xyz);
                if (dist_cur > (double)dist_to_pos_) {
                    d = (double)dist_to_pos_;
                    reached_dist = true;
                }
            }
#endif

            if (abs(diffx) > 0)
                // dx = diffx * (speed_ * elapsed / 1000) / d;
                dx = (int)((diffx * (speed_ * elapsed) / d) / 1000);
            if (abs(diffy) > 0)
                // dy = diffy * (speed_ * elapsed / 1000) / d;
                dy = (int)((diffy * (speed_ * elapsed) / d) / 1000);

            if (abs(dx) > abs(diffx))
                dx = diffx;
            if (abs(dy) > abs(diffy))
                dy = diffy;

            updatePlacement(off_x_ + dx, off_y_ + dy);
            // TODO :
            // what obstacles? cars? doors are already
            // setting stop signal, reuse it?
#if 0
            if (updatePlacement(off_x_ + dx, off_y_ + dy)) {
                ;
            } else {
                // TODO: avoid obstacles.
                speed_ = 0;
            }
#endif
            if(nxtTileX == tile_x_ && nxtTileY == tile_y_)
                tile_z_ = nxtTileZ;
#ifdef NEW_ANIMATE_HANDLING
            if (reached_dist) {
                dest_path_.clear();
                speed_ = 0;
            } else {
#endif
                if(nxtTileX == tile_x_ && nxtTileY == tile_y_
                    && nxtTileZ == tile_z_ 
                    && dest_path_.front().offX() == off_x_
                    && dest_path_.front().offY() == off_y_)
                    dest_path_.pop_front();
                if (dest_path_.size() == 0)
                    speed_ = 0;
#ifdef NEW_ANIMATE_HANDLING
            }
#endif

            updated = true;
        }
        unsigned char twd = m->mtsurfaces_[tile_x_ + tile_y_ * m->mmax_x_
            + tile_z_ * m->mmax_m_xy].twd;
        switch (twd) {
            case 0x01:
                vis_z_ = tile_z_ - 1;
                off_z_ = 127 - (off_y_ >> 1);
                break;
            case 0x02:
                vis_z_ = tile_z_ - 1;
                off_z_ = off_y_ >> 1;
                break;
            case 0x03:
                vis_z_ = tile_z_ - 1;
                off_z_ = off_x_ >> 1;
                break;
            case 0x04:
                vis_z_ = tile_z_ - 1;
                off_z_ = 127 - (off_x_ >> 1);
                break;
            default:
                vis_z_ = tile_z_;
                off_z_ = 0;
                break;
        }
    } else if (speed_) {
        printf("Running at speed %i, destination unknown\n", speed_);
        speed_ = 0;
    }

    return updated;
}

bool PedInstance::handleDamage(ShootableMapObject::DamageInflictType *d) {
    if (health_ <= 0 || rcv_damage_def_ == MapObject::ddmg_Invulnerable
        || (d->dtype & rcv_damage_def_) == 0)
        return false;

    if ((d->dtype & MapObject::dmg_Physical) != 0)
        health_ -= d->dvalue;
    else if (d->dtype == MapObject::dmg_Mental) {
#ifndef NEW_ANIMATE_HANDLING
        speed_ = 0;
        clearDestination();
        putdown_weapon_ = NULL;
        pickup_weapon_ = NULL;
        target_ = NULL;
#endif
        // TODO: check for required number of persuade points before applying
        setObjGroupDef((obj_group_def_ & 0xFFFFFF00) |
            (((PedInstance *)d->d_owner)->objGroupDef() & 0xFF));
        setObjGroupID(((PedInstance *)d->d_owner)->objGroupID());
        setDrawnAnim(PedInstance::ad_PersuadedAnim);
        return true;
    }
    if (d->ddir != -1) {
        dir_ = (d->ddir + 128) % 256;
    }
    if (health_ <= 0) {
        health_ = -1;
        // TODO: "current action drop" function will be
        // better for this purpose
#ifndef NEW_ANIMATE_HANDLING
        resetDest_Speed();
        putdown_weapon_ = NULL;
        pickup_weapon_ = NULL;
        target_ = NULL;
#else
        switchActionStateTo(PedInstance::pa_smDead);
#endif

        switch ((unsigned int)d->dtype) {
            case MapObject::dmg_Bullet:
                setDrawnAnim(PedInstance::ad_DieAnim);
                break;
            case MapObject::dmg_Laser:
                setDrawnAnim(PedInstance::ad_VaporizeAnim);
                destroyAllWeapons();
                break;
            case MapObject::dmg_Burn:
                // TODO: sometimes we will walk burning
                setDrawnAnim(PedInstance::ad_StandBurnAnim);
                destroyAllWeapons();
                setTimeShowAnim(4000);
                break;
            case MapObject::dmg_Explosion:
                // TODO: sometimes we will walk burning
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
        // TODO: agent sometimes can survive explosion, they need to walk burning?
        setDrawnAnim(PedInstance::ad_HitAnim);
    }
    return true;
}

void PedInstance::destroyAllWeapons() {
    while (weapons_.size()) {
        WeaponInstance * w = removeWeapon(0);
        w->setMap(-1);
        w->setPosition(tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
        w->setVisZ(vis_z_);
        w->setOwner(NULL);
        w->setIsIgnored(true);
    }
    selected_weapon_ = -1;
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
    if (obj->majorType() == MapObject::mjt_Vehicle) {
        // TODO: add this check later, create a list of all in vehicle
    } else if (obj->majorType() == MapObject::mjt_Ped) {
        if (((PedInstance *)obj)->emulatedGroupDefsEmpty()) {
            hostile_rsp =
                isInEnemyGroupDef(((PedInstance *)obj)->objGroupID(),
                ((PedInstance *)obj)->objGroupDef());
            if (!hostile_rsp) {
                if (hostile_desc_alt == PedInstance::pd_smUndefined)
                    hostile_desc_alt = hostile_desc_;
                hostile_rsp =
                    (((PedInstance *)obj)->descStateMasks() & hostile_desc_alt) != 0;
            }
        } else {
            hostile_rsp =
                ((PedInstance *)obj)->isInEmulatedGroupDef(enemy_group_defs_);
        }
        if (!hostile_rsp) {
            hostile_rsp = hostiles_found_.find(obj) != hostiles_found_.end();
        }
    }
    return hostile_rsp;
}

bool PedInstance::checkFriendIs(PedInstance *p) {
    // TODO: friend list check add
    if (p->isInEmulatedGroupDef(obj_group_id_))
        return true;
    if (!hostiles_found_.empty()) {
        if (hostiles_found_.find((ShootableMapObject *)p) !=
            hostiles_found_.end())
            return false;
    }
    return (p->objGroupID() == obj_group_id_);
}

void PedInstance::verifyHostilesFound(Mission *m) {
    std::vector <ShootableMapObject *> rm_set;
    toDefineXYZ cur_xyz;
    convertPosToXYZ(&cur_xyz);
    for (Msmod_t::iterator it = hostiles_found_.begin();
        it != hostiles_found_.end(); it++)
    {
        ShootableMapObject *smo = it->first;
        double distTo = 0;
        if (smo->health() <= 0 || (smo->majorType() == MapObject::mjt_Ped
            && checkFriendIs((PedInstance *)(smo)))
            || (m->inRangeCPos(&cur_xyz, &smo, NULL, false, false,
            (sight_range_ << 8), &distTo) != 1))
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
    aq.ot_execute = Mission::objv_Wait;
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
    aq.ot_execute = Mission::objv_ReachLocation;
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
    aq.ot_execute = Mission::objv_ReachLocation;
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

void PedInstance::createActQFiring(actionQueueGroupType &as, PathNode &tpn,
    ShootableMapObject *tsmo)
{
    as.state = 1;
    as.actions.clear();
    actionQueueType aq;
    aq.as = PedInstance::pa_smFiring;
    aq.group_desc = PedInstance::gd_mFire;
    if (tsmo) {
        aq.t_smo = tsmo;
        aq.ot_execute = Mission::objv_DestroyObject;
    } else {
#ifdef _DEBUG
        aq.t_smo = NULL;
#endif
        aq.t_pn = tpn;
        aq.ot_execute = Mission::objv_AttackLocation;
    }
    aq.state = 1;
    as.actions.push_back(aq);
}

void PedInstance::createActQFollowing(actionQueueGroupType &as,
    ShootableMapObject *tsmo, uint32 condition, int32 dist)
{
    as.state = 1;
    as.actions.clear();
    actionQueueType aq;
    aq.as = PedInstance::pa_smFollowing;
    aq.ot_execute = Mission::objv_FollowObject;
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
    aq.ot_execute = Mission::objv_ReachLocation;
    aq.state = 1;
    aq.t_smo = tsmo;
    aq.multi_var.dist_var.dir = -1;
    aq.multi_var.dist_var.dist = 0;
    aq.condition = 2;
    as.actions.push_back(aq);
    aq.as = PedInstance::pa_smPickUp;
    aq.ot_execute = Mission::objv_PickUpObject;
    aq.ot_execute |= Mission::objv_Wait;
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
    aq.ot_execute = Mission::objv_PutDownObject;
    aq.state = 1;
    aq.t_smo = tsmo;
    aq.ot_execute |= Mission::objv_Wait;
    aq.multi_var.time_var.elapsed = 0;
    aq.multi_var.time_var.time_total = 500;
    as.actions.push_back(aq);
}

void PedInstance::createActQBurning(actionQueueGroupType &as) {
    as.state = 1;
    as.actions.clear();
    actionQueueType aq;
    aq.as = PedInstance::pa_smBurning;
    aq.ot_execute = Mission::objv_ReachLocation;
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
    aq.ot_execute = Mission::objv_Wait;
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
    aq.ot_execute = Mission::objv_ReachLocation;
    aq.state = 1;
    aq.t_smo = tsmo;
    aq.condition = 2;
    aq.multi_var.dist_var.dist = 0;
    aq.multi_var.dist_var.dir = -1;
    aq.group_desc = PedInstance::gd_mStandWalk;
    as.actions.push_back(aq);
    // TODO: change this
    aq.condition = 0;
    aq.as = PedInstance::pa_smGetInCar;
    aq.group_desc = PedInstance::gd_mExclusive;
    aq.ot_execute = Mission::objv_AquireControl;
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
    aq.ot_execute = Mission::objv_LoseControl;
    aq.state = 1;
    aq.t_smo = tsmo;
    aq.group_desc = PedInstance::gd_mExclusive;
    as.actions.push_back(aq);
}

void PedInstance::setActQInQueue(actionQueueGroupType &as) {
    // NOTE: if action is invalidated all remaining actions in queue are
    // invalid, they should be removed
    if ((as.group_desc & PedInstance::gd_mExclusive) != 0)
        actions_queue_.clear();
    else {
        for (std::vector <actionQueueGroupType>::iterator it = actions_queue_.begin();
            it != actions_queue_.end(); it++)
        {
            if ((it->group_desc & as.group_desc) != 0) {
                actions_queue_.erase(it, actions_queue_.end());
                break;
            }
        }
    }
    /*
    if ((as.group_desc & 1) != 0)
        resetDest_Speed();
    */
    actions_queue_.push_back(as);
}

bool PedInstance::addActQToQueue(actionQueueGroupType &as) {
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
