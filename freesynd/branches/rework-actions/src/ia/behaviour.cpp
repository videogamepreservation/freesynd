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

const int PersuaderBehaviourComponent::kMaxDistanceForPersuadotron = 50;
const int CommonAgentBehaviourComponent::kRegeratesHealthStep = 1;

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
        BehaviourComponent *pComp = *it;
        if (pComp->isEnabled()) {
            pComp->handleBehaviourEvent(evtType, pThisPed_);
        }
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
