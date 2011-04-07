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
#include "utils/log.h"
#include "utils/file.h"
#include "utils/configfile.h"

const char *g_Fields[] =
    { "AUTOMATIC", "HEAVY", "ASSAULT", "MISCELLANEOUS",
    "LEGS V2", "ARMS V2", "CHEST V2", "HEART V2", "EYES V2", "BRAIN V2"
};

ResearchManager::ResearchManager() {
}

ResearchManager::~ResearchManager() {
    LOG(Log::k_FLG_MEM, "ResearchManager", "~ResearchManager", ("Destruction..."))
    destroy();
    listeners_.clear();
}

void ResearchManager::destroy() {
    // Destroy researches on mods
    for (int i=0; i<availableModsSearch_.size(); i++) {
        Research *pRes = availableModsSearch_.get(i);
        delete pRes;
    }

    // Destroy researches on weapons
    for (int i=0; i<availableWeaponsSearch_.size(); i++) {
        Research *pRes = availableWeaponsSearch_.get(i);
        delete pRes;
    }

    availableModsSearch_.clear();
    availableWeaponsSearch_.clear();
}

/*!
 * Loads a research for the given weapon type from the configuration file.
 * \param wt Weapon type to research
 * \return NULL if a problem has happened
 */
Research *ResearchManager::loadResearch(Weapon::WeaponType wt) {
    std::string filename(File::fileFullPath("ref/research.dat", false));
    try {
        // Loads configuration file
        ConfigFile conf(filename);
        std::string name;
        int fund;
        int next;
        Weapon::WeaponType nextWeap = Weapon::Unknown;
        char tmp[25];
        char *pattern = "res.weap.%d.%s";

        sprintf(tmp, pattern, wt, "name");
        conf.readInto(name, tmp);
        sprintf(tmp, pattern, wt, "minFund");
        conf.readInto(fund, tmp);
        sprintf(tmp, pattern, wt, "next");
        conf.readInto(next, tmp);

        // Convert file constants to game constants
        if (next == 2) {
            nextWeap = Weapon::GaussGun;
        } else if (next == 4) {
            nextWeap = Weapon::Uzi;
        } else if (next == 5) {
            nextWeap = Weapon::Minigun;
        } else if (next == 6) {
            nextWeap = Weapon::Laser;
        } else if (next == 7) {
            nextWeap = Weapon::Flamer;
        } else if (next == 8) {
            nextWeap = Weapon::LongRange;
        } else if (next == 11) {
            nextWeap = Weapon::TimeBomb;
        } else if (next == 12) {
            nextWeap = Weapon::AccessCard;
        } else if (next == 13) {
            nextWeap = Weapon::EnergyShield;
        }

        // Create new research
        return new Research(wt, name, fund, nextWeap);
    } catch (...) {
        FSERR(Log::k_FLG_IO, "ResearchManager", "loadResearch", ("File %s cannot be read", filename.c_str()))
        return NULL;
    }
    return NULL;
}

bool ResearchManager::reset() {
    destroy();

    Weapon::WeaponType defWeapons[] = {Weapon::Uzi, Weapon::Laser, Weapon::Flamer, Weapon::AccessCard};

    for (int i=0; i<4; i++) {
        Research *pRes = loadResearch(defWeapons[i]);
        if (pRes == NULL) {
            return false;
        }
        availableWeaponsSearch_.add(pRes);
    }

    availableModsSearch_.add(new Research(g_Fields[4], 4440));
    availableModsSearch_.add(new Research(g_Fields[5], 4440));
    availableModsSearch_.add(new Research(g_Fields[6], 6000));
    availableModsSearch_.add(new Research(g_Fields[7], 3480));
    availableModsSearch_.add(new Research(g_Fields[8], 3480));
    availableModsSearch_.add(new Research(g_Fields[9], 34800));

    return true;
}

void ResearchManager::addListener(GameEventListener *pListener) {
    if (pListener) {
        listeners_.push_back(pListener);
    }
}

void ResearchManager::removeListener(GameEventListener *pListener) {
    for (std::list < GameEventListener * >::iterator it = listeners_.begin();
         it != listeners_.end(); it++) {
             if (pListener == *it) {
                 listeners_.erase(it);
                 return;
             }
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

    // process research on weapons
    amount += processList(hourElapsed, moneyLeft, &availableWeaponsSearch_);
    
    return amount;
}

int ResearchManager::processList(int hourElapsed, int moneyLeft, VectorModel < Research * > *pList) {
    int amount = 0;
    // process research 
    for (int i=0; i<pList->size();) {
        Research *pRes = pList->get(i);
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
                    Research *pNextRes = loadResearch(pRes->getNextWeaponRes());
                    if (pNextRes) {
                        pList->setAt(i, pNextRes);
                    }
                } else {
                    // There's no more research for this category
                    pList->remove(i);
                    incrIt = false;
                }
                // alerts of change
                fireGameEvent(pRes);
                delete pRes;
            }
        }

        if (incrIt) {
            i++;
        }
    }

    return amount;
}