/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2010  Bohdan Stelmakh <chamel@users.sourceforge.net> *
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

#ifndef WEAPON_HOLDER_H
#define WEAPON_HOLDER_H

#include <vector>

class WeaponInstance;

/*!
 * Weapon holder class.
 */
class WeaponHolder {
public:
    WeaponHolder() {}
    virtual ~WeaponHolder() {}

    int numWeapons() { return weapons_.size(); }

    WeaponInstance *weapon(int n) {
        assert(n < (int) weapons_.size());
        return weapons_[n];
    }

    void addWeapon(WeaponInstance *w) {
        assert(w);
        assert(weapons_.size() < 8);
        weapons_.push_back(w);
    }

    // caller is responsible for freeing the returned value
    WeaponInstance *removeWeapon(int n) {
        assert(n < (int) weapons_.size());
        WeaponInstance *w = weapons_[n];
        std::vector<WeaponInstance *>::iterator it = weapons_.begin() + n;

        weapons_.erase(it);
        return w;
    }

    // caller is responsible for freeing the returned value
    void removeWeaponInstance(WeaponInstance *w) {
        for (std::vector<WeaponInstance *>::iterator it = weapons_.begin();
             it != weapons_.end(); ++it)
        {
            if (*it == w) {
                weapons_.erase(it);
                break;
            }
        }
    }

protected:
    std::vector<WeaponInstance *> weapons_;
};

#endif
