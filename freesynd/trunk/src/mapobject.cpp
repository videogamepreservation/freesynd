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
#include "vehicle.h"
#include "core/gamesession.h"
#include "mission.h"

MapObject::MapObject(int m):size_x_(1), size_y_(1), size_z_(2),
    map_(m), frame_(0), elapsed_carry_(0),
    frames_per_sec_(8), sub_type_(0), main_type_(0),
    major_type_(MapObject::mjt_Undefined), dir_(0),
    time_show_anim_(-1), time_showing_anim_(-1),
    is_ignored_(false), is_frame_drawn_(false),
    state_(0xFFFFFFFF)
{
#ifdef _DEBUG
    debug_id_ = 0xFFFFFFFF;
#endif
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

/* NOTE posx = targetx - objx
 * posy = targety - objy
 * if dir == NULL, object callers dir_ will be set
 */
void MapObject::setDirection(int posx, int posy, int * dir) {

    int direction = -1;

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
        posx = posy;
        posy = swapx;
        direction = (int)((128.0
            * atan((double)posy / (double)posx)) / PI);
    }
    if (direction != -1) {
        if (dir == NULL)
            dir_ = direction;
        else
            *dir = direction;
    }
}

/*!
 * @returns direction for selected number of surfaces
 */
int MapObject::getDirection(int snum) {
    assert(snum > 0);

    int direction = 0;
    int sinc = 256 / snum;
    int sdec = sinc / 2;
    do {
        int s = direction * sinc;
        if (direction == 0) {
            if ((256 - sdec) <= dir_ || (s + sdec) > dir_)
                break;
        } else if ((s - sdec) <= dir_ && (s + sdec) > dir_)
            break;
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
    // TODO: better set values of size for object, use to values
    // for single coordinate, rel_x_start(-vlaue) and rel_x_end(+value).
    // Vehicle and other objects with different directions will be handleDamage
    // correctly

    /* NOTE: algorithm, checks whether object is located within range
     * defined by "start" and "end", then we calculate distances from x, y, z
     * to their respective startXYZ, choose shortest, then longest between
     * them and recalculate position of entering of shot and exit point
     */

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

    // range_z check
    int range_z_l = tile_z_ * 128 + off_z_;
    int range_z_h = range_z_l + size_z_;
    range_z_h--;
    bool flipped_z = false;
    if (startXYZ->z > endXYZ->z) {
        high_num = startXYZ->z;
        low_num = endXYZ->z;
        flipped_z = true;
    } else {
        low_num = startXYZ->z;
        high_num = endXYZ->z;
    }
    if (range_z_l > high_num || range_z_h < low_num)
        return false;

    double d_l[3];
    double d_h[3];
    if (inc_xyz[0] != 0) {
        d_l[0] = ((double)(range_x_l - startXYZ->x)) / inc_xyz[0];
        d_h[0] = ((double)(range_x_h - startXYZ->x)) / inc_xyz[0];
    } else {
        d_l[0] = 0.0;
        d_h[0] = 0.0;
    }
    if (inc_xyz[1] != 0) {
        d_l[1] = ((double)(range_y_l - startXYZ->y)) / inc_xyz[1];
        d_h[1] = ((double)(range_y_h - startXYZ->y)) / inc_xyz[1];
    } else {
        d_l[1] = 0.0;
        d_h[1] = 0.0;
    }
    if (inc_xyz[0] != 0) {
        d_l[2] = ((double)(range_z_l - startXYZ->z)) / inc_xyz[2];
        d_h[2] = ((double)(range_z_h - startXYZ->z)) / inc_xyz[2];
    } else {
        d_l[2] = 0.0;
        d_h[2] = 0.0;
    }

    // shortest distances to starting point
    double d_s[3];
    if (d_l[0] > d_h[0])
        d_s[0] = d_h[0];
    else
        d_s[0] = d_l[0];

    if (d_l[1] > d_h[1])
        d_s[1] = d_h[1];
    else
        d_s[1] = d_l[1];

    if (d_l[2] > d_h[2])
        d_s[2] = d_h[2];
    else
        d_s[2] = d_l[2];

    // TODO: another look at this function later
    uint8 indx = 0;
    // longest non-zero distance to start
    if (d_s[0] != 0.0) {
        if (d_s[1] != 0) {
            if (d_s[0] > d_s[1]) {
                if (d_s[2] != 0.0 && d_s[0] < d_s[2])
                    indx = 2;
            } else {
                indx = 1;
                if (d_s[2] != 0.0 && d_s[1] < d_s[2])
                    indx = 2;
            }
        }
    } else if (d_s[1] != 0) {
        indx = 1;
        if (d_s[2] != 0.0) {
            if (d_s[1] < d_s[2])
                indx = 2;
        }
    } else
        indx = 2;

    int range_g_l = (int)(d_l[indx] * inc_xyz[0] + startXYZ->x);
    int range_g_h = (int)(d_h[indx] * inc_xyz[0] + startXYZ->x);
    if (range_g_h < range_g_l) {
        low_num = range_g_h;
        high_num = range_g_l;
    } else {
        low_num = range_g_l;
        high_num = range_g_h;
    }
    if (low_num > range_x_h || high_num < range_x_l)
        return false;
    if (range_x_l < low_num)
        range_x_l = low_num;
    if (range_x_h > high_num)
        range_x_h = high_num;

    range_g_l = (int)(d_l[indx] * inc_xyz[1] + startXYZ->y);
    range_g_h = (int)(d_h[indx] * inc_xyz[1] + startXYZ->y);
    if (range_g_h < range_g_l) {
        low_num = range_g_h;
        high_num = range_g_l;
    } else {
        low_num = range_g_l;
        high_num = range_g_h;
    }
    if (low_num > range_y_h || high_num < range_y_l)
        return false;
    if (range_y_l < low_num)
        range_y_l = low_num;
    if (range_y_h > high_num)
        range_y_h = high_num;

    range_g_l = (int)(d_l[indx] * inc_xyz[2] + startXYZ->z);
    range_g_h = (int)(d_h[indx] * inc_xyz[2] + startXYZ->z);
    if (range_g_h < range_g_l) {
        low_num = range_g_h;
        high_num = range_g_l;
    } else {
        low_num = range_g_l;
        high_num = range_g_h;
    }
    if (low_num > range_z_h || high_num < range_z_l)
        return false;
    if (range_z_l < low_num)
        range_z_l = low_num;
    if (range_z_h > high_num)
        range_z_h = high_num;

    // restoring coordinates to their respective low/high values
    if (flipped_x) {
        startXYZ->x = range_x_h;
        endXYZ->x = range_x_l;
    } else {
        startXYZ->x = range_x_l;
        endXYZ->x = range_x_h;
    }

    if (flipped_y) {
        startXYZ->y = range_y_h;
        endXYZ->y = range_y_l;
    } else {
        startXYZ->y = range_y_l;
        endXYZ->y = range_y_h;
    }

    if (flipped_z) {
        startXYZ->z = range_z_h;
        endXYZ->z = range_z_l;
    } else {
        startXYZ->z = range_z_l;
        endXYZ->z = range_z_h;
    }

    return true;
}

void MapObject::offzOnStairs(uint8 twd) {
    switch (twd) {
        case 0x01:
            off_z_ = 127 - (off_y_ >> 1);
            break;
        case 0x02:
            off_z_ = off_y_ >> 1;
            break;
        case 0x03:
            off_z_ = off_x_ >> 1;
            break;
        case 0x04:
            off_z_ = 127 - (off_x_ >> 1);
            break;
        default:
            off_z_ = 0;
            break;
    }
}

SFXObject::SFXObject(int m, int type, int t_show) : MapObject(m),
    sfx_life_over_(false), draw_all_frames_(true), elapsed_left_(0)
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
            setFramesPerSec(12);
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
            setFramesPerSec(6);
            break;
        case SFXObject::sfxt_ExplosionBall:
            anim_ = 391;
            setFramesPerSec(6);
            break;
        case SFXObject::sfxt_LargeFire:
            anim_ = 243;
            setTimeShowAnim(3000 + t_show);
            break;
        case SFXObject::sfxt_SelArrow:
            anim_ = 601;
            time_show_anim_ = -1;
            setFramesPerSec(6);
            break;
        case SFXObject::sfxt_AgentFirst:
            anim_ = 1951;
            time_show_anim_ = -1;
            setFramesPerSec(4);
            break;
        case SFXObject::sfxt_AgentSecond:
            anim_ = 1952;
            time_show_anim_ = -1;
            setFramesPerSec(4);
            break;
        case SFXObject::sfxt_AgentThird:
            anim_ = 1953;
            time_show_anim_ = -1;
            setFramesPerSec(4);
            break;
        case SFXObject::sfxt_AgentFourth:
            anim_ = 1954;
            time_show_anim_ = -1;
            setFramesPerSec(4);
            break;
    }
}

void SFXObject::draw(int x, int y) {
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
}

bool SFXObject::animate(int elapsed) {

    if (is_frame_drawn_) {
        bool changed = draw_all_frames_ ? MapObject::animate(elapsed) : false;
        if (main_type_ == SFXObject::sfxt_ExplosionBall) {
            int z = tile_z_ * 128 + off_z_;
            // 250 per sec
            z += ((elapsed + elapsed_left_) >> 2);
            elapsed_left_ = elapsed &3;
            if (z > (g_Session.getMission()->mmax_z_ - 1) * 128)
                z = (g_Session.getMission()->mmax_z_ - 1) * 128;
            tile_z_ = z / 128;
            off_z_ = z % 128;
        }
        if (frame_ > g_App.gameSprites().lastFrame(anim_)
            && !leftTimeShowAnim(elapsed))
        {
            sfx_life_over_ = true;
        }
        return changed;
    }
    is_frame_drawn_ = true;
    return false;
}

void SFXObject::correctZ() {
    if (main_type_ == SFXObject::sfxt_ExplosionBall) {
        int z = tile_z_ * 128 + off_z_;
        z += 512;
        if (z > (g_Session.getMission()->mmax_z_ - 1) * 128)
            z = (g_Session.getMission()->mmax_z_ - 1) * 128;
        tile_z_ = z / 128;
        off_z_ = z % 128;
    }
}

ShootableMapObject::ShootableMapObject(int m):MapObject(m)
{
    rcv_damage_def_ = MapObject::ddmg_Invulnerable;
}

ShootableMovableMapObject::
ShootableMovableMapObject(int m):ShootableMapObject(m),
speed_(0), base_speed_(0), dist_to_pos_(0)
{
}

/*!
 * This method adds the given offsets to the object's offX and offY 
 * and moves it to a new tile if necessary.
 * \param nOffX amount to add to offX
 * \param nOffY amount to add to offY
 */
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

bool ShootableMovableMapObject::checkFinalDest(PathNode &pn) {
    if (dest_path_.empty())
        return false;
    PathNode &target_pn = dest_path_.back();
    return pn.tileX() == target_pn.tileX()
        && pn.tileY() == target_pn.tileY()
        && pn.tileZ() == target_pn.tileZ()
        && pn.offX() == target_pn.offX()
        && pn.offY() == target_pn.offY()
        && pn.offZ() == target_pn.offZ();
}

bool ShootableMovableMapObject::checkCurrPos(PathNode &pn) {
    return pn.tileX() == tile_x_
        && pn.tileY() == tile_y_
        && pn.tileZ() == tile_z_
        && pn.offX() == off_x_
        && pn.offY() == off_y_
        && pn.offZ() == off_z_;
}

bool ShootableMovableMapObject::checkCurrPosTileOnly(PathNode &pn) {
    return pn.tileX() == tile_x_
        && pn.tileY() == tile_y_
        && pn.tileZ() == tile_z_;
}

Static *Static::loadInstance(uint8 * data, int m)
{
    LevelData::Statics * gamdata =
        (LevelData::Statics *) data;
    Static *s = NULL;

    // TODO: verify whether object description is correct
    // subtype for doors, use instead orientation?

    // NOTE: objects states are usually mixed with type,
    // no separation between object type and its state
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
            s = new Semaphore(m, 1040, 1044);
            s->setSizeX(48);
            s->setSizeY(48);
            s->setSizeZ(48);
            s->state_ = sttsem_Stt0;
            s->setHealth(1);
            s->setStartHealth(1);
            break;
        case 0x06:
            // crossroad things
            s = new Semaphore(m, 1040, 1044);
            s->setSizeX(48);
            s->setSizeY(48);
            s->setSizeZ(48);
            s->state_ = sttsem_Stt1;
            s->setHealth(1);
            s->setStartHealth(1);
            break;
        case 0x07:
            // crossroad things
            s = new Semaphore(m, 1040, 1044);
            s->setSizeX(48);
            s->setSizeY(48);
            s->setSizeZ(48);
            s->state_ = sttsem_Stt2;
            s->setHealth(1);
            s->setStartHealth(1);
            break;
        case 0x08:
            // crossroad things
            s = new Semaphore(m, 1040, 1044);
            s->setSizeX(48);
            s->setSizeY(48);
            s->setSizeZ(48);
            s->state_ = sttsem_Stt3;
            s->setHealth(1);
            s->setStartHealth(1);
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
                s->setSizeX(256);
                s->setSizeY(1);
                s->setSizeZ(196);
            } else {
                baseanim++;
                s = new Door(m, baseanim, baseanim + 2, baseanim + 4, baseanim + 6);
                s->setSubType(2);
                s->setSizeX(1);
                s->setSizeY(256);
                s->setSizeZ(196);
            }
            break;
        case 0x0D: // closed door
            if (gamdata->orientation == 0x00 || gamdata->orientation == 0x80
                || gamdata->orientation == 0x7E || gamdata->orientation == 0xFE) {
                s = new Door(m, baseanim, baseanim + 2, baseanim + 4, baseanim + 6);
                s->setSubType(0);
                s->setSizeX(256);
                s->setSizeY(1);
                s->setSizeZ(196);
            } else {
                baseanim++;
                s = new Door(m, baseanim, baseanim + 2, baseanim + 4, baseanim + 6);
                s->setSubType(2);
                s->setSizeX(1);
                s->setSizeY(256);
                s->setSizeZ(196);
            }
            break;
        case 0x0E: // opening doors, not open
            if (gamdata->orientation == 0x00 || gamdata->orientation == 0x80
                || gamdata->orientation == 0x7E || gamdata->orientation == 0xFE) {
                s = new Door(m, baseanim, baseanim + 2, baseanim + 4, baseanim + 6);
                s->setSubType(0);
                s->setSizeX(256);
                s->setSizeY(1);
                s->setSizeZ(196);
            } else {
                baseanim++;
                s = new Door(m, baseanim, baseanim + 2, baseanim + 4, baseanim + 6);
                s->setSubType(2);
                s->setSizeX(1);
                s->setSizeY(256);
                s->setSizeZ(196);
            }
            s->state_ = sttdoor_Opening;
            break;
        case 0x0F: // opening doors, not open
            if (gamdata->orientation == 0x00 || gamdata->orientation == 0x80
                || gamdata->orientation == 0x7E || gamdata->orientation == 0xFE) {
                s = new Door(m, baseanim, baseanim + 2, baseanim + 4, baseanim + 6);
                s->setSubType(0);
                s->setSizeX(256);
                s->setSizeY(1);
                s->setSizeZ(196);
            } else {
                baseanim++;
                s = new Door(m, baseanim, baseanim + 2, baseanim + 4, baseanim + 6);
                s->setSubType(2);
                s->setSizeX(1);
                s->setSizeY(256);
                s->setSizeZ(196);
            }
            s->state_ = sttdoor_Opening;
            break;
        case 0x11:
            // ???? what is this
            //s = new EtcObj(m, bas, curanim, curanim);
            //printf("0x11 anim %X\n", curanim);
            break;
        case 0x12:
            // open window
            s = new WindowObj(m, curanim - 2, curanim, curanim + 2, curanim + 4);
            if (gamdata->orientation == 0x00 || gamdata->orientation == 0x80) {
                s->setSubType(0);
                s->setSizeX(96);
                s->setSizeY(4);
                s->setSizeZ(96);
            } else {
                s->setSubType(2);
                s->setSizeX(4);
                s->setSizeY(96);
                s->setSizeZ(96);
            }
            s->setHealth(1);
            s->setStartHealth(1);
            s->state_ = Static::sttwnd_Open;
            break;
        case 0x13:
            // closed window
            s = new WindowObj(m, curanim, curanim + 2, curanim + 4, curanim + 6);
            if (gamdata->orientation == 0x00 || gamdata->orientation == 0x80) {
                s->setSubType(0);
                s->setSizeX(96);
                s->setSizeY(4);
                s->setSizeZ(96);
            } else {
                s->setSubType(2);
                s->setSizeX(4);
                s->setSizeY(96);
                s->setSizeZ(96);
            }
            s->setHealth(1);
            s->setStartHealth(1);
            s->state_ = Static::sttwnd_Closed;
            break;
        case 0x15:
            // damaged window
            s = new WindowObj(m, curanim - 6, curanim - 4, curanim - 2, curanim);
            s->setIsIgnored(true);
            s->setHealth(0);
            s->setStartHealth(1);
            s->state_ = Static::sttwnd_Damaged;
            break;
        case 0x16:
            // TODO: set state if damaged trees exist
            s = new Tree(m, curanim, curanim + 1, curanim + 2);
            s->setSizeX(64);
            s->setSizeY(64);
            s->setSizeZ(256);
            s->setHealth(1);
            s->setStartHealth(1);
            break;
        case 0x19:
            // trash bin
            s = new EtcObj(m, curanim, curanim, curanim);
            s->setSizeX(64);
            s->setSizeY(64);
            s->setSizeZ(96);
            break;
        case 0x1A:
            // mail box
            s = new EtcObj(m, curanim, curanim, curanim);
            s->setSizeX(64);
            s->setSizeY(64);
            s->setSizeZ(96);
            break;
        case 0x1C:
            // ???? what is this?
            //s = new EtcObj(m, curanim, curanim, curanim);
            //printf("0x1C anim %X\n", curanim);
            break;
        case 0x1F:
            // advertisement on wall
            s = new EtcObj(m, curanim, curanim, curanim);
            s->setMainType(smt_Advertisement);
            s->setIsIgnored(true);
            break;

        case 0x20:
            // window without light
            s = new AnimWindow(m, curanim);
            s->setStateMasks(sttawnd_LightOff);
            s->setTimeShowAnim(30000 + (rand() % 30000));
            break;
        case 0x21:
            // window light turns on
            s = new AnimWindow(m, curanim - 2);
            s->setTimeShowAnim(1000 + (rand() % 1000));
            s->setStateMasks(sttawnd_LightSwitching);

            // NOTE : 0x22 should have existed but it doesn't appear anywhere

        case 0x23:
            // window with person's shadow non animated,
            // even though on 1 map person appears I will ignore it
            s = new AnimWindow(m, 1959 + ((gamdata->orientation & 0x40) >> 5));
            s->setStateMasks(sttawnd_ShowPed);
            s->setTimeShowAnim(15000 + (rand() % 5000));
            break;
        case 0x24:
            // window with person's shadow, hides, actually animation
            // is of ped standing, but I will ignore it
            s = new AnimWindow(m, 1959 + 8 + ((gamdata->orientation & 0x40) >> 5));
            s->setStateMasks(sttawnd_PedDisappears);
            break;
        case 0x25:
            s = new AnimWindow(m, curanim);

            // NOTE : orientation, I assume, plays role of hidding object,
            // orientation 0x40, 0x80 are drawn (gamdata->desc always 7)
            // window without light
            s->setTimeShowAnim(30000 + (rand() % 30000));
            if (gamdata->orientation == 0x40 || gamdata->orientation == 0x80)
                s->setStateMasks(sttawnd_LightOff);
            else
                s->setStateMasks(sttawnd_LightOn);

            break;

        case 0x26:
            // 0x00,0x80 south - north = 0
            // 0x40,0xC0 weast - east = 2
            s = new LargeDoor(m, curanim, curanim + 1, curanim + 2);
            if (gamdata->orientation == 0x00 || gamdata->orientation == 0x80) {
                s->setSubType(0);
                s->setSizeX(384);
                s->setSizeY(64);
                s->setSizeZ(192);
            } else {
                s->setSubType(2);
                s->setSizeX(64);
                s->setSizeY(384);
                s->setSizeZ(192);
            }
            break;
#ifdef _DEBUG
        default:
            printf("uknown static object type %02X , %02X, %X\n",
                gamdata->sub_type, gamdata->orientation,
                READ_LE_UINT16(gamdata->index_current_frame));
            printf("x = %i, xoff = %i, ", gamdata->mapposx[1],
                gamdata->mapposx[0]);
            printf("y = %i, yoff = %i, ", gamdata->mapposy[1],
                gamdata->mapposy[0]);
            printf("z = %i, zoff = %i\n", gamdata->mapposz[1],
                gamdata->mapposz[0]);
            break;
#endif
    }

    if (s) {
        int z = READ_LE_UINT16(gamdata->mapposz) >> 7;
        int oz = gamdata->mapposz[0] & 0x7F;
        // trick to draw
        if (s->getMainType() == Static::smt_Advertisement)
            z += 1;

        s->setPosition(gamdata->mapposx[1], gamdata->mapposy[1],
            z, gamdata->mapposx[0], gamdata->mapposy[0], oz);

        //s->setMainType(gamdata->sub_type);
#if 0
        if (s->tileX() == 66 && s->tileY() == 49)
            oz = 2;
#endif
        s->setDirection(gamdata->orientation);
    }

    return s;
}

Door::Door(int m, int anim, int closingAnim, int openAnim, int openingAnim)
: Static(m), anim_(anim), closing_anim_(closingAnim),open_anim_(openAnim),
opening_anim_(openingAnim)
{
    state_ = Static::sttdoor_Closed;
    major_type_ = MapObject::mjt_Static;
    main_type_ = Static::smt_Door;
}

void Door::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_ + (state_ << 1), frame_, x, y);
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
                mt = MapObject::mjt_Ped; si = 0;
                do {
                    p = (PedInstance *)(obj->findAt(x + inc_rel,
                        y + rel_inc, z, &mt, &si, true));
                    if (!p && state_ == Static::sttdoor_Open && (!found)) {
                        state_ = Static::sttdoor_Closing;
                        is_ignored_ = false;
                        frame_ = 0;
                    } else if (p && p->isAlive()){
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
            mt = MapObject::mjt_Ped; si = 0;
            do {
                p = (PedInstance *)(obj->findAt(x + inc_rel,
                    y + rel_inc, z, &mt, &si, true));
                if (p && p->isAlive()) {
                    if (!found) {
                        state_ = Static::sttdoor_Opening;
                        is_ignored_ = false;
                        found = true;
                        frame_ = 0;
                    }
                    p->hold_on_.wayFree = 1;
                    p->hold_on_.tilex = x;
                    p->hold_on_.tiley = y;
                    p->hold_on_.tilez = z;
                    p->hold_on_.xadj = 0;
                    p->hold_on_.yadj = 0;
                    p->hold_on_.pathBlocker = this;
                }
            } while (p);
            *i = 0;
            mt = MapObject::mjt_Ped; si = 0;
            do {
                p = (PedInstance *)(obj->findAt(x + inc_rel,
                    y + rel_inc, z, &mt, &si, true));
                if (p && p->isAlive()) {
                    if (!found) {
                        state_ = Static::sttdoor_Opening;
                        is_ignored_ = false;
                        found = true;
                        frame_ = 0;
                    }
                    p->hold_on_.wayFree = 1;
                    p->hold_on_.tilex = x;
                    p->hold_on_.tiley = y;
                    p->hold_on_.tilez = z;
                    p->hold_on_.xadj = 0;
                    p->hold_on_.yadj = 0;
                    p->hold_on_.pathBlocker = this;
                }
            } while (p);
            break;
        case Static::sttdoor_Closing:
            if (frame_ >= g_App.gameSprites().lastFrame(closing_anim_)) {
                state_ = Static::sttdoor_Closed;
                is_ignored_ = false;
                frame_ = 0;
            }
            break;
        case Static::sttdoor_Opening:
            if (frame_ >= g_App.gameSprites().lastFrame(opening_anim_)) {
                state_ = Static::sttdoor_Open;
                is_ignored_ = true;
                frame_ = 0;
            }
            break;
    }
    return changed;
}

bool Door::isPathBlocker()
{
    return state_ != Static::sttdoor_Open;
}


LargeDoor::LargeDoor(int m, int anim, int closingAnim, int openingAnim):Static(m), anim_(anim),
closing_anim_(closingAnim), opening_anim_(openingAnim)
{
    state_ = Static::sttdoor_Closed;
    major_type_ = MapObject::mjt_Static;
    main_type_ = Static::smt_LargeDoor;
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
    std::vector<PedInstance *> found_peds;
    found_peds.reserve(256);
    std::vector<PedInstance *> found_peds_mid;
    found_peds_mid.reserve(256);
    char sign;
    int set_wayFree = 0;

    bool changed = MapObject::animate(elapsed);
    uint32 cur_state = state_;
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
            for(*i = -2; *i < 3; (*i)++) {
                mt = MapObject::mjt_Vehicle; si = 0;
                v = (VehicleInstance *)(obj->findAt(x + inc_rel,
                    y + rel_inc,z, &mt, &si, true));
                if (!v && !found) {
                    state_ = Static::sttdoor_Closing;
                    is_ignored_ = false;
                } else if (v){
                    state_ = Static::sttdoor_Open;
                    is_ignored_ = true;
                    found = true;
                    v->hold_on_.wayFree = 0;
                }
            }
            *j = 1;
            for(*i = -2; *i < 3; (*i)++) {
                mt = MapObject::mjt_Vehicle; si = 0;
                v = (VehicleInstance *)(obj->findAt(x + inc_rel,
                    y + rel_inc,z,&mt,&si,true));
                if (!v && !found) {
                    state_ = Static::sttdoor_Closing;
                    is_ignored_ = false;
                } else if (v){
                    state_ = Static::sttdoor_Open;
                    is_ignored_ = true;
                    found = true;
                    v->hold_on_.wayFree = 0;
                }
            }
            *j = -1;
            for (*i = -1; *i <= 1; (*i)++ ) {
                mt = MapObject::mjt_Ped; si = 0;
                do {
                    p = (PedInstance *)(obj->findAt(x + rel_inc,
                        y + inc_rel, z, &mt, &si, true));
                    if (p) {
                        found_peds.push_back(p);
                        if (!found && p->hasAccessCard()) {
                            state_ = Static::sttdoor_Open;
                            is_ignored_ = true;
                            found = true;
                        }
                    }
                } while (p);
            }
            *j = 1;
            for (*i = -1; *i <= 1; (*i)++ ) {
                mt = MapObject::mjt_Ped; si = 0;
                do {
                    p = (PedInstance *)(obj->findAt(x + rel_inc,
                        y + inc_rel, z, &mt, &si, true));
                    if (p) {
                        found_peds.push_back(p);
                        if (!found && p->hasAccessCard()) {
                            state_ = Static::sttdoor_Open;
                            is_ignored_ = true;
                            found = true;
                        }
                    }
                } while (p);
            }
            *j = 0;
            for (*i = -1; *i <= 1; (*i)++ ) {
                mt = MapObject::mjt_Ped; si = 0;
                do {
                    p = (PedInstance *)(obj->findAt(x + rel_inc,
                        y + inc_rel, z, &mt, &si, true));
                    if (p) {
                        found_peds_mid.push_back(p);
                        if (!found && p->hasAccessCard()) {
                            state_ = Static::sttdoor_Open;
                            is_ignored_ = true;
                            found = true;
                        }
                    }
                } while (p);
            }
            if (state_ == Static::sttdoor_Open) {
                for (std::vector<PedInstance *>::iterator it = found_peds.begin();
                    it != found_peds.end(); ++it )
                {
                    (*it)->hold_on_.wayFree = 0;
                }
                for (std::vector<PedInstance *>::iterator it = found_peds_mid.begin();
                    it != found_peds_mid.end(); ++it )
                {
                    (*it)->hold_on_.wayFree = 0;
                }
            } else {
                for (std::vector<PedInstance *>::iterator it = found_peds.begin();
                    it != found_peds.end(); ++it )
                {
                    p = *it;
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
                    p->hold_on_.pathBlocker = this;
                }
                for (std::vector<PedInstance *>::iterator it = found_peds_mid.begin();
                    it != found_peds_mid.end(); ++it )
                {
                    p = *it;
                    ShootableMapObject::DamageInflictType d;
                    d.dtype = MapObject::dmg_Hit;
                    d.d_owner = NULL;
                    d.dvalue = 1024;
                    d.ddir = -1;
                    p->handleDamage(&d);
                }
            }
            break;
        case Static::sttdoor_Closed:
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
            mt = MapObject::mjt_Vehicle; si = 0;
            v = (VehicleInstance *)(obj->findAt(x + inc_rel,
                y + rel_inc,z, &mt, &si,true));
            if (v) {
                if (!found) {
                    state_ = Static::sttdoor_Opening;
                    is_ignored_ = false;
                    found = true;
                }
                v->hold_on_.wayFree = 1;
                v->hold_on_.pathBlocker = this;
            }
            *j = 1 * sign;
            *i = 2;
            mt = MapObject::mjt_Vehicle; si = 0;
            v = (VehicleInstance *)(obj->findAt(x + inc_rel,
                y + rel_inc,z, &mt, &si,true));
            if (v) {
                if (!found) {
                    state_ = Static::sttdoor_Opening;
                    is_ignored_ = false;
                    found = true;
                }
                v->hold_on_.wayFree = 1;
                v->hold_on_.pathBlocker = this;
            }
            *j = -1;
            for (*i = -1; *i <= 1; (*i)++ ) {
                mt = MapObject::mjt_Ped; si = 0;
                do {
                    p = (PedInstance *)(obj->findAt(x + rel_inc,
                        y + inc_rel, z, &mt, &si, true));
                    if (p) {
                        found_peds.push_back(p);
                        if (!found && p->hasAccessCard()) {
                            state_ = Static::sttdoor_Opening;
                            is_ignored_ = false;
                            found = true;
                        }
                    }
                } while (p);
            }
            *j = 1;
            for (*i = -1; *i <= 1; (*i)++ ) {
                mt = MapObject::mjt_Ped; si = 0;
                do {
                    p = (PedInstance *)(obj->findAt(x + rel_inc,
                        y + inc_rel, z, &mt, &si, true));
                    if (p) {
                        found_peds.push_back(p);
                        if (!found && p->hasAccessCard()) {
                            state_ = Static::sttdoor_Opening;
                            is_ignored_ = false;
                            found = true;
                        }
                    }
                } while (p);
            }
            set_wayFree = state_ == Static::sttdoor_Opening ? 1 : 2;
            for (std::vector<PedInstance *>::iterator it = found_peds.begin();
                it != found_peds.end(); ++it )
            {
                p = *it;
                p->hold_on_.wayFree = set_wayFree;
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
                p->hold_on_.pathBlocker = this;
            }
            break;
        case Static::sttdoor_Closing:
            if (frame_ >= g_App.gameSprites().lastFrame(closing_anim_)) {
                state_ = Static::sttdoor_Closed;
                is_ignored_ = false;
            }
        case Static::sttdoor_Opening:
            if (state_ == Static::sttdoor_Opening
                && frame_ >= g_App.gameSprites().lastFrame(opening_anim_))
            {
                state_ = Static::sttdoor_Open;
                is_ignored_ = true;
            }
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
            set_wayFree = state_ == Static::sttdoor_Opening ? 1 : 2;
            mt = MapObject::mjt_Vehicle; si = 0;
            v = (VehicleInstance *)(obj->findAt(x + inc_rel,
                y + rel_inc,z, &mt, &si,true));
            if (v) {
                v->hold_on_.wayFree = 1;
                v->hold_on_.pathBlocker = this;
            }
            *j = 1 * sign;
            *i = 2;
            mt = MapObject::mjt_Vehicle; si = 0;
            v = (VehicleInstance *)(obj->findAt(x + inc_rel,
                y + rel_inc,z, &mt, &si,true));
            if (v) {
                v->hold_on_.wayFree = 1;
                v->hold_on_.pathBlocker = this;
            }
            *j = -1;
            for (*i = -1; *i <= 1; (*i)++ ) {
                mt = MapObject::mjt_Ped; si = 0;
                do {
                    p = (PedInstance *)(obj->findAt(x + rel_inc,
                        y + inc_rel, z, &mt, &si, true));
                    if (p) {
                        found_peds.push_back(p);
                    }
                } while (p);
            }
            *j = 1;
            for (*i = -1; *i <= 1; (*i)++ ) {
                mt = MapObject::mjt_Ped; si = 0;
                do {
                    p = (PedInstance *)(obj->findAt(x + rel_inc,
                        y + inc_rel, z, &mt, &si, true));
                    if (p) {
                        found_peds.push_back(p);
                    }
                } while (p);
            }
            for (std::vector<PedInstance *>::iterator it = found_peds.begin();
                it != found_peds.end(); ++it )
            {
                p = *it;
                p->hold_on_.wayFree = set_wayFree;
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
                p->hold_on_.pathBlocker = this;
            }
            break;
    }
    if (cur_state != state_)
        frame_ = 0;
    return changed;
}

bool LargeDoor::isPathBlocker()
{
    return state_ != Static::sttdoor_Open;
}


Tree::Tree(int m, int anim, int burningAnim, int damagedAnim) : Static(m),
anim_(anim), burning_anim_(burningAnim), damaged_anim_(damagedAnim)
{
    rcv_damage_def_ = MapObject::ddmg_StaticTree;
    major_type_ = MapObject::mjt_Static;
    state_ = Static::stttree_Healthy;
    main_type_ = Static::smt_Tree;
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

bool Tree::handleDamage(ShootableMapObject::DamageInflictType *d) {
    if (health_ <= 0 || rcv_damage_def_ == MapObject::ddmg_Invulnerable
        || (d->dtype & rcv_damage_def_) == 0)
        return false;

    health_ -= d->dvalue;
    if (health_ <= 0) {
        state_ = Static::stttree_Burning;
        setTimeShowAnim(10000);
        is_ignored_ = true;
        health_ = 0;
    }
    return true;
}

WindowObj::WindowObj(int m, int anim, int openAnim,
int breakingAnim, int damagedAnim) : Static(m),
anim_(anim), open_anim_(openAnim), breaking_anim_(breakingAnim),
damaged_anim_(damagedAnim)
{
    rcv_damage_def_ = MapObject::ddmg_StaticWindow;
    major_type_ = MapObject::mjt_Static;
    main_type_ = Static::smt_Window;
}

bool WindowObj::animate(int elapsed, Mission *obj) {
    bool updated = MapObject::animate(elapsed);

    if (state_ == Static::sttwnd_Breaking
        && frame_ >= g_App.gameSprites().lastFrame(breaking_anim_)
    ) {
        state_ = sttwnd_Damaged;
        updated = true;
    }
    return updated;
}

void WindowObj::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_ + (state_ << 1), frame_, x, y);
}

bool WindowObj::handleDamage(ShootableMapObject::DamageInflictType *d) {
    if (health_ <= 0 || (d->dtype & rcv_damage_def_) == 0)
        return false;

    health_ -= d->dvalue;
    if (health_ <= 0) {
        state_ = Static::sttwnd_Breaking;
        is_ignored_ = true;
        frame_ = 0;
        setFramesPerSec(6);
    }
    return true;
}

EtcObj::EtcObj(int m, int anim, int burningAnim, int damagedAnim) : Static(m),
anim_(anim), burning_anim_(burningAnim), damaged_anim_(damagedAnim)
{
    rcv_damage_def_ = MapObject::ddmg_StaticGeneral;
    major_type_ = MapObject::mjt_Static;
    main_type_ = -1;
}

void EtcObj::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
}

NeonSign::NeonSign(int m, int anim):Static(m),
anim_(anim)
{
    major_type_ = MapObject::mjt_Static;
    main_type_ = Static::smt_NeonSign;
}

void NeonSign::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_, frame_, x, y);
}

Semaphore::Semaphore(int m, int anim, int damagedAnim) : Static(m),
anim_(anim), damaged_anim_(damagedAnim), elapsed_left_smaller_(0),
elapsed_left_bigger_(0), up_down_(1)
{
    rcv_damage_def_ = MapObject::ddmg_StaticGeneral;
    major_type_ = MapObject::mjt_Static;
    main_type_ = Static::smt_Semaphore;
    setFramesPerSec(2);
}

bool Semaphore::animate(int elapsed, Mission *obj) {
    if (state_ == Static::sttsem_Damaged) {
        if (elapsed_left_bigger_ == 0)
            return false;
        int chng = (elapsed + elapsed_left_smaller_) >> 1;
        elapsed_left_smaller_ = elapsed & 2;
        elapsed_left_bigger_ -= chng;
        if (elapsed_left_bigger_ < 0) {
            chng += elapsed_left_bigger_;
            elapsed_left_bigger_ = 0;
        }
        int z = tile_z_ * 128 + off_z_ - chng;
        tile_z_ = z / 128;
        off_z_ = z % 128;
        return true;
    }

    int chng = (elapsed + elapsed_left_smaller_) >> 2;
    elapsed_left_smaller_ = elapsed & 4;
    if (chng) {
        int oz = off_z_ + chng * up_down_;
        if (oz > 127) {
            oz = 127 - (oz & 0x7F);
            up_down_ -= 2;
        } else if (oz < 64) {
            oz = 64 + (64 - oz);
            up_down_ += 2;
        }
        off_z_ = oz;
    }

    chng = (elapsed + elapsed_left_bigger_) >> 6;
    elapsed_left_bigger_ = elapsed & 63;
    if (chng) {
        // Direction is used as storage for animation change, not my idea
        dir_ += chng;
        dir_ &= 0xFF;
        state_ = dir_ >> 6;
        state_++;
        if (state_ > Static::sttsem_Stt3)
            state_ = Static::sttsem_Stt0;
    }

    return MapObject::animate(elapsed);
}

bool Semaphore::handleDamage(ShootableMapObject::DamageInflictType *d) {
    if (health_ <= 0 || (d->dtype & rcv_damage_def_) == 0)
        return false;

    health_ -= d->dvalue;
    if (health_ <= 0) {
        state_ = Static::sttsem_Damaged;
        // To make this thing reach the ground need to get solid surface 0x0F
        Mission * m = g_Session.getMission();
        int z = tile_z_;
        int indx = tile_x_ + tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy;
        elapsed_left_bigger_ = 0;
        while (z != 0) {
            z--;
            indx -= m->mmax_m_xy;
            int twd = m->mtsurfaces_[indx].twd;
            if (twd == 0x0F) {
                elapsed_left_bigger_ = (tile_z_ - z) * 128 + off_z_;
                break;
            }
        }
        is_ignored_ = true;
        health_ = 0;
    }
    return true;
}

void Semaphore::draw(int x, int y)
{
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_ +  state_, frame_, x, y);
}

AnimWindow::AnimWindow(int m, int anim) : Static(m),
anim_(anim)
{
    rcv_damage_def_ = MapObject::ddmg_StaticGeneral;
    major_type_ = MapObject::mjt_Static;
    main_type_ = smt_AnimatedWindow;
    setIsIgnored(true);
    setFramesPerSec(4);
}

void AnimWindow::draw(int x, int y)
{
    // When light is on, don't draw window
    // because lighted window is part of the map
    if (state_ == Static::sttawnd_LightOn)
        return;
    addOffs(x, y);
    g_App.gameSprites().drawFrame(anim_ + (state_ << 1), frame_, x, y);
}

bool AnimWindow::animate(int elapsed, Mission *obj)
{
    switch (state_) {
        case Static::sttawnd_LightOff:
            if (!leftTimeShowAnim(elapsed)) {
                // decide to start switching lights on
                // or continue being in dark
                if (rand() % 100 > 60) {
                    setTimeShowAnim(30000 + (rand() % 30000));
                } else {
                    state_ = Static::sttawnd_LightSwitching;
                    frame_ = 0;
                    setTimeShowAnim(1000 + (rand() % 1000));
                }
            }
            break;
        case Static::sttawnd_LightSwitching:
            if (!leftTimeShowAnim(elapsed)) {
                state_ = Static::sttawnd_LightOn;
                setTimeShowAnim(30000 + (rand() % 30000));
            }
            break;
        case Static::sttawnd_PedAppears:
            if (frame_ >= g_App.gameSprites().lastFrame(anim_
                + (Static::sttawnd_PedAppears << 1)))
            {
                state_ = Static::sttawnd_ShowPed;
                setTimeShowAnim(15000 + (rand() % 15000));
            }
            break;
        case Static::sttawnd_ShowPed:
            if (!leftTimeShowAnim(elapsed)) {
                // continue showing ped or hide it
                if (rand() % 100 > 50) {
                    setTimeShowAnim(15000 + (rand() % 5000));
                } else {
                    frame_ = 0;
                    state_ = Static::sttawnd_PedDisappears;
                }
            }
            break;
        case Static::sttawnd_PedDisappears:
            if (frame_ >= g_App.gameSprites().lastFrame(anim_
                + (Static::sttawnd_PedDisappears << 1)))
            {
                state_ = Static::sttawnd_LightOn;
                setTimeShowAnim(30000 + (rand() % 30000));
            }
            break;
        case Static::sttawnd_LightOn:
            if (!leftTimeShowAnim(elapsed)) {
                // we will continue showing lightson or switch
                // lights off or show ped
                int rnd_v = rand() % 100;
                if (rnd_v > 80) {
                    setTimeShowAnim(30000 + (rand() % 30000));
                } else if (rnd_v > 60) {
                    frame_ = 0;
                    state_ = Static::sttawnd_PedAppears;
                } else if (rnd_v > 10) {
                    state_ = Static::sttawnd_LightOff;
                    setTimeShowAnim(30000 + (rand() % 30000));
                } else {
                    frame_ = 0;
                    state_ = Static::sttawnd_LightSwitching;
                    setTimeShowAnim(1000 + (rand() % 1000));
                }
            }
            break;
    }

    return MapObject::animate(elapsed);
    return false;
}

