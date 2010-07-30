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

#include "gamesession.h"

Block g_Blocks[50] = {
     {"ALASKA", 46000000, 17, false, false},
    {"NORTHWEST TERRITORIES", 56000000, 39, false, false},
    {"NORTHEAST TERRITORIES", 58000000, 8, false, false},
    {"GREENLAND", 40000000, 16, false, false},
    {"SCANDINAVIA", 54000000, 20, false, false},
    {"URALS", 40000000, 18, false, false},
    {"SIBERIA", 54000000, 22, false, false},
    {"KAMCHATKA", 56000000, 12, false, false},
    {"YUKON", 58000000, 21, false, false},
    {"WESTERN EUROPE", 48000000, 1, false, false},
    {"CENTRAL EUROPE", 50000000, 15, false, false},
    {"EASTERN EUROPE", 52000000, 10, false, false},
    {"KAZAKHSTAN", 42000000, 9, false, false},
    {"MONGOLIA", 52000000, 3, false, false},
    {"FAR EAST", 42000000, 2, false, false},
    {"NEWFOUNDLAND", 44000000, 42, false, false},
    {"CALIFORNIA", 46000000, 5, false, false},
    {"ROCKIES", 56000000, 23, false, false},
    {"MID WEST", 58000000, 34, false, false},
    {"NEW ENGLAND", 40000000, 29, false, false},
    {"ALGERIA", 50000000, 28, false, false},
    {"LYBIA", 40000000, 35, false, false},
    {"IRAQ", 50000000, 6, false, false},
    {"IRAN", 52000000, 4, false, false},
    {"CHINA", 54000000, 50, false, false},
    {"COLORADO", 40000000, 32, false, false},
    {"SOUTHERN STATES", 42000000, 24, false, false},
    {"ATLANTIC ACCELERATOR", 44000000, 37, false, false},
    {"MAURITANIA", 58000453, 41, false, false},
    {"SUDAN", 43999510, 33, false, false},
    {"ARABIA", 54000471, 38, false, false},
    {"INDIA", 55999864, 7, false, false},
    {"PACIFIC RIM", 57999593, 48, false, false},
    {"MEXICO", 43999722, 26, false, false},
    {"COLOMBIA", 45999547, 44, false, false},
    {"NIGERIA", 48000204, 45, false, false},
    {"ZAIRE", 57999525, 27, false, false},
    {"KENYA", 47999574, 40, false, false},
    {"PERU", 57999719, 14, false, false},
    {"VENEZUALA", 40000488, 36, false, false},
    {"BRAZIL", 41999785, 46, false, false},
    {"SOUTH AFRICA", 48000138, 31, false, false},
    {"MOZAMBIQUE", 45999915, 13, false, false},
    {"WESTERN AUSTRALIA", 47999716, 11, false, false},
    {"NORTHERN TERRITORIES", 41999837, 19, false, false},
    {"NEW SOUTH WALES", 47999878, 43, false, false},
    {"PARAGUAY", 58000207, 49, false, false},
    {"ARGENTINA", 40000352, 30, false, false},
    {"URUGUAY", 57999681, 47, false, false},
    {"INDONESIA", 47999794, 25, false, false}
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

    // TODO : select the next mission
    if (selected_blck_ < 49) {
        g_Blocks[selected_blck_ + 1].available = true;
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
