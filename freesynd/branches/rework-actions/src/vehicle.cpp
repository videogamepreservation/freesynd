/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2010  Bohdan Stelmakh <chamel@users.sourceforge.net> *
 *   Copyright (C) 2013  Benoit Blancard <benblan@users.sourceforge.net>*
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
#include "vehicle.h"

VehicleAnimation::VehicleAnimation() {
    vehicle_anim_ = kNormalAnim;
}

void VehicleAnimation::draw(int x, int y, int dir, int frame)
{
    switch (vehicle_anim_) {
        case kNormalAnim:
            g_App.gameSprites().drawFrame(anims_ + dir * 2, frame, x, y);
            break;
        case kOnFireAnim:
            g_App.gameSprites().drawFrame(anims_burning_ + dir, frame, x, y);
            break;
        case kBurntAnim:
            g_App.gameSprites().drawFrame(anims_burnt_ + dir, frame, x, y);
            break;
    }
}

void VehicleAnimation::set_base_anims(int anims) {
    anims_ = anims;
    anims_burning_ = anims + 8;
    anims_burnt_ = anims + 12;
}

void Vehicle::addPassenger(PedInstance *p) {
    passengers_.insert(p);
}

/*!
 * Removes given passenger from vehicle.
 * \param pPed Ped to remove
 */
void Vehicle::dropPassenger(PedInstance *pPed) {
    if(isInsideVehicle(pPed)) {
        pPed->leaveVehicle();
        passengers_.erase(passengers_.find(pPed));
        pPed->setPosition(tile_x_, tile_y_, tile_z_,
        off_x_, off_y_, off_z_);
    }
}

/*!
 * Returns true if at least one of our agent is inside the vehicle.
 */
bool Vehicle::containsOurAgents() {
    for (std::set<PedInstance *>::iterator it = passengers_.begin();
        it != passengers_.end(); it++)
    {
        if ((*it)->isOurAgent()) {
            return true;
        }
    }
    return false;
}

/*!
 * Returns true if the vehicle contains peds considered hostile by the given ped.
 * \param pPed The ped evaluating the hostility of the vehicle
 * \param hostile_desc_alt Parameter for evaluating the hostility
 * \return True if at least one hostile ped is found.
 */
bool Vehicle::containsHostilesForPed(PedInstance* p,
                                          unsigned int hostile_desc_alt)
{
    for (std::set<PedInstance *>::iterator it = passengers_.begin();
        it != passengers_.end(); it++)
    {
        if (p->isHostileTo((ShootableMapObject *)(*it), hostile_desc_alt))
            return true;
    }
    return false;
}

VehicleInstance::VehicleInstance(VehicleAnimation * vehicle, int m):
    Vehicle(m, true), vehicle_(vehicle), vehicle_driver_(NULL)
{
    hold_on_.wayFree = 0;
    rcv_damage_def_ = MapObject::ddmg_Vehicle;
    major_type_ = MapObject::mjt_Vehicle;
}

bool VehicleInstance::animate(int elapsed)
{
    bool updated = false;

    if (health_ > 0) {
        updated = move_vehicle(elapsed);
    }

    if (vehicle_->animation_type() == VehicleAnimation::kOnFireAnim) {
        if (leftTimeShowAnim(elapsed))
            updated |= MapObject::animate(elapsed);
        else {
            vehicle_->set_animation_type(VehicleAnimation::kBurntAnim);
            frame_ = 0;
            updated = true;
        }
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

    vehicle_->draw(x, y, getDirection(4), frame_);
}

bool VehicleInstance::walkable(int x, int y, int z)
{
    Map *pMap = g_App.maps().map(map());
    Tile *p_this_tile = pMap->getTileAt(x, y, z);
    uint8 this_tile_id = p_this_tile->id();

    if(this_tile_id == 80) {
        Tile::EType near_type = pMap->getTileAt(x, y - 1, z)->type();
        if((near_type < Tile::kRoadSideEW || near_type > Tile::kRoadSideNS)
            && near_type != Tile::kRoadPedCross) {
            return false;
        }
        near_type = pMap->getTileAt(x, y + 1, z)->type();
         if((near_type < Tile::kRoadSideEW || near_type > Tile::kRoadSideNS)
             && near_type != Tile::kRoadPedCross)
         {
            return false;
         }
        return true;
    }
    if(this_tile_id == 81) {
        Tile::EType near_type = pMap->getTileAt(x - 1, y, z)->type();
         if((near_type < Tile::kRoadSideEW || near_type > Tile::kRoadSideNS)
             && near_type != Tile::kRoadPedCross)
         {
            return false;
         }
        near_type = pMap->getTileAt(x + 1, y, z)->type();
         if((near_type < Tile::kRoadSideEW || near_type > Tile::kRoadSideNS)
             && near_type != Tile::kRoadPedCross)
         {
            return false;
         }
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
    uint16 dir = 0x0;
    int near_tile;
    Map *pMap = g_App.maps().map(map());

    switch(pMap->tileAt(x, y, z)){
        case 80:
            if(g_App.maps().map(map())->tileAt(x + 1, y, z) == 80)
                dir = (0)|(0xFFF0);
            if(pMap->tileAt(x - 1, y, z) == 80)
                dir = (4<<8)|(0xF0FF);
            break;
        case 81:
            if(pMap->tileAt(x, y - 1, z) == 81)
                dir = (2<<4)|(0xFF0F);
            if(pMap->tileAt(x, y + 1, z) == 81)
                dir = (6<<12)|(0x0FFF);
            break;
        case 106:
            dir = (0)|(2<<4)|(6<<12)|(0x0F00);

            if(pMap->tileAt(x + 1, y - 1, z) != 118)
                dir |= 0x0FF0;
            if(pMap->tileAt(x + 1, y + 1, z) != 118)
                dir |= 0xFF00;
            near_tile = pMap->tileAt(x + 1, y, z);
            if (near_tile == 108 || near_tile == 109)
                dir = (dir & 0x0FFF) | 0x6000;

            break;
        case 107:
            dir = (2<<4)|(4<<8)|(6<<12)|(0x000F);

            if(pMap->tileAt(x - 1, y - 1, z) != 118)
                dir |= 0x00FF;
            if(pMap->tileAt(x - 1, y + 1, z) != 118)
                dir |= 0xF00F;
            near_tile = pMap->tileAt(x - 1, y, z);
            if (near_tile == 108 || near_tile == 109)
                dir = (dir & 0xFF0F) | 0x0020;

            break;
        case 108:
            dir = (0)|(2<<4)|(4<<8)|(0xF000);

            if(pMap->tileAt(x + 1, y - 1, z) != 118)
                dir |= 0xF00F;
            if(pMap->tileAt(x - 1, y - 1, z) != 118)
                dir |= 0xFF00;
            near_tile = pMap->tileAt(x, y - 1, z);
            if (near_tile == 106 || near_tile == 107)
                dir = dir & 0xFFF0;

            break;
        case 109:
            dir = (0)|(4<<8)|(6<<12)|(0x00F0);

            if(pMap->tileAt(x + 1, y + 1, z) != 118)
                dir |= 0x00FF;
            if(pMap->tileAt(x - 1, y + 1, z) != 118)
                dir |= 0x0FF0;
            near_tile = pMap->tileAt(x, y + 1, z);
            if (near_tile == 106 || near_tile == 107)
                dir = (dir & 0xF0FF) | 0x0400;

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
        /*case 119:
            // TODO: Greenland map needs fixing
            dir = 0xFFFF;
            near_tile = pMap->tileAt(x, y + 1, z);
            if (near_tile == 107 || near_tile == 225 || near_tile == 226)
                dir = (dir & 0xF0FF) | 0x0400;
            near_tile = pMap->tileAt(x, y + 1, z);
            if (near_tile == 106 || near_tile == 225 || near_tile == 226)
               dir &= 0xFFF0;
            near_tile = pMap->tileAt(x + 1, y, z);
            if (near_tile == 109 || near_tile == 225 || near_tile == 226)
                dir = (dir & 0xFF0F) | 0x0020;
            near_tile = pMap->tileAt(x - 1, y, z);
            if (near_tile == 108 || near_tile == 225 || near_tile == 226)
                dir = (dir & 0x0FFF) | 0x6000;
            if (dir ==0xFFFF)
                dir = 0x0;
            break;*/
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
        case 225:/*
            if(g_App.maps().map(map())->getTileAt(x + 1, y, z)->type() == Tile::kRoadPedCross)
                dir = (0)|(0xFFF0);
            else if(g_App.maps().map(map())->getTileAt(x - 1, y, z)->type() == Tile::kRoadPedCross)
                dir = (4<<8)|(0xF0FF);
            else {*/
                dir = 0xFFFF;
                near_tile = pMap->tileAt(x, y + 1, z);
                if (/*near_tile == 119 || */near_tile == 106
                    || near_tile == 107 || near_tile == 80 || near_tile == 225)
                    dir = (dir & 0xF0FF) | 0x0400;
                near_tile = pMap->tileAt(x, y - 1, z);
                if (/*near_tile == 119 || */near_tile == 106
                    || near_tile == 107 || near_tile == 80 || near_tile == 225)
                    dir &= 0xFFF0;
                near_tile = pMap->tileAt(x + 1, y, z);
                if (/*near_tile == 119 || */near_tile == 108 || near_tile == 81)
                    dir = (dir & 0xFF0F) | 0x0020;
                near_tile = pMap->tileAt(x - 1, y, z);
                if (/*near_tile == 119 || */near_tile == 109 || near_tile == 81)
                    dir = (dir & 0x0FFF) | 0x6000;
                if (dir == 0xFFFF)
                    dir = 0x0;
            //}
            break;
        case 226:/*
            if(g_App.maps().map(map())->getTileAt(x, y - 1, z)->type() == Tile::kRoadPedCross)
                dir = (2<<4)|(0xFF0F);
            else if(g_App.maps().map(map())->getTileAt(x, y + 1, z)->type() == Tile::kRoadPedCross)
                dir = (6<<12)|(0x0FFF);
            else {*/
                dir = 0xFFFF;
                near_tile = pMap->tileAt(x, y + 1, z);
                if (/*near_tile == 119 || */near_tile == 106 || near_tile == 80)
                    dir = (dir & 0xF0FF) | 0x0400;
                near_tile = pMap->tileAt(x, y - 1, z);
                if (/*near_tile == 119 || */near_tile == 107 || near_tile == 80)
                    dir &= 0xFFF0;
                near_tile = pMap->tileAt(x + 1, y, z);
                if (/*near_tile == 119 || */near_tile == 108 || near_tile == 109
                    || near_tile == 81 || near_tile == 226)
                    dir = (dir & 0xFF0F) | 0x0020;
                near_tile = pMap->tileAt(x - 1, y, z);
                if (/*near_tile == 119 || */near_tile == 108 || near_tile == 109
                    || near_tile == 81 || near_tile == 226)
                    dir = (dir & 0x0FFF) | 0x6000;
                if (dir == 0xFFFF)
                    dir = 0;
            //}
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
    if (dirStart == 0x0 || dirEnd == 0x0)
        return false;
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

/*!
 * Sets a destination point for the vehicle to reach at given speed.
 * \param m
 * \param node destination point
 * \param newSpeed Speed of movement
 * \return true if destination has been set correctly.
 */
bool VehicleInstance::setDestination(Mission *m, PathNode &node, int newSpeed) {
    speed_ = newSpeed;
    setDestinationV(node.tileX(), node.tileY(), node.tileZ(), node.offX(), node.offY(), newSpeed);
    return !dest_path_.empty();
}

void VehicleInstance::setDestinationV(int x, int y, int z, int ox, int oy, int new_speed)
{
    std::map < PathNode, uint16 > open;
    std::set < PathNode > closed;
    std::map < PathNode, PathNode > parent;
    int basex = tile_x_, basey = tile_y_;
    std::vector < PathNode > path2add;
    path2add.reserve(16);
    Map *pMap = g_App.maps().map(map_);

    pMap->adjXYZ(x, y, z);
    // NOTE: we will be using lower tiles, later will restore Z coord
    z = tile_z_ - 1;

    dest_path_.clear();
    setSpeed(0);

    if (map_ == -1 || health_ <= 0 || !(walkable(x, y, z))) {
#if 0
#if _DEBUG
        if (!(map_ == -1 || health_ <= 0)) {
            printf("non-walking tile is target to drive\n");
            printf("tileAt %i\n",
                (unsigned int)g_App.maps().map(map())->tileAt(x, y, z));
            printf("tile x = %i, y = %i, z = %i\n", x, y, z);
        }
#endif
#endif
        return;
    }

    if (!walkable(tile_x_, tile_y_, z)) {
        int dBest = 100000, dCur;
        std::vector < PathNode > path2wtile;
        path2wtile.reserve(16);
        // we got somewhere we shouldn't, we need to find somewhere that is walkable
        PathNode pntile(tile_x_ , tile_y_, z, off_x_, off_y_);
        for (int i = 1; i < 16; i++) {
            if (tile_x_ + i >= pMap->maxX())
                break;
            pntile.setTileX(tile_x_ + i);
            path2wtile.push_back(pntile);
            if (walkable(tile_x_ + i, tile_y_, z)) {
                dCur = i * i;
                if(dCur < dBest) {
                    dBest = dCur;
                    path2add = path2wtile;
                    basex = tile_x_ + i;
                    basey = tile_y_;
                    break;
                }
            }
        }

        path2wtile.clear();
        pntile = PathNode(tile_x_ , tile_y_, z, off_x_, off_y_);
        for (int i = -1; i > -16; --i) {
            if (tile_x_ + i < 0)
                break;
            pntile.setTileX(tile_x_ + i);
            path2wtile.push_back(pntile);
            if (walkable(tile_x_ + i, tile_y_, z)) {
                dCur = i * i;
                if(dCur < dBest) {
                    dBest = dCur;
                    path2add = path2wtile;
                    basex = tile_x_ + i;
                    basey = tile_y_;
                    break;
                }
            }
        }

        path2wtile.clear();
        pntile = PathNode(tile_x_ , tile_y_, z, off_x_, off_y_);
        for (int i = -1; i > -16; --i) {
            if (tile_y_ + i < 0)
                break;
            pntile.setTileY(tile_y_ + i);
            path2wtile.push_back(pntile);
            if (walkable(tile_x_, tile_y_ + i, z)) {
                dCur = i * i;
                if(dCur < dBest) {
                    dBest = dCur;
                    path2add = path2wtile;
                    basex = tile_x_;
                    basey = tile_y_ + i;
                    break;
                }
            }
        }

        path2wtile.clear();
        pntile = PathNode(tile_x_ , tile_y_, z, off_x_, off_y_);
        for (int i = 1; i < 16; i++) {
            if (tile_y_ + i >= pMap->maxY())
                break;
            pntile.setTileY(tile_y_ + i);
            path2wtile.push_back(pntile);
            if (walkable(tile_x_, tile_y_ + i, z)) {
                dCur = i * i;
                if(dCur < dBest) {
                    dBest = dCur;
                    path2add = path2wtile;
                    basex = tile_x_;
                    basey = tile_y_ + i;
                    break;
                }
            }
        }
        if(dBest == 100000)
            return;
    }

    PathNode closest;
    float closest_dist = 100000;

    uint16 wrong_dir = (uint16)getDirection(4);
    if (wrong_dir == 0x0)
        wrong_dir = 0x0400;
    else if(wrong_dir == 0x1)
        wrong_dir = 0x6000;
    else if(wrong_dir == 0x2)
        wrong_dir = 0x0;
    else if(wrong_dir == 0x3)
        wrong_dir = 0x0020;
    open.insert(std::pair< PathNode, uint16 >(PathNode(basex, basey, z, off_x_, off_y_),
        wrong_dir));
    int watchDog = 1000;

    while (!open.empty()) {
        watchDog--;
        float dist = 100000;
        PathNode p;
        std::map < PathNode, uint16 >::iterator pit;
        for (std::map < PathNode, uint16 >::iterator it = open.begin();
             it != open.end(); it++)
        {
            float d =
                sqrt((float) (x - it->first.tileX()) * (x - it->first.tileX()) +
                     (y - it->first.tileY()) * (y - it->first.tileY()));
            if (d < dist) {
                dist = d;
                p = it->first;
                pit = it;       // it cannot be const_iterator because of this assign
                wrong_dir = it->second;
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
                    && p.tileZ() == z)
                    break;
                dest_path_.push_front(p);
            }
            break;
        }

        std::map <PathNode, uint16> neighbours;
        uint16 goodDir = tileDir(p.tileX(), p.tileY(), p.tileZ());

        if (wrong_dir != 0x6000 && p.tileX() > 0) {
            if (dirWalkable(&p, p.tileX() - 1, p.tileY(), p.tileZ())
                && ((goodDir & 0xF000) == 0x6000 || goodDir == 0xFFFF))
                neighbours[PathNode(p.tileX() - 1, p.tileY(), p.tileZ())] = 0x0020;
        }

        if (wrong_dir != 0x0020 && p.tileX() < g_App.maps().map(map())->maxX()) {
            if (dirWalkable(&p, p.tileX() + 1, p.tileY(), p.tileZ())
                && ((goodDir & 0x00F0) == 0x0020 || goodDir == 0xFFFF))
                neighbours[PathNode(p.tileX() + 1, p.tileY(), p.tileZ())] = 0x6000;
        }

        if (wrong_dir != 0x0400 && p.tileY() > 0)
            if (dirWalkable(&p, p.tileX(), p.tileY() - 1, p.tileZ())
                && ((goodDir & 0x0F00) == 0x0400 || goodDir == 0xFFFF))
                neighbours[PathNode(p.tileX(), p.tileY() - 1, p.tileZ())] = 0x0;

        if (wrong_dir != 0x0000 && p.tileY() < g_App.maps().map(map())->maxY())
            if (dirWalkable(&p, p.tileX(), p.tileY() + 1, p.tileZ())
                && ((goodDir & 0x000F) == 0x0 || goodDir == 0xFFFF))
                neighbours[PathNode(p.tileX(), p.tileY() + 1, p.tileZ())] = 0x0400;

        for (std::map <PathNode, uint16>::iterator it = neighbours.begin();
            it != neighbours.end(); it++)
            if (dirWalkable(&p, it->first.tileX(), it->first.tileY(),
                it->first.tileZ())
                && open.find(it->first) == open.end()
                && closed.find(it->first) == closed.end())
            {
                parent[it->first] = p;
                open.insert(*it);
            }
    }

    if(!dest_path_.empty()) {
        // Adjusting offsets for correct positioning
        speed_ = new_speed;
        int curox = off_x_;
        int curoy = off_y_;
        for(std::list < PathNode >::iterator it = dest_path_.begin();
            it != dest_path_.end(); it++)
        {
            // TODO : adjust offsets respecting direction relative to
            // close next tiles
            switch(tileDir(it->tileX(), it->tileY(), it->tileZ())) {
                case 0xFFF0:
                case 0xFF20:
                    it->setOffX(200);
                    it->setOffY(32);
                    curox = 200;
                    curoy = 32;
                    break;
                case 0xF4FF:
                    it->setOffX(32);
                    it->setOffY(200);
                    curox = 32;
                    curoy = 200;
                    break;
                case 0xFF2F:
                case 0xF42F:
                    it->setOffX(32);
                    it->setOffY(32);
                    curox = 32;
                    curoy = 32;
                    break;
                case 0x6FFF:
                case 0x64FF:
                    it->setOffX(32);
                    it->setOffY(200);
                    curox = 32;
                    curoy = 200;
                    break;
                case 0x6FF0:
                    it->setOffX(200);
                    it->setOffY(200);
                    curox = 200;
                    curoy = 200;
                    break;
                default:
#if 0
#if _DEBUG
                    printf("hmm tileDir %X at %i, %i, %i\n",
                        (unsigned int)tileDir(it->tileX(), it->tileY(),
                        it->tileZ()), it->tileX(), it->tileY(), it->tileZ());
                    printf("tileAt %i\n",
                        (unsigned int)g_App.maps().map(map())->tileAt(
                        it->tileX(), it->tileY(), it->tileZ()));
#endif
#endif
                    it->setOffX(curox);
                    it->setOffY(curoy);
                    break;
            }
            it->setTileZ(tile_z_);
        }
    }
    if((!path2add.empty()) && (!dest_path_.empty())) {
        for (std::vector < PathNode >::reverse_iterator it = path2add.rbegin();
            it != path2add.rend(); it++)
        {
            it->setTileZ(tile_z_);
            dest_path_.push_front(*it);
        }
    }
}

/*!
 * Moves a vehicle on the map.
 * \param elapsed Elapsed time sine last frame.
 */
bool VehicleInstance::move_vehicle(int elapsed)
{
    bool updated = false;
    int used_time = elapsed;

    while ((!dest_path_.empty()) && used_time != 0) {
        if (hold_on_.wayFree == 1) { // Must wait
            return updated;
        } else if (hold_on_.wayFree == 2){
            // Must stop : clear destination and stop
            clearDestination();
            return updated;
        }

        // Get distance between car and next NodePath
        int adx =
            dest_path_.front().tileX() * 256 + dest_path_.front().offX();
        int ady =
            dest_path_.front().tileY() * 256 + dest_path_.front().offY();
        int atx = tile_x_ * 256 + off_x_;
        int aty = tile_y_ * 256 + off_y_;
        int diffx = adx - atx, diffy = ady - aty;

        if (abs(diffx) < 16 && abs(diffy) < 16) {
            // We reached the next point : remove it from path
            off_y_ = dest_path_.front().offY();
            off_x_ = dest_path_.front().offX();
            tile_y_ = dest_path_.front().tileY();
            tile_x_ = dest_path_.front().tileX();
            dest_path_.pop_front();
            // There's no following point so stop moving
            if (dest_path_.size() == 0)
                speed_ = 0;
            updated = true;
        } else {
            setDirection(diffx, diffy, &dir_);
            int dx = 0, dy = 0;
            double d = sqrt((double)(diffx * diffx + diffy * diffy));
            // This is the time for all the remaining distance to the node
            double avail_time_use = (d / (double)speed_) * 1000.0;
            // correcting time available regarding the time we have
            if (avail_time_use > used_time)
                avail_time_use = used_time;

            // computes distance travelled by vehicle in the available time
            if (abs(diffx) > 0)
                // dx = diffx * (speed_ * used_time / 1000) / d;
                dx = (int)((diffx * (speed_ * avail_time_use) / d) / 1000);
            if (abs(diffy) > 0)
                // dy = diffy * (speed_ * used_time / 1000) / d;
                dy = (int)((diffy * (speed_ * avail_time_use) / d) / 1000);

            // Updates the available time
            if (dx || dy) {
                int prv_time = used_time;
                if (dx) {
                    used_time -= (int)(((double) dx * 1000.0 * d)
                        / (double)(diffx * speed_));
                } else if (dy) {
                    used_time -= (int)(((double) dy * 1000.0 * d)
                        / (double)(diffy * speed_));
                } else
                    used_time = 0;
                if (used_time < 0 || prv_time == used_time)
                    used_time = 0;
            } else
                used_time = 0;

            // Moves vehicle
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
    }

    if (dest_path_.empty() && speed_) {
        printf("Destination Unknown, full speed driving = %i ... doing full stop\n",
               speed_);
        speed_ = 0;
    }
    if (!passengers_.empty()) {
        for (std::set<PedInstance *>::iterator it = passengers_.begin();
            it != passengers_.end(); it++
        ) {
            (*it)->setPosition(tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
        }
    }

    return updated;
}


bool VehicleInstance::handleDamage(ShootableMapObject::DamageInflictType *d) {
    if (health_ <= 0 || d->dtype == MapObject::dmg_Persuasion)
        return false;
    health_ -= d->dvalue;
    if (health_ <= 0) {
        health_ = 0;
        is_ignored_ = true;
        clearDestination();
        switch ((unsigned int)d->dtype) {
            case MapObject::dmg_Bullet:
            case MapObject::dmg_Laser:
            case MapObject::dmg_Burn:
            case MapObject::dmg_Explosion:
                vehicle_->set_animation_type(VehicleAnimation::kOnFireAnim);
                setTimeShowAnim(10000);
                break;
        }
        vehicle_driver_ = NULL;
        while (passengers_.size() != 0)
        {
            PedInstance *p = *(passengers_.begin());
            dropPassenger(p);
        }
        ShotClass explosion;
        explosion.createExplosion(this, 512.0);
    } else {// NOTE: maybe reduce speed on hit?
        // TODO: let passengers know that vehicle is attacked
    }
    return true;
}

/*!
 * Adds the given ped to the passenger but if the vehicle
 * has no driver, ped becomes the driver.
 * \param p The ped
 */
void VehicleInstance::addPassenger(PedInstance *p) {
    Vehicle::addPassenger(p);
    if (hasDriver()) {
        // There's already a driver
        p->putInVehicle(this, PedInstance::pa_smInCar);
    } else {
        // Ped becomes the driver
        vehicle_driver_ = p;
        p->putInVehicle(this, PedInstance::pa_smUsingCar);
    }
}

/*!
 * Overload initial method to manage driver.
 * \param pPed The ped to remove.
 */
void VehicleInstance::dropPassenger(PedInstance *pPed) {
    Vehicle::dropPassenger(pPed);
    if (vehicle_driver_ == pPed) {
        vehicle_driver_ = NULL;
        clearDestination();

        // find another driver in the remaining passengers
        for (std::set<PedInstance *>::iterator it = passengers_.begin();
            it != passengers_.end(); it++) {
            // take the first one
            vehicle_driver_ = *it;
            break;
        }
    }
}

void VehicleInstance::forceSetDriver(PedInstance *vehicleDriver) {
    vehicle_driver_ = vehicleDriver;
    if (!isInsideVehicle(vehicleDriver)) {
        Vehicle::addPassenger(vehicleDriver);
    }
}
