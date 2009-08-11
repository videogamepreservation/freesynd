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

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "app.h"
#include "screen.h"

Weapon::Weapon(const char *w_name, int smallIcon, int bigIcon, int w_cost,
        int w_ammo, int w_range, int w_shot, int w_rank, int w_anim,
        Ped::WeaponIndex w_idx, Sound::InGameSample w_sample) :
name_(w_name), small_icon_(smallIcon), big_icon_(bigIcon), cost_(w_cost),
ammo_(w_ammo), range_(w_range), shot_(w_shot), anim_(w_anim), rank_(w_rank),
idx_(w_idx), sample_(w_sample) {}

void Weapon::drawSmallIcon(int x, int y)
{
    g_App.menuSprites().drawSpriteXYZ(small_icon_, x, y, 0, false, true);
}

void Weapon::drawBigIcon(int x, int y)
{
    g_App.menuSprites().drawSpriteXYZ(big_icon_, x, y, 0, false, true);
}

void Weapon::drawInfo(int x, int y)
{
    char tmp[100];
    g_App.fonts().drawText(x, y, name_.c_str(), 0, false);
    sprintf(tmp, "COST   :%d", cost_);
    g_App.fonts().drawText(x, y + 12, tmp, 0, false);
    y += 24;

    if (ammo_ >= 0) {
        sprintf(tmp, "AMMO   :%d", ammo_);
        g_App.fonts().drawText(x, y, tmp, 0, false);
        y += 12;
    }

    if (range_ >= 0) {
        sprintf(tmp, "RANGE  :%d", range_);
        g_App.fonts().drawText(x, y, tmp, 0, false);
        y += 12;
    }

    if (shot_ >= 0) {
        sprintf(tmp, "SHOT   :%d", shot_);
        g_App.fonts().drawText(x, y, tmp, 0, false);
        y += 12;
    }
}

Sound *Weapon::sound()
{
	return g_App.gameSounds().sound(sample_);
}

WeaponInstance *Weapon::createInstance()
{
    return new WeaponInstance(this);
}

WeaponInstance::WeaponInstance(Weapon * w) : Weapon(*w), MapObject(-1),
        ammo_remaining_(w->ammo())
{
}

bool WeaponInstance::animate(int elapsed)
{
    if (map_ == -1)
        return false;

    return MapObject::animate(elapsed);
}

void WeaponInstance::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
}
