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

AgentManager::AgentManager() {

}

AgentManager::~AgentManager() {
    for (unsigned int i = 0; i != agents_.size(); ++i)
        delete agents_[i];
}

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

void AgentManager::loadAgents() {
    for (int i = 0; g_AgentNames[i]; i++) {
        agents_.push_back(new Agent(g_AgentNames[i], true));
        agents_.push_back(new Agent(g_AgentNames[i], false));
    }
}

void AgentManager::reset() {
    for (unsigned int i = 0; i < agents_.size(); i++) {
        agents_[i]->setHealth(255);
        agents_[i]->clearSlots();
        agents_[i]->removeAllWeapons();
        agents_[i]->addWeapon(
                g_App.weapons().findWeapon("PISTOL")->createInstance());
    }
}
