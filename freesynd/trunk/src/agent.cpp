/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
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

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "gfx/screen.h"
#include "agent.h"
#include "weapon.h"
#include "mod.h"

/*!
 * Constructs a new agent.
 * \param id Object id.
 * \param agent_name The agent name.
 * \param male True means the agent is a male.
 */
Agent::Agent(int id, const char *agent_name, bool male) : name_(agent_name),
male_(male), active_(true), health_(255) {
    id_ = id;
    for (int i = 0; i < 6; i++)
        slots_[i] = NULL;
}

Agent::~Agent() {
    removeAllWeapons();
}

void Agent::removeAllWeapons() {
    while (weapons_.size())
        delete removeWeapon(0);
}

/*!
 * Returns true if the agent can be equiped with that mod version.
 */
bool Agent::canHaveMod(Mod *pNewMod) {
    if (pNewMod == NULL) {
        return false;
    }

    Mod *pMod = slots_[pNewMod->getType()];
    if (pMod) {
        // Agent has a mod of the same type
        // Returns true if equiped version if less than new version
        return (pMod->getVersion() < pNewMod->getVersion());
    }
    
    // There is no mod of that type so agent can be equiped
    return true;
}

void Agent::addMod(Mod *pNewMod) {
    if (pNewMod) {
        slots_[pNewMod->getType()] = pNewMod;
    }
}