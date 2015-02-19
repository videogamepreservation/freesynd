/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2010  Bohdan Stelmakh <chamel@users.sourceforge.net> *
 *   Copyright (C) 2014  Benoit Blancard <benblan@users.sourceforge.net>*
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

#include "ia/behaviour.h"
#include "ped.h"
#include "mission.h"
#include "core/squad.h"
#include "core/gamesession.h"

//*************************************
// Constant definition
//*************************************
const int CommonAgentBehaviourComponent::kRegeratesHealthStep = 1;
const int PanicComponent::kScoutDistance = 1500;
const int PanicComponent::kDistanceToRun = 500;
const double PersuadedBehaviourComponent::kMaxRangeForSearchingWeapon = 500.0;
const int PoliceBehaviourComponent::kPoliceScoutDistance = 1500;
const int PoliceBehaviourComponent::kPolicePendingTime = 1500;

Behaviour::~Behaviour() {
    destroyComponents();
}

void Behaviour::destroyComponents() {
    while(compLst_.size() != 0) {
        BehaviourComponent *pComp = compLst_.front();
        compLst_.pop_front();
        delete pComp;
    }
}

void Behaviour::handleBehaviourEvent(BehaviourEvent evtType, void *pCtxt) {
    for (std::list < BehaviourComponent * >::iterator it = compLst_.begin();
            it != compLst_.end(); it++) {
        (*it)->handleBehaviourEvent(pThisPed_, evtType, pCtxt);
    }
}

void Behaviour::addComponent(BehaviourComponent *pComp) {
    compLst_.push_back(pComp);
}

/*!
 * Destroy existing components and set given one as new one
 */
void Behaviour::replaceAllcomponentsBy(BehaviourComponent *pComp) {
    destroyComponents();
    addComponent(pComp);
}

/*!
 * Run the execute method  of each component listed in the behaviour.
 * Component must be enabled.
 * \param elapsed Time elapsed since last frame
 * \param pMission Mission data
 */
void Behaviour::execute(int elapsed, Mission *pMission) {
    if (pThisPed_->isDead()) {
        return;
    }

    for (std::list < BehaviourComponent * >::iterator it = compLst_.begin();
            it != compLst_.end(); it++) {
        BehaviourComponent *pComp = *it;
        if (pComp->isEnabled()) {
            pComp->execute(elapsed, pMission, pThisPed_);
        }
    }
}

CommonAgentBehaviourComponent::CommonAgentBehaviourComponent(PedInstance *pPed):
        BehaviourComponent(), healthTimer_(pPed->getHealthRegenerationPeriod()) {
    doRegenerates_ = false;
}

/*!
 * 
 * \param elapsed Time elapsed since last frame
 * \param pMission Mission data
 * \param pPed The owner of the behaviour
 */
void CommonAgentBehaviourComponent::execute(int elapsed, Mission *pMission, PedInstance *pPed) {
    // If Agent is equiped with right chest, his health periodically updates
    if (doRegenerates_ && healthTimer_.update(elapsed)) {
        if (pPed->increaseHealth(kRegeratesHealthStep)) {
            doRegenerates_ = false;
        }
    }
}

void CommonAgentBehaviourComponent::handleBehaviourEvent(PedInstance *pPed, Behaviour::BehaviourEvent evtType, void *pCtxt) {
    switch(evtType) {
    case Behaviour::kBehvEvtHit:
        if (pPed->hasMinimumVersionOfMod(Mod::MOD_CHEST, Mod::MOD_V2)) {
            doRegenerates_ = true;
        }
        break;
    }
}

PersuaderBehaviourComponent::PersuaderBehaviourComponent():
        BehaviourComponent() {
    doUsePersuadotron_ = false;
}

void PersuaderBehaviourComponent::execute(int elapsed, Mission *pMission, PedInstance *pPed) {
    // Check if Agent has selected his Persuadotron
    if (doUsePersuadotron_) {
        // iterate through all peds except our agents
        for (size_t i = pMission->getSquad()->size(); i < pMission->numPeds(); i++) {
            PedInstance *pOtherPed = pMission->ped(i);
            if (pPed->canPersuade(pOtherPed)) {
                ShootableMapObject::DamageInflictType dmg;
                dmg.dtype = MapObject::dmg_Persuasion;
                dmg.d_owner = pPed;
                pOtherPed->insertHitAction(dmg);
            }
        }
    }
}

void PersuaderBehaviourComponent::handleBehaviourEvent(PedInstance *pPed, Behaviour::BehaviourEvent evtType, void *pCtxt) {
    switch(evtType) {
    case Behaviour::kBehvEvtPersuadotronActivated:
        doUsePersuadotron_ = true;
        break;
    case Behaviour::kBehvEvtPersuadotronDeactivated:
        doUsePersuadotron_ = false;
    }
}

PersuadedBehaviourComponent::PersuadedBehaviourComponent():
        BehaviourComponent(), checkWeaponTimer_(1000) {
    status_ = kPersuadStatusFollow;
}

void PersuadedBehaviourComponent::execute(int elapsed, Mission *pMission, PedInstance *pPed) {
    if (status_ == kPersuadStatusLookForWeapon) {
        if (checkWeaponTimer_.update(elapsed)) {
            WeaponInstance *pWeapon = findWeaponWithAmmo(pMission, pPed);
            if (pWeapon) {
                status_ = kPersuadStatusTakeWeapon;
                pPed->addActionPickup(pWeapon, false);
                // add a reset action to automatically restore follow action after picking up weapon
                pPed->addMovementAction(new fs_actions::ResetScriptedAction(), true);
            }
        }
    }
}

void PersuadedBehaviourComponent::handleBehaviourEvent(PedInstance *pPed, Behaviour::BehaviourEvent evtType, void *pCtxt) {
    if (evtType == Behaviour::kBehvEvtWeaponPickedUp) {
        // weapon found so back to normal
        status_ = kPersuadStatusFollow;
    } else if (evtType == Behaviour::kBehvEvtWeaponOut) {
        PedInstance *pPedSource = static_cast<PedInstance *> (pCtxt);
        if (pPedSource == pPed->owner()) {
            if (pPed->numWeapons() > 0) {
                pPed->selectWeapon(0);
            } else {
                // ped has no weapon -> start looking for some
                status_ = kPersuadStatusLookForWeapon;
            }
        }
    } else if (evtType == Behaviour::kBehvEvtWeaponCleared) {
        PedInstance *pPedSource = static_cast<PedInstance *> (pCtxt);
        if (pPedSource == pPed->owner() && pPed->selectedWeapon()) {
            pPed->deselectWeapon();
        }
    } else if (evtType == Behaviour::kBehvEvtWeaponDropped) {
        // 
        status_ = kPersuadStatusLookForWeapon;
    }
}

/*!
 * Look for weapon on the ground with ammo.
 * The closest weapon within the given range will be return.
 * \param pMission Mission data
 * \param pPed The ped searching for the weapon
 * \return NULL if no weapon is found.
 */
WeaponInstance * PersuadedBehaviourComponent::findWeaponWithAmmo(Mission *pMission, PedInstance *pPed) {
   WeaponInstance *pWeaponFound = NULL;
   double currentDistance = kMaxRangeForSearchingWeapon;

    int numweapons = pMission->numWeapons();
    for (int32 i = 0; i < numweapons; ++i) {
        WeaponInstance *w = pMission->weapon(i);
        if (w->isIgnored())
            continue;
        if (w->canShoot() && w->ammoRemaining() > 0) {
            double length = 0;
            if (pMission->getPathLengthBetween(pPed, w, kMaxRangeForSearchingWeapon, &length) == 0) {
                if (currentDistance > length) {
                    pWeaponFound = w;
                    currentDistance = length;
                }
            }
        }
    }
    return pWeaponFound;
}

PanicComponent::PanicComponent():
        BehaviourComponent(), scoutTimer_(500) {
    panicking_ = false;
    // this component will be activated by event to
    // lower CPU consumption
    setEnabled(false);
}

void PanicComponent::execute(int elapsed, Mission *pMission, PedInstance *pPed) {
    if (pPed->isPanicImmuned()) {
        return;
    }

    // True means ped must check if someone's armed around him
    bool checkArmedPed = false;
    fs_actions::MovementAction *pAction = pPed->currentAction();

    if (pAction == NULL) {
        // No action means that ped has finished running
        // but we are still in panic mode, so check if there are armed people nearby
        checkArmedPed = true;
    } else if (!panicking_) {
        // ped is walking calmly -> check if he should panic
        if (scoutTimer_.update(elapsed)) {
            checkArmedPed = true;
        }
    }

    if (checkArmedPed) {
        PedInstance *pArmedPed = findNearbyArmedPed(pMission, pPed);
        if (pArmedPed) {
            runAway(pPed, pArmedPed, pAction);
        } else if (pAction == NULL) {
            // ped has finished panicking and there no reason to panick
            // so continue walking normaly
            pPed->restoreDefaultAction();
            panicking_ = false;
        }
    }
}

void PanicComponent::handleBehaviourEvent(PedInstance *pPed, Behaviour::BehaviourEvent evtType, void *pCtxt) {
    switch(evtType) {
    case Behaviour::kBehvEvtWeaponOut:
        if (!pPed->isPanicImmuned()) {
            setEnabled(true);
        }
        break;
    case Behaviour::kBehvEvtWeaponCleared:
        if (g_Session.getMission()->numArmedPeds() == 0) {
            setEnabled(false);
            if (panicking_) {
                pPed->restoreDefaultAction();
                panicking_ = false;
            }
        }
        break;
    }
}

/*!
 * Return the first armed ped that is close to the given ped.
 * \param pMission
 * \param pPed
 * \return NULL if no ped is found
 */
PedInstance * PanicComponent::findNearbyArmedPed(Mission *pMission, PedInstance *pPed) {
    for (size_t i = 0; i < pMission->numArmedPeds(); i++) {
        PedInstance *pOtherPed = pMission->armedPedAtIndex(i);
        if (pPed->isCloseTo(pOtherPed, kScoutDistance)) {
            return pOtherPed;
        }
    }
    return NULL;
}

/*!
 * Makes the ped runs in the opposite way of the armed ped.
 * Saves the default action so it can be restored when the ped
 * don't panic anymore.
 * \param pPed The panicking ped
 * \param pArmedPed The armed ped that caused the panic
 * \param pWalkAction The action the ped was doing before panicking
 */
void  PanicComponent::runAway(PedInstance *pPed, PedInstance *pArmedPed, fs_actions::MovementAction *pWalkAction) {
    // setting opposite direction for movement
    toDefineXYZ thisPedLoc;
    toDefineXYZ otherLoc;
    pPed->convertPosToXYZ(&thisPedLoc);
    pArmedPed->convertPosToXYZ(&otherLoc);
    
    pPed->setDirection(otherLoc.x - thisPedLoc.x,
        otherLoc.y - thisPedLoc.y);
    // Adds the action of running away
    fs_actions::WalkToDirectionAction *pAction = 
        new fs_actions::WalkToDirectionAction(fs_actions::kOrigAction, 256);
    // walk for a certain distance
    pAction->setmaxDistanceToWalk(kDistanceToRun);
    pPed->addMovementAction(pAction, false);
    panicking_ = true;
}

PoliceBehaviourComponent::PoliceBehaviourComponent():
        BehaviourComponent(), scoutTimer_(200), endFollowTimer_(kPolicePendingTime) {
    status_ = kPoliceStatusOnPatrol;
    pTarget_ = NULL;
}

void PoliceBehaviourComponent::execute(int elapsed, Mission *pMission, PedInstance *pPed) {
    if (status_ == kPoliceStatusAlert && scoutTimer_.update(elapsed)) {
        PedInstance *pArmedGuy = findArmedPedNotPolice(pMission, pPed);
        if (pArmedGuy != NULL) {
            status_ = kPoliceStatusFollowAndShoot;
            followAndShootTarget(pPed, pArmedGuy);
        }
    } else if (status_ == kPoliceStatusPendingEndFollow && endFollowTimer_.update(elapsed)) {
        if (pMission->numArmedPeds() != 0) {
            // There are still some armed peds so keep on alert
            status_ = kPoliceStatusAlert;
        } else {
            status_ = kPoliceStatusOnPatrol;
        }
        pPed->destroyAllActions(true);
        pPed->destroyUseWeaponAction();
        pPed->deselectWeapon();
        pPed->addMovementAction(
                new fs_actions::WalkToDirectionAction(fs_actions::kOrigDefault), 
                true);
    }
}

void PoliceBehaviourComponent::handleBehaviourEvent(PedInstance *pPed, Behaviour::BehaviourEvent evtType, void *pCtxt) {
    switch(evtType) {
    case Behaviour::kBehvEvtWeaponOut:
        if (status_ == kPoliceStatusOnPatrol && !pPed->inVehicle()) {
            // When someone get his weapon out, police is on alert
            status_ = kPoliceStatusAlert;
        }
        break;
    case Behaviour::kBehvEvtWeaponCleared:
        // Someone has dropped his weapon
        if (pTarget_ == pCtxt) {
            status_ = kPoliceStatusPendingEndFollow;
            if (pTarget_->isDead()) {
                // target dropped his weapon because he's dead
                // so no need to wait
                endFollowTimer_.reset(0);
            } else {
                endFollowTimer_.reset(kPolicePendingTime);
            }
        }
        break;
    }
}

/*!
 * Return a ped that has his weapon out and is not a police man and is close to this policeman.
 */
PedInstance * PoliceBehaviourComponent::findArmedPedNotPolice(Mission *pMission, PedInstance *pPed) {
    for (size_t i = 0; i < pMission->numArmedPeds(); i++) {
        PedInstance *pOtherPed = pMission->armedPedAtIndex(i);
        if (pPed != pOtherPed && pOtherPed->type() != PedInstance::kPedTypePolice && pPed->isCloseTo(pOtherPed, kPoliceScoutDistance)) {
            return pOtherPed;
        }
    }
    return NULL;
}

void PoliceBehaviourComponent::followAndShootTarget(PedInstance *pPed, PedInstance *pArmedGuy) {
    pTarget_ = pArmedGuy;
    // stop walking
    pPed->clearDestination();
    // force quiting walking now because entering standing state does not remove walking state
    pPed->leaveState(PedInstance::pa_smWalking);
    pPed->destroyAllActions(true);
    // Set new actions
    fs_actions::WaitAndWarnAction *pWarnAction = new fs_actions::WaitAndWarnAction(pArmedGuy);
    pPed->addMovementAction(pWarnAction, false);
    fs_actions::FollowToShootAction* pFollowAction = new fs_actions::FollowToShootAction(fs_actions::kOrigDefault, pArmedGuy);
    pPed->addMovementAction(pFollowAction, true);
    fs_actions::FireWeaponAction *pFireAction = new fs_actions::FireWeaponAction(pArmedGuy);
    pPed->addMovementAction(pFireAction, true);
    pPed->addMovementAction(new fs_actions::ResetScriptedAction(), true);
}