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

ResearchManager::ResearchManager() {
    pCurrResearch_ = NULL;
}

ResearchManager::~ResearchManager() {
    LOG(Log::k_FLG_MEM, "ResearchManager", "~ResearchManager", ("Destruction..."))
    destroy();
    listeners_.clear();
}

void ResearchManager::destroy() {
    // Destroy researches on mods
    for (unsigned int i=0; i<availableModsSearch_.size(); i++) {
        Research *pRes = availableModsSearch_.get(i);
        delete pRes;
    }

    // Destroy researches on weapons
    for (unsigned int i=0; i<availableWeaponsSearch_.size(); i++) {
        Research *pRes = availableWeaponsSearch_.get(i);
        delete pRes;
    }

    availableModsSearch_.clear();
    availableWeaponsSearch_.clear();
    pCurrResearch_ = NULL;
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

        try {
            std::string name;
            int fund;
            int next;
            Weapon::WeaponType nextWeap = Weapon::Unknown;
            char tmp[25];
            const char *pattern = "res.weap.%d.%s";

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
            FSERR(Log::k_FLG_GAME, "ResearchManager", "loadResearch", ("Cannot load weapon research %d", wt))
        }
    } catch (...) {
        FSERR(Log::k_FLG_IO, "ResearchManager", "loadResearch", ("File %s cannot be read", filename.c_str()))
    }
    return NULL;
}

Research *ResearchManager::loadResearch(Mod::EModType mt, Mod::EModVersion version) {
    std::string filename(File::fileFullPath("ref/research.dat", false));
    try {
        // Loads configuration file
        ConfigFile conf(filename);

        std::string name;
        int min;

        char tmp[25];
        const char *pattern = "res.mod.%d.%d.%s";

        sprintf(tmp, pattern, mt, version, "name");
        conf.readInto(name, tmp);
        sprintf(tmp, pattern, mt, version, "minFund");
        conf.readInto(min, tmp, 0);

        if (min == 0 || name.size() == 0) {
            FSERR(Log::k_FLG_GAME, "ResearchManager", "loadResearch", ("Cannot load mod research %d, version %d", mt, version))
            return NULL;
        }
        return new Research(mt, version, name, min);

    } catch (...) {
        FSERR(Log::k_FLG_IO, "ResearchManager", "loadResearch", ("File %s cannot be read", filename.c_str()))
        return NULL;
    }
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

    Mod::EModType defMods[] = {Mod::MOD_LEGS, Mod::MOD_ARMS, Mod::MOD_CHEST,Mod::MOD_HEART, Mod::MOD_EYES, Mod::MOD_BRAIN};
    for (int i=0; i<6; i++) {
        Research *pRes = loadResearch(defMods[i], Mod::MOD_V2);
        if (pRes == NULL) {
            return false;
        }
        availableModsSearch_.add(pRes);
    }

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

void ResearchManager::start(Research *pResearch) {
    // If there is already a running search => suspends it
    if (pCurrResearch_) {
        pCurrResearch_->suspend();
    }

    // Starts research
    pCurrResearch_ = pResearch;
    pCurrResearch_->start();
}

/*!
 * Process all started research and returns the amount of money
 * corresponding those researches
 */
int ResearchManager::process(int hourElapsed, int moneyLeft) {
    int amount = 0;

    if (pCurrResearch_ && pCurrResearch_->getStatus() == Research::STARTED) {
            amount = pCurrResearch_->updateProgression(hourElapsed, moneyLeft);

            if (pCurrResearch_->getStatus() == Research::FINISHED) {
                Research *pNextRes = NULL;
                // Enable new weapon or mods
                if (pCurrResearch_->getType() == Research::EQUIPS) {
                    g_App.weapons().enableWeapon(pCurrResearch_->getSearchWeapon());
                    // Loads next research
                    if (pCurrResearch_->getNextWeaponRes() != Weapon::Unknown) {
                        pNextRes = loadResearch(pCurrResearch_->getNextWeaponRes());
                        
                    }
                } else {
                    g_App.mods().enableMod(pCurrResearch_->getSearchModType(), pCurrResearch_->getSearchModVersion());
                    // Loads next research
                    if (pCurrResearch_->getSearchModVersion() == Mod::MOD_V2) {
                        pNextRes = loadResearch(pCurrResearch_->getSearchModType(), Mod::MOD_V3);
                    }
                }

                // There is a new research of the same field
                if (pNextRes) {
                    // Replace with new search
                    replaceSearch(pCurrResearch_, pNextRes);
                } else {
                    // There's no more research for this category
                    removeSearch(pCurrResearch_);
                }
                // alerts of change
                fireGameEvent(pCurrResearch_);
                delete pCurrResearch_;
                pCurrResearch_ = NULL;
            }
        }
    
    return amount;
}

void ResearchManager::replaceSearch(Research *pOldSearch, Research *pNewSearch) {
    VectorModel < Research * > *pList = 
        pOldSearch->getType() == Research::EQUIPS ? &availableWeaponsSearch_ : &availableModsSearch_;
    
    for (unsigned int i=0; i<pList->size(); i++) {
        if (pOldSearch->getId() == pList->get(i)->getId()) {
            pList->setAt(i, pNewSearch);
            return;
        }
    }
}

// There's no more research for this category
void ResearchManager::removeSearch(Research *pOldSearch) {
    VectorModel < Research * > *pList = 
        pOldSearch->getType() == Research::EQUIPS ? &availableWeaponsSearch_ : &availableModsSearch_;
    
    for (unsigned int i=0; i<pList->size(); i++) {
        if (pOldSearch->getId() == pList->get(i)->getId()) {
            pList->remove(i);
            return;
        }
    }
}
