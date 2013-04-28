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

#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>

#include "mapobject.h"
#include "ped.h"

/*!
 * This class holds informations about the animation of a vehicle.
 */
class VehicleAnimation {
public:
    /*!
     * This enumeration lists the type of animations.
     */
    enum AnimationType {
        //! Vehicule under normal condition
        kNormalAnim,
        //! Burning vehicle
        kOnFireAnim,
        //! Burnt vehicle
        kBurntAnim,
    } ;

    VehicleAnimation();
    virtual ~VehicleAnimation() {}
    
    //! Draw the vehicle
    void draw(int x, int y, int dir, int frame);

    void set_base_anims(int anims);
    //void setAnimsBurning(int anims) { anims_burning_ = anims; }
    //void setAnimsBurnt(int anims) { anims_burnt_ = anims; }
    //! Sets the current animation for the vehicle
    void set_animation_type(AnimationType anim) {
        vehicle_anim_ = anim;
    }
    //! Returns the current vehicle animation
    AnimationType animation_type() {
        return vehicle_anim_;
    }
protected:
    int anims_, anims_burning_, anims_burnt_;
    //! Current type of animation
    AnimationType vehicle_anim_;
};

/*!
 * This class represents a Vehicle on a map.
 */
class VehicleInstance : public ShootableMovableMapObject
{
public:
    VehicleInstance(VehicleAnimation *vehicle, int m);
    virtual ~VehicleInstance() { delete vehicle_;}

    bool animate(int elapsed);
    void draw(int x, int y);

    void addDestinationV(int x, int y, int z, int ox = 128, int oy = 128,
            int new_speed = 160) {
        dest_path_.push_back(PathNode(x, y, z, ox, oy));
        speed_ = new_speed;
    }

    void setDestinationV(Mission *m, int x, int y, int z, int ox = 128, int oy = 128,
            int new_speed = 160);

    PedInstance *getDriver(void) {
        return vehicle_driver_;
    }
    void setDriver(PedInstance *vehicleDriver) {
        if (vehicle_driver_ == NULL)
            vehicle_driver_ = vehicleDriver;
        all_passengers_.insert(vehicleDriver);
    }
    void removeDriver(PedInstance *vehicleDriver);
    void forceSetDriver(PedInstance *vehicleDriver) {
        vehicle_driver_ = vehicleDriver;
        all_passengers_.insert(vehicleDriver);
    }
    bool hasDriver() { return (vehicle_driver_ != NULL); }
    bool isDriver(PedInstance *vehicleDriver) {
        if (vehicle_driver_ == NULL)
            return false;
        return (vehicle_driver_ == vehicleDriver);
    }
    bool isInsideVehicle(PedInstance *p) {
        return (all_passengers_.find(p) != all_passengers_.end());
    }

    bool handleDamage(ShootableMapObject::DamageInflictType *d);
    bool checkHostilesInside(PedInstance *p, unsigned int hostile_desc_alt);

protected:
    bool move_vehicle(int elapsed);
    bool walkable(int x, int y, int z);
    uint16 tileDir(int x, int y, int z);
    bool dirWalkable(PathNode *p, int x, int y, int z);

protected:
    VehicleAnimation *vehicle_;
    PedInstance *vehicle_driver_;
    std::set <PedInstance *> all_passengers_;
};

#endif
