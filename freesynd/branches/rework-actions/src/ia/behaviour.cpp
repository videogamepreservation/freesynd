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

const int AgentBehaviour::kMaxDistanceForPersuadotron = 50;
const int AgentBehaviour::kRegeratesHealthStep = 1;

AgentBehaviour::AgentBehaviour(PedInstance *pPed) : Behaviour(pPed), healthTimer_(0) {
    doRegenerates_ = false;
    healthTimer_.reset(pPed->getHealthRegenerationPeriod());
    doUsePersuadotron_ = false;
}

void AgentBehaviour::handleBehaviourEvent(BehaviourEvent evtType) {
    switch(evtType) {
    case kBehvEvtPersuadotronActivated:
        doUsePersuadotron_ = true;
        break;
    case kBehvEvtPersuadotronDeactivated:
        doUsePersuadotron_ = false;
    case kBehvEvtHit:
        if (pThisPed_->hasMinimumVersionOfMod(Mod::MOD_CHEST, Mod::MOD_V2)) {
            doRegenerates_ = true;
        }
        break;
    }
}

void AgentBehaviour::execute(int elapsed, Mission *pMission) {
    if (pThisPed_->isDead()) {
        return;
    }

    // If Agent is equiped with right chest, his health periodically updates
    if (doRegenerates_ && healthTimer_.update(elapsed)) {
        if (pThisPed_->increaseHealth(kRegeratesHealthStep)) {
            doRegenerates_ = false;
        }
    }
    // Check if Agent has selected his Persuadotron
    if (doUsePersuadotron_) {
        for (size_t i = 0; i < pMission->numPeds(); i++) {
            PedInstance *pOtherPed = pMission->ped(i);
            // Agent can only persuad peds from another group
            if (pThisPed_->objGroupID() != pOtherPed->objGroupID() &&
                    pOtherPed->isAlive() &&
                    pThisPed_->isCloseTo(pOtherPed, kMaxDistanceForPersuadotron)) {
                        pOtherPed->handlePersuadedBy(pThisPed_);
            }
        }
    }
}