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
#include "map.h"
#include "mission.h"
#include "missionmanager.h"
#include "utils/file.h"

MissionManager::MissionManager()
{
}

Mission *MissionManager::loadMission(int n)
{
    Mission *m = new Mission();

    printf("loading mission %i\n", n);

    char tmp[100];
    sprintf(tmp, "miss%02d.dat", n);
    int size;
    uint8 *data = File::loadFile(tmp, size);
    if (data == NULL) {
        delete m;
        return NULL;
    }

    if (!m->loadMission(data, size)) {
        delete[] data;
        delete m;
        return NULL;
    }
    delete[] data;

    sprintf(tmp, "game%02d.dat", n);
    data = File::loadFile(tmp, size);
    if (data == NULL) {
        delete m;
        return NULL;
    }

    if (!m->loadLevel(data)) {
        delete[] data;
        delete m;
        return NULL;
    }
    delete[] data;

    missions_[n] = m;
    return m;
}
