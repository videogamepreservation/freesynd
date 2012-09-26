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

#ifndef CORE_SQUAD_H_
#define CORE_SQUAD_H_

#include "common.h"

class Agent;

/*!
 * A squad is a team of 1 to 4 agents that perform the mission.
 * A squad is composed of 4 slots that can be filled with agents or can
 * be left empty.
 * An agent on a slot must be active to participate in a mission.
 */
class Squad {
public:
    //! Default constructor
    Squad();

    //! Empties the squad
    void clear();
    //! Returns true if the slot holds an agent and if he's active
    bool isSlotActive(size_t slotId);
    /*!
     * Sets an agent in the squad at the given index.
     * \param n The agent's index in the team (from 0 to 3)
     * \param a The new agent
     */
    void setMember(size_t slotId, Agent *pAgent) { 
        assert(slotId < kMaxSlot);
        a_members_[slotId] = pAgent;
    };
    //! Returns the agent on the given slot
    Agent * member(size_t slotId) {
        assert(slotId < kMaxSlot);
        return a_members_[slotId];
    }
    //! Return the slot that holds the given agent or -1 if ni agent is found
    int getTeamSlot(Agent *pAgent);

public:
    /*! Total number of slots.*/
    static const size_t kMaxSlot;
    /*! Id of slot 1.*/
    static const size_t kSlot1;
    /*! Id of slot 2.*/
    static const size_t kSlot2;
    /*! Id of slot 3.*/
    static const size_t kSlot3;
    /*! Id of slot 4.*/
    static const size_t kSlot4;
private:
    /*!
     * Selected agents for the next mission. Up to 4 agents.
     */
    Agent *a_members_[4];
};

#endif  // CORE_SQUAD_H_
