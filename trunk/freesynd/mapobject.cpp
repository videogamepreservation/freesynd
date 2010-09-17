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
    while (off_x_ > 255) {
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
    while (off_y_ > 255) {
        off_y_ -= 256;
        tile_y_++;
    }
}

void MapObject::setOffZ(int n)
{
    off_z_ = n;
    while (off_z_ < 0) {
        off_z_ += 128;
        tile_z_--;
    }
    while (off_z_ > 127) {
        off_z_ -= 128;
        tile_z_++;
    }
}

void MapObject::addOffs(int &x, int &y)
{
    x += ((off_x_ - off_y_) * (TILE_WIDTH / 2)) / 256;
    y += ((off_x_ + off_y_) * (TILE_HEIGHT / 3)) / 256;
    y -= (off_z_ * (TILE_HEIGHT / 3)) / 128;
    y += (tile_z_ - vis_z_) * TILE_HEIGHT / 3;
}

bool MapObject::animate(int elapsed)
{
    int frame_tics_ = 1000 / frames_per_sec_;
    int total_elapsed = elapsed + elapsed_carry_;
    elapsed_carry_ = total_elapsed % frame_tics_;
    int framewas = frame_;
    bool changed = true;
    frame_ += (total_elapsed / frame_tics_);
    if (framewas == frame_)
        changed = false;
    frame_ %= frames_per_sec_ << 2;
    return changed;
}

ShootableMapObject::ShootableMapObject(int m):MapObject(m)
{
}

ShootableMovableMapObject::
ShootableMovableMapObject(int m):ShootableMapObject(m), dir_(0),
speed_(0)
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
    bool changed = false;

    while (off_x_ < 0) {
        off_x_ += 256;
        tile_x_--;
        changed = true;
    }
    while (off_x_ > 255) {
        off_x_ -= 256;
        tile_x_++;
        changed = true;
    }
    while (off_y_ < 0) {
        off_y_ += 256;
        tile_y_--;
        changed = true;
    }
    while (off_y_ > 255) {
        off_y_ -= 256;
        tile_y_++;
        changed = true;
    }

    return changed;
}

Static *Static::loadInstance(uint8 * data, int m)
{
    // TODO: Add unknown objects, adjust animation for etcObj
    Mission::LEVELDATA_STATICS * gamdata =
        (Mission::LEVELDATA_STATICS *) data;
    Static *s = 0;
    uint16 firstAnim = READ_LE_UINT16(gamdata->firstAnim);
    // maybe, subtype stores direction somewhere
    // it is easier to get type from animation then from sub-type
    // also looks like sub-type contains frame number for animation

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
                // tree in pot
                s = new Tree(m, firstAnim, firstAnim + 1, firstAnim + 2);
            } else if(gamdata->subType == 0xEF) {
                // damaged window
                s = new WindowObj(m, firstAnim, firstAnim, firstAnim);
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
                || gamdata->subType == 0xDA || gamdata->subType == 0xED
                || gamdata->subType == 0xF0 || gamdata->subType == 0xF4
                || gamdata->subType == 0xEC || gamdata->subType == 0xEA
                || gamdata->subType == 0xE4 || gamdata->subType == 0xF8
                || gamdata->subType == 0xE6 || gamdata->subType == 0xF6
                || gamdata->subType == 0xF3 || gamdata->subType == 0xEE
                || gamdata->subType == 0xFF || gamdata->subType == 0xF5
                || gamdata->subType == 0xFE || gamdata->subType == 0xDC
                || gamdata->subType == 0xE2) {
                // advertisement on wall 570-573
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
            } else if(gamdata->subType == 0xC7 || gamdata->subType == 0xC5
                || gamdata->subType == 0xC6 || gamdata->subType == 0x1F
                || gamdata->subType == 0x1E || gamdata->subType == 0x1D) {
                // advertisement on wall bad signal 568 - 569 574 - 575
                s = new EtcObj(m, firstAnim, firstAnim, firstAnim);
            } else if(gamdata->subType == 0x1C || gamdata->subType == 0x03
                || gamdata->subType == 0x18 || gamdata->subType == 0x08
                || gamdata->subType == 0x0D || gamdata->subType == 0x07
                || gamdata->subType == 0x01 || gamdata->subType == 0x10
                || gamdata->subType == 0x14 || gamdata->subType == 0x17
                || gamdata->subType == 0x0B || gamdata->subType == 0x1A) {
                // advertisement on wall
                s = new EtcObj(m, firstAnim, firstAnim, firstAnim);
            } else {
                printf("%02X , %02X, %i\n",gamdata->objType,gamdata->subType,firstAnim);
                printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
                printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
                printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
            }
            break;
        case 0x0B:
            if( (gamdata->subType >= 0x16 && gamdata->subType <= 0x19)
                || (gamdata->subType >= 0x32 && gamdata->subType <= 0x39)
                || (gamdata->subType >= 0x43 && gamdata->subType <= 0x47)
                || gamdata->subType == 0x20 || gamdata->subType == 0x10
                || gamdata->subType == 0x40 || gamdata->subType == 0x0F
                || gamdata->subType == 0x12 || gamdata->subType == 0x0E
                || gamdata->subType == 0x29 || gamdata->subType == 0x3B
                || gamdata->subType == 0x0B || gamdata->subType == 0x21
                || gamdata->subType == 0x1B || gamdata->subType == 0x25
                || gamdata->subType == 0x14 || gamdata->subType == 0x3E
                || gamdata->subType == 0x2A || gamdata->subType == 0x13
                || gamdata->subType == 0x0C || gamdata->subType == 0x28
                || gamdata->subType == 0x1E || gamdata->subType == 0x1C) {
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
                || gamdata->subType == 0x6B || gamdata->subType == 0x91
                || gamdata->subType == 0x6A || gamdata->subType == 0x8B
                || gamdata->subType == 0x9C || gamdata->subType == 0x9D
                || gamdata->subType == 0x69 || gamdata->subType == 0x84
                || gamdata->subType == 0x79 || gamdata->subType == 0x77
                || gamdata->subType == 0x90 || gamdata->subType == 0x86) {
                    // Doors 1077 - 1116
                    // not all doors are good in this implementation
                    // some are already open
                s = new Door(m, firstAnim, firstAnim + 2,
                    firstAnim + 4, firstAnim + 6);
            } else if(gamdata->subType >= 0x17 && gamdata->subType <= 0x1A) {
                // crossroad things 1040 - 1043
                s = new EtcObj(m, firstAnim, firstAnim, firstAnim);
            } else if(gamdata->subType == 0x1C || gamdata->subType == 0x23
                || (gamdata->subType >= 0x1E && gamdata->subType <= 0x21)
                || (gamdata->subType >= 0x25 && gamdata->subType <= 0x29)
                || (gamdata->subType >= 0x2C && gamdata->subType <= 0x37)
                || (gamdata->subType >= 0x39 && gamdata->subType <= 0x47)
                || (gamdata->subType >= 0x4A && gamdata->subType <= 0x4C)
                || (gamdata->subType >= 0x50 && gamdata->subType <= 0x5C)
                || (gamdata->subType >= 0xE0 && gamdata->subType <= 0xE2)
                || (gamdata->subType >= 0xE4 && gamdata->subType <= 0xE7)
                || (gamdata->subType >= 0xE9 && gamdata->subType <= 0xFD)
                || gamdata->subType == 0xFF || gamdata->subType == 0x4E) {
                // neon signs 1045 - 1076
                s = new NeonSign(m, firstAnim);
            } else if(gamdata->subType == 0xA6 || gamdata->subType == 0xA5) {
                // hmm 1121-1122
                s = new EtcObj(m, firstAnim, firstAnim, firstAnim);
            } else {
                printf("%02X , %02X, %i\n",gamdata->objType,gamdata->subType,firstAnim);
                printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
                printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
                printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
            }
            break;
        case 0x13:
            if(gamdata->subType == 0x1C || gamdata->subType == 0x27
                || gamdata->subType == 0x86 || gamdata->subType == 0x83
                || (gamdata->subType >= 0x00 && gamdata->subType <= 0x05)
                || (gamdata->subType >= 0x07 && gamdata->subType <= 0x16)
                || (gamdata->subType >= 0x21 && gamdata->subType <= 0x25)
                || (gamdata->subType >= 0x2A && gamdata->subType <= 0x2E)
                || (gamdata->subType >= 0x30 && gamdata->subType <= 0x42)
                || (gamdata->subType >= 0x44 && gamdata->subType <= 0x4C)
                || (gamdata->subType >= 0x4E && gamdata->subType <= 0x66)
                || (gamdata->subType >= 0x68 && gamdata->subType <= 0x6B)
                || (gamdata->subType >= 0x6D && gamdata->subType <= 0x70)
                || (gamdata->subType >= 0x72 && gamdata->subType <= 0x7C)
                || (gamdata->subType >= 0x7E && gamdata->subType <= 0x80)
                || gamdata->subType == 0x1D || gamdata->subType == 0x89
                || gamdata->subType == 0x28 || gamdata->subType == 0x19
                || gamdata->subType == 0x85 || gamdata->subType == 0x1A) {
                // neon signs 1045 - 1076 
                s = new NeonSign(m, firstAnim);
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
            } else if(gamdata->subType == 0x2B || gamdata->subType == 0x27
                || gamdata->subType == 0x36) {
                // window with person's shadow 1965-1968
                s = new EtcObj(m, firstAnim, firstAnim, firstAnim);
            } else {
                printf("%02X , %02X, %i\n",gamdata->objType,gamdata->subType,firstAnim);
                printf("x is %i, xoff is %i ==", gamdata->mapposx[1], gamdata->mapposx[0]);
                printf("y is %i, yoff is %i ==", gamdata->mapposy[1], gamdata->mapposy[0]);
                printf("z is %i, zoff is %i\n", gamdata->mapposz[1], gamdata->mapposz[0]);
            }
            break;
        case 0x21:
            if(gamdata->subType == 0xD2 || gamdata->subType == 0xD3
                || gamdata->subType == 0xDF || gamdata->subType == 0xD9) {//1961 - 1964 windows animated
                // window without light
                s = new EtcObj(m, firstAnim, firstAnim, firstAnim);
            } else if(gamdata->subType == 0xD5) {
                // window with person's shadow 1965-1966
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
        int z = ((gamdata->mapposz[0] & 0x80) == 0 ? gamdata->mapposz[1]: gamdata->mapposz[1] << 1 );
        s->setVisZ(z);
        int oz = gamdata->mapposz[0] & 0x7F;
        if (oz > 0)
            z++;
        s->setPosition(gamdata->mapposx[1], gamdata->mapposy[1],
                       z, gamdata->mapposx[0],
                       gamdata->mapposy[0], oz);
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
    state_ = 2;// closed
}

void LargeDoor::draw(int x, int y)
{
    addOffs(x, y);
    switch(state_) {
        case 0:
            break;
        case 1:
            g_App.gameSprites().drawFrame(closing_anim_, frame_, x, y);
            break;
        case 2:
            g_App.gameSprites().drawFrame(anim_, frame_, x, y);
            break;
        case 3:
            g_App.gameSprites().drawFrame(opening_anim_, frame_, x, y);
            break;
    }
}

bool LargeDoor::animate(int elapsed, Mission *obj)
{
    // TODO: there must be somewhere locked door
    VehicleInstance *v = NULL;
    PedInstance *p = NULL;
    int x = tileX();
    int y = tileY();
    int z = tileZ();
    int mt;
    int si;
    char inc_rel = 0, rel_inc = 0;
    char *i = 0, *j = 0;
    bool found = false;

    bool changed = MapObject::animate(elapsed);
    switch(state_) {
        case 0: //open
            if (sub_type_ == 0x14) {
                i = &rel_inc;
                j = &inc_rel;
            } else if (sub_type_ == 0x0B) {
                i = &inc_rel;
                j = &rel_inc;
            }
            assert(i != 0 && j != 0);
            *j = -1;
            for(*i = -2; *i < 3; *i += 1) {
                mt = 0; si = 0;
                v = (VehicleInstance *)(obj->findAt(x + inc_rel,
                    y + rel_inc,z, &mt, &si, true));
                if (!v && state_ == 0 && !found) {
                    state_ = state_++;
                    frame_ = 0;
                } else if (v){
                    state_ = 0;
                    found = true;
                    v->hold_on_.wayFree = 0;
                }
            }
            *j = 1;
            for(*i = -2; *i < 3; *i += 1) {
                mt = 0; si = 0;
                v = (VehicleInstance *)(obj->findAt(x + inc_rel,
                    y + rel_inc,z,&mt,&si,true));
                if (!v && state_ == 0 && !found) {
                    state_ = state_++;
                    frame_ = 0;
                } else if (v){
                    state_ = 0;
                    found = true;
                    v->hold_on_.wayFree = 0;
                }
            }
            for (int a = (y - 1); a <= (y + 1); a++ ) {
                for (int b = (x - 1); b <= (x + 1); b++) {
                    mt = 1; si = 0;
                    do {
                        p = (PedInstance *)(obj->findAt(b, a, z,
                            &mt, &si, true));
                        if (p) {
                            p->hold_on_.wayFree = 0;
                        }
                    } while (p);
                }
            }
            break;
        case 2: //closed
            char sign;
            if (sub_type_ == 0x14) {
                i = &rel_inc;
                j = &inc_rel;
                sign = 1;
            } else if (sub_type_ == 0x0B) {
                i = &inc_rel;
                j = &rel_inc;
                sign = -1;
            }
            assert(i != 0 && j != 0);
            *j = -1 * sign;
            *i = -2;
            mt = 0; si = 0;
            v = (VehicleInstance *)(obj->findAt(x + inc_rel,
                y + rel_inc,z,&mt,&si,true));
            if (v) {
                if (!found) {
                    state_ = state_++;
                    found = true;
                    frame_ = 0;
                }
                v->hold_on_.wayFree = 1;
            }
            *j = 1 * sign;
            *i = 2;
            mt = 0; si = 0;
            v = (VehicleInstance *)(obj->findAt(x + inc_rel,
                y + rel_inc,z,&mt,&si,true));
            if (v) {
                if (!found) {
                    state_ = state_++;
                    found = true;
                    frame_ = 0;
                }
                v->hold_on_.wayFree = 1;
            }
            *j = -1;
            for ( *i = -1; *i <= 1; *i += 1 ) {
                mt = 1; si = 0;
                do {
                    p = (PedInstance *)(obj->findAt(x + rel_inc,
                        y + inc_rel, z, &mt, &si, true));
                    if (p) {
                        p->hold_on_.wayFree = 2;
                        p->hold_on_.tilex = x;
                        p->hold_on_.tiley = y;
                        if (sub_type_ == 0x14) {
                            p->hold_on_.xadj = 1;
                            p->hold_on_.yadj = 0;
                        } else if (sub_type_ == 0x0B) {
                            p->hold_on_.xadj = 0;
                            p->hold_on_.yadj = 1;
                        }
                        p->hold_on_.tilez = z;
                    }
                } while (p);
            }
            *j = 1;
            for ( *i = -1; *i <= 1; *i += 1 ) {
                mt = 1; si = 0;
                do {
                    p = (PedInstance *)(obj->findAt(x + rel_inc,
                        y + inc_rel, z, &mt, &si, true));
                    if (p) {
                        p->hold_on_.wayFree = 2;
                        p->hold_on_.tilex = x;
                        p->hold_on_.tiley = y;
                        if (sub_type_ == 0x14) {
                            p->hold_on_.xadj = 1;
                            p->hold_on_.yadj = 0;
                        } else if (sub_type_ == 0x0B) {
                            p->hold_on_.xadj = 0;
                            p->hold_on_.yadj = 1;
                        }
                        p->hold_on_.tilez = z;
                    }
                } while (p);
            }
            break;
        case 1: //closing
            if (frame_ >= g_App.gameSprites().lastFrame(closing_anim_)) {
                state_ = 2;
                frame_ = 0;
            }
            break;
        case 3: // opening
            if (frame_ >= g_App.gameSprites().lastFrame(opening_anim_)) {
                state_ = 0;
                frame_ = 0;
            }
            break;
    }
    return changed;
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

NeonSign::NeonSign(int m, int anim):Static(m),
anim_(anim)
{
}

void NeonSign::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
}
