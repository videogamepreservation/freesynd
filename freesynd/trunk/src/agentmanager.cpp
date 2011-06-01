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
#include <assert.h>
#include "app.h"


const char *g_AgentNames[] = {
    "AFSHAR",
    "ARNOLD",
    "BAIRD",
    "BALDWIN",
    "BLACK",
    "BOYD",
    "BOYESEN",
    "BRAZIER",
    "BROWN",
    "BUSH",
    "CARR",
    "CHRISMAS",
    "CLINTON",
    "COOPER",
    "CORPES",
    "COX",
    "DAWSON",
    "DONKIN",
    "DISKETT",
    "DUNNE",
    "EDGAR",
    "EVANS",
    "FAIRLEY",
    "FAWCETT",
    "FLINT",
    "FLOYD",
    "GRIFFITHS",
    "HARRIS",
    "HASTINGS",
    "HERBERT",
    "HICKMAN",
    "HICKS",
    "HILL",
    "JAMES",
    "JEFFERY",
    "JOESEPH",
    "JOHNSON",
    "JOHNSTON",
    "JONES",
    "LEWIS",
    "LINDSELL",
    "LOCKLEY",
    "MARTIN",
    "MCENTEE",
    "MCLAUGHIN",
    "MOLYNEUX",
    "MUNRO",
    "MORRIS",
    "MUMFORD",
    "NIXON",
    "PARKER",
    "PRATT",
    "REID",
    "RENNIE",
    "RICE",
    "RIPLEY",
    "ROBERTSON",
    "ROMANO",
    "SEAT",
    "SEN",
    "SHAW",
    "SIMMONS",
    "SNELLING",
    "TAYLOR",
    "TROWERS",
    "WEBLEY",
    "WELLESLEY",
    "WILD",
    "WILLIS",
    0
};

int const AgentManager::MAX_AGENT = 18;

AgentManager::AgentManager() {
    nextName_ = 0;
    for (int i = 0; i != MAX_AGENT; ++i)
        agents_.add(NULL);
}

AgentManager::~AgentManager() {
    for (int i = 0; i != MAX_AGENT; ++i) {
        if (agents_.get(i)) {
            delete agents_.get(i);
        }
    }

    agents_.clear();
}

void AgentManager::loadAgents() {
    // TODO : load names from file
}

void AgentManager::reset(bool onlyWomen) {
    nextName_ = 0;
    for (int i = 0; i < MAX_AGENT; i++) {
        if (agents_.get(i)) {
            delete agents_.get(i);
            agents_.setAt(i, NULL);
        }
    }

    for (int i = 0; i < 8; i++) {
        Agent * pAgent = new Agent(g_AgentNames[nextName_], onlyWomen ? true : ((i % 2) == 0));
        pAgent->addWeapon(g_App.weapons().getWeapon(Weapon::Pistol)->createInstance());
        
        agents_.setAt(i, pAgent);
        nextName_++;
    }
}

void AgentManager::destroyAgentSlot(int n) {
    delete agents_.get(n);
    agents_.setAt(n, NULL);
}

bool AgentManager::saveToFile(std::ofstream &file) {
    for (int i=0; i<AgentManager::MAX_AGENT; i++) {
        Agent *pAgent = agents_.get(i);
        // This flag tells if there is an agent on this slot
        unsigned char uchar = pAgent ? 1 : 0;
        file.write(reinterpret_cast<const char*>(&uchar), sizeof(unsigned char));
        if (pAgent) {
            pAgent->saveToFile(file);
        }
    }
    return true;
}

bool AgentManager::loadFromFile(std::ifstream &infile) {
    return false;
}