/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
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

#include <string>
#include "ped.h"
#include "mapobject.h"
#include "sound.h"

class WeaponInstance;

/*!
 * Weapon class.
 */
class Weapon {
  public:
    Weapon(const char *w_name, int smallIcon, int bigIcon, int w_cost,
            int w_ammo, int w_range, int w_shot, int w_rank, int w_anim,
            Ped::WeaponIndex w_idx, Sound::InGameSample w_sample);

    const char *name() {
        return name_.c_str();
    }

    void drawSmallIcon(int x, int y);
    void drawBigIcon(int x, int y);
    void drawInfo(int x, int y);

    int cost() {
        return cost_;
    }

    int ammo() {
        return ammo_;
    }

    int range() {
        return range_;
    }

    int shot() {
        return shot_;
    }

    int rank() {
        return rank_;
    }

    int selector() {
        return small_icon_ == 28 ? 1618 : small_icon_ - 14 + 1602;
    }

    Ped::WeaponIndex index() {
        return idx_;
    }

    Sound *sound();

    WeaponInstance *createInstance();

    bool operator==(Weapon weapon) {
        return this->idx_ == weapon.index();
    }

  protected:
    std::string name_;
    int small_icon_, big_icon_;
    int cost_, ammo_, range_, shot_;
    int anim_;
    int rank_;  //!> weapon rank
    Ped::WeaponIndex idx_;
    Sound::InGameSample sample_;
};

/*!
 * Weapon instance class.
 */
class WeaponInstance : public Weapon, public MapObject {
  public:
    WeaponInstance(Weapon *w);

    int ammoRemaining() {
        return ammo_remaining_;
    }

    void setAmmoRemaining(int n) {
        ammo_remaining_ = n;
    }

    bool animate(int elapsed);
    void draw(int x, int y);

  protected:
    int ammo_remaining_;
};
