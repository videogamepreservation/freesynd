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

#include "researchmanager.h"
#include "research.h"

const char *g_Fields[] =
    { "AUTOMATIC", "HEAVY", "ASSAULT", "MISCELLANEOUS",
    "LEGS V2", "ARMS V2", "CHEST V2", "HEART V2", "EYES V2", "BRAIN V2"
};

ResearchManager::ResearchManager() {
    availableEquipsSearch_.push_back(new Research(g_Fields[0], 96));
    availableEquipsSearch_.push_back(new Research(g_Fields[1], 7992));
    availableEquipsSearch_.push_back(new Research(g_Fields[2], 244));
    availableEquipsSearch_.push_back(new Research(g_Fields[3], 1992));

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