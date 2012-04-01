/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2012  Benoit Blancard <benblan@users.sourceforge.net>*
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

#include <stdlib.h>
#include <string>

#include "core/missionbriefing.h"

const int MissionBriefing::kMaxInfos = MAX_INFOS;
const int MissionBriefing::kMaxEnht = MAX_ENHT;

MissionBriefing::MissionBriefing() {
    i_nb_infos_ = 0;
    i_nb_enhts_ = 0;

    for (int i = 0; i < kMaxInfos; i++) {
        a_info_costs_[i] = 0;
    }

    for (int i = 0; i < kMaxEnht; i++) {
        a_enhts_costs_[i] = 0;
    }
}

/*!
 * Loads briefing text, infos and enhancement from a mission file.
 * The file is divided in three sections : infos, enhancement and briefing.
 * Each section is separated by the '|' character.
 * Each string in a section is separated with an EOL.
 * \param missData
 * \param size
 */
bool MissionBriefing::loadBriefing(uint8 * missData, int size) {
    char *miss = reinterpret_cast<char *>(missData);
    miss[size - 1] = 0;

    // reading infos
    i_nb_infos_ = 0;
    while (*miss != '|') {
        a_info_costs_[i_nb_infos_++] = atoi(miss);
        miss = strchr(miss, '\n') + 1;
    }

    miss += 2;

    // reading enhancements
    i_nb_enhts_ = 0;
    while (*miss != '|') {
        a_enhts_costs_[i_nb_enhts_++] = atoi(miss);
        miss = strchr(miss, '\n') + 1;
    }

    miss += 2;

    // reading briefing text
    if (miss) {
        std::string tmp(miss);

        int start = 0;
        // We store the default information plus
        // the additional informations for each level on info
        for (int i = 0; i < i_nb_infos_+1; i++) {
            int idx = tmp.find_first_of('|', start);
            if (std::string::npos != idx) {
                a_briefing_[i].assign(tmp.substr(start, idx - start));
                start = idx + 1;
            } else {
                a_briefing_[i].assign(tmp.substr(start));
            }
        }
    }

    return true;
}
