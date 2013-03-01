/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
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
#include "utils/file.h"
#include "utils/log.h"
#include "resources.h"
#include "core/missionbriefing.h"

MissionManager::MissionManager()
{
}

/*!
 * Loads the briefing for the given mission id.
 * \param n Mission id
 * \return NULL if mission could not be loaded
 */
MissionBriefing *MissionManager::loadBriefing(int n) {
	char tmp[100];
    // Briefing file depends on the current language
    switch(g_App.menus().currLanguage()) {
        case MenuManager::ENGLISH:
            sprintf(tmp, MISSION_PATTERN_EN, n);
            break;
        case MenuManager::FRENCH:
            sprintf(tmp, MISSION_PATTERN_FR, n);
            break;
        case MenuManager::ITALIAN:
            sprintf(tmp, MISSION_PATTERN_IT, n);
            break;
        case MenuManager::GERMAN:
            sprintf(tmp, MISSION_PATTERN_GE, n);
            break;
    }
    int size;
    uint8 *data = File::loadOriginalFile(tmp, size);
    if (data == NULL) {
        return NULL;
    }

    // Create Briefing
	MissionBriefing *p_mb = new MissionBriefing();
    // Loads briefing text
    if (!p_mb->loadBriefing(data, size)) {
        delete[] data;
        delete p_mb;
        return NULL;
    }
    delete[] data;

	return p_mb;
}

/*!
 * Loads a mission.
 * \param n Mission id.
 * \return NULL if Mission could not be loaded.
 */
Mission *MissionManager::loadMission(int n)
{
    LOG(Log::k_FLG_IO, "MissionManager", "loadMission()", ("loading mission %i", n));

    char tmp[100];
    int size;
    
    sprintf(tmp, GAME_PATTERN, n);
    uint8 *data = File::loadOriginalFile(tmp, size);
    if (data == NULL) {
        return NULL;
    }

    Mission *m = new Mission();

    if (!m->loadLevel(data)) {
        delete[] data;
        delete m;
        return NULL;
    }
    delete[] data;

    Map *p_map = g_App.maps().loadMap(m->mapId());
    if (p_map == NULL) {
        delete m;
        return NULL;
    }
    m->set_map(p_map);

    return m;
}
