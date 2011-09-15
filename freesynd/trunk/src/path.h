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

#ifndef PATH_H
#define PATH_H

#include "common.h"
#include "pathsurfaces.h"

/*!
 * Path node class.
 */
class PathNode {
public:
    PathNode() :
        tile_x_(0), tile_y_(0), tile_z_(0), off_x_(0), off_y_(0) {}
    PathNode(int tile_x, int tile_y, int tile_z, int off_x = 128,
            int off_y = 128, int off_z = 0) :
        tile_x_(tile_x), tile_y_(tile_y), tile_z_(tile_z), off_x_(off_x),
                off_y_(off_y), off_z_(off_z) {}

    int tileX() const { return tile_x_; }
    int tileY() const { return tile_y_; }
    int tileZ() const { return tile_z_; }

    int offX() const { return off_x_; }
    int offY() const { return off_y_; }
    int offZ() const { return off_z_; }

    bool operator<(const PathNode &other) const {
        int a = tile_x_ | (tile_y_ << 16);
        int b = other.tile_x_ | (other.tile_y_ << 16);
        return a < b;
    }

    void setOffX(int x) { off_x_ = x; }
    void setOffY(int y) { off_y_ = y; }
    void setOffZ(int z) { off_z_ = z; }
    void setOffXY(int x, int y) { off_x_ = x; off_y_ = y; }
    void setOffXYZ(int x, int y, int z) {
        off_x_ = x;
        off_y_ = y;
        off_z_ = z;
    }

    void setTileX(int x) { tile_x_ = x; }
    void setTileY(int y) { tile_y_ = y; }
    void setTileZ(int z) { tile_z_ = z; }
    void setTileXYZ(int x, int y, int z) {
        tile_x_ = x;
        tile_y_ = y;
        tile_z_ = z;
    }

    void convertPosToXYZ(toDefineXYZ *xyz) {
        xyz->x = tile_x_ * 256 + off_x_;
        xyz->y = tile_y_ * 256 + off_y_;
        xyz->z = tile_z_ * 128 + off_z_;
    }

protected:
    int tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_;
};

#endif
