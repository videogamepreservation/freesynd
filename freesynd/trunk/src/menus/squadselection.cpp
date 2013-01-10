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

#include "menus/squadselection.h"
#include "agentmanager.h"
#include "ped.h"

/*!
 * Default constructor.
 */
SquadSelection::SquadSelection() {
    pSquad_ = NULL;
    selected_agents_ = 0;
    leader_ = 0;
}

/*!
 * Sets the squad and selects the first selectable agent.
 * Pre existing selection has been cleared.
 * \param pSquad The current squad.
 */
void SquadSelection::setSquad(Squad *pSquad) {
    clear();
    pSquad_ = pSquad;

    for (size_t i = AgentManager::kSlot1; i < AgentManager::kMaxSlot; i++) {
        // try to select agent. if true means agent has been selected
        if (selectAgent(i, false)) {
            break;
        }
    }
}

/*!
 * Clear current selection.
 */
void SquadSelection::clear() {
    selected_agents_ = 0;
}

/*!
 * Returns the number of selected agents
 * \return 0 if no agent is selected.
 */
size_t SquadSelection::size() {
    int agents = 0;

    for (size_t i = AgentManager::kSlot1; i < AgentManager::kMaxSlot; i++) {
        if (isAgentSelected(i)) {
            agents++;
        }
    }

    return agents;
}

/*!
 * Selects or deselects the agent on the given slot.
 * \param agentNo
 * \param addToGroup True if agent is added to the selection.
 * \return True if agent has been selected or deselected.
 */
bool SquadSelection::selectAgent(size_t agentNo, bool addToGroup) {
    if (isAgentSelectable(agentNo)) {
        // saves the current selection to check if it changes
        size_t oldSelection = selected_agents_;
        if (addToGroup) {
            // Add/remove to the group
            selected_agents_ ^= 1 << agentNo;
            // check if leader was deselected
            checkLeader(agentNo);
        } else {
            selected_agents_ = 1 << agentNo;
            leader_ = agentNo;
        }

        return (oldSelection != selected_agents_);
    }

    return false;
}

/*!
 * Deselects an agent. Called when an agent died.
 * \param agentNo
 */
void SquadSelection::deselectAgent(size_t agentNo) {
    selected_agents_ &= ~(1 << agentNo);
    // check if leader was deselected
    checkLeader(agentNo);
}

/*!
 * Selects all agents.
 * \param b_selectAll True to select, false deselect
 */
void SquadSelection::selectAllAgents(bool b_selectAll) {
    if (b_selectAll) {
        // Select all agents
        for (size_t i = AgentManager::kSlot1; i < AgentManager::kMaxSlot; i++) {
            if (isAgentSelectable(i)) {
                selected_agents_ |= (1 << i);
            }
        }
    } else {
        // Deselect all except leader
        selected_agents_ = (1 << leader_);
    }
}

/*!
 * Returns true if agent is active and alive.
 */
bool SquadSelection::isAgentSelectable(size_t agentNo) {
    PedInstance *pPed = pSquad_->member(agentNo);
    return  pPed && pPed->health();
}

/*!
 * checks if leader has been deselected then finds
 * a new leader.
 */
void SquadSelection::checkLeader(size_t agentNo) {
    // TODO: why not to check selected and choose leader from them
    // and only if none selected, choose leader from first possible and
    // also select him?(chamel)
    if (agentNo == leader_) {
        // The leader has been deselected
        // find another leader among the remaining selection
        for (size_t i = AgentManager::kSlot1; i < AgentManager::kMaxSlot; i++) {
            if (isAgentSelectable(i) && leader_ != i) {
                leader_ = i;
                break;
            }
        }
    }
}
