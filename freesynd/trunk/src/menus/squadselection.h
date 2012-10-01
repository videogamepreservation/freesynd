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

#ifndef MENUS_SQUADSELECTION_H_
#define MENUS_SQUADSELECTION_H_

#include "core/squad.h"

/*!
 * This class manages the agents selection during a gameplay session.
 * A selection always contains at least one element.
 * A selection has a leader : it the first selected agent. The leader 
 * doesn't change if an agent is added to the current selection.
 */
class SquadSelection {
 public:
    //! Default constructor
    SquadSelection();

    /*!
     * Returns true if an agent is selected.
     * \param agentNo The index of the slot to test (use constant kSlotN)
     */
    bool isAgentSelected(size_t agentNo) {
        return (selected_agents_ & (1 << agentNo)) != 0;
    }

    //! Sets the squand and selects the first active agent of the squad.
    void setSquad(Squad *pSquad);
    //! Clear current selection
    void clear();
    //! Returns the number of selected agents
    size_t size();
    //! selects/deselects agent
    bool selectAgent(size_t agentNo, bool addToGroup);
    //! Force deselection of agent
    void deselectAgent(size_t agentNo);
    //! Selects/deselects all agent
    void selectAllAgents(bool b_selectAll);
    //! Returns the leader's slot
    size_t getLeaderSlot() { return leader_; }

 private:
    /*!
     * Return true if an agent can be selected :
     * slot must be active and agent not dead.
     */
    bool isAgentSelectable(size_t agentNo);
    //! checks if leader must be changed
    void checkLeader(size_t agentNo);

 private:
    /*! 
     * This field is seen as a field of bits. Each bit tells if
     * a slot is selected.
     */
    size_t selected_agents_;
    /*! Leader slot number.*/
    size_t leader_;
    /*! A reference to the current squad so we can check the agents
     * status.*/
    Squad  *pSquad_;
};

#endif  // MENUS_SQUADSELECTION_H_
