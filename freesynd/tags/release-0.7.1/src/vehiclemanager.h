/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
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

#ifndef VEHICLEMANAGER_H
#define VEHICLEMANAGER_H

#include "common.h"
#include "vehicle.h"
#include <vector>

/*!
 * Vehicle manager class.
 */
class VehicleManager {
public:
    VehicleManager();
    ~VehicleManager();

    void setVehicleBaseAnim(Vehicle *vehicleanim, unsigned short baseAnim);

    int numVehicles() { return vehicles_.size(); }

    Vehicle *vehicle(int n) {
        assert(n < (int) vehicles_.size());
        return vehicles_[n];
    }

    VehicleInstance *loadInstance(uint8 *data, int map);

protected:
    // TODO:remove this(?)
    std::vector<Vehicle *> vehicles_;
};

#endif
