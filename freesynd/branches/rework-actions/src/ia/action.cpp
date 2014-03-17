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

#include "ia/actions.h"
#include "ped.h"
#include "weapon.h"
#include "vehicle.h"

using namespace fs_actions;

//*************************************
// Constant definition
//*************************************
const int FollowAction::kFollowDistance = 192;

void NopeBehaviour::execute(Mission *pMission, PedInstance *pPed) {
}

/*!
 * Default constructor.
 * \param origin Who has created this action.
 */
Action::Action(ActionType type, CreatOrigin origin) {
    origin_ = origin;
    status_ = kActStatusNotStarted;
    type_ = type;
}

/*!
 * Default constructor.
 * \param origin Who has created this action.
 * \param isExclusive Does action allow shooting
 * \param canExecVehicle Is action is allowed while ped is in vehicle
 */
MovementAction::MovementAction(ActionType type, CreatOrigin origin, bool isExclusive, bool canExecVehicle) :
Action(type, origin) {
    pNext_ = NULL;
    isExclusive_ = isExclusive;
    canExecInVehicle_ = canExecVehicle;
    targetState_ = PedInstance::pa_smNone;
}

/*!
 * Action execution.
 * If action is not started :
 *   - if ped is in a vehicle and action cannot be executed inside, fail
 *   - else call doStart()
 * If action has failed, exit.
 * Else change Ped's state to targetState.
 * Calls doExecute() method.
 * If action is finished (failed or succeeded), quits state.
 * \return True to update screen.
 */
bool MovementAction::execute(int elapsed, Mission *pMission, PedInstance *pPed) {
    if (status_ == kActStatusNotStarted) {
        // Action is not started so start it
        status_ = kActStatusRunning;
        if (pPed->inVehicle() && !canExecInVehicle()) {
            // Ped is in a vehicle and action cannot be executed in a vehicle
            setFailed();
        } else {
            doStart(pMission, pPed);
        }
        if (status_ == kActStatusFailed) {
            // action failed so nothing changed
            return false;
        } else {
            // action has started correctly -> change state
            pPed->goToState(targetState_);
        }
    }

    bool update = false;
    if (!isFinished()) {
        update = doExecute(elapsed, pMission, pPed);
    }

    if (isFinished()) {
        pPed->leaveState(targetState_);
    }

    return update;
}

void MovementAction::suspend(Mission *pMission, PedInstance *pPed) { 
    if (status_ != kActStatusNotStarted) {
        pPed->leaveState(targetState_);
        savedStatus_ = status_;
        status_ = kActStatusSuspended;
    }
}

void MovementAction::resume(Mission *pMission, PedInstance *pPed) { 
    status_ = savedStatus_;
    pPed->goToState(targetState_);
}

WalkAction::WalkAction(CreatOrigin origin, PathNode pn, int speed) :
    MovementAction(kActTypeWalk, origin) {
    newSpeed_ = speed;
    dest_ = pn;
    targetState_ = PedInstance::pa_smWalking;
}

WalkAction::WalkAction(CreatOrigin origin, ShootableMapObject *smo, int speed) : 
MovementAction(kActTypeWalk, origin) {
    newSpeed_ = speed;
    // Set destination point
    dest_.setTileX(smo->tileX());
    dest_.setTileY(smo->tileY());
    dest_.setTileZ(smo->tileZ());
    dest_.setOffX(smo->offX());
    dest_.setOffY(smo->offY());
    dest_.setOffZ(smo->offZ());
}

void WalkAction::doStart(Mission *pMission, PedInstance *pPed) {
    // Go to given location at given speed
    if (!pPed->setDestination(pMission, dest_, newSpeed_)) {
        setFailed();
        return;
    }
}

/*!
 * This method first updates movement for the ped.
 * Then if the ped has no more destination point, it means that
 * he has arrived.
 * Else action continue.
 * \param elapsed Time elapsed since last frame
 * \param pMission Mission data
 * \param pPed The ped executing the action.
 */
bool WalkAction::doExecute(int elapsed, Mission *pMission, PedInstance *pPed) {
    bool updated = pPed->movementP(pMission, elapsed);
    if (!pPed->hasDestination()) {
        // Ped has arrived at destination
        setSucceeded();
    }
    return updated;
}

/*!
 * Class constructor.
 * \param pTarget The ped to follow.
 */
FollowAction::FollowAction(PedInstance *pTarget) :
MovementAction(kActTypeUndefined, kOrigUser) {
    pTarget_ = pTarget;
    targetState_ = PedInstance::pa_smWalking;
}

/*!
 * Saves the target current position in the targetLastPos_ field.
 */
void FollowAction::updateLastTargetPos() {
    targetLastPos_.setTileX(pTarget_->tileX());
    targetLastPos_.setTileY(pTarget_->tileY());
    targetLastPos_.setTileZ(pTarget_->tileZ());
    targetLastPos_.setOffX(pTarget_->offX());
    targetLastPos_.setOffY(pTarget_->offY());
    targetLastPos_.setOffZ(pTarget_->offZ());
}

void FollowAction::doStart(Mission *pMission, PedInstance *pPed) {
    updateLastTargetPos();
    // If target is not too close, then initiate movement.
    if (!pPed->isCloseTo(pTarget_, kFollowDistance)) {
        if (!pPed->setDestination(pMission, targetLastPos_)) {
            setFailed();
        }
    } else {
        // Else, Ped must stay standing. But the method Action::execute() will do a goToState()
        // right after doStart() has been called. So we'll have a walking
        // animation without the ped moving. See doExecute() for more
        targetState_ = PedInstance::pa_smStanding;
    }
}

bool FollowAction::doExecute(int elapsed, Mission *pMission, PedInstance *pPed) {
    bool updated = false;

    if (pTarget_->isDead()) {
        // target is dead so stop moving and terminate action
        pPed->clearDestination();
        setFailed();
    } else {
        if (pPed->speed() != 0) {
            if (pPed->isCloseTo(pTarget_, kFollowDistance)) {
                // We reached the target so stop moving temporarily
                pPed->clearDestination();
                pPed->leaveState(targetState_);
            } else {
                updated = pPed->movementP(pMission, elapsed);
            }
        }

        // target has moved: we use checkCurrPosTileOnly() to give time to ped
        // to walk away else animation is buggy
        if (!pTarget_->checkCurrPosTileOnly(targetLastPos_)) {
            // resetting target position
            updateLastTargetPos();
            if (pPed->setDestination(pMission, targetLastPos_)) {
                targetState_ = PedInstance::pa_smWalking;
                pPed->goToState(targetState_);
            } else {
                setFailed();
            }
        }
    }
    return updated;
}

PutdownWeaponAction::PutdownWeaponAction(uint8 weaponIdx) : MovementAction(kActTypeUndefined, kOrigUser, true) {
    weaponIdx_ = weaponIdx;
    targetState_ = PedInstance::pa_smPutDown;
}

void PutdownWeaponAction::doStart(Mission *pMission, PedInstance *pPed) {
    status_ = kActStatusWaitForAnim;
}

/*!
 * Action can be executed only when the animation of dropping a weapon is done.
 */
bool PutdownWeaponAction::doExecute(int elapsed, Mission *pMission, PedInstance *pPed) {
    if (status_ != kActStatusWaitForAnim) {
        pPed->dropWeapon(weaponIdx_);
        setSucceeded();
    }

    return true;
}

PickupWeaponAction::PickupWeaponAction(WeaponInstance *pWeapon) :
    MovementAction(kActTypeUndefined, fs_actions::kOrigUser, true) {
    pWeapon_ = pWeapon;
    targetState_ = PedInstance::pa_smPickUp;
}

void PickupWeaponAction::doStart(Mission *pMission, PedInstance *pPed) {
    // recheck here in case weapon was pickup between the time the action
    // was added and now
    if (pWeapon_->hasOwner() || pWeapon_->isDead() ||
        pPed->numWeapons() == WeaponHolder::kMaxHoldedWeapons ||
        !pPed->samePosition(pWeapon_)) {
        setFailed();
    } else {
        // the animation must run first then the object will be picked up
        status_ = kActStatusWaitForAnim;
    }
}

bool PickupWeaponAction::doExecute(int elapsed, Mission *pMission, PedInstance *pPed) {
    if (status_ != kActStatusWaitForAnim) {
        pWeapon_->setOwner(pPed);
        pWeapon_->setMap(-1);
        pWeapon_->setIsIgnored(true);
        pWeapon_->deactivate();
        pPed->addWeapon(pWeapon_);
        setSucceeded();
    }
    return true;
}

EnterVehicleAction::EnterVehicleAction(Vehicle *pVehicle) : MovementAction(kActTypeUndefined, kOrigUser, true) {
    pVehicle_ = pVehicle;
}

void EnterVehicleAction::doStart(Mission *pMission, PedInstance *pPed) {
    if (pVehicle_->isDead()) {
        setFailed();
    } 
}

bool EnterVehicleAction::doExecute(int elapsed, Mission *pMission, PedInstance *pPed) {
    // Ped needs to be near vehicle to get in
    if (pPed->samePosition(pVehicle_)) {
        // state of ped is set in addPassenger
        pVehicle_->addPassenger(pPed);
    }
    // Finish action anyway
    setSucceeded();
    return true;
}

DriveVehicleAction::DriveVehicleAction(CreatOrigin origin, VehicleInstance *pVehicle, PathNode &dest) : 
    MovementAction(kActTypeUndefined, origin, false, true) {
    pVehicle_ = pVehicle;
    dest_ = dest;
}

void DriveVehicleAction::doStart(Mission *pMission, PedInstance *pPed) {
    if (pVehicle_->isDead() || !pVehicle_->isInsideVehicle(pPed)) {
        setFailed();
    }

    if (!pVehicle_->setDestination(pMission, dest_, 1024)) {
        setFailed();
    }
}

bool DriveVehicleAction::doExecute(int elapsed, Mission *pMission, PedInstance *pPed) {
    if (pPed->checkCurrPosTileOnly(dest_)) {
        setSucceeded();
    }
    return true;
}

HitAction::HitAction(CreatOrigin origin, ShootableMapObject::DamageInflictType &d) : 
MovementAction(kActTypeHit, origin) {
    damage_.aimedLoc = d.aimedLoc;
    damage_.dtype = d.dtype;
    damage_.dvalue = d.dvalue;
    damage_.d_owner = d.d_owner;
    damage_.originLocW = d.originLocW;
    damage_.pWeapon = d.pWeapon;
}

FallDeadHitAction::FallDeadHitAction(ShootableMapObject::DamageInflictType &d) : 
HitAction(kOrigAction, d) {
    targetState_ = PedInstance::pa_smNone;
}

/*!
 * 
 * \param elapsed Time elapsed since last frame
 * \param pMission Mission data
 * \param pPed The ped executing the action.
 */
bool FallDeadHitAction::doExecute(int elapsed, Mission *pMission, PedInstance *pPed) {
    if (status_ == kActStatusRunning) {
        pPed->handleDeath(damage_);
        setSucceeded();
    }
    return true;
}

RecoilHitAction::RecoilHitAction(ShootableMapObject::DamageInflictType &d) : 
HitAction(kOrigAction, d) {
    targetState_ = PedInstance::pa_smHit;
}

/*!
 * 
 * \param pMission Mission data
 * \param pPed The ped executing the action.
 */
void RecoilHitAction::doStart(Mission *pMission, PedInstance *pPed) {
    // Change direction due to impact
    toDefineXYZ locW;
    pPed->convertPosToXYZ(&locW);
    pPed->setDirection(damage_.originLocW.x - locW.x, damage_.originLocW.y - locW.y);
    status_ = kActStatusWaitForAnim;
}

/*!
 * 
 * \param elapsed Time elapsed since last frame
 * \param pMission Mission data
 * \param pPed The ped executing the action.
 */
bool RecoilHitAction::doExecute(int elapsed, Mission *pMission, PedInstance *pPed) {
    if (status_ == kActStatusRunning) {
        if (pPed->handleDeath(damage_)) {
            targetState_ = PedInstance::pa_smNone;
        }
        setSucceeded();
    }
    return true;
}

LaserHitAction::LaserHitAction(ShootableMapObject::DamageInflictType &d) : 
HitAction(kOrigAction, d) {
    targetState_ = PedInstance::pa_smHitByLaser;
}

/*!
 * 
 * \param pMission Mission data
 * \param pPed The ped executing the action.
 */
void LaserHitAction::doStart(Mission *pMission, PedInstance *pPed) {
    // Change direction due to impact
    toDefineXYZ locW;
    pPed->convertPosToXYZ(&locW);
    pPed->setDirection(damage_.originLocW.x - locW.x, damage_.originLocW.y - locW.y);
    status_ = kActStatusWaitForAnim;
}

/*!
 * 
 * \param elapsed Time elapsed since last frame
 * \param pMission Mission data
 * \param pPed The ped executing the action.
 */
bool LaserHitAction::doExecute(int elapsed, Mission *pMission, PedInstance *pPed) {
    if (status_ == kActStatusRunning) {
        if (pPed->handleDeath(damage_)) {
            targetState_ = PedInstance::pa_smNone;
        }
        setSucceeded();
    }
    return true;
}

WalkBurnHitAction::WalkBurnHitAction(ShootableMapObject::DamageInflictType &d) : 
HitAction(kOrigAction, d) {
    targetState_ = PedInstance::pa_smWalkingBurning;
}

/*!
 * 
 * \param pMission Mission data
 * \param pPed The ped executing the action.
 */
void WalkBurnHitAction::doStart(Mission *pMission, PedInstance *pPed) {
    moveDirdesc_.clear();
    walkedDist_ = 0;
    moveDirection_ = rand() % 256;
    pPed->setSpeed(pPed->getDefaultSpeed());
}

/*!
 * 
 * \param elapsed Time elapsed since last frame
 * \param pMission Mission data
 * \param pPed The ped executing the action.
 */
bool WalkBurnHitAction::doExecute(int elapsed, Mission *pMission, PedInstance *pPed) {
    int walkDistDiff = 0;
    pPed->moveToDir(pMission, elapsed, moveDirdesc_, moveDirection_, -1, -1, &walkDistDiff, true);
    walkedDist_ += walkDistDiff;

    if (walkedDist_ >= 800) {
        setSucceeded();

        if (pPed->handleDeath(damage_)) {
            targetState_ = PedInstance::pa_smNone;
        }
    }

    return true;
}

ShootAction::ShootAction(CreatOrigin origin, PathNode &aimedAt) : 
    UseWeaponAction(origin) {
        aimedAt_ = aimedAt;
}

/*!
 * Execute the shoot action.
 * When action starts, it sets the firing state (and animation) then
 * waits for the end of animation.
 * \param elapsed Time since last frame.
 * \param pMission Mission data
 * \param pPed The ped executing the action.
 * \return true to redraw
 */
bool ShootAction::execute(int elapsed, Mission *pMission, PedInstance *pPed) {
    if (status_ == kActStatusNotStarted) {
        // The first time
        status_ = kActStatusWaitForAnim;
        WeaponInstance *pWeapon = pPed->selectedWeapon();
        pWeapon->playSound();

        // change state to firing
        pPed->goToState(PedInstance::pa_smFiring);
    }

    bool update = doShoot(elapsed, pMission, pPed);

    return update;
}

/*!
 * Execute the shoot action.
 * When animation is over, use the weapon to shoot and wait for weapon
 * to reload before finishing.
 * \param elapsed Time since last frame.
 * \param pMission Mission data
 * \param pPed The ped executing the action.
 * \return always return true
 */
bool ShootAction::doShoot(int elapsed, Mission *pMission, PedInstance *pPed) {
    if (status_ == kActStatusRunning) {
        WeaponInstance *pWeapon = pPed->selectedWeapon();
        ShootableMapObject::DamageInflictType dmg;
        fillDamageDesc(pMission, pPed, pWeapon, dmg);
        pWeapon->fire(pMission, dmg);
        // Shooting animation is finished
        pPed->leaveState(PedInstance::pa_smFiring);

        // The action is complete only after a certain laps of time to
        // simulate the fact that the weapon needs to be reloaded
        // and the shooter's reactivity to that
        timeToWait_ = pPed->getTimeBetweenShoots(pWeapon);
        setWaitingForTime();
    } else if (status_ == kActStatusWaitForTime) {
        timeToWait_ -= elapsed;
        if (timeToWait_ <= 0) {
            // time is reached so action can finish
            setSucceeded();
        }
    }

    return true;
}

/*!
 * Fills the ShotAttributes structure with needed information.
 * \param pMission Mission data
 * \param targetLoc Where to shoot
 * \param dmg Structure to fill
 */
void ShootAction::fillDamageDesc(Mission *pMission,
                                    PedInstance *pShooter,
                                    WeaponInstance *pWeapon,
                                    ShootableMapObject::DamageInflictType &dmg) {
    dmg.pWeapon = pWeapon;
    dmg.dtype = pWeapon->getWeaponClass()->dmgType();
    dmg.dvalue =  pWeapon->getWeaponClass()->damagePerShot();
    dmg.range = pWeapon->getWeaponClass()->range();
    dmg.d_owner = pShooter;
    dmg.aimedLoc = aimedAt_;
    pShooter->convertPosToXYZ(&(dmg.originLocW));
}

AutomaticShootAction::AutomaticShootAction(CreatOrigin origin, PathNode &aimedAt) : 
    ShootAction(origin, aimedAt) {

}

bool AutomaticShootAction::doExecute(int elapsed, Mission *pMission, PedInstance *pPed) {
    printf("AutomaticShootAction::doExecute\n");
    return false;
}

void AutomaticShootAction::stop() {
    setSucceeded();
}

/*!
 * Execute the Use medikit action.
 * \param elapsed Time since last frame.
 * \param pMission Mission data
 * \param pPed The ped executing the action.
 * \return true to redraw
 */
bool UseMedikitAction::execute(int elapsed, Mission *pMission, PedInstance *pPed) {
    bool update = false;
    if (status_ == kActStatusNotStarted) {
        // set time before completing action
        WeaponInstance *pWeapon = pPed->selectedWeapon();
        status_ = kActStatusWaitForTime;
        timeToWait_ = pPed->getTimeBetweenShoots(pWeapon);

        if (pWeapon->getWeaponType() != Weapon::MediKit) {
            setFailed();
        } else {
            pWeapon->playSound();
            ShootableMapObject::DamageInflictType dmg;
            dmg.d_owner = pPed;
            pWeapon->fire(pMission, dmg);
            update = true;
        }
    } else if (status_ == kActStatusWaitForTime) {
        timeToWait_ -= elapsed;
        if (timeToWait_ <= 0) {
            // time is reached so action can finish
            setSucceeded();
            update = true;
        }
    }

    return update;
}
