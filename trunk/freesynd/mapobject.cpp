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
    Mission::LEVELDATA_STATICS * gamdata =
        (Mission::LEVELDATA_STATICS *) data;
    Static *s = 0;
    uint16 firstAnim = READ_LE_UINT16(gamdata->firstAnim);

    switch(gamdata->unkn4){
    case 0x0:
        switch(gamdata->unkn6){
            //small windows, open/closed
            case 0x5f:
                s = new WindowObj(m, 551, 555, 557);
                break;
            case 0x5e:
                s = new WindowObj(m, 550, 555, 557);
                break;
            case 0x6f:
                s = new WindowObj(m, 552, 555, 557);
                break;
            case 0x6e:
                s = new WindowObj(m, 553, 555, 557);
                break;

            // crosroad thing
            case 0x19:
                break;

            // double doors
            // these can be locked more info needed
            case 0x0b:
                s = new LargeDoor(m, 392, 393, 394);
                break;
            case 0x14:
                s = new LargeDoor(m, 395, 396, 397);
                break;
            default:
                if(gamdata->unkn5!=0 || gamdata->unkn6!=0)
                    printf("unknown obj =0= xx %02x\n", gamdata->unkn6);
        }
        break;
    case 0x35:
        if (gamdata->unkn6 == 0x6e)
            s = new Door(m, 1078, 1080, 1082, 1084);
        else if (gamdata->unkn6 == 0x63)
            s = new Door(m, 1077, 1079, 1081, 1083);
        else
            printf("unknown door 35 xx %02x\n", gamdata->unkn6);
        break;
    case 0x4d:
        if (gamdata->unkn6 == 0x8a)
            s = new Door(m, 1101, 1103, 1105, 1107);
        else
            printf("unknown door 4d xx %02x\n", gamdata->unkn6);
        break;
    case 0x3d:
        if (gamdata->unkn6 == 0x73)
            s = new Door(m, 1085, 1087, 1089, 1091);
        else if (gamdata->unkn6 == 0x75)
            s = new Door(m, 1086, 1088, 1090, 1092);
        else
            printf("unknown door 3d %02x\n", gamdata->unkn6);
        break;
    case 0x45:
        if (gamdata->unkn6 == 0x7e)
            s = new Door(m, 1093, 1095, 1097, 1099);
        else if (gamdata->unkn6 == 0x85)
            s = new Door(m, 1094, 1096, 1098, 1100);
        else
            printf("unknown door 45 %02x\n", gamdata->unkn6);
        break;
    case 0x25:
        if (gamdata->unkn6 == 0x7c)
            s = new Door(m, 1110, 1112, 1114, 1116);
        else
            printf("unknown door 25 %02x\n", gamdata->unkn6);
        break;
    case 0x01:
        if (gamdata->unkn6 == 0x74)
            s = new Tree(m, 227, 228, 229);
        else
            printf("unknown tree 01 %02x\n", gamdata->unkn6);
        break;
    case 0x02:
        if (gamdata->unkn6 == 0x79)
            s = new Tree(m, 230, 231, 232);
        else
            printf("unknown tree 02 %02x\n", gamdata->unkn6);
        break;
    case 0x03:
        if (gamdata->unkn6 == 0x81)
            s = new Tree(m, 233, 234, 235);
        else
            printf("unknown tree 03 %02x\n", gamdata->unkn6);
        break;
    default:
        break;
    }
    /*
    if(!s && (gamdata->unkn4==0 && (gamdata->unkn5!=0 || gamdata->unkn6!=0))){//this is for debug
        printf("unknown obj %02x xx %02x xx %02x\n", gamdata->unkn4,gamdata->unkn5,gamdata->unkn6);
        printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
        printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
        printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
    }
    */
    /*
    if(gamdata->unkn4!=0 && gamdata->unkn5!=0 && gamdata->unkn6 != 0)
        if (!s) {
            s = new EtcObj(m, firstAnim, firstAnim + 1, firstAnim +2);
        }
    */
    if (s) {
        /*
        printf("d %x ==",gamdata->unkn4);
        printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
        printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
        printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
        */
        s->setPosition(gamdata->mapposx[1], gamdata->mapposy[1],
                       gamdata->mapposz[1], gamdata->mapposx[0],
                       gamdata->mapposy[0], gamdata->mapposz[0]);
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

LargeDoor::LargeDoor(int m, int anim, int closingAnim, int openingAnim):Static(m), anim_(anim),
closing_anim_(closingAnim), opening_anim_(openingAnim)
{
}

void LargeDoor::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
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
