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

#define copydata(x, y) memcpy(&level_data.x, data + y, sizeof(level_data.x))

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

#if 1
    hackMissions(n, data);
#endif

    // Initialize LevelData structure from data read in file
    LevelData::LevelDataAll level_data;
    memset(&level_data, 0, sizeof(level_data));
    copydata(u01, 0);
    copydata(map, 6);
    copydata(offset_ref, 32774);
    copydata(people, 32776);
    copydata(cars, 56328);
    copydata(statics, 59016);
    copydata(weapons, 71016);
    copydata(sfx, 89448);
    copydata(scenarios, 97128);
    copydata(u09, 113512);
    copydata(mapinfos, 113960);
    copydata(objectives, 113974);
    copydata(u11, 114058);

    Mission *m = new Mission();
    if (!m->loadLevel(level_data)) {
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

/*!
 *
 */
void MissionManager::hackMissions(int n, uint8 *data) {
    // Kenya map, adding additional walking points to scenarios
    if (n == 40) {
        uint8 *scen_start = data + 97128 + 8 * 86;
        // coord offsets changing for next point
        scen_start[4] = ((scen_start[4] & 0xFE) | 1);
        scen_start[5] = (scen_start[5] & 0xFE);

        scen_start = data + 97128 + 8 * 87;
        WRITE_LE_UINT16(scen_start, 90 * 8);
        scen_start = data + 97128 + 8 * 90;
        WRITE_LE_UINT16(scen_start, 91 *8);
        scen_start += 4;
        // coords x = 72,ox = 128, y = 32, oy = 128, z = 2
        scen_start[0] = (72 << 1) | 1;
        scen_start[1] = (32 << 1) | 1;
        scen_start[3] = 2;
        // type 1 = reach location
        scen_start[4] = 1;

        scen_start += 4;
        WRITE_LE_UINT16(scen_start, 92 *8);
        scen_start += 4;
        // coords x = 61,ox = 0, y = 32, oy = 0, z = 2
        scen_start[0] = (62 << 1) | 1;
        scen_start[1] = (57 << 1) | 1;
        scen_start[3] = 2;
        // type 1 = reach location
        scen_start[4] = 1;

        scen_start += 4;
        WRITE_LE_UINT16(scen_start, 88 *8);
        scen_start += 4;
        // coords x = 42,ox = 0, y = 59, oy = 0, z = 2
        scen_start[0] = (42 << 1) | 1;
        scen_start[1] = (59 << 1);
        scen_start[3] = 2;
        // type 1 = reach location
        scen_start[4] = 1;
    }
}