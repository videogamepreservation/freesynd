/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2011  Bohdan Stelmakh <chamel@users.sourceforge.net> *
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
#ifndef AGENT_H
#define AGENT_H

#include "common.h"
#include "weaponholder.h"
#include <string>
#include <vector>
#include <fstream>

class Mod;
class WeaponInstance;

/*!
 * Stores information about agent health, weapons inventory and modifications.
 */
class Agent : public WeaponHolder {
public:
    Agent(const char *agent_name, bool male);
    ~Agent();

    int getId() { return id_;}
    const char *getName() { return name_.c_str(); }
    bool isMale() { return male_; }
    bool isActive() { return active_; }
    void setActive(bool a) { active_ = a; }
    int health() { return health_; }

    void setHealth(int n) {
        if (n > 255)
            n = 255;

        health_ = n;
    }

    bool canHaveMod(Mod *pNewMod);

    void addMod(Mod *pNewMod);

    Mod *slot(int n) {
        assert(n < 6);
        return slots_[n];
    }

    void clearSlots() {
        for (int i = 0; i < 6; i++)
            slots_[i] = NULL;
    }

    void removeAllWeapons();

    //! Save instance to file
    bool saveToFile(std::ofstream &file);
    //! Load instance from file
    bool loadFromFile(std::ifstream &infile);

protected:
    /*! A counter to have unique IDs.*/
    static int agentCnt;
    /*! A unique id for the instance of Agent.*/
    int id_;
    std::string name_;
    bool male_;
    bool active_;
    int health_;
    Mod *slots_[6];
};

#endif
