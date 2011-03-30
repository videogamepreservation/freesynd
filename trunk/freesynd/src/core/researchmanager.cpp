/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2011  Benoit Blancard <benblan@users.sourceforge.net>*
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

#include "app.h"
#include "researchmanager.h"
#include "research.h"

const char *g_Fields[] =
    { "AUTOMATIC", "HEAVY", "ASSAULT", "MISCELLANEOUS",
    "LEGS V2", "ARMS V2", "CHEST V2", "HEART V2", "EYES V2", "BRAIN V2"
};

ResearchManager::ResearchManager() {
    availableEquipsSearch_.push_back(new Research(Weapon::Uzi, g_Fields[0], 96));
    availableEquipsSearch_.push_back(new Research(Weapon::Unknown, g_Fields[1], 7992));
    availableEquipsSearch_.push_back(new Research(Weapon::Unknown, g_Fields[2], 244));
    availableEquipsSearch_.push_back(new Research(Weapon::Unknown, g_Fields[3], 1992));

    availableModsSearch_.push_back(new Research(g_Fields[4], 4440));
    availableModsSearch_.push_back(new Research(g_Fields[5], 4440));
    availableModsSearch_.push_back(new Research(g_Fields[6], 6000));
    availableModsSearch_.push_back(new Research(g_Fields[7], 3480));
    availableModsSearch_.push_back(new Research(g_Fields[8], 3480));
    availableModsSearch_.push_back(new Research(g_Fields[9], 34800));
}

ResearchManager::~ResearchManager() {
    for (std::list < Research * >::iterator it = availableModsSearch_.begin();
         it != availableModsSearch_.end(); it++) {
        delete (*it);
    }

    for (std::list < Research * >::iterator it = availableEquipsSearch_.begin();
         it != availableEquipsSearch_.end(); it++) {
        delete (*it);
    }

    availableModsSearch_.clear();
    availableEquipsSearch_.clear();
    listeners_.clear();
}

void ResearchManager::reset() {
}

/*!
 * Returns a Research on equips with given id.
 * \param id
 * \return a pointer on a research. NULL if no research was found.
 */
Research * ResearchManager::getEquipsSearch(int id) {
    for (std::list < Research * >::iterator it = availableEquipsSearch_.begin();
         it != availableEquipsSearch_.end(); it++) {
        Research * pRes = *it;
        if (pRes->getId() == id) {
            return pRes;
        }
    }

    return NULL;
}

/*!
 * Returns a Research on mods with given id.
 * \param id
 * \return a pointer on a research. NULL if no research was found.
 */
Research * ResearchManager::getModsSearch(int id) {
    for (std::list < Research * >::iterator it = availableModsSearch_.begin();
         it != availableModsSearch_.end(); it++) {
        Research * pRes = *it;
        if (pRes->getId() == id) {
            return pRes;
        }
    }

    return NULL;
}

void ResearchManager::addListener(GameEventListener *pListener) {
    if (pListener) {
        listeners_.push_back(pListener);
    }
}

void ResearchManager::fireGameEvent(Research *pResearch) {
    for (std::list < GameEventListener * >::iterator it = listeners_.begin();
                        it != listeners_.end(); it++) {
        GameEvent evt;
        evt.type_ = GameEvent::GE_SEARCH;
        evt.pCtxt_ = pResearch;
        (*it)->handleGameEvent(evt);
    }
}

/*!
 * Process all started research and returns the amount of money
 * corresponding those researches
 */
int ResearchManager::process(int hourElapsed, int moneyLeft) {
    int amount = 0;

    // process research on mods
    amount += processList(hourElapsed, moneyLeft, &availableModsSearch_);
    hourElapsed -= amount;

    // process research on equips
    amount += processList(hourElapsed, moneyLeft, &availableEquipsSearch_);
    
    return amount;
}

int ResearchManager::processList(int hourElapsed, int moneyLeft, std::list < Research * > *pList) {
    int amount = 0;
    // process research on equips
    for (std::list < Research * >::iterator it = pList->begin();
         it != pList->end(); ) {
        Research *pRes = *it;
        bool incrIt = true;
        
        if (pRes->getStatus() == Research::STARTED) {
            amount += pRes->updateProgression(hourElapsed, moneyLeft);
            moneyLeft -= amount;

            if (pRes->getStatus() == Research::FINISHED) {
                // Enable new weapon or mods
                if (pRes->getType() == Research::EQUIPS) {
                    g_App.weapons().enableWeapon(pRes->getSearchWeapon());
                } else {
                    
                }

                // Loads next research
                if (pRes->getNextWeaponRes() != Weapon::Unknown) {
                    
                } else {
                    // There's no more research for this category
                    it = pList->erase(it);
                    incrIt = false;
                }
                // alerts of change
                fireGameEvent(pRes);
                delete pRes;
            }
        }

        if (incrIt) {
            it++;
        }
    }

    return amount;
}