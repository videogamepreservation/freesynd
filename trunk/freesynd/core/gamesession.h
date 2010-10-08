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

#ifndef CORE_GAME_SESSION_H
#define CORE_GAME_SESSION_H

#include <string>
#include <set>

#include "../common.h"

enum Status_Pop {
    STAT_VERY_HAPPY = 5,
    STAT_HAPPY = 4,
    STAT_CONTENT = 3,
    STAT_UNHAPPY = 2,
    STAT_DISCONTENT = 1,
    STAT_REBEL = 0
};

enum Status_Blk {
    BLK_UNAVAIL = 0,
    BLK_AVAIL = 1,
    BLK_FINISHED = 2,
    BLK_REBEL = 3
};

typedef struct Block_ {
    const char *name;
    int population;
    int mis_id;
    int tax;
    /**! 
     * Status of the population satisfaction.
     */
    Status_Pop popStatus;
    /**
     * Tells whether the mission can be played or is finished.
     */
    Status_Blk status;
    /*! The list of blocks available after finishing this mission.*/
    const char *next;
} Block;

/*!
 * A holder for player data.
 * This class stores all user data.
 */
class GameSession {
public:
    /*! Number of millisecond for an hour in the game.*/
    static const int HOUR_DELAY;

    GameSession();
    ~GameSession();

    /*!
     * Resets all data.
     */
    void reset();

    /*!
     * Returns the id of the user logo.
     */
    int getLogo() const {
        return logo_;
    }

    /*!
     * Returns the logo colour.
     */
    int getLogoColour() const {
        return logo_colour_;
    }

    /*!
     * Sets the id of the user's logo.
     */
    void setLogo(int new_logo) {
        logo_ = new_logo;
    }

    /*!
     * Sets the colour of the user's logo.
     */
    void setLogoColour(int colour) {
        logo_colour_ = colour;
    }

    /*!
     * Returns the user's company name.
     */
    const char *getCompanyName() const {
        return company_name_.c_str();
    }

    /*!
     * Returns the user's name.
     */
    const char *getUserName() const {
        return username_.c_str();
    }

    /*!
     * Sets the user's company name.
     */
    void setCompanyName(const char *name) {
        company_name_ = name;
    }

    /*!
     * Sets the user's name.
     */
    void setUserName(const char *name) {
        username_ = name;
    }

    /*!
     * Returns the user's money.
     */
    int getMoney() const {
        return money_;
    }

    /*!
     * Sets the user's money.
     */
    void setMoney(int m) {
        money_ = m;
    }

    //! Sets the representation of the time in the given string
    void getTimeAsStr(char *dest);

    /*!
     * Returns the index of the current selected region on map menu.
     */
    uint8 getSelectedBlockId() { return selected_blck_; }

    /*!
     * Sets the index of the current selected region on map menu.
     * \param index The region index (between 0 and 49 inclusive)
     */
    void setSelectedBlockId(uint8 index) { if (index >= 0 && index < 50) selected_blck_ = index; }

    //! Adds the given mission index to the list of finished missions.
    Block & getBlock(uint8 index);

    //! Convenience method to get the selected block
    Block & getSelectedBlock();

    //! Update state when finishing a mission
    void completeSelectedBlock();

    //! Cheat method to enable all missions
    void cheatEnableAllMission() { enable_all_mis_ = true; }

    //! Tells if cheat mode All missions enabled is on
    bool isAllMissionEnabled() { return enable_all_mis_; }

    //! Cheat method to replay finished mission
    void cheatReplayMission() { replay_mission_ = true; }

    //! Tells if cheat mode Replay missions is on
    bool canReplayMission() { return replay_mission_; }

    //! Do all time related updates
    bool updateTime(int elapsed);

    //! Adds the given amount to the selected block tax rate.
    bool addToTaxRate(int amount);

    //! Returns a revenue for a given population and rate.
    int getTaxRevenue(int population, int rate);

private:
    int logo_;
    int logo_colour_;
    int money_;
    std::string company_name_;
    std::string username_;
    /*! Stores the current hour. */
    int time_hour_;
    /*! Stores the current day. */
    int time_day_;
    /*! Stores the current year. */
    int time_year_;
    /*! Time in millisecond since the last time update.*/
    int time_elapsed_;
    /*! 
     * Stores the index of the current selected
     * region on the mission map.
     */
    uint8 selected_blck_;
    /*! Cheat flag to tell that all missions are playable.*/
    bool enable_all_mis_;
    /*! Cheat flag to enable replay of finished missions. */
    bool replay_mission_;

    // Update population, status and returns money
    int updateCountries();
};

#endif //CORE_GAME_SESSION_H