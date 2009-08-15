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

#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include <math.h>
#include <list>
#include "path.h"

/*!
 * Map object class.
 */
class MapObject {
public:
    MapObject(int m);
    virtual ~MapObject() {}

    void setPosition(int tile_x, int tile_y, int tile_z, int off_x = 0,
            int off_y = 0, int off_z = 0) {
        tile_x_ = tile_x;
        tile_y_ = tile_y;
        tile_z_ = tile_z;
        off_x_ = off_x;
        off_y_ = off_y;
        off_z_ = off_z;
    }

    int tileX() { return tile_x_; }
    void setTileX(int x) { tile_x_ = x; }
    int tileY() { return tile_y_; }
    void setTileY(int y) { tile_y_ = y; }
    int tileZ() { return tile_z_; }
    void setTileZ(int z) { tile_z_ = z; }

    int offX() { return off_x_; }
    int offY() { return off_y_; }
    int offZ() { return off_z_; }

    void setOffX(int n);
    void setOffY(int n);
    void setOffZ(int n);

    int screenX();
    int screenY();

    virtual int map() { return map_; }
    void setMap(int new_map) { map_ = new_map; }

    bool samePosition(MapObject * other) {
        return other->tile_x_ == tile_x_
                && other->tile_y_ == tile_y_
                && other->tile_z_ == tile_z_
                && other->off_x_ == off_x_
                && other->off_y_ == off_y_;
    }

    float distanceTo(MapObject *t) {
        return sqrt((float) (t->tileX() - tileX()) * (t->tileX() - tileX())
                + (t->tileY() - tileY()) * (t->tileY() - tileY())
                + (t->tileZ() - tileZ()) * (t->tileZ() - tileZ()));
    }

    virtual bool animate(int elapsed);

protected:
    int tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_;
    int map_;
    int frame_;
    int elapsed_carry_;
    int frames_per_sec_;

    void addOffs(int &x, int &y);
};

/*!
 * Shootable map object class.
 */
class ShootableMapObject : public MapObject {
public:
    ShootableMapObject(int m);
    virtual ~ShootableMapObject() {}

    int health() { return health_; }

    void setHealth(int n) {
        if (n > 255)
            n = 255;

        if (n < 0)
            n = 0;

        health_ = n;
    }

    int startHealth() { return start_health_; }

    void setStartHealth(int n) {
        if (n > 255)
            n = 255;

        if (n < 0)
            n = 0;

        start_health_ = n;
    }

    virtual void inflictDamage(int d) {
        health_ -= d;

        if (health_ < 0)
            health_ = 0;
    }

  protected:
    int health_, start_health_;
};

/*!
 * Shootable movable map object class.
 */
class ShootableMovableMapObject : public ShootableMapObject {
public:
    ShootableMovableMapObject(int m);
    virtual ~ShootableMovableMapObject() {}

    void setDirection(int dir) {
        if (dir < 0)
            dir = 8 + dir;

        dir_ = dir % 8;
    }

    int direction() {
        return dir_;
    }

    void setSpeed(int new_speed) {
        speed_ = new_speed;
    }

    int speed() {
        return speed_;
    }

    void addDestination(int x, int y, int z, int ox = 128, int oy = 128,
            int new_speed = 160) {
        dest_path_.push_back(PathNode(x, y, z, ox, oy));
        speed_ = new_speed;
    }

    void setDestination(int x, int y, int z, int ox = 128, int oy = 128,
            int new_speed = 160);

    void clearDestination() {
        dest_path_.clear();
    }

    virtual bool animate(int elapsed);

protected:
    int dir_;
    int speed_;
    std::list<PathNode> dest_path_;

    bool updatePlacement(int nOffX, int nOffY);

    virtual bool walkable(int x, int y, int z) = 0;
};

/*!
 * Static map object class.
 */
class Static : public MapObject {
public:
    static Static *loadInstance(uint8 *data, int m);

    virtual void draw(int x, int y) = 0;

protected:
    Static(int m):MapObject(m) {}
    virtual ~Static() {}
};

/*!
 * Door map object class.
 */
class Door : public Static {
public:
    Door(int m, int anim, int openingAnim);
    virtual ~Door() {}

    void draw(int x, int y);

protected:
    int anim_, opening_anim_;
};

/*!
 * Tree map object class.
 */
class Tree : public Static {
public:
    Tree(int m, int anim, int burningAnim);
    virtual ~Tree() {}

    void draw(int x, int y);

protected:
    int anim_, burning_anim_;
};

#endif
