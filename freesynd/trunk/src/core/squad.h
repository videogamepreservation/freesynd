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

class PedInstance;

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
    //! Sets an agent in the squad at the given index.
    void setMember(size_t slotId, PedInstance *pPedAgent) ;
    //! Returns the agent on the given slot
    PedInstance * member(size_t slotId);
    //! Returns true if at least one agent has a scanner
    bool hasScanner();
private:
    /*!
     * Selected agents for the next mission. Up to 4 agents.
     */
    PedInstance *a_members_[4];
};

#endif  // CORE_SQUAD_H_
