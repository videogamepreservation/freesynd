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

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "app.h"
#include "gfx/screen.h"

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
    g_App.gameSprites().drawFrame(anims_ + dir * 2, frame, x, y);
}

void Vehicle::drawOnFire(int x, int y, int dir, int frame)
{
    g_App.gameSprites().drawFrame(anims_burning_ + dir, frame, x, y);
}

void Vehicle::drawBurnt(int x, int y, int dir, int frame)
{
    g_App.gameSprites().drawFrame(anims_burnt_ + dir, frame, x, y);
}

VehicleInstance::VehicleInstance(Vehicle * vehicle,
int m):ShootableMovableMapObject(m), vehicle_(vehicle), vehicle_driver_(NULL)
{
    hold_on_.wayFree = 0;
    rcv_damage_def_ = MapObject::ddmg_Vehicle;
    vehicle_->setVehicleAnim(Vehicle::NormalAnim);
}

bool VehicleInstance::animate(int elapsed)
{
    bool updated = false;

    if (health_ <= 0) {
        vehicle_driver_ = 0;
        for (std::set <PedInstance *> ::iterator it = all_passengers_.begin();
            it != all_passengers_.end(); it++) {
                //TODO: inflict damage on explosion
                (*it)->leaveVehicle();
        }
        all_passengers_.clear();
        dest_path_.clear();
    } else {
        updated = movementV(elapsed);
    }

    switch (vehicle_->getVehicleAnim()) {
        case Vehicle::NormalAnim:
            break;
        case Vehicle::OnFireAnim:
            if (leftTimeShowAnim(elapsed))
                updated |= MapObject::animate(elapsed);
            else {
                vehicle_->setVehicleAnim(Vehicle::BurntAnim);
                frame_ = 0;
                updated = true;
            }
            break;
        case Vehicle::BurntAnim:
            break;
    }

    return updated;
}

void VehicleInstance::draw(int x, int y)
{
    addOffs(x, y);

    // ensure on map
    if (x < 90 || y < -20)
        return;

    switch (vehicle_->getVehicleAnim()) {
        case Vehicle::NormalAnim:
            vehicle_->draw(x, y, getDirection(4), frame_);
            break;
        case Vehicle::OnFireAnim:
            vehicle_->drawOnFire(x, y, getDirection(4), frame_);
            break;
        case Vehicle::BurntAnim:
            vehicle_->drawBurnt(x, y, getDirection(4), frame_);
            break;
    }
}

bool VehicleInstance::walkable(int x, int y, int z)
{
    uint8 thisTile = g_App.maps().map(map())->tileAt(x, y, z);
    uint8 isRoad = g_App.walkdata_[thisTile];

    if(thisTile == 80) {
        uint8 nearTile = g_App.walkdata_[g_App.maps().map(map())->tileAt(x, y - 1, z)];
        if((nearTile < 0x06 || nearTile > 0x09) && nearTile != 0x0E)
            return false;
        nearTile = g_App.walkdata_[g_App.maps().map(map())->tileAt(x, y + 1, z)];
        if((nearTile < 0x06 || nearTile > 0x09) && nearTile != 0x0E)
            return false;
        return true;
    }
    if(thisTile == 81) {
        uint8 nearTile = g_App.walkdata_[g_App.maps().map(map())->tileAt(x - 1, y, z)];
        if((nearTile < 0x06 || nearTile > 0x09) && nearTile != 0x0E)
            return false;
        nearTile = g_App.walkdata_[g_App.maps().map(map())->tileAt(x + 1, y, z)];
        if((nearTile < 0x06 || nearTile > 0x09) && nearTile != 0x0E)
            return false;
        return true;
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

uint32 VehicleInstance::tileDir(int x, int y, int z) {
    int dir = 0;

    switch(g_App.maps().map(map())->tileAt(x, y, z)){
        case 80:
            if(g_App.maps().map(map())->tileAt(x + 1, y, z) == 80)
                dir = (0)|(0xFFFFFF00);
            if(g_App.maps().map(map())->tileAt(x - 1, y, z) == 80)
                dir = (4<<16)|(0xFF00FFFF);
            break;
        case 81:
            if(g_App.maps().map(map())->tileAt(x, y - 1, z) == 81)
                dir = (2<<8)|(0xFFFF00FF);
            if(g_App.maps().map(map())->tileAt(x, y + 1, z) == 81)
                dir = (6<<24)|(0x00FFFFFF);
            break;
        case 106:
            dir = (0)|(2<<8)|(6<<24)|(0x00FF0000);

            if(g_App.maps().map(map())->tileAt(x + 1, y - 1, z) != 118)
                dir |= 0x00FFFF00;
            if(g_App.maps().map(map())->tileAt(x + 1, y + 1, z) != 118)
                dir |= 0xFFFF0000;

            break;
        case 107:
            dir = (2<<8)|(4<<16)|(6<<24)|(0x000000FF);

            if(g_App.maps().map(map())->tileAt(x - 1, y - 1, z) != 118)
                dir |= 0xFF0000FF;
            if(g_App.maps().map(map())->tileAt(x - 1, y + 1, z) != 118)
                dir |= 0x0000FFFF;

            break;
        case 108:
            dir = (0)|(2<<8)|(4<<16)|(0xFF000000);

            if(g_App.maps().map(map())->tileAt(x + 1, y - 1, z) != 118)
                dir |= 0xFF0000FF;
            if(g_App.maps().map(map())->tileAt(x - 1, y - 1, z) != 118)
                dir |= 0xFFFF0000;

            break;
        case 109:
            dir = (0)|(4<<16)|(6<<24)|(0x0000FF00);

            if(g_App.maps().map(map())->tileAt(x + 1, y + 1, z) != 118)
                dir |= 0x0000FFFF;
            if(g_App.maps().map(map())->tileAt(x - 1, y + 1, z) != 118)
                dir |= 0x00FFFF00;

            break;
        case 110:
            dir = (0) | (2<<8)|(0xFFFF0000);
            break;
        case 111:
            dir = (0) | (6<<24)|(0x00FFFF00);
            break;
        case 112:
            dir = (2<<8)|(4<<16)|(0xFF0000FF);
            break;
        case 113:
            dir = (4<<16)|(6<<24)|(0x0000FFFF);
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
        case 225:
            if(g_App.walkdata_[g_App.maps().map(map())->tileAt(x + 1, y, z)] == 0x0E)
                dir = (0)|(0xFFFFFF00);
            if(g_App.walkdata_[g_App.maps().map(map())->tileAt(x - 1, y, z)] == 0x0E)
                dir = (4<<16)|(0xFF00FFFF);
            break;
        case 226:
            if(g_App.walkdata_[g_App.maps().map(map())->tileAt(x, y - 1, z)] == 0x0E)
                dir = (2<<8)|(0xFFFF00FF);
            if(g_App.walkdata_[g_App.maps().map(map())->tileAt(x, y + 1, z)] == 0x0E)
                dir = (6<<24)|(0x00FFFFFF);
            break;
        default:
            dir = 0xFFFFFFFF;
    }

    return dir;
}

bool VehicleInstance::dirWalkable(PathNode *p, int x, int y, int z) {

    if(!(walkable(x,y,z)))
        return false;

    uint32 dirStart = tileDir(p->tileX(),p->tileY(),p->tileZ());
    uint32 dirEnd = tileDir(x,y,z);
    if (dirStart == 0xFFFFFFFF || dirEnd == 0xFFFFFFFF)
        return true;

    if (((dirStart & 0xFF000000) != 0xFF000000)
        || ((dirEnd & 0xFF000000) != 0xFF000000))
        if ((dirStart & 0xFF000000) == (dirEnd & 0xFF000000))
                return true;
    if (((dirStart & 0x00FF0000) != 0x00FF0000)
        || ((dirEnd & 0x00FF0000) != 0x00FF0000))
        if ((dirStart & 0x00FF0000) == (dirEnd & 0x00FF0000))
                return true;
    if (((dirStart & 0x0000FF00) != 0x0000FF00)
        || ((dirEnd & 0x0000FF00) != 0x0000FF00))
        if ((dirStart & 0x0000FF00) == (dirEnd & 0x0000FF00))
                return true;
    if (((dirStart & 0x000000FF) != 0x000000FF)
        || ((dirEnd & 0x000000FF) != 0x000000FF))
        if ((dirStart & 0x000000FF) == (dirEnd & 0x000000FF))
                return true;

    return false;
}

void VehicleInstance::setDestinationV(Mission *m, int x, int y, int z, int ox,
                                       int oy, int new_speed)
{
    std::set < PathNode > open, closed;
    std::map < PathNode, PathNode > parent;

    m->adjXYZ(x, y, z);
    z = tile_z_;

    dest_path_.clear();
    setSpeed(0);

    if (map_ == -1 || health_ <= 0 || !(walkable(x, y, z)))
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
    int watchDog = 1000;
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
        uint32 goodDir = tileDir(p.tileX(), p.tileY(), p.tileZ());

        if (p.tileX() > 0) {
            if (dirWalkable(&p,p.tileX() - 1, p.tileY(), p.tileZ())
                && ((goodDir & 0xFF000000) == 0x06000000 || goodDir == 0xFFFFFFFF))
                neighbours.
                    push_back(PathNode(p.tileX() - 1, p.tileY(), p.tileZ()));
        }

        if (p.tileX() < g_App.maps().map(map())->maxX()) {
            if (dirWalkable(&p,p.tileX() + 1, p.tileY(), p.tileZ())
                && ((goodDir & 0x0000FF00) == 0x00000200 || goodDir == 0xFFFFFFFF))
                neighbours.
                    push_back(PathNode(p.tileX() + 1, p.tileY(), p.tileZ()));
        }

        if (p.tileY() > 0)
            if (dirWalkable(&p,p.tileX(), p.tileY() - 1, p.tileZ())
                && ((goodDir & 0x00FF0000) == 0x00040000 || goodDir == 0xFFFFFFFF))
                neighbours.
                    push_back(PathNode(p.tileX(), p.tileY() - 1, p.tileZ()));

        if (p.tileY() < g_App.maps().map(map())->maxY())
            if (dirWalkable(&p,p.tileX(), p.tileY() + 1, p.tileZ())
                && ((goodDir & 0x000000FF) == 0x0 || goodDir == 0xFFFFFFFF))
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

    if(!dest_path_.empty()) {
        // Adjusting offsets for correct positioning
        speed_ = new_speed;
        for(std::list < PathNode >::iterator it = dest_path_.begin();
            it != dest_path_.end(); it++) {

            // TODO : requires testing for correct offsets per
            // every direction, because in some part of game
            // vehicle position on start of game can create incorrect
            // visual representation
            // maybe offsets depend on type or tileZ?
            switch(tileDir(it->tileX(), it->tileY(), it->tileZ())) {
                case 0xFFFFFF00:
                case 0xFFFF0200:
                    it->setOffX(200);
                    it->setOffY(32);
                    break;
                case 0xFF04FFFF:
                    it->setOffX(32);
                    it->setOffY(200);
                    break;
                case 0xFFFF02FF:
                case 0xFF0402FF:
                    it->setOffX(32);
                    it->setOffY(32);
                    break;
                case 0x06FFFFFF:
                case 0x0604FFFF:
                    it->setOffX(32);
                    it->setOffY(200);
                    break;
                case 0x06FFFF00:
                    it->setOffX(200);
                    it->setOffY(200);
                    break;
                default:
                    printf("hmm tileDir %X\n", (unsigned int)tileDir(it->tileX(), it->tileY(), it->tileZ()));
                    break;
            }

        }
    }
}

bool VehicleInstance::movementV(int elapsed)
{
    bool updated = false;

    if (!dest_path_.empty()) {
        if (hold_on_.wayFree == 1) {
            return updated;
        } else if (hold_on_.wayFree == 2){
            dest_path_.clear();
            speed_ = 0;
            return updated;
        }
        int adx =
            dest_path_.front().tileX() * 256 + dest_path_.front().offX();
        int ady =
            dest_path_.front().tileY() * 256 + dest_path_.front().offY();
        int atx = tile_x_ * 256 + off_x_;
        int aty = tile_y_ * 256 + off_y_;
        int diffx = adx - atx, diffy = ady - aty;

        if (abs(diffx) < 16 && abs(diffy) < 16) {
            off_y_ = dest_path_.front().offY();
            off_x_ = dest_path_.front().offX();
            tile_y_ = dest_path_.front().tileY();
            tile_x_ = dest_path_.front().tileX();
            dest_path_.pop_front();
            if (dest_path_.size() == 0)
                speed_ = 0;
            updated = true;
        } else {
            setDirection(diffx, diffy, &dir_);
            int dx = 0, dy = 0;
            double d = sqrt((double)(diffx * diffx + diffy * diffy));

            if (abs(diffx) > 0)
                // dx = diffx * (speed_ * elapsed / 1000) / d;
                dx = (int)((diffx * (speed_ * elapsed) / d) / 1000);
            if (abs(diffy) > 0)
                // dy = diffy * (speed_ * elapsed / 1000) / d;
                dy = (int)((diffy * (speed_ * elapsed) / d) / 1000);

            if (abs(dx) > abs(diffx))
                dx = diffx;
            if (abs(dy) > abs(diffy))
                dy = diffy;
            updatePlacement(off_x_ + dx, off_y_ + dy);
#if 0
            if (updatePlacement(off_x_ + dx, off_y_ + dy)) {
                ;
            } else {
                // TODO: avoid obstacles.
                speed_ = 0;
            }
#endif
            if(dest_path_.front().tileX() == tile_x_
                && dest_path_.front().tileY() == tile_y_
                && dest_path_.front().offX() == off_x_
                && dest_path_.front().offY() == off_y_)
                dest_path_.pop_front();
            if (dest_path_.size() == 0)
                speed_ = 0;

            updated = true;
        }
    } else if (speed_) {
        printf("Destination Unknown, full speed driving = %i ... doing full stop\n", speed_);
        speed_ = 0;
    }

    return updated;
}


bool VehicleInstance::handleDamage(ShootableMapObject::DamageInflictType *d) {
    if (health_ < 0 || d->dtype == MapObject::dmg_Mental)
        return false;
    health_ -= d->dvalue;
    if (health_ <= 0){
        speed_ = 0;
        clearDestination();
        switch ((unsigned int)d->dtype) {
            case MapObject::dmg_Bullet:
            case MapObject::dmg_Laser:
            case MapObject::dmg_Burn:
            case MapObject::dmg_Explosion:
                vehicle_->setVehicleAnim(Vehicle::OnFireAnim);
                setTimeShowAnim(10000);
                break;
        }
    } // NOTE: maybe reduce speed on hit?
    return true;
}
