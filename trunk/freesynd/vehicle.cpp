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
    bool updated = movementV(elapsed);

    if (health_ <= start_health_) {
        return MapObject::animate(elapsed);
    }

    return updated;
}

void VehicleInstance::draw(int x, int y)
{
    addOffs(x, y);

    // ensure on map
    if (x < 90 || y < -20)
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
    uint8 thisTile = g_App.maps().map(map())->tileAt(x, y, z);
    uint8 isRoad = g_App.walkdata_[thisTile];

    if(thisTile == 80) {
        uint8 nearTile = g_App.walkdata_[g_App.maps().map(map())->tileAt(x, y - 1, z)];
        if(nearTile < 0x06 || nearTile > 0x09)
            return false;
        nearTile = g_App.walkdata_[g_App.maps().map(map())->tileAt(x, y + 1, z)];
        if(nearTile < 0x06 || nearTile > 0x09)
            return false;
    }
    if(thisTile == 81) {
        uint8 nearTile = g_App.walkdata_[g_App.maps().map(map())->tileAt(x - 1, y, z)];
        if(nearTile < 0x06 || nearTile > 0x09)
            return false;
        nearTile = g_App.walkdata_[g_App.maps().map(map())->tileAt(x + 1, y, z)];
        if(nearTile < 0x06 || nearTile > 0x09)
            return false;
    }
    if(thisTile == 72) {
        return false;
    }

    if(thisTile == 119) {
        return false;
    }
    return  (isRoad == 0x0B || isRoad == 0x0E
        || (isRoad > 0x05 && isRoad < 0x0A));
}

int VehicleInstance::tileDir(int x, int y, int z) {
    int dir = 0;

    switch(g_App.maps().map(map())->tileAt(x, y, z)){
        case 106:
            dir = (0)|(0xFFFFFF00);
            break;
        case 107:
            dir = (4<<16)|(0xFF00FFFF);
            break;
        case 108:
            dir = (2<<8)|(0xFFFF00FF);
            break;
        case 109:
            dir = (6<<24)|(0x00FFFFFF);
            break;
        case 110:
            dir = (0) | (2<<8)|(0xFFFF0000);
            break;
        case 111:
            dir = (0) | (6<<24)|(0x00FFFF00);
            break;
        case 112:
            dir = (4<<16)|(2<<8)|(0xFF0000FF);
            break;
        case 113:
            dir = (4<<16)|(6<<24)|(0x0000FFFF);
            break;
        case 119:
            dir = 0xFFFFFFFF;
            break;
        case 120:
            dir = (0)|(2<<8)|(0xFFFF0000);
            break;
        case 121:
            dir = (0)|(6<<24)|(0x00FFFF00);
            break;
        case 122:
            dir = (4<<16)|(6<<24)|(0x0000FFFF);
            break;
        case 123:
            dir = (2<<8)|(4<<16)|(0xFF0000FF);
            break;
            /*
        case 225:
            dir = (2<<8)|(4<<16)|(0xFF0000FF);
            break;
        case 226:
            dir = (2<<8)|(4<<16)|(0xFF0000FF);
            break;
            */
        default:
            dir = 0xFFFFFFFF;
    }

    return dir;
}

bool VehicleInstance::dirWalkable(PathNode *p, int x, int y, int z) {

    if(!(walkable(x,y,z)))
        return false;
    int dirStart = tileDir(p->tileX(),p->tileY(),p->tileZ());
    int dirEnd = tileDir(x,y,z);
    if (dirStart == 0xFFFFFFFF || dirEnd == 0xFFFFFFFF)
        return true;

    if ((dirStart >> 24) != 0x0F && (dirEnd >> 24) != 0x0F)
        if ((dirStart >> 24) == (dirEnd >> 24))
            if((p->tileX() - x) == 1 && (y - p->tileY()) == 0)
                return true;
    if ((dirStart >> 16) != 0x0F && (dirEnd >> 16) != 0x0F)
        if ((dirStart >> 16) == (dirEnd >> 16))
            if((p->tileY() - y) == 1 && (x - p->tileX()) == 0)
                return true;
    if ((dirStart >> 8) != 0x0F && (dirEnd >> 8) != 0x0F)
        if ((dirStart >> 8) == (dirEnd >> 8))
            if((x - p->tileX()) == 1 && (y - p->tileY()) == 0)
                return true;
    if ((dirStart & 0xFF) != 0x0F && (dirEnd & 0xFF) != 0x0F)
        if ((dirStart & 0xFF) == (dirEnd & 0xFF))
            if((y - p->tileY()) == 1 && (x - p->tileX()) == 0)
                return true;

    return false;
}

void VehicleInstance::setDestinationV(int x, int y, int z, int ox,
                                               int oy, int new_speed)
{
    std::set < PathNode > open, closed;
    std::map < PathNode, PathNode > parent;

    z = tile_z_;

    dest_path_.clear();
    setSpeed(0);

    if (map_ == -1 || health_ <= 0
        || !(walkable(x, y, z)))
        return;

    if (!walkable(tile_x_, tile_y_, tile_z_)) {
        float dBest = 100000, dCur;
        int xBest,yBest;
        // we got somewhere we shouldn't, we need to find somewhere that is walkable
        for (int j = 0; j < 5; j++)
            for (int i = 0; i < 5; i++)
                if (walkable(tile_x_ + i, tile_y_ + j, tile_z_)) {
                    dCur = sqrt((float)(i*i + j*j));
                    if(dCur < dBest) {
                        xBest = tile_x_ + i;
                        yBest = tile_y_ + j;
                        dBest = dCur;
                    }
                }
        for (int j = 0; j < 5; j++)
            for (int i = 0; i > -5; --i)
                if (walkable(tile_x_ + i, tile_y_ + j, tile_z_)) {
                    dCur = sqrt((float)(i*i + j*j));
                    if(dCur < dBest) {
                        xBest = tile_x_ + i;
                        yBest = tile_y_ + j;
                        dBest = dCur;
                    }
                }
        for (int j = 0; j > -5; --j)
            for (int i = 0; i > -5; --i)
                if (walkable(tile_x_ + i, tile_y_ + j, tile_z_)) {
                    dCur = sqrt((float)(i*i + j*j));
                    if(dCur < dBest) {
                        xBest = tile_x_ + i;
                        yBest = tile_y_ + j;
                    }
                }
        for (int j = 0; j > -5; --j)
            for (int i = 0; i < 5; i++)
                if (walkable(tile_x_ + i, tile_y_ + j, tile_z_)) {
                    dCur = sqrt((float)(i*i + j*j));
                    if(dCur < dBest) {
                        xBest = tile_x_ + i;
                        yBest = tile_y_ + j;
                        dBest = dCur;
                    }
                }
        if(dBest == 100000)
            return;
        else {
            tile_x_ = xBest;
            tile_y_ = yBest;
        }
    }

    PathNode closest;
    float closest_dist = 100000;

    open.insert(PathNode(tile_x_, tile_y_, tile_z_, off_x_, off_y_));
    int watchDog = 3000;
    while (!open.empty()) {
        watchDog--;
        float dist = 100000;
        PathNode p;
        std::set < PathNode >::iterator pit;
        for (std::set < PathNode >::iterator it = open.begin();
             it != open.end(); it++) {
            float d =
                sqrt((float) (x - it->tileX()) * (x - it->tileX()) +
                     (y - it->tileY()) * (y - it->tileY()));
            if (d < dist) {
                dist = d;
                p = *it;
                pit = it;       // it cannot be const_iterator because of this assign
            }
        }
        if (dist < closest_dist) {
            closest = p;
            closest_dist = dist;
        }
        //printf("found best dist %f in %i nodes\n", dist, open.size());
        open.erase(pit);
        closed.insert(p);

        if ((p.tileX() == x && p.tileY() == y && p.tileZ() == z)
            || watchDog < 0) {
            if (watchDog < 0) {
                p = closest;
                dest_path_.
                    push_front(PathNode
                               (p.tileX(), p.tileY(), p.tileZ(), ox, oy));
            } else
                dest_path_.push_front(PathNode(x, y, z, ox, oy));
            while (parent.find(p) != parent.end()) {
                p = parent[p];
                if (p.tileX() == tile_x_ && p.tileY() == tile_y_
                    && p.tileZ() == tile_z_)
                    break;
                dest_path_.push_front(p);
            }
            break;
        }

        std::list < PathNode > neighbours;
        if (p.tileX() > 0) {
            if (dirWalkable(&p,p.tileX() - 1, p.tileY(), p.tileZ()))
                neighbours.
                    push_back(PathNode(p.tileX() - 1, p.tileY(), p.tileZ()));
            if (p.tileY() > 0) {
                // check for fences
                if (dirWalkable(&p,p.tileX() - 1, p.tileY(), p.tileZ()) &&
                    dirWalkable(&p,p.tileX(), p.tileY() - 1, p.tileZ()))
                    neighbours.
                        push_back(PathNode
                                  (p.tileX() - 1, p.tileY() - 1,
                                   p.tileZ()));
            }
            if (p.tileY() < g_App.maps().map(map())->maxY()) {
                // check for fences
                if (dirWalkable(&p,p.tileX() - 1, p.tileY(), p.tileZ()) &&
                    dirWalkable(&p,p.tileX(), p.tileY() + 1, p.tileZ()))
                    neighbours.
                        push_back(PathNode
                                  (p.tileX() - 1, p.tileY() + 1,
                                   p.tileZ()));
            }
        }
        if (p.tileX() < g_App.maps().map(map())->maxX()) {
            if (dirWalkable(&p,p.tileX() + 1, p.tileY(), p.tileZ()))
                neighbours.
                    push_back(PathNode(p.tileX() + 1, p.tileY(), p.tileZ()));
            if (p.tileY() > 0) {
                // check for fences
                if (dirWalkable(&p,p.tileX() + 1, p.tileY(), p.tileZ()) &&
                    dirWalkable(&p,p.tileX(), p.tileY() - 1, p.tileZ()))
                    neighbours.
                        push_back(PathNode
                                  (p.tileX() + 1, p.tileY() - 1,
                                   p.tileZ()));
            }
            if (p.tileY() < g_App.maps().map(map())->maxY()) {
                // check for fences
                if (dirWalkable(&p,p.tileX() + 1, p.tileY(), p.tileZ()) &&
                    dirWalkable(&p,p.tileX(), p.tileY() + 1, p.tileZ()))
                    neighbours.
                        push_back(PathNode
                                  (p.tileX() + 1, p.tileY() + 1,
                                   p.tileZ()));
            }
        }
        if (p.tileY() > 0)
            if (dirWalkable(&p,p.tileX(), p.tileY() - 1, p.tileZ()))
                neighbours.
                    push_back(PathNode(p.tileX(), p.tileY() - 1, p.tileZ()));
        if (p.tileY() < g_App.maps().map(map())->maxY())
            if (dirWalkable(&p,p.tileX(), p.tileY() + 1, p.tileZ()))
                neighbours.
                    push_back(PathNode(p.tileX(), p.tileY() + 1, p.tileZ()));

        for (std::list < PathNode >::iterator it = neighbours.begin();
             it != neighbours.end(); it++)
            if (dirWalkable(&p,it->tileX(), it->tileY(), it->tileZ())
                && open.find(*it) == open.end()
                && closed.find(*it) == closed.end()) {
                parent[*it] = p;
                open.insert(*it);
            }
    }

    speed_ = new_speed;
}

bool VehicleInstance::movementV(int elapsed)
{
    bool updated = false;

    if (!dest_path_.empty()) {
        int adx =
            dest_path_.front().tileX() * 256 + dest_path_.front().offX();
        int ady =
            dest_path_.front().tileY() * 256 + dest_path_.front().offY();
        int atx = tile_x_ * 256 + off_x_;
        int aty = tile_y_ * 256 + off_y_;

        if (abs(adx - atx) < 16 && abs(ady - aty) < 16) {
            off_y_ = dest_path_.front().offY();
            off_x_ = dest_path_.front().offX();
            tile_y_ = dest_path_.front().tileY();
            tile_x_ = dest_path_.front().tileX();
            dest_path_.pop_front();
            if (dest_path_.size() == 0)
                speed_ = 0;
            updated = true;
        } else {

            // TODO: something better?
            int fuzz = 16;
            if (ady < (aty - fuzz)) {
                if (adx < (atx - fuzz))
                    dir_ = 5;
                else if (adx > (atx + fuzz))
                    dir_ = 3;
                else if (adx < (atx + fuzz))
                    dir_ = 4;
            } else if (abs(ady - aty) < fuzz) {
                if (adx < (atx - fuzz))
                    dir_ = 6;
                else if (adx > (atx + fuzz))
                    dir_ = 2;
            } else if (abs(ady - aty) > fuzz) {
                if (adx < (atx - fuzz))
                    dir_ = 7;
                else if (adx > (atx + fuzz))
                    dir_ = 1;
                else if (adx < (atx + fuzz))
                    dir_ = 0;
            }

            int dx = 0, dy = 0;
            int d =
                (int) sqrt((float) (adx - atx) * (adx - atx) +
                           (ady - aty) * (ady - aty));

            if (abs(adx - atx) > 0)
                dx = (adx - atx) * (speed_ * elapsed / 1000) / d;
            if (abs(ady - aty) > 0)
                dy = (ady - aty) * (speed_ * elapsed / 1000) / d;

            if (abs(dx) > abs(adx - atx))
                dx = (adx - atx);
            if (abs(dy) > abs(ady - aty))
                dy = (ady - aty);

            if (updatePlacement(off_x_ + dx, off_y_ + dy) ||
                updatePlacement(off_x_, off_y_ + dy) ||
                updatePlacement(off_x_ + dx, off_y_)) {
                ;
            } else {
                // TODO: avoid obstacles.
                speed_ = 0;
            }

            updated = true;
        }
    } else if (speed_) {
        int dx = 0, dy = 0;
        switch (dir_) {
        case 0:
            dy = speed_;
            break;
        case 1:
            dy = speed_;
            dx = speed_;
            break;
        case 2:
            dx = speed_;
            break;
        case 3:
            dy = -speed_;
            dx = speed_;
            break;
        case 4:
            dy = -speed_;
            break;
        case 5:
            dy = -speed_;
            dx = -speed_;
            break;
        case 6:
            dx = -speed_;
            break;
        case 7:
            dy = speed_;
            dx = -speed_;
            break;
        }

        updatePlacement(off_x_ + dx, off_y_ + dy);
        updated = true;
    }

    return updated;
}
