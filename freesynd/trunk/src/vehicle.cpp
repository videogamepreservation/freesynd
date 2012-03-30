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
    major_type_ = MapObject::mjt_Vehicle;
}

bool VehicleInstance::animate(int elapsed)
{
    bool updated = false;

    if (health_ > 0) {
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
    y += TILE_HEIGHT / 3;
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
    Tile *p_this_tile = g_App.maps().map(map())->getTileAt(x, y, z);
    uint8 this_tile_id = p_this_tile->id();

    if(this_tile_id == 80) {
        Tile::EType near_type = g_App.maps().map(map())->getTileAt(x, y - 1, z)->type();
        if((near_type < Tile::kRoadSideEW || near_type > Tile::kRoadSideNS) && near_type != Tile::kRoadPedCross)
            return false;
        near_type = g_App.maps().map(map())->getTileAt(x, y + 1, z)->type();
         if((near_type < Tile::kRoadSideEW || near_type > Tile::kRoadSideNS) && near_type != Tile::kRoadPedCross)
            return false;
        return true;
    }
    if(this_tile_id == 81) {
        Tile::EType near_type = g_App.maps().map(map())->getTileAt(x - 1, y, z)->type();
         if((near_type < Tile::kRoadSideEW || near_type > Tile::kRoadSideNS) && near_type != Tile::kRoadPedCross)
            return false;
        near_type = g_App.maps().map(map())->getTileAt(x + 1, y, z)->type();
         if((near_type < Tile::kRoadSideEW || near_type > Tile::kRoadSideNS) && near_type != Tile::kRoadPedCross)
            return false;
        return true;
    }
    if(this_tile_id == 72) {
        return false;
    }

    if(this_tile_id == 119) {
        return false;
    }
    return  p_this_tile->isRoad();
}

uint16 VehicleInstance::tileDir(int x, int y, int z) {
    uint16 dir = 0;

    switch(g_App.maps().map(map())->tileAt(x, y, z)){
        case 80:
            if(g_App.maps().map(map())->tileAt(x + 1, y, z) == 80)
                dir = (0)|(0xFFF0);
            if(g_App.maps().map(map())->tileAt(x - 1, y, z) == 80)
                dir = (4<<8)|(0xF0FF);
            break;
        case 81:
            if(g_App.maps().map(map())->tileAt(x, y - 1, z) == 81)
                dir = (2<<4)|(0xFF0F);
            if(g_App.maps().map(map())->tileAt(x, y + 1, z) == 81)
                dir = (6<<12)|(0x0FFF);
            break;
        case 106:
            dir = (0)|(2<<4)|(6<<12)|(0x0F00);

            if(g_App.maps().map(map())->tileAt(x + 1, y - 1, z) != 118)
                dir |= 0x0FF0;
            if(g_App.maps().map(map())->tileAt(x + 1, y + 1, z) != 118)
                dir |= 0xFF00;

            break;
        case 107:
            dir = (2<<4)|(4<<8)|(6<<12)|(0x000F);

            if(g_App.maps().map(map())->tileAt(x - 1, y - 1, z) != 118)
                dir |= 0xF00F;
            if(g_App.maps().map(map())->tileAt(x - 1, y + 1, z) != 118)
                dir |= 0x00FF;

            break;
        case 108:
            dir = (0)|(2<<4)|(4<<8)|(0xF000);

            if(g_App.maps().map(map())->tileAt(x + 1, y - 1, z) != 118)
                dir |= 0xF00F;
            if(g_App.maps().map(map())->tileAt(x - 1, y - 1, z) != 118)
                dir |= 0xFF00;

            break;
        case 109:
            dir = (0)|(4<<8)|(6<<12)|(0x00F0);

            if(g_App.maps().map(map())->tileAt(x + 1, y + 1, z) != 118)
                dir |= 0x00FF;
            if(g_App.maps().map(map())->tileAt(x - 1, y + 1, z) != 118)
                dir |= 0x0FF0;

            break;
        case 110:
            dir = (0) | (2<<4)|(0xFF00);
            break;
        case 111:
            dir = (0) | (6<<12)|(0x0FF0);
            break;
        case 112:
            dir = (2<<4)|(4<<8)|(0xF00F);
            break;
        case 113:
            dir = (4<<8)|(6<<12)|(0x00FF);
            break;
        case 120:
            dir = (0)|(2<<4)|(0xFF00);
            break;
        case 121:
            dir = (0)|(6<<12)|(0x0FF0);
            break;
        case 122:
            dir = (4<<8)|(6<<12)|(0x00FF);
            break;
        case 123:
            dir = (2<<4)|(4<<8)|(0xF00F);
            break;
        case 225:
            if(g_App.maps().map(map())->getTileAt(x + 1, y, z)->type() == Tile::kRoadPedCross)
                dir = (0)|(0xFFF0);
            if(g_App.maps().map(map())->getTileAt(x - 1, y, z)->type() == Tile::kRoadPedCross)
                dir = (4<<8)|(0xF0FF);
            break;
        case 226:
            if(g_App.maps().map(map())->getTileAt(x, y - 1, z)->type() == Tile::kRoadPedCross)
                dir = (2<<4)|(0xFF0F);
            if(g_App.maps().map(map())->getTileAt(x, y + 1, z)->type() == Tile::kRoadPedCross)
                dir = (6<<12)|(0x0FFF);
            break;
        default:
            dir = 0xFFFF;
    }

    return dir;
}

bool VehicleInstance::dirWalkable(PathNode *p, int x, int y, int z) {

    if(!(walkable(x,y,z)))
        return false;

    uint16 dirStart = tileDir(p->tileX(),p->tileY(),p->tileZ());
    uint16 dirEnd = tileDir(x,y,z);
    if (dirStart == 0xFFFF || dirEnd == 0xFFFF)
        return true;

    if (((dirStart & 0xF000) != 0xF000)
        || ((dirEnd & 0xF000) != 0xF000))
        if ((dirStart & 0xF000) == (dirEnd & 0xF000))
                return true;
    if (((dirStart & 0x0F00) != 0x0F00)
        || ((dirEnd & 0x0F00) != 0x0F00))
        if ((dirStart & 0x0F00) == (dirEnd & 0x0F00))
                return true;
    if (((dirStart & 0x00F0) != 0x00F0)
        || ((dirEnd & 0x00F0) != 0x00F0))
        if ((dirStart & 0x00F0) == (dirEnd & 0x00F0))
                return true;
    if (((dirStart & 0x000F) != 0x000F)
        || ((dirEnd & 0x000F) != 0x000F))
        if ((dirStart & 0x000F) == (dirEnd & 0x000F))
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
             it != open.end(); it++)
        {
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
            || watchDog < 0)
        {
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
        uint16 goodDir = tileDir(p.tileX(), p.tileY(), p.tileZ());

        if (p.tileX() > 0) {
            if (dirWalkable(&p,p.tileX() - 1, p.tileY(), p.tileZ())
                && ((goodDir & 0xF000) == 0x6000 || goodDir == 0xFFFF))
                neighbours.
                    push_back(PathNode(p.tileX() - 1, p.tileY(), p.tileZ()));
        }

        if (p.tileX() < g_App.maps().map(map())->maxX()) {
            if (dirWalkable(&p,p.tileX() + 1, p.tileY(), p.tileZ())
                && ((goodDir & 0x00F0) == 0x0020 || goodDir == 0xFFFF))
                neighbours.
                    push_back(PathNode(p.tileX() + 1, p.tileY(), p.tileZ()));
        }

        if (p.tileY() > 0)
            if (dirWalkable(&p,p.tileX(), p.tileY() - 1, p.tileZ())
                && ((goodDir & 0x0F00) == 0x0400 || goodDir == 0xFFFF))
                neighbours.
                    push_back(PathNode(p.tileX(), p.tileY() - 1, p.tileZ()));

        if (p.tileY() < g_App.maps().map(map())->maxY())
            if (dirWalkable(&p,p.tileX(), p.tileY() + 1, p.tileZ())
                && ((goodDir & 0x000F) == 0x0 || goodDir == 0xFFFF))
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
                case 0xFFF0:
                case 0xFF20:
                    it->setOffX(200);
                    it->setOffY(32);
                    break;
                case 0xF4FF:
                    it->setOffX(32);
                    it->setOffY(200);
                    break;
                case 0xFF2F:
                case 0xF42F:
                    it->setOffX(32);
                    it->setOffY(32);
                    break;
                case 0x6FFF:
                case 0x64FF:
                    it->setOffX(32);
                    it->setOffY(200);
                    break;
                case 0x6FF0:
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
    if (all_passengers_.size() != 0) {
        for (std::set<PedInstance *>::iterator it = all_passengers_.begin();
            it != all_passengers_.end(); it++
        ) {
            (*it)->setPosition(tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
            (*it)->setVisZ(tile_z_);
        }
    }

    return updated;
}


bool VehicleInstance::handleDamage(ShootableMapObject::DamageInflictType *d) {
    if (health_ < 0 || d->dtype == MapObject::dmg_Mental)
        return false;
    health_ -= d->dvalue;
    if (health_ <= 0) {
        speed_ = 0;
        is_ignored_ = true;
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
        vehicle_driver_ = NULL;
        while (all_passengers_.size() != 0)
        {
            PedInstance *p = *(all_passengers_.begin());
            p->leaveVehicle();
            removeDriver(p);
        }
        Mission *m = g_Session.getMission();
        std::vector<ShootableMapObject *> all_targets;
        DamageInflictType dit;
        dit.d_owner = this;
        dit.dvalue = 16;
        dit.dtype = MapObject::dmg_Explosion;
        dit.ddir = -1;
        toDefineXYZ xyz;
        this->convertPosToXYZ(&xyz);
        xyz.z += 8;
        m->getInRangeAll(&xyz, all_targets, Weapon::stm_AllObjects,
            true, 512.0);
        for (std::vector<ShootableMapObject *>::iterator it = all_targets.begin();
            it != all_targets.end(); it++
        ) {
        // TODO: set directiom?
            (*it)->handleDamage(&dit);
        }
        rangeDamageAnim(xyz, 512, 16, SFXObject::sfxt_ExplosionFire);
    } else {// NOTE: maybe reduce speed on hit?
        // TODO: let passengers know that vehicle is attacked
    }
    return true;
}
