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

#include "common.h"
#include "app.h"
#ifdef SYSTEM_SDL
#include "system_sdl.h"
#endif
MapObject::MapObject(int m):map_(m), frame_(0), elapsed_carry_(0),
frames_per_sec_(8), sub_type_(0), main_type_(0), dir_(0), is_ignored_(false),
size_x_(1), size_y_(1), size_z_(1), major_type_(MapObject::mt_Undefined)
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
    frame_ %= frames_per_sec_ << 3;
    return changed;
}

void MapObject::setDirection(int dir) {
    assert(dir >= 0);
    dir_ = dir;
}

// posx = targetx - objx
// posy = targety - objy
// if dir == NULL, object callers dir_ will be set
void MapObject::setDirection(int posx, int posy, int * dir) {

    int direction = -1;
    double PI = 3.14159265;
    if (posx == 0) {
        if (posy < 0) {
            direction = 128;
        } else if (posy > 0) {
            direction = 0;
        }
    } else if (posy == 0) {
        if (posx > 0) {
            direction = 64;
        } else if (posx < 0) {
            direction = 192;
        }
    } else if (posx < 0) {
        if (posy > 0) {
            posx = -posx;
            direction = (int)((128.0
                * atan((double)posy / (double)posx)) / PI + 192.0);
        } else { // posy < 0
            int swapx = -posx;
            posx = -posy;
            posy = swapx;
            direction = (int)((128.0
                * atan((double)posy / (double)posx)) / PI + 128.0);
        }
    } else if (posx > 0 && posy < 0) {
        posy = -posy;
        direction = (int)((128.0
            * atan((double)posy / (double)posx)) / PI + 64.0);
    } else { // posx > 0 && posy > 0
        int swapx = posx;
        posy = posx;
        posx = swapx;
        direction = (int)((128.0
            * atan((double)posy / (double)posx)) / PI);
    }
    if (direction != -1) {
        if (dir == NULL)
            this->dir_ = direction;
        else
            *dir = direction;
    }
}

int MapObject::getDirection(int snum) {
    assert(snum > 0);

    int direction = 0;
    int sinc = 256 / snum;
    int sdec = sinc / 2;
    do {
        int s = direction * sinc;
        if (direction == 0) {
            if ((256 - sdec) < dir_ || (s + sdec) >= dir_)
                break;
        } else {
            if ((s - sdec) < dir_ && (s + sdec) >= dir_)
                break;
        }
        direction++;
    } while (direction < snum);
    assert(direction < snum);

    return direction;
}

/*
* NOTE: inc_xyz should point to array of three elements of type
* double for x,y,z
*/
bool MapObject::isBlocker(toDefineXYZ * startXYZ, toDefineXYZ * endXYZ,
               double * inc_xyz)
{
    // NOTE: algorithm used check whether object is located within range
    // defined by "start" and "end", then assuming that x ccord belongs to
    // vector calculate y from x range and compare range by y, if it is ok,
    // calculate z from y range, then if in range by z, recalculate x and y

    // range_x check
    int range_x_h = tile_x_ * 256 + off_x_;
    int range_x_l = range_x_h - size_x_;
    range_x_h += size_x_;
    range_x_h--;
    int low_num = startXYZ->x;
    int high_num = endXYZ->x;
    bool flipped_x = false;
    if (startXYZ->x > endXYZ->x) {
        high_num = startXYZ->x;
        low_num = endXYZ->x;
        flipped_x = true;
    }
    if (range_x_l > high_num || range_x_h < low_num)
        return false;
    if (range_x_l < low_num)
        range_x_l = low_num;
    if (range_x_h > high_num)
        range_x_h = high_num;

    // range_y check
    int range_y_h = tile_y_ * 256 + off_y_;
    int range_y_l = range_y_h - size_y_;
    range_y_h += size_y_;
    range_y_h--;
    bool flipped_y = false;
    if (startXYZ->y > endXYZ->y) {
        high_num = startXYZ->y;
        low_num = endXYZ->y;
        flipped_y = true;
    } else {
        low_num = startXYZ->y;
        high_num = endXYZ->y;
    }
    if (range_y_l > high_num || range_y_h < low_num)
        return false;
    if (range_y_l < low_num)
        range_y_l = low_num;
    if (range_y_h > high_num)
        range_y_h = high_num;

    // range_z check
    int range_z_l = vis_z_ * 128 + off_z_;
    int range_z_h = range_z_l + size_z_ * 2;
    range_z_h--;
    bool flipped_z = false;
    if (startXYZ->z > endXYZ->z) {
        high_num = startXYZ->z;
        low_num = endXYZ->z;
        flipped_z = false;
    } else {
        low_num = startXYZ->z;
        high_num = endXYZ->z;
    }
    if (range_z_l > high_num || range_z_h < low_num)
        return false;
    if (range_z_l < low_num)
        range_z_l = low_num;
    if (range_z_h > high_num)
        range_z_h = high_num;

    double d_l = 0;
    double d_h = 0;
    int range_g_l = 0;
    int range_g_h = 0;
    if (inc_xyz[0] != 0) {
        if (flipped_x) {
            d_h = ((double)(range_x_l - startXYZ->x)) / inc_xyz[0];
            d_l = ((double)(range_x_h - startXYZ->x)) / inc_xyz[0];
        } else {
            d_l = ((double)(range_x_l - startXYZ->x)) / inc_xyz[0];
            d_h = ((double)(range_x_h - startXYZ->x)) / inc_xyz[0];
        }
        range_g_l = (int)(d_l * inc_xyz[1] + startXYZ->y);
        range_g_h = (int)(d_h * inc_xyz[1] + startXYZ->y);
        if (range_g_h < range_g_l) {
            low_num = range_g_h;
            high_num = range_g_l;
        } else {
            low_num = range_g_l;
            high_num = range_g_h;
        }
        if (low_num > range_y_h || high_num < range_y_l)
            return false;
        if (low_num > range_y_l)
            range_y_l = low_num;
        if (high_num < range_y_h)
            range_y_h = high_num;
    }

    if (inc_xyz[1] != 0) {
        if (flipped_y) {
            d_h = ((double)(range_y_l - startXYZ->y)) / inc_xyz[1];
            d_l = ((double)(range_y_h - startXYZ->y)) / inc_xyz[1];
        } else {
            d_l = ((double)(range_y_l - startXYZ->y)) / inc_xyz[1];
            d_h = ((double)(range_y_h - startXYZ->y)) / inc_xyz[1];
        }
    }
    if (inc_xyz[1] != 0 || inc_xyz[0] != 0) {
        range_g_l = (int)(d_l * inc_xyz[2] + startXYZ->z);
        range_g_h = (int)(d_h * inc_xyz[2] + startXYZ->z);
        if (range_g_h < range_g_l) {
            low_num = range_g_h;
            high_num = range_g_l;
        } else {
            low_num = range_g_l;
            high_num = range_g_h;
        }
        if (low_num > range_z_h || high_num < range_z_l)
            return false;
        if (low_num > range_z_l)
            range_z_l = low_num;
        if (high_num < range_z_h)
            range_z_h = high_num;
    }

    if (flipped_z) {
        low_num = range_z_l;
        range_z_l = range_z_h;
        range_z_h = low_num;
    }
    if (inc_xyz[2] != 0) {
        d_l = ((double)(range_z_l - startXYZ->z)) / inc_xyz[2];
        d_h = ((double)(range_z_h - startXYZ->z)) / inc_xyz[2];
        range_g_l = (int)(d_l * inc_xyz[1] + startXYZ->y);
        range_g_h = (int)(d_h * inc_xyz[1] + startXYZ->y);
        if (range_g_h < range_g_l) {
            low_num = range_g_h;
            high_num = range_g_l;
        } else {
            low_num = range_g_l;
            high_num = range_g_h;
        }
        if (low_num > range_y_h || high_num < range_y_l)
            return false;
        range_y_l = range_g_l;
        range_y_h = range_g_h;

        range_g_l = (int)(d_l * inc_xyz[0] + startXYZ->x);
        range_g_h = (int)(d_h * inc_xyz[0] + startXYZ->x);
        if (range_g_h < range_g_l) {
            low_num = range_g_h;
            high_num = range_g_l;
        } else {
            low_num = range_g_l;
            high_num = range_g_h;
        }
        if (low_num > range_x_h || high_num < range_x_l)
            return false;
        range_x_l = range_g_l;
        range_x_h = range_g_h;
    } else {
        if (inc_xyz[1] != 0) {
            range_g_l = (int)(d_l * inc_xyz[0] + startXYZ->x);
            range_g_h = (int)(d_h * inc_xyz[0] + startXYZ->x);
            if (range_g_h < range_g_l) {
                low_num = range_g_h;
                high_num = range_g_l;
            } else {
                low_num = range_g_l;
                high_num = range_g_h;
            }
            if (low_num > range_x_h || high_num < range_x_l)
                return false;
            range_x_l = range_g_l;
            range_x_h = range_g_h;
        }
    }

    startXYZ->x = range_x_l;
    startXYZ->y = range_y_l;
    startXYZ->z = range_z_l;
    endXYZ->x = range_x_h;
    endXYZ->y = range_y_h;
    endXYZ->z = range_z_h;

    return true;
}

SFXObject::SFXObject(int m, int type):MapObject(m), sfx_life_over_(false)
{
    main_type_ = type;
    setTimeShowAnim(0);
    switch(type) {
        case SFXObject::sfxt_Unknown:
            printf("Unknown sfx created");
            sfx_life_over_ = true;
            break;
        case SFXObject::sfxt_BulletHit:
            anim_ = 382;
            break;
        case SFXObject::sfxt_FlamerFire:
            anim_ = 383;
            break;
        case SFXObject::sfxt_Smoke:
            anim_ = 244;
            break;
        case SFXObject::sfxt_Fire_LongSmoke:
            // point of impact for laser
            anim_ = 389;
            break;
        case SFXObject::sfxt_ExplosionFire:
            anim_ = 390;
            break;
        case SFXObject::sfxt_ExplosionBall:
            anim_ = 391;
            setFramesPerSec(6);
            break;
        case SFXObject::sfxt_LargeFire:
            anim_ = 243;
            setTimeShowAnim(4000);
            break;
    }
}

void SFXObject::draw(int x, int y) {
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
}

bool SFXObject::animate(int elapsed) {

    bool changed = MapObject::animate(elapsed);
    if (main_type_ == SFXObject::sfxt_ExplosionBall) {
        int z = tile_z_ * 128 + off_z_;
        // 250 per sec
        z += (elapsed >> 2);
        if (z > (g_Session.getMission()->mmax_z_ - 1) * 128)
            z = (g_Session.getMission()->mmax_z_ - 1) * 128;
        tile_z_ = z / 128;
        vis_z_ = tile_z_;
        off_z_ = z % 128;
    }
    if (frame_ > g_App.gameSprites().lastFrame(anim_)
        && !leftTimeShowAnim(elapsed))
        sfx_life_over_ = true;
    return changed;
}

void SFXObject::correctZ() {
    if (main_type_ == SFXObject::sfxt_ExplosionBall) {
        int z = tile_z_ * 128 + off_z_;
        z += 512;
        if (z > (g_Session.getMission()->mmax_z_ - 1) * 128)
            z = (g_Session.getMission()->mmax_z_ - 1) * 128;
        tile_z_ = z / 128;
        vis_z_ = tile_z_;
        off_z_ = z % 128;
    }
}

ShootableMapObject::ShootableMapObject(int m):MapObject(m)
{
}

ShootableMovableMapObject::
ShootableMovableMapObject(int m):ShootableMapObject(m),
speed_(0)
{
}

bool ShootableMovableMapObject::updatePlacement(int nOffX, int nOffY)
{

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
    Mission::LEVELDATA_STATICS * gamdata =
        (Mission::LEVELDATA_STATICS *) data;
    Static *s = 0;

    // TODO: find where object current state is,
    // some windows are broken/open etc
    // Also verify whether object description is correct
    // subtype for doors, use instead orientation?
    // add sizes for windows
    uint16 curanim = READ_LE_UINT16(gamdata->index_current_anim);
    uint16 baseanim = READ_LE_UINT16(gamdata->index_base_anim);
    uint16 curframe = READ_LE_UINT16(gamdata->index_current_frame);
    switch(gamdata->sub_type) {
        case 0x01:
            // phone booth
            s = new EtcObj(m, curanim, curanim, curanim);
            s->setSizeX(128);
            s->setSizeY(128);
            s->setSizeZ(128);
            break;
        case 0x05:// 1040-1043, 1044 - damaged
            // crossroad things
            s = new EtcObj(m, curanim, curanim, curanim);
            s->setSizeX(64);
            s->setSizeY(64);
            s->setSizeZ(64);
            break;
        case 0x06:
            // crossroad things
            s = new EtcObj(m, curanim, curanim, curanim);
            s->setSizeX(64);
            s->setSizeY(64);
            s->setSizeZ(64);
            break;
        case 0x07:
            // crossroad things
            s = new EtcObj(m, curanim, curanim, curanim);
            s->setSizeX(64);
            s->setSizeY(64);
            s->setSizeZ(64);
            break;
        case 0x08:
            // crossroad things
            s = new EtcObj(m, curanim, curanim, curanim);
            s->setSizeX(64);
            s->setSizeY(64);
            s->setSizeZ(64);
            break;
        case 0x0B:
            // 0x0270 animation, is this object present in original game?
            //s = new EtcObj(m, curanim, curanim, curanim);
            //printf("0x0B anim %X\n", curanim);
            break;
        case 0x0A:
            s = new NeonSign(m, curanim);
            s->setFrame(g_App.gameSprites().getFrameFromFrameIndx(curframe));
            s->setIsIgnored(true);
            s->setSizeX(32);
            s->setSizeY(1);
            s->setSizeZ(48);
            break;
        case 0x0C: // closed door
            if (gamdata->orientation == 0x00 || gamdata->orientation == 0x80
                || gamdata->orientation == 0x7E || gamdata->orientation == 0xFE) {
                s = new Door(m, baseanim, baseanim + 2, baseanim + 4, baseanim + 6);
                s->setSubType(0);
                s->setSizeX(128);
                s->setSizeY(1);
                s->setSizeZ(128);
            } else {
                baseanim++;
                s = new Door(m, baseanim, baseanim + 2, baseanim + 4, baseanim + 6);
                s->setSubType(2);
                s->setSizeX(1);
                s->setSizeY(128);
                s->setSizeZ(128);
            }
            break;
        case 0x0D: // closed door
            if (gamdata->orientation == 0x00 || gamdata->orientation == 0x80
                || gamdata->orientation == 0x7E || gamdata->orientation == 0xFE) {
                s = new Door(m, baseanim, baseanim + 2, baseanim + 4, baseanim + 6);
                s->setSubType(0);
                s->setSizeX(128);
                s->setSizeY(1);
                s->setSizeZ(128);
            } else {
                baseanim++;
                s = new Door(m, baseanim, baseanim + 2, baseanim + 4, baseanim + 6);
                s->setSubType(2);
                s->setSizeX(1);
                s->setSizeY(128);
                s->setSizeZ(128);
            }
            break;
        case 0x0E: // opening doors, not open
            if (gamdata->orientation == 0x00 || gamdata->orientation == 0x80
                || gamdata->orientation == 0x7E || gamdata->orientation == 0xFE) {
                s = new Door(m, baseanim, baseanim + 2, baseanim + 4, baseanim + 6);
                s->setSubType(0);
                s->setSizeX(128);
                s->setSizeY(1);
                s->setSizeZ(128);
            } else {
                baseanim++;
                s = new Door(m, baseanim, baseanim + 2, baseanim + 4, baseanim + 6);
                s->setSubType(2);
                s->setSizeX(1);
                s->setSizeY(128);
                s->setSizeZ(128);
            }
            s->state_ = sttdoor_Opening;
            break;
        case 0x0F: // opening doors, not open
            if (gamdata->orientation == 0x00 || gamdata->orientation == 0x80
                || gamdata->orientation == 0x7E || gamdata->orientation == 0xFE) {
                s = new Door(m, baseanim, baseanim + 2, baseanim + 4, baseanim + 6);
                s->setSubType(0);
                s->setSizeX(128);
                s->setSizeY(1);
                s->setSizeZ(128);
            } else {
                baseanim++;
                s = new Door(m, baseanim, baseanim + 2, baseanim + 4, baseanim + 6);
                s->setSubType(2);
                s->setSizeX(1);
                s->setSizeY(128);
                s->setSizeZ(128);
            }
            s->state_ = sttdoor_Opening;
            break;
        case 0x11:
            // ???? what is this
            //s = new EtcObj(m, curanim, curanim, curanim);
            //printf("0x11 anim %X\n", curanim);
            break;
        case 0x12:
            // open window
            s = new WindowObj(m, curanim, curanim + 2, curanim + 4);
            s->setIsIgnored(true);
            break;
        case 0x13:
            // closed window
            s = new WindowObj(m, curanim, curanim + 4, curanim + 6);
            s->setIsIgnored(true);
            break;
        case 0x15:
            // damaged window
            s = new WindowObj(m, curanim, curanim, curanim);
            s->setIsIgnored(true);
            break;
        case 0x16:
            // TODO: set state if damaged trees exist
            s = new Tree(m, curanim, curanim + 1, curanim + 2);
            s->setSizeX(64);
            s->setSizeY(64);
            s->setSizeZ(128);
            s->setHealth(1);
            s->setStartHealth(1);
            break;
        case 0x19:
            // trash can / mail box
            s = new EtcObj(m, curanim, curanim, curanim);
            s->setSizeX(64);
            s->setSizeY(64);
            s->setSizeZ(96);
            break;
        case 0x1A:
            // advertisement on wall
            s = new EtcObj(m, curanim, curanim, curanim);
            s->setMainType(1);
            s->setIsIgnored(true);
            break;
        case 0x1C:
            // ???? what is this?
            //s = new EtcObj(m, curanim, curanim, curanim);
            //printf("0x1C anim %X\n", curanim);
            break;
        case 0x1F:
            // advertisement on wall + brokem signal
            s = new EtcObj(m, curanim, curanim, curanim);
            s->setMainType(1);
            s->setIsIgnored(true);
            break;
        case 0x20:
            // window without light animated
            s = new EtcObj(m, curanim, curanim, curanim);
            s->setIsIgnored(true);
            break;
        case 0x21:
            // window without light animated
            s = new EtcObj(m, curanim, curanim, curanim);
            s->setIsIgnored(true);
            break;
        case 0x23:
            // window with person's shadow
            s = new EtcObj(m, curanim, curanim, curanim);
            s->setIsIgnored(true);
            break;
        case 0x24:
            // window with person's shadow
            s = new EtcObj(m, curanim, curanim, curanim);
            s->setIsIgnored(true);
            break;
        case 0x25:
            // window without light
            s = new EtcObj(m, curanim, curanim, curanim);
            s->setIsIgnored(true);
            break;
        case 0x26:
            // 0x00,0x80 south - north = 0
            // 0x40,0xC0 weast - east = 2
            s = new LargeDoor(m, curanim, curanim + 1, curanim + 2);
            if (gamdata->orientation == 0x00 || gamdata->orientation == 0x80) {
                s->setSubType(0);
                s->setSizeX(256);
                s->setSizeY(64);
                s->setSizeZ(192);
            } else {
                s->setSubType(2);
                s->setSizeX(64);
                s->setSizeY(256);
                s->setSizeZ(192);
            }
            break;
            /*
        default:
            printf("uknown type %02X , %02X, %X\n", gamdata->sub_type,
                gamdata->orientation,
                READ_LE_UINT16(gamdata->index_current_frame));
            printf("x is %i, xoff is %i ==", gamdata->mapposx[1],
                gamdata->mapposx[0]);
            printf("y is %i, yoff is %i ==", gamdata->mapposy[1],
                gamdata->mapposy[0]);
            printf("z is %i, zoff is %i\n", gamdata->mapposz[1],
                gamdata->mapposz[0]);
            break;
            */
    }

    if (s) {
        int z = READ_LE_UINT16(gamdata->mapposz) >> 7;
        z--;
        int oz = gamdata->mapposz[0] & 0x7F;
        s->setVisZ(z);
        // patch for advertisement on wall
        if (s->getMainType() == 1)
            z += 2;

        if (oz > 0)
            z++;
        s->setPosition(gamdata->mapposx[1], gamdata->mapposy[1],
                       z, gamdata->mapposx[0],
                       gamdata->mapposy[0], oz);
        s->setMainType(gamdata->sub_type);
        s->setDirection(gamdata->orientation);
    }

    return s;
}

Door::Door(int m, int anim, int closingAnim, int openAnim, int openingAnim):Static(m),
anim_(anim), closing_anim_(closingAnim),open_anim_(openAnim),
opening_anim_(openingAnim)
{
    state_ = Static::sttdoor_Closed;
    rcv_damage_def_ = MapObject::ddmg_Invulnerable;
    major_type_ = MapObject::mt_Static;
}

void Door::draw(int x, int y)
{
    addOffs(x, y);
    switch(state_) {
        case Static::sttdoor_Open:
            g_App.gameSprites().drawFrame(open_anim_, frame_, x, y);
            break;
        case Static::sttdoor_Closing:
            g_App.gameSprites().drawFrame(closing_anim_, frame_, x, y);
            break;
        case Static::sttdoor_Closed:
            g_App.gameSprites().drawFrame(anim_, frame_, x, y);
            break;
        case Static::sttdoor_Opening:
            g_App.gameSprites().drawFrame(opening_anim_, frame_, x, y);
            break;
    }
}

bool Door::animate(int elapsed, Mission *obj)
{
    PedInstance *p = NULL;
    int x = tileX();
    int y = tileY();
    int z = tileZ();
    MapObject::MajorTypeEnum mt;
    int si;
    char inc_rel = 0, rel_inc = 0;
    char *i = 0, *j = 0;
    bool found = false;

    bool changed = MapObject::animate(elapsed);
    switch(state_) {
        case Static::sttdoor_Open:
            if (sub_type_ == 0) {
                i = &rel_inc;
                j = &inc_rel;
            } else if (sub_type_ == 2) {
                i = &inc_rel;
                j = &rel_inc;
            }
            assert(i != 0 && j != 0);
            for(*i = 0; *i < 2; *i += 1) {
                mt = MapObject::mt_Ped; si = 0;
                do {
                    p = (PedInstance *)(obj->findAt(x + inc_rel,
                        y + rel_inc, z, &mt, &si, true));
                    if (!p && state_ == Static::sttdoor_Open && (!found)) {
                        state_ = Static::sttdoor_Closing;
                        is_ignored_ = false;
                        frame_ = 0;
                    } else if (p && p->health() > 0){
                        state_ = Static::sttdoor_Open;
                        is_ignored_ = true;
                        found = true;
                        p->hold_on_.wayFree = 0;
                    }
                } while (p);
            }
            break;
        case Static::sttdoor_Closed:
            if (sub_type_ == 0) {
                i = &rel_inc;
                j = &inc_rel;
            } else if (sub_type_ == 2) {
                i = &inc_rel;
                j = &rel_inc;
            }
            assert(i != 0 && j != 0);
            *i = 1;
            mt = MapObject::mt_Ped; si = 0;
            do {
                p = (PedInstance *)(obj->findAt(x + inc_rel,
                    y + rel_inc, z, &mt, &si, true));
                if (p && p->health() > 0) {
                    if (!found) {
                        state_ = Static::sttdoor_Opening;
                        found = true;
                        frame_ = 0;
                    }
                    p->hold_on_.wayFree = 1;
                }
            } while (p);
            *i = 0;
            mt = MapObject::mt_Ped; si = 0;
            do {
                p = (PedInstance *)(obj->findAt(x + inc_rel,
                    y + rel_inc, z, &mt, &si, true));
                if (p && p->health() > 0) {
                    if (!found) {
                        state_ = Static::sttdoor_Opening;
                        found = true;
                        frame_ = 0;
                    }
                    p->hold_on_.wayFree = 1;
                }
            } while (p);
            break;
        case Static::sttdoor_Closing:
            if (frame_ > g_App.gameSprites().lastFrame(closing_anim_)) {
                state_ = Static::sttdoor_Closed;
                frame_ = 0;
            }
            break;
        case Static::sttdoor_Opening:
            if (frame_ > g_App.gameSprites().lastFrame(opening_anim_)) {
                state_ = Static::sttdoor_Open;
                is_ignored_ = true;
                frame_ = 0;
            }
            break;
    }
    return changed;
}

LargeDoor::LargeDoor(int m, int anim, int closingAnim, int openingAnim):Static(m), anim_(anim),
closing_anim_(closingAnim), opening_anim_(openingAnim)
{
    state_ = Static::sttdoor_Closed;
    rcv_damage_def_ = MapObject::ddmg_Invulnerable;
    major_type_ = MapObject::mt_Static;
}

void LargeDoor::draw(int x, int y)
{
    addOffs(x, y);
    switch(state_) {
        case Static::sttdoor_Open:
            break;
        case Static::sttdoor_Closing:
            g_App.gameSprites().drawFrame(closing_anim_, frame_, x, y);
            break;
        case Static::sttdoor_Closed:
            g_App.gameSprites().drawFrame(anim_, frame_, x, y);
            break;
        case Static::sttdoor_Opening:
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
    MapObject::MajorTypeEnum mt;
    int si;
    char inc_rel = 0, rel_inc = 0;
    char *i = 0, *j = 0;
    bool found = false;

    bool changed = MapObject::animate(elapsed);
    switch(state_) {
        case Static::sttdoor_Open:
            if (sub_type_ == 0) {
                i = &rel_inc;
                j = &inc_rel;
            } else if (sub_type_ == 2) {
                i = &inc_rel;
                j = &rel_inc;
            }
            assert(i != 0 && j != 0);
            *j = -1;
            for(*i = -2; *i < 3; *i += 1) {
                mt = MapObject::mt_Vehicle; si = 0;
                v = (VehicleInstance *)(obj->findAt(x + inc_rel,
                    y + rel_inc,z, &mt, &si, true));
                if (!v && state_ == Static::sttdoor_Open && (!found)) {
                    state_ = Static::sttdoor_Closing;
                    is_ignored_ = false;
                    frame_ = 0;
                } else if (v){
                    state_ = Static::sttdoor_Open;
                    is_ignored_ = true;
                    found = true;
                    v->hold_on_.wayFree = 0;
                }
            }
            *j = 1;
            for(*i = -2; *i < 3; *i += 1) {
                mt = MapObject::mt_Vehicle; si = 0;
                v = (VehicleInstance *)(obj->findAt(x + inc_rel,
                    y + rel_inc,z,&mt,&si,true));
                if (!v && state_ == Static::sttdoor_Open && (!found)) {
                    state_ = Static::sttdoor_Closing;
                    is_ignored_ = false;
                    frame_ = 0;
                } else if (v){
                    state_ = Static::sttdoor_Open;
                    is_ignored_ = true;
                    found = true;
                    v->hold_on_.wayFree = 0;
                }
            }
            for (int a = (y - 1); a <= (y + 1); a++ ) {
                for (int b = (x - 1); b <= (x + 1); b++) {
                    mt = MapObject::mt_Ped; si = 0;
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
        case Static::sttdoor_Closed:
            char sign;
            if (sub_type_ == 0) {
                i = &rel_inc;
                j = &inc_rel;
                sign = 1;
            } else if (sub_type_ == 2) {
                i = &inc_rel;
                j = &rel_inc;
                sign = -1;
            }
            assert(i != 0 && j != 0);
            *j = -1 * sign;
            *i = -2;
            mt = MapObject::mt_Vehicle; si = 0;
            v = (VehicleInstance *)(obj->findAt(x + inc_rel,
                y + rel_inc,z,&mt,&si,true));
            if (v) {
                if (!found) {
                    state_ = Static::sttdoor_Opening;
                    found = true;
                    frame_ = 0;
                }
                v->hold_on_.wayFree = 1;
            }
            *j = 1 * sign;
            *i = 2;
            mt = MapObject::mt_Vehicle; si = 0;
            v = (VehicleInstance *)(obj->findAt(x + inc_rel,
                y + rel_inc,z,&mt,&si,true));
            if (v) {
                if (!found) {
                    state_ = Static::sttdoor_Opening;
                    found = true;
                    frame_ = 0;
                }
                v->hold_on_.wayFree = 1;
            }
            *j = -1;
            for ( *i = -1; *i <= 1; *i += 1 ) {
                mt = MapObject::mt_Ped; si = 0;
                do {
                    p = (PedInstance *)(obj->findAt(x + rel_inc,
                        y + inc_rel, z, &mt, &si, true));
                    if (p) {
                        p->hold_on_.wayFree = 2;
                        p->hold_on_.tilex = x;
                        p->hold_on_.tiley = y;
                        if (sub_type_ == 0) {
                            p->hold_on_.xadj = 1;
                            p->hold_on_.yadj = 0;
                        } else if (sub_type_ == 2) {
                            p->hold_on_.xadj = 0;
                            p->hold_on_.yadj = 1;
                        }
                        p->hold_on_.tilez = z;
                    }
                } while (p);
            }
            *j = 1;
            for ( *i = -1; *i <= 1; *i += 1 ) {
                mt = MapObject::mt_Ped; si = 0;
                do {
                    p = (PedInstance *)(obj->findAt(x + rel_inc,
                        y + inc_rel, z, &mt, &si, true));
                    if (p) {
                        p->hold_on_.wayFree = 2;
                        p->hold_on_.tilex = x;
                        p->hold_on_.tiley = y;
                        if (sub_type_ == 0) {
                            p->hold_on_.xadj = 1;
                            p->hold_on_.yadj = 0;
                        } else if (sub_type_ == 2) {
                            p->hold_on_.xadj = 0;
                            p->hold_on_.yadj = 1;
                        }
                        p->hold_on_.tilez = z;
                    }
                } while (p);
            }
            break;
        case Static::sttdoor_Closing:
            if (frame_ > g_App.gameSprites().lastFrame(closing_anim_)) {
                state_ = Static::sttdoor_Closed;
                frame_ = 0;
            }
            break;
        case Static::sttdoor_Opening:
            if (frame_ > g_App.gameSprites().lastFrame(opening_anim_)) {
                state_ = Static::sttdoor_Open;
                is_ignored_ = true;
                frame_ = 0;
            }
            break;
    }
    return changed;
}

Tree::Tree(int m, int anim, int burningAnim, int damagedAnim):Static(m),
anim_(anim), burning_anim_(burningAnim), damaged_anim_(damagedAnim)
{
    rcv_damage_def_ = MapObject::ddmg_StaticTree;
    major_type_ = MapObject::mt_Static;
    state_ = Static::stttree_Healthy;
}

void Tree::draw(int x, int y)
{
    addOffs(x, y);
    switch (state_) {
        case Static::stttree_Healthy:
            g_App.gameSprites().drawFrame(anim_, frame_, x, y);
            break;
        case Static::stttree_Burning:
            g_App.gameSprites().drawFrame(burning_anim_, frame_, x, y);
            break;
        case Static::stttree_Damaged:
            g_App.gameSprites().drawFrame(damaged_anim_, frame_, x, y);
            break;
    }
}

bool Tree::animate(int elapsed, Mission *obj) {

    if (state_ == Static::stttree_Burning) {
        if (!(leftTimeShowAnim(elapsed))) {
            state_ = Static::stttree_Damaged;
            frame_ = 0;
            setFramesPerSec(2);
            return true;
        }
    }

    return MapObject::animate(elapsed);
}

bool Tree::handleDamage(MapObject::DamageInflictType *d) {
    if (health_ <= 0 || rcv_damage_def_ == MapObject::ddmg_Invulnerable
        || (d->dtype & rcv_damage_def_) == 0)
        return false;

    health_ -= d->dvalue;
    if (health_ <= 0) {
        state_ = Static::stttree_Burning;
        setTimeShowAnim(7000);
        is_ignored_ = true;
    }
    return true;
}

WindowObj::WindowObj(int m, int anim, int breakingAnim, int damagedAnim):Static(m),
anim_(anim), breaking_anim_(breakingAnim), damaged_anim_(damagedAnim)
{
    rcv_damage_def_ = MapObject::ddmg_StaticWindow;
    major_type_ = MapObject::mt_Static;
}

void WindowObj::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
}

EtcObj::EtcObj(int m, int anim, int burningAnim , int damagedAnim):Static(m),
anim_(anim), burning_anim_(burningAnim), damaged_anim_(damagedAnim)
{
    rcv_damage_def_ = MapObject::ddmg_StaticGeneral;
    major_type_ = MapObject::mt_Static;
}

void EtcObj::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
}

NeonSign::NeonSign(int m, int anim):Static(m),
anim_(anim)
{
    rcv_damage_def_ = MapObject::ddmg_Invulnerable;
    major_type_ = MapObject::mt_Static;
}

void NeonSign::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
}
