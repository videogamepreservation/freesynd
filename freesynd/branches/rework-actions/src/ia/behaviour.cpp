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

//*************************************
// Constant definition
//*************************************
const int PersuaderBehaviourComponent::kMaxDistanceForPersuadotron = 100;
const int CommonAgentBehaviourComponent::kRegeratesHealthStep = 1;
const int PanicComponent::kScoutDistance = 1500;
const int PanicComponent::kDistanceToRun = 500;

Behaviour::~Behaviour() {
    while(compLst_.size() != 0) {
        BehaviourComponent *pComp = compLst_.front();
        compLst_.pop_front();
        delete pComp;
    }
}

void Behaviour::handleBehaviourEvent(BehaviourEvent evtType) {
    for (std::list < BehaviourComponent * >::iterator it = compLst_.begin();
            it != compLst_.end(); it++) {
        (*it)->handleBehaviourEvent(evtType, pThisPed_);
    }
}

void Behaviour::addComponent(BehaviourComponent *pComp) {
    compLst_.push_back(pComp);
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

void CommonAgentBehaviourComponent::handleBehaviourEvent(Behaviour::BehaviourEvent evtType, PedInstance *pPed) {
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
        for (size_t i = 0; i < pMission->numPeds(); i++) {
            PedInstance *pOtherPed = pMission->ped(i);
            // Agent can only persuad peds from another group
            if (pPed->objGroupID() != pOtherPed->objGroupID() &&
                    pOtherPed->isAlive() &&
                    pPed->isCloseTo(pOtherPed, kMaxDistanceForPersuadotron)) {
                        pOtherPed->handlePersuadedBy(pPed);
            }
        }
    }
}

void PersuaderBehaviourComponent::handleBehaviourEvent(Behaviour::BehaviourEvent evtType, PedInstance *pPed) {
    switch(evtType) {
    case Behaviour::kBehvEvtPersuadotronActivated:
        doUsePersuadotron_ = true;
        break;
    case Behaviour::kBehvEvtPersuadotronDeactivated:
        doUsePersuadotron_ = false;
    }
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

void PanicComponent::handleBehaviourEvent(Behaviour::BehaviourEvent evtType, PedInstance *pPed) {
    switch(evtType) {
    case Behaviour::kBehvEvtPanicEnabled:
        setEnabled(true);
        break;
    case Behaviour::kBehvEvtPanicDisabled:
        setEnabled(false);
        if (panicking_) {
            pPed->restoreDefaultAction();
            panicking_ = false;
        }
        break;
    }
}

/*!
 * Return the first ped that is close to the given ped and who is armed.
 * \param pMission
 * \param pPed
 * \return NULL if no ped is found
 */
PedInstance * PanicComponent::findNearbyArmedPed(Mission *pMission, PedInstance *pPed) {
    for (size_t i = 0; i < pMission->numPeds(); i++) {
        PedInstance *pOtherPed = pMission->ped(i);
        // Agent can only persuad peds from another group
        if (pOtherPed->isArmed() && pPed->isCloseTo(pOtherPed, kScoutDistance)) {
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
