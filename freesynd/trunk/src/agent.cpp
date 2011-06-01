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

int Agent::agentCnt = 1;

/*!
 * Constructs a new agent.
 * \param id Object id.
 * \param agent_name The agent name.
 * \param male True means the agent is a male.
 */
Agent::Agent(const char *agent_name, bool male) : name_(agent_name),
male_(male), active_(true), health_(255) {
    id_ = agentCnt++;
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

bool Agent::saveToFile(std::ofstream &file) {
    // id
    file.write(reinterpret_cast<const char*>(&id_), sizeof(int));
    // Agent name : 15 caracters max
    char buf[15];
    memset(buf, '\0', 15);
    fs_strcpy(buf, 15, name_.c_str());
    file.write(buf, 15);
    // gender : male = 1, female = 0
    unsigned char uchar = male_ ? 1 : 0;
    file.write(reinterpret_cast<const char*>(&uchar), sizeof(unsigned char));
    // active : true = 1, false = 0
    uchar = active_ ? 1 : 0;
    file.write(reinterpret_cast<const char*>(&uchar), sizeof(unsigned char));
    // Health
    file.write(reinterpret_cast<const char*>(&health_), sizeof(int));
    // Mods
    int nb = 0;
    for (int i = 0; i < 6; i++) {
        if (slots_[i]) {
            nb++;
        }
    }
    file.write(reinterpret_cast<const char*>(&nb), sizeof(int));
    for (int mIndex = 0; mIndex < 6; mIndex++) {
        Mod *pMod = slot(mIndex);
        if (pMod) {
            int ival = pMod->getType();
            file.write(reinterpret_cast<const char*>(&ival), sizeof(int));
            ival = pMod->getVersion();
            file.write(reinterpret_cast<const char*>(&ival), sizeof(int));
        }
    }
    // Weapons
    nb = numWeapons();
    file.write(reinterpret_cast<const char*>(&nb), sizeof(int));
    for (int wIndex = 0; wIndex < numWeapons(); wIndex++) {
        WeaponInstance *pWinst = weapon(wIndex);
        int ival = pWinst->getWeaponType();
        file.write(reinterpret_cast<const char*>(&ival), sizeof(int));
        ival = pWinst->ammo();
        file.write(reinterpret_cast<const char*>(&ival), sizeof(int));
    }
    return true;
}

bool Agent::loadFromFile(std::ifstream &infile) {
    return false;
}