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

Vehicle::Vehicle(const char *vehicle_name, int first_anim):
name_(vehicle_name), anims_(first_anim)
{
}

VehicleInstance *Vehicle::createInstance(int map)
{
    return new VehicleInstance(this, map);
}

void Vehicle::draw(int x, int y, int dir, int frame)
{
    g_App.gameSprites().drawFrame(anims_ + dir - (dir % 2), frame, x, y);
}

void Vehicle::drawOnFire(int x, int y, int dir, int frame)
{
    g_App.gameSprites().drawFrame(anims_ + 8 + dir / 2, frame, x, y);
}

void Vehicle::drawBurnt(int x, int y, int dir, int frame)
{
    g_App.gameSprites().drawFrame(anims_ + 12 + dir / 2, frame, x, y);
}

VehicleInstance::VehicleInstance(Vehicle * vehicle,
                                 int m):ShootableMovableMapObject(m),
vehicle_(vehicle)
{
}

bool VehicleInstance::animate(int elapsed)
{
    bool updated = ShootableMovableMapObject::animate(elapsed);

    if (health_ <= start_health_) {
        return MapObject::animate(elapsed);
    }

    return updated;
}

void VehicleInstance::draw(int x, int y)
{
    addOffs(x, y);

    // ensure on map
    if (x < 129 || y < 0)
        return;

    if (health_ == 0)
        vehicle_->drawBurnt(x, y, dir_, frame_);
    else if (health_ != start_health_)
        vehicle_->drawOnFire(x, y, dir_, frame_);
    else
        vehicle_->draw(x, y, dir_, frame_);
}

bool VehicleInstance::walkable(int x, int y, int z)
{
    return g_App.maps().map(map())->tileAt(x, y, z) > 100;
    //return g_App.maps().map(map())->tileAt(x, y, z) > 5 && g_App.maps().map(map())->tileAt(x, y, z + 1) <= 5;
}
