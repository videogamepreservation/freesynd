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
    file.write(reinterpret_cast<const char*>(&nextName_), sizeof(int));
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
    infile.read(reinterpret_cast<char*>(&nextName_), sizeof(int));
    for (int i=0; i<AgentManager::MAX_AGENT; i++) {
        unsigned char uchar;
        infile.read(reinterpret_cast<char*>(&uchar), sizeof(unsigned char));
        if (uchar) {
            Agent *pAgent = agents_.get(i);
            if (pAgent == NULL) {
                // Create an empty agent
                pAgent = new Agent("", true);
            }
            pAgent->loadFromFile(infile);

            // Mods
            int nb = 0;
            infile.read(reinterpret_cast<char*>(&nb), sizeof(int));
            for (int mIndex = 0; mIndex < nb; mIndex++) {
                int type = 0;
                Mod::EModType mt = Mod::Unknown;
                Mod::EModVersion mv = Mod::MOD_V1;
                infile.read(reinterpret_cast<char*>(&type), sizeof(int));
                switch (type) {
                    case 0: mt = Mod::MOD_LEGS;break;
                    case 1: mt = Mod::MOD_ARMS;break;
                    case 2: mt = Mod::MOD_CHEST;break;
                    case 3: mt = Mod::MOD_HEART;break;
                    case 4: mt = Mod::MOD_EYES;break;
                    case 5: mt = Mod::MOD_BRAIN;break;
                    default: mt = Mod::Unknown;
                }

                int ver = 0;
                infile.read(reinterpret_cast<char*>(&ver), sizeof(int));
                switch (ver) {
                    case 0: mv = Mod::MOD_V1;break;
                    case 1: mv = Mod::MOD_V2;break;
                    case 2: mv = Mod::MOD_V3;break;
                    default: mv = Mod::MOD_V1;
                }
                // add the mod
                if (mt != Mod::Unknown) {
                    pAgent->addMod(g_App.mods().getMod(mt, mv));
                }
            }
            // Weapons
            infile.read(reinterpret_cast<char*>(&nb), sizeof(int));
            for (int wIndex = 0; wIndex < nb; wIndex++) {
                int type = 0;
                Weapon::WeaponType wt = Weapon::Unknown;
                infile.read(reinterpret_cast<char*>(&type), sizeof(int));
                switch (type) {
                    case 0: wt = Weapon::Persuadatron;break;
                    case 1: wt = Weapon::Pistol;break;
                    case 2: wt = Weapon::GaussGun;break;
                    case 3: wt = Weapon::Shotgun;break;
                    case 4: wt = Weapon::Uzi;break;
                    case 5: wt = Weapon::Minigun;break;
                    case 6: wt = Weapon::Laser;break;
                    case 7: wt = Weapon::Flamer;break;
                    case 8: wt = Weapon::LongRange;break;
                    case 9: wt = Weapon::Scanner;break;
                    case 10: wt = Weapon::MediKit;break;
                    case 11: wt = Weapon::TimeBomb;break;
                    case 12: wt = Weapon::AccessCard;break;
                    case 13: wt = Weapon::EnergyShield;break;
                    default: wt = Weapon::Unknown;
                }
                if (wt != Weapon::Unknown) {
                    WeaponInstance *pInst = g_App.weapons().getWeapon(wt)->createInstance();
                    int ammo = 0;
                    infile.read(reinterpret_cast<char*>(&ammo), sizeof(int));
                    pInst->setAmmoRemaining(ammo);
                    pAgent->addWeapon(pInst);
                }
            }
        } else if (agents_.get(i)) {
            destroyAgentSlot(i);
        }
    }
    return true;
}