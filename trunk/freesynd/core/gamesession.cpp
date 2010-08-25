/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2010  Benoit Blancard <benblan@users.sourceforge.net>*
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
#include "gamesession.h"

Block g_Blocks[50] = {
     {"ALASKA", 46000000, 17, 30, false, false, NULL},
    {"NORTHWEST TERRITORIES", 56000000, 39, 30, false, false, NULL},
    {"NORTHEAST TERRITORIES", 58000000, 8, 30, false, false, NULL},
    {"GREENLAND", 40000000, 16, 30, false, false, NULL},
    {"SCANDINAVIA", 54000000, 20, 30, false, false, NULL},
    {"URALS", 40000000, 18, 30, false, false, NULL},
    {"SIBERIA", 54000000, 22, 30, false, false, NULL},
    {"KAMCHATKA", 56000000, 12, 30, false, false, NULL},
    {"YUKON", 58000000, 21, 30, false, false, NULL},
    {"WESTERN EUROPE", 48000000, 1, 30, false, false, "4"},
    {"CENTRAL EUROPE", 50000000, 15, 30, false, false, NULL},
    {"EASTERN EUROPE", 52000000, 10, 30, false, false, NULL},
    {"KAZAKHSTAN", 42000000, 9, 30, false, false, NULL},
    {"MONGOLIA", 52000000, 3, 30, false, false, NULL},
    {"FAR EAST", 42000000, 2, 30, false, false, NULL},
    {"NEWFOUNDLAND", 44000000, 42, 30, false, false, NULL},
    {"CALIFORNIA", 46000000, 5, 30, false, false, NULL},
    {"ROCKIES", 56000000, 23, 30, false, false, NULL},
    {"MID WEST", 58000000, 34, 30, false, false, NULL},
    {"NEW ENGLAND", 40000000, 29, 30, false, false, NULL},
    {"ALGERIA", 50000000, 28, 30, false, false, NULL},
    {"LYBIA", 40000000, 35, 30, false, false, NULL},
    {"IRAQ", 50000000, 6, 30, false, false, NULL},
    {"IRAN", 52000000, 4, 30, false, false, NULL},
    {"CHINA", 54000000, 50, 30, false, false, NULL},
    {"COLORADO", 40000000, 32, 30, false, false, NULL},
    {"SOUTHERN STATES", 42000000, 24, 30, false, false, NULL},
    {"ATLANTIC ACCELERATOR", 44000000, 37, 30, false, false, NULL},
    {"MAURITANIA", 58000453, 41, 30, false, false, NULL},
    {"SUDAN", 43999510, 33, 30, false, false, NULL},
    {"ARABIA", 54000471, 38, 30, false, false, NULL},
    {"INDIA", 55999864, 7, 30, false, false, NULL},
    {"PACIFIC RIM", 57999593, 48, 30, false, false, NULL},
    {"MEXICO", 43999722, 26, 30, false, false, NULL},
    {"COLOMBIA", 45999547, 44, 30, false, false, NULL},
    {"NIGERIA", 48000204, 45, 30, false, false, NULL},
    {"ZAIRE", 57999525, 27, 30, false, false, NULL},
    {"KENYA", 47999574, 40, 30, false, false, NULL},
    {"PERU", 57999719, 14, 30, false, false, NULL},
    {"VENEZUALA", 40000488, 36, 30, false, false, NULL},
    {"BRAZIL", 41999785, 46, 30, false, false, NULL},
    {"SOUTH AFRICA", 48000138, 31, 30, false, false, NULL},
    {"MOZAMBIQUE", 45999915, 13, 30, false, false, NULL},
    {"WESTERN AUSTRALIA", 47999716, 11, 30, false, false, NULL},
    {"NORTHERN TERRITORIES", 41999837, 19, 30, false, false, NULL},
    {"NEW SOUTH WALES", 47999878, 43, 30, false, false, NULL},
    {"PARAGUAY", 58000207, 49, 30, false, false, NULL},
    {"ARGENTINA", 40000352, 30, 30, false, false, NULL},
    {"URUGUAY", 57999681, 47, 30, false, false, NULL},
    {"INDONESIA", 47999794, 25, 30, false, false, NULL}
};

GameSession::GameSession() {
    reset();
    enable_all_mis_ = false;
    replay_mission_ = false;
}

GameSession::~GameSession() {
    reset();
}

void GameSession::reset() {
    logo_ = 0;
    logo_colour_ = 6;
    company_name_.clear();
    username_.clear();
    money_ = 30000;
    selected_blck_ = 9; // By default, the index of West Europe
    for (int i=0; i<50; i++) {
        g_Blocks[i].available = false;
        g_Blocks[i].finished = false;
    }

    g_Blocks[selected_blck_].available = true;
}

Block & GameSession::getBlock(uint8 index) {
    return g_Blocks[index];
}

Block & GameSession::getSelectedBlock() {
    return g_Blocks[selected_blck_];
}

/*!
 * Called when user finishes the current mission.
 * The block cannot be played again, a tax is set
 * and next missions are made available.
 */
void GameSession::completeSelectedBlock() {
    g_Blocks[selected_blck_].finished = true;
    g_Blocks[selected_blck_].available = false;

    // Make the next missions available
    if (g_Blocks[selected_blck_].next != NULL) {
        char s[50];
        STR_CPY(s, g_Blocks[selected_blck_].next);
        char *token = strtok(s, ":");
        while ( token != NULL ) {
            int id = atoi(token);
            g_Blocks[id].available = true;
            token = strtok(NULL, ":");
        }
    }
}

/*!
 * Adds the given mission index to the list of finished missions.
 * Removes the mission from the list of available missions.
 * \param index The mission index (between 0 and 49 inclusive)
 */
/*void GameSession::addToFinishedMissions(uint8 index) { 
    if (index >= 0 && index < 50) {
        finished_missions_.insert(index);

        // Remove the mission from available missions
        if (finished_missions_.find(index) != finished_missions_.end()) {
            finished_missions_.erase(index);
        }
    }
}*/
