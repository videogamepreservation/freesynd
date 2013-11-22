/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2012  Benoit Blancard <benblan@users.sourceforge.net>*
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

#include "core/squad.h"
#include "agentmanager.h"
#include "ped.h"


//! Default constructor
Squad::Squad() {
    clear();
}

void Squad::clear() {
    for (size_t s = 0; s < AgentManager::kMaxSlot; s++) {
        a_members_[s] = NULL;
    }
    size_ = 0;
}

/*!
* Sets an agent in the squad at the given index.
* \param slotId The agent's index in the team (from 0 to 3)
* \param pPedAgent The new agent
*/
void Squad::setMember(size_t slotId, PedInstance *pPedAgent) {
    assert(slotId < AgentManager::kMaxSlot);
    a_members_[slotId] = pPedAgent;
    
    // recount the number of agent
    size_ = 0;
    for (size_t i=0; i < AgentManager::kMaxSlot; i++) {
        if (a_members_[i]) {
            size_++;
        }
    }
};

//! Returns the agent on the given slot
PedInstance * Squad::member(size_t slotId) {
    assert(slotId < AgentManager::kMaxSlot);
    return a_members_[slotId];
}

/*! 
 * Returns true if one living agent has a scanner.
 */
bool Squad::hasScanner() {
    for (size_t indx = AgentManager::kSlot1; indx < AgentManager::kMaxSlot; indx++) {
        PedInstance *pAgent = a_members_[indx];
        if (pAgent && pAgent->isAlive()) {
            for (int windx=0; windx<pAgent->numWeapons(); windx++) {
                if (pAgent->weapon(windx)->getWeaponType() == Weapon::Scanner) {
                    return true;
                }
            }
        }
    }

    // No agent has a scanner
    return false;
}

/*!
 * Check if all agents are dead.
 * \return true if at least one agent is alive.
 */
bool Squad::isAllDead() {
    for (size_t indx = AgentManager::kSlot1; indx < AgentManager::kMaxSlot; indx++) {
        PedInstance *pAgent = a_members_[indx];
        if (pAgent && pAgent->isAlive()) {
            return false;
        }
    }

    // No agent is alive
    return true;
}
