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
#ifdef SYSTEM_SDL
#include "system_sdl.h"
#endif
MapObject::MapObject(int m):map_(m), frame_(0), elapsed_carry_(0),
frames_per_sec_(15), sub_type_(0), main_type_(0)
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
    frame_ %= frames_per_sec_ << 2;
    return true;
}

ShootableMapObject::ShootableMapObject(int m):MapObject(m)
{
}

ShootableMovableMapObject::
ShootableMovableMapObject(int m):ShootableMapObject(m), dir_(0), speed_(0)
{
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

Static *Static::loadInstance(uint8 * data, int m)
{
    // TODO: Add unknown objects, adjust animation for etcObj
    Mission::LEVELDATA_STATICS * gamdata =
        (Mission::LEVELDATA_STATICS *) data;
    Static *s = 0;
    uint16 firstAnim = READ_LE_UINT16(gamdata->firstAnim);

    switch(gamdata->objType) {
        case 0x05:
            if(gamdata->subType == 0x74 || gamdata->subType == 0x79
                || gamdata->subType == 0x81) {
                s = new Tree(m, firstAnim, firstAnim + 1, firstAnim + 2);
            } else {
                printf("%02X , %02X, %i\n",gamdata->objType,gamdata->subType,firstAnim);
                printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
                printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
                printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
            }
            break;
        case 0x07:
            if(gamdata->subType == 0x5E || gamdata->subType == 0x5F) {
                s = new WindowObj(m, firstAnim, firstAnim + 4, firstAnim + 6);
            } else {
                printf("%02X , %02X, %i\n",gamdata->objType,gamdata->subType,firstAnim);
                printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
                printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
                printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
            }
            break;
        case 0x08:
            if(gamdata->subType == 0x14 || gamdata->subType == 0x0B) {
                s = new LargeDoor(m, firstAnim, firstAnim + 1, firstAnim + 2);
            } else if(gamdata->subType == 0xF0) {
                // tree in vase
                s = new Tree(m, firstAnim, firstAnim + 1, firstAnim + 2);
            } else {
                printf("%02X , %02X, %i\n",gamdata->objType,gamdata->subType,firstAnim);
                printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
                printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
                printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
            }
            break;
        case 0x09:
            if(gamdata->subType == 0x9C || gamdata->subType == 0xD9) {
                // trash can / mail box
                s = new EtcObj(m, firstAnim, firstAnim, firstAnim);
            } else if(gamdata->subType == 0xDF || gamdata->subType == 0xE1
                || gamdata->subType == 0xE8 || gamdata->subType == 0xFA
                || gamdata->subType == 0xDA) {
                // advertisement on wall
                s = new Tree(m, firstAnim, firstAnim + 1, firstAnim + 2);
            } else {
                printf("%02X , %02X, %i\n",gamdata->objType,gamdata->subType,firstAnim);
                printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
                printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
                printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
            }
            break;
        case 0x0A:
            if(gamdata->subType == 0x6E || gamdata->subType == 0x6F) {
                s = new WindowObj(m, firstAnim, firstAnim + 2, firstAnim + 4);
            } else if(gamdata->subType == 0xA5 || gamdata->subType == 0xA4) {
                // phone booth
                s = new EtcObj(m, firstAnim, firstAnim, firstAnim);
            } else if(gamdata->subType == 0xC7) {
                // advertisement on wall broken signal
                s = new EtcObj(m, firstAnim, firstAnim, firstAnim);
            } else {
                printf("%02X , %02X, %i\n",gamdata->objType,gamdata->subType,firstAnim);
                printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
                printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
                printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
            }
            break;
        case 0x0B:
            if(gamdata->subType == 0x35 || gamdata->subType == 0x36
                || gamdata->subType == 0x47 || gamdata->subType == 0x10
                || gamdata->subType == 0x20 || gamdata->subType == 0x46
                || gamdata->subType == 0x44) {
                // advertisement on wall
                s = new EtcObj(m, firstAnim, firstAnim, firstAnim);
            } else {
                printf("%02X , %02X, %i\n",gamdata->objType,gamdata->subType,firstAnim);
                printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
                printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
                printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
            }
            break;
        case 0x12:
            if(gamdata->subType == 0x6E || gamdata->subType == 0x8A
                || gamdata->subType == 0x63 || gamdata->subType == 0x73
                || gamdata->subType == 0x7E || gamdata->subType == 0x75
                || gamdata->subType == 0x97 || gamdata->subType == 0x7F
                || gamdata->subType == 0x96 || gamdata->subType == 0x85
                || gamdata->subType == 0x6B/* || gamdata->subType == 0x7F*/) {
                s = new Door(m, firstAnim, firstAnim + 2,
                    firstAnim + 4, firstAnim + 6);
            } else if(gamdata->subType >= 0x17 && gamdata->subType <= 0x1A) {
                // crossroad things
                s = new EtcObj(m, firstAnim, firstAnim, firstAnim);
            } else if(gamdata->subType == 0xE2 || gamdata->subType == 0x1C
                || gamdata->subType == 0xF1 || gamdata->subType == 0xF8
                || gamdata->subType == 0xFA || gamdata->subType == 0x44
                || gamdata->subType == 0x25 || gamdata->subType == 0x4C
                || gamdata->subType == 0xEB || gamdata->subType == 0xEF
                || gamdata->subType == 0xED || gamdata->subType == 0x40) {
                // blinking things
                s = new EtcObj(m, firstAnim, firstAnim, firstAnim);
            } else {
                printf("%02X , %02X, %i\n",gamdata->objType,gamdata->subType,firstAnim);
                printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
                printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
                printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
            }
            break;
        case 0x13:
            if(gamdata->subType == 0x7C || gamdata->subType == 0x0A
                || gamdata->subType == 0x15) {
                // blinking things
                s = new EtcObj(m, firstAnim, firstAnim, firstAnim);
            } else if(gamdata->subType == 0x6F || gamdata->subType == 0x75) {
                // blinking little things
                s = new EtcObj(m, firstAnim, firstAnim, firstAnim);
            } else {
                printf("%02X , %02X, %i\n",gamdata->objType,gamdata->subType,firstAnim);
                printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
                printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
                printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
            }
            break;
        case 0x20:
            if(gamdata->subType == 0x4B) {
                // window without light
                s = new EtcObj(m, firstAnim, firstAnim, firstAnim);
            } else {
                printf("%02X , %02X, %i\n",gamdata->objType,gamdata->subType,firstAnim);
                printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
                printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
                printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
            }
            break;
        case 0x21:
            if(gamdata->subType == 0xD2) {
                // window without light
                s = new EtcObj(m, firstAnim, firstAnim, firstAnim);
            } else {
                printf("%02X , %02X, %i\n",gamdata->objType,gamdata->subType,firstAnim);
                printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
                printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
                printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
            }
            break;
        default:
            if(gamdata->objType) {
                printf("%02X , %02X, %i\n",gamdata->objType,gamdata->subType,firstAnim);
                printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
                printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
                printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
            }
            break;
    }

    if (s) {
        /*
        printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
        printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
        printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
        */
        s->setPosition(gamdata->mapposx[1], gamdata->mapposy[1],
                       gamdata->mapposz[1], gamdata->mapposx[0],
                       gamdata->mapposy[0], gamdata->mapposz[0]);
        s->setSubType(gamdata->subType);
    }

    return s;
}

Door::Door(int m, int anim, int closingAnim, int openAnim, int openingAnim):Static(m),
anim_(anim), closing_anim_(closingAnim),open_anim_(openAnim),
opening_anim_(openingAnim)
{
}

void Door::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
}

bool Door::animate(int elapsed, Mission *obj)
{
    return MapObject::animate(elapsed);
}

LargeDoor::LargeDoor(int m, int anim, int closingAnim, int openingAnim):Static(m), anim_(anim),
closing_anim_(closingAnim), opening_anim_(openingAnim)
{
}

void LargeDoor::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
}

bool LargeDoor::animate(int elapsed, Mission *obj)
{
    return MapObject::animate(elapsed);
}

Tree::Tree(int m, int anim, int burningAnim, int damagedAnim):Static(m),
anim_(anim), burning_anim_(burningAnim), damaged_anim_(damagedAnim)
{
}

void Tree::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
}

WindowObj::WindowObj(int m, int anim, int breakingAnim, int damagedAnim):Static(m),
anim_(anim), breaking_anim_(breakingAnim), damaged_anim_(damagedAnim)
{
}

void WindowObj::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
}

EtcObj::EtcObj(int m, int anim, int burningAnim , int damagedAnim):Static(m),
anim_(anim), burning_anim_(burningAnim), damaged_anim_(damagedAnim)
{
}

void EtcObj::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
}
