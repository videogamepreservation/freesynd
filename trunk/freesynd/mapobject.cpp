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

#include "common.h"
#include "app.h"

MapObject::MapObject(int m):map_(m), frame_(0), elapsed_carry_(0),
frames_per_sec_(15)
{
}

int MapObject::screenX()
{
    return g_App.maps().map(map())->tileToScreenX(tile_x_, tile_y_,
                                                  tile_z_, off_x_, off_y_);
}

int MapObject::screenY()
{
    return g_App.maps().map(map())->tileToScreenY(tile_x_, tile_y_,
                                                  tile_z_, off_x_, off_y_);
}

void MapObject::setOffX(int n)
{
    off_x_ = n;
    while (off_x_ < 0) {
        off_x_ += 256;
        tile_x_--;
    }
    while (off_x_ > 256) {
        off_x_ -= 256;
        tile_x_++;
    }
}

void MapObject::setOffY(int n)
{
    off_y_ = n;
    while (off_y_ < 0) {
        off_y_ += 256;
        tile_y_--;
    }
    while (off_y_ > 256) {
        off_y_ -= 256;
        tile_y_++;
    }
}

void MapObject::setOffZ(int n)
{
    off_z_ = n;
    while (off_z_ < 0) {
        off_z_ += 256;
        tile_z_--;
    }
    while (off_z_ > 256) {
        off_z_ -= 256;
        tile_z_++;
    }
}

void MapObject::addOffs(int &x, int &y)
{
    float fx = off_x_ / 256.0f;
    float fy = off_y_ / 256.0f;
    float fz = off_z_ / 256.0f;
    x += (int) ((fx - fy) * TILE_WIDTH / 2);
    y += (int) ((fx + fy - fz) * TILE_HEIGHT / 3);
}

bool MapObject::animate(int elapsed)
{
    int frame_tics_ = 1000 / frames_per_sec_;
    int total_elapsed = elapsed + elapsed_carry_;
    elapsed_carry_ = total_elapsed % frame_tics_;
    frame_ += (total_elapsed / frame_tics_);
    frame_ %= 100;
    return true;
}

ShootableMapObject::ShootableMapObject(int m):MapObject(m)
{
}

ShootableMovableMapObject::
ShootableMovableMapObject(int m):ShootableMapObject(m), dir_(0), speed_(0)
{
}

void ShootableMovableMapObject::setDestination(int x, int y, int z, int ox,
                                               int oy, int new_speed)
{
    std::set < PathNode > open, closed;
    std::map < PathNode, PathNode > parent;

    z = tile_z_;

    dest_path_.clear();

    if (map_ == -1)
        return;

    if (!walkable(tile_x_, tile_y_, tile_z_)) {
        // we got somewhere we shouldn't, we need to find somewhere that is walkable
        for (int j = 0; j < 5; j++)
            for (int i = 0; i < 5; i++)
                if (walkable(tile_x_ + i, tile_y_ + j, tile_z_)) {
                    tile_x_ = tile_x_ + i;
                    tile_y_ = tile_y_ + j;
                    goto gotwalkable;
                }
        for (int j = 0; j < 5; j++)
            for (int i = 0; i > -5; --i)
                if (walkable(tile_x_ + i, tile_y_ + j, tile_z_)) {
                    tile_x_ = tile_x_ + i;
                    tile_y_ = tile_y_ + j;
                    goto gotwalkable;
                }
        for (int j = 0; j > -5; --j)
            for (int i = 0; i > -5; --i)
                if (walkable(tile_x_ + i, tile_y_ + j, tile_z_)) {
                    tile_x_ = tile_x_ + i;
                    tile_y_ = tile_y_ + j;
                    goto gotwalkable;
                }
        for (int j = 0; j > -5; --j)
            for (int i = 0; i < 5; i++)
                if (walkable(tile_x_ + i, tile_y_ + j, tile_z_)) {
                    tile_x_ = tile_x_ + i;
                    tile_y_ = tile_y_ + j;
                    goto gotwalkable;
                }
        return;
      gotwalkable:
        ;
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
            neighbours.
                push_back(PathNode(p.tileX() - 1, p.tileY(), p.tileZ()));
            if (p.tileY() > 0) {
                // check for fences
                if (walkable(p.tileX() - 1, p.tileY(), p.tileZ()) &&
                    walkable(p.tileX(), p.tileY() - 1, p.tileZ()))
                    neighbours.
                        push_back(PathNode
                                  (p.tileX() - 1, p.tileY() - 1,
                                   p.tileZ()));
            }
            if (p.tileY() < g_App.maps().map(map())->maxY()) {
                // check for fences
                if (walkable(p.tileX() - 1, p.tileY(), p.tileZ()) &&
                    walkable(p.tileX(), p.tileY() + 1, p.tileZ()))
                    neighbours.
                        push_back(PathNode
                                  (p.tileX() - 1, p.tileY() + 1,
                                   p.tileZ()));
            }
        }
        if (p.tileX() < g_App.maps().map(map())->maxX()) {
            neighbours.
                push_back(PathNode(p.tileX() + 1, p.tileY(), p.tileZ()));
            if (p.tileY() > 0) {
                // check for fences
                if (walkable(p.tileX() + 1, p.tileY(), p.tileZ()) &&
                    walkable(p.tileX(), p.tileY() - 1, p.tileZ()))
                    neighbours.
                        push_back(PathNode
                                  (p.tileX() + 1, p.tileY() - 1,
                                   p.tileZ()));
            }
            if (p.tileY() < g_App.maps().map(map())->maxY()) {
                // check for fences
                if (walkable(p.tileX() + 1, p.tileY(), p.tileZ()) &&
                    walkable(p.tileX(), p.tileY() + 1, p.tileZ()))
                    neighbours.
                        push_back(PathNode
                                  (p.tileX() + 1, p.tileY() + 1,
                                   p.tileZ()));
            }
        }
        if (p.tileY() > 0)
            neighbours.
                push_back(PathNode(p.tileX(), p.tileY() - 1, p.tileZ()));
        if (p.tileY() < g_App.maps().map(map())->maxY())
            neighbours.
                push_back(PathNode(p.tileX(), p.tileY() + 1, p.tileZ()));

        for (std::list < PathNode >::iterator it = neighbours.begin();
             it != neighbours.end(); it++)
            if (walkable(it->tileX(), it->tileY(), it->tileZ())
                && open.find(*it) == open.end()
                && closed.find(*it) == closed.end()) {
                parent[*it] = p;
                open.insert(*it);
            }
    }

    speed_ = new_speed;
}

bool ShootableMovableMapObject::updatePlacement(int nOffX, int nOffY)
{
    int oOffX = off_x_;
    int oOffY = off_y_;
    int oTileX = tile_x_;
    int oTileY = tile_y_;

    off_x_ = nOffX;
    off_y_ = nOffY;

    while (off_x_ < 0) {
        off_x_ += 256;
        tile_x_--;
    }
    while (off_x_ >= 256) {
        off_x_ -= 256;
        tile_x_++;
    }
    while (off_y_ < 0) {
        off_y_ += 256;
        tile_y_--;
    }
    while (off_y_ >= 256) {
        off_y_ -= 256;
        tile_y_++;
    }

    Map *m = g_App.maps().map(map());

    if (!walkable(tile_x_, tile_y_, tile_z_)
        && m->stairsAt(oTileX, oTileY, tile_z_ + 1)) {
        tile_z_++;
        if (!walkable(tile_x_, tile_y_, tile_z_))
            tile_z_--;
    }

    if (!walkable(tile_x_, tile_y_, tile_z_)) {
        printf("butted into %i %i\n", m->tileAt(tile_x_, tile_y_, tile_z_),
               g_App.maps().map(map())->tileAt(tile_x_, tile_y_,
                                               tile_z_ + 1));
        off_x_ = oOffX;
        off_y_ = oOffY;
        tile_x_ = oTileX;
        tile_y_ = oTileY;
        return false;
    }

    if (m->stairsAt(tile_x_, tile_y_, tile_z_)) {
        tile_z_--;
        if (!walkable(tile_x_, tile_y_, tile_z_))
            tile_z_++;
    }

    if (m->stairsAt(tile_x_, tile_y_, tile_z_ + 1)) {
        //off_z_ = -off_x_;
    }

    return true;
}

bool ShootableMovableMapObject::animate(int elapsed)
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
            // TODO: this needs tweaking
            int fuzz = 16;
            if (ady < aty - fuzz) {
                if (adx < atx - fuzz)
                    dir_ = 5;
                else if (adx > atx + fuzz)
                    dir_ = 3;
                else
                    dir_ = 4;
            } else if (abs(ady - aty) < fuzz) {
                if (adx < atx - fuzz)
                    dir_ = 6;
                else if (adx > atx + fuzz)
                    dir_ = 2;
            } else {
                if (adx < atx - fuzz)
                    dir_ = 7;
                else if (adx > atx + fuzz)
                    dir_ = 1;
                else
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

Static *Static::loadInstance(uint8 * data, int m)
{
    Mission::LEVELDATA_STATICS * gamdata =
        (Mission::LEVELDATA_STATICS *) data;
    Static *s = 0;
    if (gamdata->unkn4 == 0x35) {
        if (gamdata->unkn6 == 0x6e)
            s = new Door(m, 1078, 1080);
        else if (gamdata->unkn6 == 0x63)
            s = new Door(m, 1077, 1079);
        else
            printf("unknown door 35 xx %02x\n", gamdata->unkn6);
    }
    if (gamdata->unkn4 == 0x4d) {
        if (gamdata->unkn6 == 0x8a)
            s = new Door(m, 1101, 1103);
        else
            printf("unknown door 4d xx %02x\n", gamdata->unkn6);
    }
    if (gamdata->unkn4 == 0x3d) {
        if (gamdata->unkn6 == 0x73)
            s = new Door(m, 1085, 1087);
        else if (gamdata->unkn6 == 0x75)
            s = new Door(m, 1086, 1088);
        else
            printf("unknown door 3d %02x\n", gamdata->unkn6);
    }
    if (gamdata->unkn4 == 0x45) {
        if (gamdata->unkn6 == 0x7e)
            s = new Door(m, 1093, 1095);
        else if (gamdata->unkn6 == 0x85)
            s = new Door(m, 1094, 1096);
        else
            printf("unknown door 45 %02x\n", gamdata->unkn6);
    }
    if (gamdata->unkn4 == 0x25) {
        if (gamdata->unkn6 == 0x7c)
            s = new Door(m, 1110, 1112);
        else
            printf("unknown door 25 %02x\n", gamdata->unkn6);
    }
    if (!s && gamdata->unkn5 == 4) {
        //printf("looks like a missing door: %02x %02x\n", gamdata->unkn4, gamdata->unkn6);
    }
    if (s) {
        //printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
        s->setPosition(gamdata->mapposx[1], gamdata->mapposy[1],
                       gamdata->mapposz[1], gamdata->mapposx[0],
                       gamdata->mapposy[0], gamdata->mapposz[0]);
    }
    return s;
}

Door::Door(int m, int anim, int openingAnim):Static(m), anim_(anim),
opening_anim_(openingAnim)
{
}

void Door::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
}

Tree::Tree(int m, int anim, int burningAnim):Static(m), anim_(anim),
burning_anim_(burningAnim)
{
}

void Tree::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
}
