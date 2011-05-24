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

Weapon::Weapon(const char *w_name, int smallIcon, int bigIcon, int w_cost,
    int w_ammo, int w_range, int w_shot, int w_rank, int w_anim,
    Weapon::WeaponAnimIndex w_idx, snd::InGameSample w_sample,
    WeaponType w_type, MapObject::DamageType w_dmg_type,
    int w_ammo_per_shot, int w_time_for_shot, int w_time_reload) {

    name_ = w_name;
    small_icon_ = smallIcon;
    big_icon_ = bigIcon;
    cost_ = w_cost;
    ammo_= w_ammo;
    range_= w_range;
    damage_per_shot_ = w_shot;
    anim_ = w_anim;
    rank_ = w_rank;
    idx_ = w_idx;
    dmg_type_ = w_dmg_type;
    sample_ = w_sample;
    type_ = w_type;
    ammo_per_shot_ = w_ammo_per_shot;
    time_for_shot_ = w_time_for_shot;
    time_reload_ = w_time_reload;
    submittedToSearch_ = false;
}

void Weapon::drawSmallIcon(int x, int y) {
    g_App.menuSprites().drawSpriteXYZ(small_icon_, x, y, 0, false, true);
}

void Weapon::drawBigIcon(int x, int y) {
    g_App.menuSprites().drawSpriteXYZ(big_icon_, x, y, 0, false, true);
}

void Weapon::drawInfo(int x, int y) {
    char tmp[100];
    g_App.fonts().drawText(x, y, name_.c_str(), 0, false);
    sprintf(tmp, "COST   :%d", cost_);
    g_App.fonts().drawText(x, y + 12, tmp, 0, false);
    y += 24;

    if (ammo_ >= 0) {
        sprintf(tmp, "AMMO   :%d", ammo_);
        g_App.fonts().drawText(x, y, tmp, 0, false);
        y += 12;
    }

    if (range_ >= 0) {
        sprintf(tmp, "RANGE  :%d", range_);
        g_App.fonts().drawText(x, y, tmp, 0, false);
        y += 12;
    }

    if (damage_per_shot_ >= 0 && ammo_ >= 0) {
        sprintf(tmp, "SHOT   :%d", damage_per_shot_);
        g_App.fonts().drawText(x, y, tmp, 0, false);
        y += 12;
    }
}

WeaponInstance *Weapon::createInstance() {
    return new WeaponInstance(this);
}

WeaponInstance::WeaponInstance(Weapon * w) : ShootableMapObject(-1)
{
    pWeaponClass_ = w;
    ammo_remaining_ = w->ammo();
    rcv_damage_def_ = MapObject::ddmg_Invulnerable;
    weapon_used_time_ = 0;
    major_type_ = MapObject::mt_Weapon;
}

bool WeaponInstance::animate(int elapsed) {
    if ((!owner_) && weapon_used_time_ != 0) {
        weapon_used_time_ += elapsed;
        if (weapon_used_time_ > (pWeaponClass_->timeForShot()
            + pWeaponClass_->timeReload()))
            weapon_used_time_ = 0;
    }

    if (map_ == -1)
        return false;

    return MapObject::animate(elapsed);
}

void WeaponInstance::draw(int x, int y) {
    addOffs(x, y);
    g_App.gameSprites().drawFrame(pWeaponClass_->anim(), frame_, x, y);
}

bool WeaponInstance::inflictDamage(ShootableMapObject * tobj, PathNode * tp,
    int duration)
{
    // TODO: "tp" will be used later for calculating vector when target
    // is not object(floor); "duration" will be used during continuos
    // shooting to calculate damage + delay(reloading) time;

    // NOTE: for now this will check "tobj"
    if (ammo_remaining_ == 0)
        return false;

    // TODO: if owner exists these two values should change(IPA, mods)
    int time_for_shot = pWeaponClass_->timeForShot();
    int time_reload = pWeaponClass_->timeReload();

    int time_full_shot = time_for_shot + time_reload;
    if (duration == -1)
        duration = time_full_shot;
    if (weapon_used_time_ >= time_for_shot) {
        weapon_used_time_ += duration;
        if (weapon_used_time_ >= time_full_shot) {
            weapon_used_time_ -= time_full_shot;
        } else
            return false;
    } else
        weapon_used_time_ += duration;

    int shots = weapon_used_time_ / time_full_shot;
    weapon_used_time_ %= time_full_shot;
    if (weapon_used_time_ >= time_for_shot)
        shots++;

    if (pWeaponClass_->dmgType() == MapObject::dmg_None) {
        if (pWeaponClass_->getWeaponType() == Weapon::EnergyShield) {
            int ammoused = shots * pWeaponClass_->ammoPerShot();
            if (ammoused >= ammo_remaining_) {
                ammo_remaining_ = 0;
                ((PedInstance *)owner_)->selectNextWeapon();
            } else
                ammo_remaining_ -= ammoused;
        }
        return false;
    } else if (pWeaponClass_->dmgType() == MapObject::dmg_Heal) {
        // NOTE: not only self-healing in future?
        owner_->setHealth(owner_->startHealth());
        ammo_remaining_ = 0;
        ((PedInstance *)owner_)->selectNextWeapon();
        weapon_used_time_ = 0;
        return true;
    } else if (pWeaponClass_->dmgType() == MapObject::dmg_Mental) {
        return false;
    }

    if (tobj == NULL)
        return false;
    DamageInflictType d;
    d.dtype = pWeaponClass_->dmgType();
    int ammoused = 1;
    if (pWeaponClass_->ammo() > 0) {
        ammoused = shots * pWeaponClass_->ammoPerShot();
        if (ammoused > ammo_remaining_) {
            ammoused = ammo_remaining_;
            ammo_remaining_ = 0;
        } else
            ammo_remaining_ -= ammoused;
    }
    int totaldmg = ammoused * pWeaponClass_->damagePerShot();
    d.dvalue = totaldmg;
    int xb = 0;
    int yb = 0;
    if (owner_) {
        xb = owner_->tileX() * 256 + owner_->offX();
        yb = owner_->tileY() * 256 + owner_->offY();
    } else {
        xb = tile_x_ * 256 + off_x_;
        yb = tile_y_ * 256 + off_y_;
    }
    d.ddir = -1;
    // our Y is inversed
    setDirection((tobj->tileX() * 256 + tobj->offX()) - xb,
        yb - (tobj->tileY() * 256 + tobj->offY()), &(d.ddir));
    if (owner_) {
        if (d.ddir != -1)
            owner_->setDirection(d.ddir);
    }
    tobj->handleDamage(&d);
    return true;
}

/*!
 * Plays the sound associated with that weapon.
 */
void WeaponInstance::playSound() {
    g_App.gameSounds().play(pWeaponClass_->getSound());
}

bool WeaponInstance::inRange(ShootableMapObject *t) {

    int maxr = range();

    double d = 0;
    if(owner_)
        d = owner_->distanceTo(t);
    else
        d = distanceTo(t);

    if (d == 0)
        return true;
    if (d >= maxr)
        return false;

    Mission *m = g_Session.getMission();

    int cx = owner_->tileX() * 256 + owner_->offX();
    int cy = owner_->tileY() * 256 + owner_->offY();
    int cz = (owner_->visZ() + 1) * 128 + owner_->offZ();
    assert((owner_->visZ() + 1) < m->mmax_z_);
    double sx = (double) cx;
    double sy = (double) cy;
    double sz = (double) cz;
    int tx = t->tileX() * 256 + t->offX();
    int ty = t->tileY() * 256 + t->offY();
    int tz = (t->visZ() + 1) * 128 + t->offZ();
    assert((t->visZ() + 1) < m->mmax_z_);

    // NOTE: these values are less then 1, if they are incremented time
    // required to check range will be shorter less precise check, if
    // decremented longer more precise. Increment is (n * 8)
    double inc_x = ((tx - cx) * 8) / d;
    double inc_y = ((ty - cy) * 8) / d;
    double inc_z = ((tz - cz) * 8) / d;

    int oldx = cx / 256;
    int oldy = cy / 256;
    int oldz = cz / 128;

    while (fabs(sx - tx) > 16.0f || fabs(sy - ty) > 16.0f || fabs(sz - tz) > 16.0f) {
        int nx = (int)sx / 256;
        int ny = (int)sy / 256;
        int nz = (int)sz / 128;
        if (oldx != nx || oldy != ny || oldz != nz) {
            unsigned char twd = m->mtsurfaces_[nx + ny * m->mmax_x_
                + nz * m->mmax_m_xy].twd;
            if (!(twd == 0x00 || twd == 0x0C || twd == 0x10))
                return false;
            oldx = nx;
            oldy = ny;
            oldz = nz;
        }
        sx += inc_x;
        sy += inc_y;
        sz += inc_z;
    }

    toDefineXYZ startXYZ = {cx, cy, cz};
    toDefineXYZ endXYZ = {tx, ty, tz};
    MapObject *blockerObj = NULL;
    owner_->setIsIgnored(true);
    t->setIsIgnored(true);
    m->blockerExists(&startXYZ, &endXYZ, d, &blockerObj);
    owner_->setIsIgnored();
    t->setIsIgnored();
    if (blockerObj)
        return false;

    return true;
}
