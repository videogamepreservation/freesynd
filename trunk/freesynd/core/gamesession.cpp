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
#include <stdio.h>
#include "gamesession.h"

Block g_Blocks[50] = {
    {"ALASKA", 46000000, 17, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"NORTHWEST TERRITORIES", 56000000, 39, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"NORTHEAST TERRITORIES", 58000000, 8, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"GREENLAND", 40000000, 16, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"SCANDINAVIA", 54000000, 20, 30, STAT_HAPPY, BLK_UNAVAIL, "10"},
    {"URALS", 40000000, 18, 30, STAT_HAPPY, BLK_UNAVAIL, "6:12:13"},
    {"SIBERIA", 54000000, 22, 30, STAT_HAPPY, BLK_UNAVAIL, "7"},
    {"KAMCHATKA", 56000000, 12, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"YUKON", 58000000, 21, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"WESTERN EUROPE", 48000000, 1, 30, STAT_HAPPY, BLK_UNAVAIL, "4"},
    {"CENTRAL EUROPE", 50000000, 15, 30, STAT_HAPPY, BLK_UNAVAIL, "11"},
    {"EASTERN EUROPE", 52000000, 10, 30, STAT_HAPPY, BLK_UNAVAIL, "5"},
    {"KAZAKHSTAN", 42000000, 9, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"MONGOLIA", 52000000, 3, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"FAR EAST", 42000000, 2, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"NEWFOUNDLAND", 44000000, 42, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"CALIFORNIA", 46000000, 5, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"ROCKIES", 56000000, 23, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"MID WEST", 58000000, 34, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"NEW ENGLAND", 40000000, 29, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"ALGERIA", 50000000, 28, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"LYBIA", 40000000, 35, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"IRAQ", 50000000, 6, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"IRAN", 52000000, 4, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"CHINA", 54000000, 50, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"COLORADO", 40000000, 32, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"SOUTHERN STATES", 42000000, 24, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"ATLANTIC ACCELERATOR", 44000000, 37, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"MAURITANIA", 58000453, 41, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"SUDAN", 43999510, 33, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"ARABIA", 54000471, 38, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"INDIA", 55999864, 7, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"PACIFIC RIM", 57999593, 48, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"MEXICO", 43999722, 26, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"COLOMBIA", 45999547, 44, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"NIGERIA", 48000204, 45, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"ZAIRE", 57999525, 27, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"KENYA", 47999574, 40, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"PERU", 57999719, 14, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"VENEZUALA", 40000488, 36, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"BRAZIL", 41999785, 46, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"SOUTH AFRICA", 48000138, 31, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"MOZAMBIQUE", 45999915, 13, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"WESTERN AUSTRALIA", 47999716, 11, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"NORTHERN TERRITORIES", 41999837, 19, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"NEW SOUTH WALES", 47999878, 43, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"PARAGUAY", 58000207, 49, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"ARGENTINA", 40000352, 30, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"URUGUAY", 57999681, 47, 30, STAT_HAPPY, BLK_UNAVAIL, NULL},
    {"INDONESIA", 47999794, 25, 30, STAT_HAPPY, BLK_UNAVAIL, NULL}
};

const int GameSession::HOUR_DELAY = 4000;

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
        g_Blocks[i].status = BLK_UNAVAIL;
        g_Blocks[i].tax = 30;
        g_Blocks[i].popStatus = STAT_HAPPY;
    }

    g_Blocks[selected_blck_].status = BLK_AVAIL;

    time_hour_ = 0;
    time_day_ = 1;
    time_year_ = 85;
    time_elapsed_ = 0;
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
    g_Blocks[selected_blck_].status = BLK_FINISHED;

    // Make the next missions available
    if (g_Blocks[selected_blck_].next != NULL) {
        char s[50];
        fs_strcpy(s, 50, g_Blocks[selected_blck_].next);
        char *token = strtok(s, ":");
        while ( token != NULL ) {
            int id = atoi(token);
            if (g_Blocks[id].status == BLK_UNAVAIL) {
                // Make available only if the mission is not already finished
                g_Blocks[id].status = BLK_AVAIL;
            }
            token = strtok(NULL, ":");
        }
    }
}

/*!
 * Updates the game time based on the given elapsed millisecond since
 * the last update.
 * For every day passed, it calls the updateCountries() method and update
 * the user's amount of money.
 * \param elapsed The number of millisecond since the last update. Set to -1
 * to reset the counter.
 * \return True if time has changed.
 */
bool GameSession::updateTime(int elapsed) {
    
    if (elapsed == -1) {
        time_elapsed_ = 0;
        return false;
    }

    time_elapsed_ += elapsed;
    if (time_elapsed_ > GameSession::HOUR_DELAY) {
        // Computes how much hours have passed
        int hour_elapsed = time_elapsed_ / HOUR_DELAY;
        // Reset the counter
        time_elapsed_ = 0;
        // Number of days in that time
        int day_elapsed = hour_elapsed / 24;
        int hour_remain = hour_elapsed % 24;

        // Hour update
        time_hour_ += hour_remain;
        if (time_hour_ > 23) {
            time_hour_ -= 24;
            time_day_++;

            if (time_day_ > 365) {
                time_day_ = 1;
                time_year_++;
            }

            // Update money
            money_ += updateCountries();
        }

        if (day_elapsed != 0) {
            for (int i=0; i<day_elapsed; i++) {
                time_day_++;
                if (time_day_ > 365) {
                    time_day_ = 1;
                    time_year_++;
                }

                // Update money
                money_ += updateCountries();
            }
        }

        return true;
    }

    return false;
}

void GameSession::getTimeAsStr(char *dest) {
#ifdef WIN_SECURE
    sprintf_s(dest, 100, "%02d:%d:%dNC", time_hour_, time_day_, time_year_);
#else
    sprintf(dest, "%02d:%d:%dNC", time_hour_, time_day_, time_year_);
#endif
}

int GameSession::getTaxRevenue(int population, int rate) {
    int amount = ((population / 1000000 + 1) * 1375 * rate) / 1000;

    return amount;
}


bool GameSession::addToTaxRate(int amount) {
    int newRate = g_Blocks[selected_blck_].tax + amount;
    
    if (newRate <= 100 && newRate >= 0) {
        g_Blocks[selected_blck_].tax = newRate;
        return true;
    }

    return false;
}

int GameSession::updateCountries() {
    int amount = 0;

    for (int i=0; i < 50; i++) {
        Block blk = g_Blocks[i];

        if (blk.status == BLK_FINISHED) {
            amount += getTaxRevenue(blk.population, blk.tax);
        }
    }

    return amount;
}