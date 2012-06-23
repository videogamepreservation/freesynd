/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2010  Bohdan Stelmakh <chamel@users.sourceforge.net> *
 *   Copyright (C) 2011  Mark <mentor66@users.sourceforge.net>          *
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

#define Z_SHIFT_TO_AIR   4

Weapon::Weapon(const std::string& w_name, int smallIcon, int bigIcon, int w_cost,
    int w_ammo, int w_range, int w_shot, int w_rank, int w_anim,
    Weapon::WeaponAnimIndex w_idx, snd::InGameSample w_sample,
    WeaponType w_type, MapObject::DamageType w_dmg_type,
    int w_ammo_per_shot, int w_time_for_shot, int w_time_reload,
    unsigned int w_shot_property, int w_hit_anim, int w_obj_hit_anim,
    int w_trace_anim, int w_rd_anim, int w_range_dmg, double w_shot_angle,
    double w_shot_accuracy, int w_shot_speed,
    int w_dmg_per_shot) : shot_speed_(w_shot_speed)
{
    name_ = w_name;
    small_icon_ = smallIcon;
    big_icon_ = bigIcon;
    cost_ = w_cost;
    ammo_cost_ = w_shot;
    ammo_= w_ammo;
    range_= w_range;
    dmg_per_shot_ = w_dmg_per_shot;
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
    shot_property_ = w_shot_property;
    anims_.hit_anim = w_hit_anim;
    anims_.obj_hit_anim = w_obj_hit_anim;
    anims_.trace_anim = w_trace_anim;
    anims_.rd_anim = w_rd_anim;
    range_dmg_ = w_range_dmg;
    shot_angle_ = w_shot_angle;
    shot_accuracy_ = w_shot_accuracy;
}

WeaponInstance *Weapon::createInstance() {
    return new WeaponInstance(this);
}

WeaponInstance::WeaponInstance(Weapon * w) : ShootableMapObject(-1),
processed_time_(0)
{
    pWeaponClass_ = w;
    ammo_remaining_ = w->ammo();
    weapon_used_time_ = 0;
    major_type_ = MapObject::mjt_Weapon;
    owner_ = NULL;
    activated_ = false;
    time_consumed_ = false;
    main_type_ = w->getWeaponType();
    if (w->getWeaponType() == Weapon::TimeBomb
        || w->getWeaponType() == Weapon::Flamer)
    {
        size_x_ = 32;
        size_y_ = 32;
        size_z_ = 32;
        setRcvDamageDef(MapObject::ddmg_WeaponBomb);
    }
    health_ = 1;
    start_health_ = 1;
}

bool WeaponInstance::animate(int elapsed) {

    if (activated_) {
        if (main_type_ == Weapon::EnergyShield) {
            if (ammo_remaining_ == 0)
                return false;
            int tm_left = elapsed;
            int ammoused = getShots(&tm_left) * pWeaponClass_->ammoPerShot();
            if (ammoused >= ammo_remaining_) {
                ammo_remaining_ = 0;
                ((PedInstance *)owner_)->selectNextWeapon();
            } else
                ammo_remaining_ -= ammoused;
            return true;
        } else if (main_type_ == Weapon::MediKit) {
            if (owner_->health() != owner_->startHealth()) {
                owner_->setHealth(owner_->startHealth());
                ammo_remaining_ = 0;
                ((PedInstance *)owner_)->selectNextWeapon();
                weapon_used_time_ = 0;
            }
            return true;
        } else if (main_type_ == Weapon::TimeBomb) {
            int tm_left = elapsed;
            if (((processed_time_ / 750)
                < (processed_time_ + elapsed) / 750)
                && (processed_time_ + elapsed) / 750 != (pWeaponClass_->timeForShot() / 750))
            {
                g_App.gameSounds().play(snd::TIMEBOMB);
            }
            processed_time_ += elapsed;
            if ((inflictDamage(NULL, NULL, &tm_left)) == 0) {
                deactivate();
                map_ = -1;
                setIsIgnored(true);

                Mission *m = g_Session.getMission();
                toDefineXYZ cur_pos = {tile_x_ * 256 + off_x_,
                    tile_y_ * 256 + off_y_, vis_z_ * 128 + off_z_};
                SFXObject *so = new SFXObject(m->map(),
                    pWeaponClass_->anims()->hit_anim);
                so->setPosition(cur_pos.x / 256, cur_pos.y / 256, cur_pos.z / 128,
                    cur_pos.x % 256, cur_pos.y % 256, cur_pos.z % 128);
                so->setTileVisZ();
                m->addSfxObject(so);

                int max_anims = 24 + rand() % 8;
                rangeDamageAnim(cur_pos, (double)pWeaponClass_->rangeDmg(),
                    max_anims, pWeaponClass_->anims()->rd_anim);
                return true;
            }
            time_consumed_ = true;
        } else if (main_type_ == Weapon::Persuadatron) {
            int tm_left = elapsed;
            inflictDamage(NULL, NULL, &tm_left);
        }
        if (time_consumed_) {
            time_consumed_ = false;
        } else if (weapon_used_time_ != 0 ) {
            weapon_used_time_ += elapsed;
            if (weapon_used_time_ > (pWeaponClass_->timeForShot()
                + pWeaponClass_->timeReload()))
            {
                weapon_used_time_ = 0;
            }
        }
    } else if (weapon_used_time_ != 0 ) {
        weapon_used_time_ += elapsed;
        if (weapon_used_time_ > (pWeaponClass_->timeForShot()
            + pWeaponClass_->timeReload()))
        {
            weapon_used_time_ = 0;
        }
    } else {
        if (main_type_ == Weapon::EnergyShield) {
            if (ammo_remaining_ != 0
                && ammo_remaining_ < pWeaponClass_->ammo())
                return false;
            int tm_left = elapsed;
            int ammoused = getShots(&tm_left) * pWeaponClass_->ammoPerShot();
            if (ammo_remaining_ != 0
                && ammo_remaining_ != pWeaponClass_->ammo())
            {
                ammo_remaining_ += ammoused;
                if (ammo_remaining_ > pWeaponClass_->ammo())
                    ammo_remaining_ = pWeaponClass_->ammo();
            }
        }
    }

    if (map_ == -1)
        return false;

    return MapObject::animate(elapsed);
}

void WeaponInstance::draw(int x, int y) {
    addOffs(x, y);
    g_App.gameSprites().drawFrame(pWeaponClass_->anim(), frame_, x, y);
}

void ShotClass::shotTargetRandomizer(toDefineXYZ * cp, toDefineXYZ * tp,
    double angle, double dist_new, bool exclude_z)
{
    // TODO: review code
    if (angle == 0)
        return;

    angle *= (double)(69 + (rand() & 0x1F)) / 100.0;
    int cx = cp->x;
    int cy = cp->y;
    int cz = cp->z;

    int tx = tp->x;
    int ty = tp->y;
    int tz = tp->z;
    double dtx = (double)(tx - cx);
    double dty = (double)(ty - cy);
    double dtz = (double)(tz - cz);
    double dist_cur = 0;
    if (dist_new != -1)
        dist_cur = dist_new;
    else
        dist_cur = sqrt(dtx * dtx + dty * dty + dtz * dtz);
    if (dist_cur == 0)
        return;

    const double PI = 3.14159265;
    angle /= (180.0 / PI);
    double angx = acos(dtx/dist_cur);
    double angy = acos(dty/dist_cur);
    double angz = acos(dtz/dist_cur);

    double set_sign = 1;
    if (rand() % 100 < 50)
        set_sign = -1;
    angx += ((angle * (double)(rand() % 100) / 200.0) * set_sign);
    int gtx = cx + (int)(cos(angx) * dist_cur);

    set_sign = 1;
    if (rand() % 100 < 50)
        set_sign = -1;
    angy += ((angle * (double)(rand() % 100) / 200.0) * set_sign);
    int gty = cy + (int)(cos(angy) * dist_cur);
    if (!exclude_z) {
        set_sign = 1;
        if (rand() % 100 < 50)
            set_sign = -1;
        angz += ((angle * (double)(rand() % 100) / 200.0) * set_sign);
    }
    int gtz = cz + (int)(cos(angz) * dist_cur);

    if (gtx < 0) {
        if (cos(angx) == 0) {
            gtx = 0;
        } else {
            dist_cur -= fabs((double)gtx / cos(angx));
            gtx = 0;
            gty = cy + (int)(cos(angy) * dist_cur);
            gtz = cz + (int)(cos(angz) * dist_cur);
        }
    }
    if (gty < 0) {
        if (cos(angy) == 0) {
            gty = 0;
        } else {
            dist_cur -= fabs((double)gty / cos(angy));
            gty = 0;
            gtx = cx + (int)(cos(angx) * dist_cur);
            gtz = cz + (int)(cos(angz) * dist_cur);
        }
    }
    if (gtz < 0) {
        if (cos(angz) == 0) {
            gtz = 0;
        } else {
            dist_cur -= fabs((double)gtz / cos(angz));
            gtz = 0;
            gtx = cx + (int)(cos(angx) * dist_cur);
            gty = cy + (int)(cos(angy) * dist_cur);
        }
    }

    int max_x = (g_Session.getMission()->mmax_x_ - 1) * 256;
    int max_y = (g_Session.getMission()->mmax_y_ - 1) * 256;
    int max_z = (g_Session.getMission()->mmax_z_ - 1) * 128;
    if (gtx > max_x) {
        if (cos(angx) == 0) {
            gtx = max_x;
        } else {
            dist_cur -= fabs((double)(gtx - max_x) / cos(angx));
            gtx = max_x;
            gty = cy + (int)(cos(angy) * dist_cur);
            gtz = cz + (int)(cos(angz) * dist_cur);
        }
    }
    if (gty > max_y) {
        if (cos(angy) == 0) {
            gty = max_y;
        } else {
            dist_cur -= fabs((double)(gty - max_y) / cos(angy));
            gty = max_y;
            gtx = cx + (int)(cos(angx) * dist_cur);
            gtz = cz + (int)(cos(angz) * dist_cur);
        }
    }
    if (gtz > max_z) {
        if (cos(angx) == 0) {
            gtz = max_z;
        } else {
            dist_cur -= fabs((double)(gtz - max_z) / cos(angz));
            gtz = max_z;
            gtx = cx + (int)(cos(angx) * dist_cur);
            gty = cy + (int)(cos(angy) * dist_cur);
        }
    }
    assert(gtx >= 0 && gty >= 0 && gtz >= 0);
    assert(gtx <= max_x && gty <= max_y && gtz <= max_z);

    tp->x = gtx;
    tp->y = gty;
    tp->z = gtz;
}

ProjectileShot::ProjectileShot(toDefineXYZ &cp, Weapon::ShotDesc & sd,
    int d_range, Weapon::ad_HitAnims *panims, ShootableMapObject * ignrd_obj,
    int range_max, int shot_speed)
{
    cur_pos_ = cp;
    base_pos_ = cur_pos_;
    sd_prj_ = sd;
    dmg_range_ = d_range;
    anims_ = *panims;
    dist_max_ = range_max;
    dist_passed_ = 0;
    speed_ = shot_speed;
    last_anim_dist_ = 0;
    life_over_ = false;
    double diffx = (double)(sd.tp.x - cur_pos_.x);
    double diffy = (double)(sd.tp.y - cur_pos_.y);
    double diffz = (double)(sd.tp.z - cur_pos_.z);
    cur_dist_ = sqrt(diffx * diffx + diffy * diffy + diffz * diffz);
    if (cur_dist_ != 0) {
        inc_x_ = diffx / cur_dist_;
        inc_y_ = diffy / cur_dist_;
        inc_z_ = diffz / cur_dist_;
    }
    cur_dist_ = 0;
    ignored_obj_ = ignrd_obj;
    owner_ = sd.d.d_owner;
    target_object_ = sd.target_object;
}

bool ProjectileShot::animate(int elapsed, Mission *m) {

    assert(!life_over_);
    bool draw_impact = false;
    bool self_remove = false;

    double inc_dist = speed_ * (double)elapsed / 1000;
    if ((cur_dist_ + inc_dist) > dist_max_) {
        assert(cur_dist_ <= dist_max_);
        inc_dist = dist_max_ - cur_dist_;
        self_remove = true;
    }
    double was_dist = cur_dist_;
    cur_dist_ += inc_dist;
    bool ignored_state;

    if (ignored_obj_) {
        ignored_state = ignored_obj_->isIgnored();
        ignored_obj_->setIsIgnored(true);
    }

    toDefineXYZ reached_pos;
    bool do_recalc = false;
    reached_pos.x = base_pos_.x + (int)(inc_x_ * cur_dist_);
    if (reached_pos.x < 0) {
        reached_pos.x = 0;
        self_remove = true;
        do_recalc = true;
    } else if (reached_pos.x > (m->mmax_x_ - 1) * 256) {
        reached_pos.x = (m->mmax_x_ - 1) * 256;
        self_remove = true;
        do_recalc = true;
    }
    if (do_recalc) {
        do_recalc = false;
        if (inc_x_ != 0) {
            cur_dist_ = (double)(reached_pos.x - base_pos_.x) / inc_x_;
        }
    }

    reached_pos.y = base_pos_.y + (int)(inc_y_ * cur_dist_);
    if (reached_pos.y < 0) {
        reached_pos.y = 0;
        self_remove = true;
        do_recalc = true;
    } else if (reached_pos.y > (m->mmax_y_ - 1) * 256) {
        reached_pos.y = (m->mmax_y_ - 1) * 256;
        self_remove = true;
        do_recalc = true;
    }
    if (do_recalc) {
        do_recalc = false;
        if (inc_y_ != 0) {
            cur_dist_ = (double)(reached_pos.y - base_pos_.y) / inc_y_;
            reached_pos.x = base_pos_.x + (int)(inc_x_ * cur_dist_);
        }
    }

    reached_pos.z = base_pos_.z + (int)(inc_z_ * cur_dist_);
    if (reached_pos.z < 0) {
        reached_pos.z = 0;
        self_remove = true;
        do_recalc = true;
    } else if (reached_pos.z > (m->mmax_z_ - 1) * 128) {
        reached_pos.z = (m->mmax_z_ - 1) * 128;
        self_remove = true;
        do_recalc = true;
    }
    if (do_recalc) {
        do_recalc = false;
        if (inc_z_ != 0) {
            cur_dist_ = (double)(reached_pos.z - base_pos_.z) / inc_z_;
            reached_pos.x = base_pos_.x + (int)(inc_x_ * cur_dist_);
            reached_pos.y = base_pos_.y + (int)(inc_y_ * cur_dist_);
        }
    }

    PathNode pn(reached_pos.x / 256, reached_pos.y / 256,
        reached_pos.z / 128, reached_pos.x % 256, reached_pos.y % 256,
        reached_pos.z % 128);
    ShootableMapObject * smo = NULL;
    // maxr here is set to maximum that projectile can fly from its
    // current position
    uint8 block_mask = m->inRangeCPos(
        &cur_pos_, &smo, &pn, true, false, dist_max_ - was_dist);
    if (block_mask == 1) {
        if (reached_pos.x == sd_prj_.tp.x
            && reached_pos.y == sd_prj_.tp.y
            && reached_pos.z == sd_prj_.tp.z)
        {
            draw_impact = true;
            self_remove = true;
        }
    } else {
        reached_pos.x = pn.tileX() * 256 + pn.offX();
        reached_pos.y = pn.tileY() * 256 + pn.offY();
        reached_pos.z = pn.tileZ() * 128 + pn.offZ();
        draw_impact = true;
        self_remove = true;
    }
    // TODO : set this somewhere?
    // gauss
    double anim_d = 64;
    if (dmg_range_ == 0)
        //flamer
        anim_d = 64;
    double diffx = (double) (base_pos_.x - reached_pos.x);
    double diffy = (double) (base_pos_.y - reached_pos.y);
    double diffz = (double) (base_pos_.z - reached_pos.z);
    double d = sqrt(diffx * diffx + diffy * diffy
        + diffz * diffz);

    if (d > last_anim_dist_) {
        int diff_dist = (int) ((d - last_anim_dist_) / anim_d);
        if (diff_dist != 0) {
            for (int i = 1; i <= diff_dist; i++) {
                toDefineXYZ t;
                last_anim_dist_ += anim_d;
                t.x = base_pos_.x + (int)(last_anim_dist_ * inc_x_);
                t.y = base_pos_.y + (int)(last_anim_dist_ * inc_y_);
                t.z = base_pos_.z + (int)(last_anim_dist_ * inc_z_);
                if (dmg_range_ != 0) {
                    t.z += 128;
                    if (t.z > (m->mmax_z_ - 1) * 128)
                        t.z = (m->mmax_z_ - 1) * 128;
                }
                SFXObject *so = new SFXObject(m->map(),
                    anims_.trace_anim);
                so->setPosition(t.x / 256, t.y / 256, t.z / 128, t.x % 256,
                    t.y % 256, t.z % 128 );
                so->setTileVisZ();
                m->addSfxObject(so);
            }
        }
    }
    cur_pos_ = reached_pos;

    // additional check for distance, just to be sure
    if (!self_remove) {
        if (d <= dist_max_)
            cur_dist_ = d;
        else
            self_remove = true;
    }

    if (ignored_obj_) {
        ignored_obj_->setIsIgnored(ignored_state);
    }

    if (draw_impact) {
        std::vector <Weapon::ShotDesc> all_shots;
        if (dmg_range_ == 0) {
            Weapon::ShotDesc sd;
            sd.tp = cur_pos_;
            sd.tpn = pn;
            sd.d = sd_prj_.d;
            sd.smo = smo;
            sd.d.d_owner = owner_;
            sd.target_object = target_object_;
            all_shots.push_back(sd);
            makeShot(true, base_pos_, anims_.hit_anim, all_shots,
                anims_.obj_hit_anim);
        } else {
            // NOTE: if projectile hits water, should hit and flames be drawn?
            SFXObject *so = new SFXObject(m->map(),
                anims_.hit_anim);
            so->setPosition(cur_pos_.x / 256, cur_pos_.y / 256,
                cur_pos_.z / 128, cur_pos_.x % 256, cur_pos_.y % 256,
                cur_pos_.z % 128);
            so->setTileVisZ();
            m->addSfxObject(so);

            toDefineXYZ cp = cur_pos_;
            // off_z_ < 128, needs to be zero here
            cp.z = (cp.z & 0xFFFFFF80) + 16;
            if (cp.z > (m->mmax_z_ - 1) * 128)
                cp.z = (m->mmax_z_ - 1) * 128;

            std::vector <ShootableMapObject *> all_targets;
            g_Session.getMission()->getInRangeAll(&cp, all_targets,
                Weapon::stm_AllObjects, true, dmg_range_ + 96);
            for (unsigned int indx = 0; indx < all_targets.size();
                indx++)
            {
                smo = all_targets[indx];
                Weapon::ShotDesc sd;
                sd.tp.x = smo->tileX() * 256 + smo->offX();
                sd.tp.y = smo->tileY() * 256 + smo->offY();
                sd.tp.z = smo->tileZ() * 128 + smo->offZ();
                sd.tpn.setTileXYZ(smo->tileX(), smo->tileY(),
                    smo->tileZ());
                sd.tpn.setOffXYZ(smo->offX(), smo->offY(),
                    smo->offZ());
                // TODO: set direction, if object servives it will need it
                sd.d = sd_prj_.d;
                sd.smo = smo;
                sd.d.d_owner = owner_;
                sd.target_object = target_object_;
                all_shots.push_back(sd);
            }
            makeShot(true, cp, anims_.hit_anim, all_shots,
                anims_.obj_hit_anim);
            int max_flames = 24 + rand() % 8;
            rangeDamageAnim(cur_pos_, (double)dmg_range_, max_flames,
                anims_.rd_anim);
            g_App.gameSounds().play(snd::EXPLOSION);
        }
    }
    if (self_remove)
        life_over_ = self_remove;
    return true;
}

/*!
 * \return mask of different actions done
 * 0b - no ammo (1), 1b - not enough time for single shot (2),
 * 2b - weapon does no damage (4), 3b - weapon's target is wrong (8),
 * 4b - an object blocker (16), 5b - a tile blocking (32), 6b - too far (64),
 * 7b - wrong shooting point, out of game boundaries(128), 8b - wrong shooting
 * point, solid tile
 * 0 - successful shot
*/
uint16 WeaponInstance::inflictDamage(ShootableMapObject * tobj, PathNode * tp,
    int *elapsed, bool ignoreBlocker, uint32 *make_shots)
{
    // TODO: check tobj completed action,
    // TODO : IPA+mods influence

    // for weapons that are not using ammo, ammo_remaining_ = -1
    if (ammo_remaining_ == 0) {
        if (make_shots)
            *make_shots = 0;
        return 1;
    }

    // NOTE: this block of code defines direction for shooter and damaged person
    // it is placed before shots calculation to set direction for shooter even if not
    // enough time is for shot
    int xb = 0;
    int yb = 0;
    int txb = 0;
    int tyb = 0;
    toDefineXYZ cp;
    Mission *m = g_Session.getMission();
    if (owner_) {
        xb = owner_->tileX() * 256 + owner_->offX();
        yb = owner_->tileY() * 256 + owner_->offY();
        if (main_type_ == Weapon::Flamer) {
            int dir = owner_->getDirection(8);
            switch (dir) {
                case 0:
                    yb += 160;
                    break;
                case 1:
                    xb += 96;
                    yb += 96;
                    break;
                case 2:
                    xb += 160;
                    break;
                case 3:
                    xb += 96;
                    yb -= 96;
                    break;
                case 4:
                    yb -= 160;
                    break;
                case 5:
                    xb -= 96;
                    yb -= 96;
                    break;
                case 6:
                    xb -= 160;
                    break;
                case 7:
                    xb -= 96;
                    yb += 96;
                    break;
            }
        }
        if (xb < 0 || (xb > (m->mmax_x_ - 1) * 256))
            return 128;
        if (yb < 0 || (yb > (m->mmax_y_ - 1) * 256))
            return 128;
        cp.x = xb;
        cp.y = yb;
        cp.z = owner_->visZ() * 128 + owner_->offZ()
            + (owner_->sizeZ() >> 1);
        if (cp.z > (m->mmax_z_ - 1) * 128)
            cp.z = (m->mmax_z_ - 1) * 128;
        int tilez = cp.z / 128;
        int tileoz = cp.z % 128;
        if (tileoz != 0)
            tilez++;
        if (m->isTileSolid(cp.x / 256, cp.y / 256, tilez, cp.x % 256,
            cp.y % 256, tileoz))
            return 256;
        if (tobj || tp) {
            if (tobj) {
                txb = tobj->tileX() * 256 + tobj->offX();
                tyb = tobj->tileY() * 256 + tobj->offY();
            } else {
                txb = tp->tileX() * 256 + tp->offX();
                tyb = tp->tileY() * 256 + tp->offY();
            }
            int dir = -1;
            setDirection(txb - xb, tyb - yb, &dir);
            if (dir != -1)
                owner_->setDirection(dir);
        }
    } else {
        xb = tile_x_ * 256 + off_x_;
        yb = tile_y_ * 256 + off_y_;
        cp.x = xb;
        cp.y = yb;
        // NOTE: it is assumed that object has off_z_ = 0, because
        // if off_z_ != 0 tile_z_ = vis_z_ + Z_SHIFT_TO_AIR, from this assumed that
        // tile above is "air", if Z_SHIFT_TO_AIR will not be added, tile will be "solid",
        // trajectory checking will return "failed" response at start
        cp.z = vis_z_ * 128 + off_z_ + Z_SHIFT_TO_AIR;
        if (cp.z > (m->mmax_z_ - 1) * 128)
            cp.z = (m->mmax_z_ - 1) * 128;
    }

    unsigned int shot_prop = pWeaponClass_->shotProperty();

    if (pWeaponClass_->dmgType() == MapObject::dmg_None) {
        return 4;
    } else if (pWeaponClass_->dmgType() == MapObject::dmg_Heal) {
        // NOTE: not only self-healing in future?
        return 4;
    } else if ((shot_prop & Weapon::spe_TargetPedOnly) != 0 && tp) {
        // NOTE: Persuadatron will not shoot on ground
        return 8;
    }

    ShootableMapObject * smp = tobj;
    PathNode pn;
    if(tp)
        pn = *tp;
    uint8 has_blocker = 1;
    if (tobj || tp)
        has_blocker = inRange(cp, &smp, &pn);
    if ((has_blocker & 30) != 0) {
        if (!ignoreBlocker) {
            if (make_shots)
                *make_shots = 0;
            if ((has_blocker & 6) != 0)
                has_blocker = 16;
            else if ((has_blocker & 16) != 0)
                has_blocker = 32;
            else if ((has_blocker & 8) != 0)
                has_blocker = 64;
            return has_blocker;
        }
    }

    uint32 shots = 0;
    if (make_shots)
        shots = *make_shots;
    shots = getShots(elapsed, shots);
    if (make_shots)
        *make_shots = shots;
    if (shots == 0)
        return 2;

    
    if (owner_
        && pWeaponClass_->dmgType() != MapObject::dmg_Mental
#ifdef _DEBUG
        && owner_->majorType() == MapObject::mjt_Ped
#endif
        &&((PedInstance *)owner_)->isOurAgent()) {
        m->incStatisticsShots((int32)shots);
    }
    // TODO: fix this, remove auto targetting for perseudatron,
    // use find non friend in ped animate
    // perseudatron skipping, due to no target
    if (main_type_ != Weapon::Persuadatron)
        this->playSound();
    // 90% accuracy agent * acurracy weapon
    double accuracy = 0.9 * pWeaponClass_->shotAcurracy();

    // angle is used to generate random shot with randomizer
    double angle = pWeaponClass_->shotAngle();

    angle *= (1.0 - accuracy);

    Weapon::ShotDesc base_shot;
    base_shot.smo = NULL;
    base_shot.d.dtype = pWeaponClass_->dmgType();
    base_shot.d.dvalue =  pWeaponClass_->damagePerShot();
    base_shot.d.d_owner = owner_;
    base_shot.target_object = tobj;
    if (tobj) {
        base_shot.tp.x = tobj->tileX() * 256 + tobj->offX();
        base_shot.tp.y = tobj->tileY() * 256 + tobj->offY();
        base_shot.tp.z = tobj->visZ() * 128 + tobj->offZ() + (tobj->sizeZ() >> 1);
        if (base_shot.tp.z > (m->mmax_z_ - 1) * 128)
            base_shot.tp.z = (m->mmax_z_ - 1) * 128;
        base_shot.tpn.setTileXYZ(tobj->tileX(), tobj->tileY(),
            base_shot.tp.z / 128);
        base_shot.tpn.setOffXYZ(tobj->offX(), tobj->offY(),
            base_shot.tp.z % 128);
    } else if (tp) {
        base_shot.tp.x = tp->tileX() * 256 + tp->offX();
        base_shot.tp.y = tp->tileY() * 256 + tp->offY();
        base_shot.tp.z = tp->tileZ() * 128 + tp->offZ() + Z_SHIFT_TO_AIR;
        if (base_shot.tp.z > (m->mmax_z_ - 1) * 128)
            base_shot.tp.z = (m->mmax_z_ - 1) * 128;
        base_shot.tpn = *tp;
        base_shot.tpn.setTileZ(base_shot.tp.z / 128);
        base_shot.tpn.setOffZ(base_shot.tp.z % 128);
    } else {
        base_shot.tp = cp;
        base_shot.tpn = PathNode(cp.x / 256, cp.y / 256, base_shot.tp.z / 128,
            cp.x % 256, cp.y % 256, base_shot.tp.z % 128);
    }
    std::vector <Weapon::ShotDesc> all_shots;
    all_shots.reserve(32);

    int mask = Weapon::stm_AllObjects;

    bool range_damage = (shot_prop & Weapon::spe_RangeDamageOnReach) != 0;

    // every shot is treated separately
    for (uint32 sc = 0; sc < shots; sc++) {
        int ammoused = 1;
        if (shot_prop & Weapon::spe_UsesAmmo) {
            ammoused = shots * pWeaponClass_->ammoPerShot();
            if (ammoused > ammo_remaining_) {
                ammoused = ammo_remaining_;
                ammo_remaining_ = 0;
            } else
                ammo_remaining_ -= ammoused;
        }
        if (shot_prop & Weapon::spe_CreatesProjectile) {
            Weapon::ShotDesc shot_new = base_shot;
            shotTargetRandomizer(&cp, &(shot_new.tp), angle);
            ProjectileShot *prjs = new ProjectileShot(cp, shot_new,
                pWeaponClass_->rangeDmg(), pWeaponClass_->anims(),
                owner_, range(), pWeaponClass_->shotSpeed());
            m->addPrjShot(prjs);
            continue;
        }
        if (shot_prop & Weapon::spe_ShootsWhileNoTarget) {
            std::vector <Weapon::ShotDesc> gen_shots;
            if (shot_prop & Weapon::spe_PointToPoint) {
                if (!tobj) {
                    getNonFriendInRange(&cp, smp, false);
                    if (smp) {
                        gen_shots.push_back(base_shot);
                        gen_shots.back().smo = smp;
                        this->playSound();
                    }
                }
                if (tobj && tobj->majorType() == MapObject::mjt_Ped) {
                    if (((PedInstance *)owner_)->checkFriendIs(
                        (PedInstance *)tobj))
                    {
                        tobj = NULL;
                    } else {
                        gen_shots.push_back(base_shot);
                        gen_shots.back().smo = tobj;
                    }
                } else
                    tobj = NULL;
            } else if (shot_prop & Weapon::spe_PointToManyPoints) {
            } else {
                gen_shots.push_back(base_shot);
            }
            if (range_damage) {
                for (unsigned int i = 0; i < gen_shots.size(); i++) {
                    std::vector<ShootableMapObject *> all_targets;

                    Weapon::ShotDesc &sdc = gen_shots[i];
                    inRange(cp, &sdc.smo, &sdc.tpn, true);
                    sdc.tpn.convertPosToXYZ(&sdc.tp);
                    getInRangeAll(sdc.tp, all_targets, mask,
                        true, pWeaponClass_->rangeDmg());
                    for (unsigned int indx = 0; indx < all_targets.size();
                        indx++)
                    {
                        ShootableMapObject * smo = all_targets[indx];
                        Weapon::ShotDesc sd;
                        sd.tp.x = smo->tileX() * 256 + smo->offX();
                        sd.tp.y = smo->tileY() * 256 + smo->offY();
                        sd.tp.z = smo->tileZ() * 128 + smo->offZ();
                        sd.tpn.setTileXYZ(smo->tileX(), smo->tileY(),
                            smo->tileZ());
                        sd.tpn.setOffXYZ(smo->offX(), smo->offY(),
                            smo->offZ());
                        sd.d = sdc.d;
                        sd.smo = smo;
                        sd.d.d_owner = sdc.d.d_owner;
                        sd.target_object = sdc.target_object;
                        all_shots.push_back(sd);
                    }
                }
            } else {
                all_shots = gen_shots;
            }
        } else {
            std::vector <Weapon::ShotDesc> gen_shots;
            switch((shot_prop & (Weapon::spe_PointToPoint
                | Weapon::spe_PointToManyPoints)))
            {
                case Weapon::spe_PointToPoint:
                    for (int i = 0; i < ammoused; i++) {
                        gen_shots.push_back(base_shot);
                        Weapon::ShotDesc &last_one = gen_shots.back();
                        shotTargetRandomizer(&cp, &(last_one.tp), angle);
                        last_one.tpn.setTileXYZ(last_one.tp.x / 256,
                            last_one.tp.y / 256, last_one.tp.z / 128);
                        last_one.tpn.setOffXYZ(last_one.tp.x % 256,
                            last_one.tp.y % 256, last_one.tp.z % 128);
                    }
                    if (range_damage) {
                        for (unsigned int i = 0; i < gen_shots.size(); i++) {
                            std::vector<ShootableMapObject *> all_targets;

                            Weapon::ShotDesc &sdc = gen_shots[i];
                            inRange(cp, &sdc.smo, &sdc.tpn, true);
                            sdc.tpn.convertPosToXYZ(&sdc.tp);
                            getInRangeAll(sdc.tp, all_targets, mask,
                                true, pWeaponClass_->rangeDmg());
                            for (unsigned int indx = 0; indx < all_targets.size();
                                indx++)
                            {
                                ShootableMapObject * smo = all_targets[indx];
                                Weapon::ShotDesc sd;
                                sd.tp.x = smo->tileX() * 256 + smo->offX();
                                sd.tp.y = smo->tileY() * 256 + smo->offY();
                                sd.tp.z = smo->tileZ() * 128 + smo->offZ();
                                sd.tpn.setTileXYZ(smo->tileX(), smo->tileY(),
                                    smo->tileZ());
                                sd.tpn.setOffXYZ(smo->offX(), smo->offY(),
                                    smo->offZ());
                                sd.d = sdc.d;
                                sd.smo = smo;
                                sd.d.d_owner = sdc.d.d_owner;
                                sd.target_object = sdc.target_object;
                                all_shots.push_back(sd);
                            }
                            if (sdc.smo == NULL) {
                                // drawing anim at point of impact
                                all_shots.push_back(sdc);
                            }
                        }
                    } else {
                        all_shots = gen_shots;
                    }
                    break;
                case Weapon::spe_PointToManyPoints:
                    for (unsigned short i = 0; i < ammoused; i++) {
                        gen_shots.push_back(base_shot);
                        Weapon::ShotDesc &last_one = gen_shots.back();
                        shotTargetRandomizer(&cp, &(last_one.tp), angle);
                        last_one.tpn.setTileXYZ(last_one.tp.x / 256,
                            last_one.tp.y / 256, last_one.tp.z / 128);
                        last_one.tpn.setOffXYZ(last_one.tp.x % 256,
                            last_one.tp.y % 256, last_one.tp.z % 128);
                    }
                    if (range_damage) {
                    } else {
                        all_shots = gen_shots;
                    }
                    break;
            }
        }
        makeShot(range_damage, cp, pWeaponClass_->anims()->hit_anim, all_shots,
            pWeaponClass_->anims()->obj_hit_anim, this);
        all_shots.clear();
    }
    return 0;
}

/*!
 * Plays the sound associated with that weapon.
 */
void WeaponInstance::playSound() {
    g_App.gameSounds().play(pWeaponClass_->getSound());
}

uint8 WeaponInstance::inRange(toDefineXYZ & cp, ShootableMapObject ** t,
    PathNode * pn, bool setBlocker, bool checkTileOnly, int maxr)
{
    // NOTE: too many calculations of type tile*tilesize + off,
    // optimize this if possible everywhere, in freesynd

    if (maxr == -1)
        maxr = range();

    bool ownerState, vehicleState, selfState = isIgnored();

    setIsIgnored(true);
    if (owner_) {
        ownerState = owner_->isIgnored();
        owner_->setIsIgnored(true);
        vehicleState = ((PedInstance *)owner_)->setVehicleIgnore(true);
    }

    uint8 block_mask = g_Session.getMission()->inRangeCPos(
        &cp, t, pn, setBlocker, checkTileOnly, maxr);

    setIsIgnored(selfState);
    if (owner_) {
        owner_->setIsIgnored(ownerState);
        if (vehicleState)
            ((PedInstance *)owner_)->setVehicleIgnore(false);
    }

    return block_mask;
}

uint8 WeaponInstance::inRangeNoCP(ShootableMapObject ** t, PathNode * pn,
                         bool setBlocker, bool checkTileOnly, int maxr)
{
    toDefineXYZ cxyz;

    // NOTE: calculations for Z should be the same as in inflictDamage for cp
    if (owner_) {
        cxyz.x = owner_->tileX() * 256 + owner_->offX();
        cxyz.y = owner_->tileY() * 256 + owner_->offY();
        cxyz.z = owner_->visZ() * 128 + owner_->offZ() + (owner_->sizeZ() >> 1);
    } else {
        cxyz.x = tile_x_ * 256 + off_x_;
        cxyz.y = tile_y_ * 256 + off_y_;
        cxyz.z = vis_z_ * 128 + off_z_ + Z_SHIFT_TO_AIR;
    }

    return inRange(cxyz, t, pn, setBlocker, checkTileOnly, maxr);
}

int WeaponInstance::getShots(int *elapsed, uint32 make_shots) {
    int time_for_shot = pWeaponClass_->timeForShot();
    int time_reload = pWeaponClass_->timeReload();
#if 0
    // TODO: if owner exists these two values should change(IPA, mods)
    if (owner_)
#endif
    // TODO check in weaponinstance animate double consuming of elapsed
    int time_full_shot = time_for_shot + time_reload;
    int elapsed_l = *elapsed;
    *elapsed = 0;
    time_consumed_ = true;
    if (weapon_used_time_ >= time_for_shot) {
        weapon_used_time_ += elapsed_l;
        if (weapon_used_time_ >= time_full_shot) {
            // reloading after previous shot
            weapon_used_time_ -= time_full_shot;
        } else {
            // reload consumed all time, no time for shooting
            return 0;
        }
    } else
        weapon_used_time_ += elapsed_l;

    if (weapon_used_time_ == 0) {
        return 0;
    }
    elapsed_l = 0;

    uint32 shots_can_do = 0xFFFFFFFF;
    if (pWeaponClass_->shotProperty() & Weapon::spe_UsesAmmo) {
        shots_can_do = ammo_remaining_ / pWeaponClass_->ammoPerShot();
        if (ammo_remaining_ % pWeaponClass_->ammoPerShot())
            shots_can_do++;
    }
    uint32 shots = weapon_used_time_ / time_full_shot;
    weapon_used_time_ %= time_full_shot;
    bool adjusted = false;
    if (weapon_used_time_ >= time_for_shot) {
        shots++;
        adjusted = true;
    }
    // Adjusting time consumed and shots done to ammo
    // that can be used
    if (shots_can_do < shots) {
        if (adjusted) {
            shots--;
            adjusted = false;
        }
        if (shots_can_do < shots) {
         elapsed_l = time_full_shot * (shots - shots_can_do);
         shots = shots_can_do;
        } else
            elapsed_l = weapon_used_time_;
        weapon_used_time_ = 0;
    }

    if (make_shots != 0 && shots != 0 && make_shots < shots) {
        // we might have some time left here
        if (adjusted)
            shots--;
        if (make_shots < shots) {
         *elapsed = time_full_shot  * (shots - make_shots) + elapsed_l;
         *elapsed += weapon_used_time_;
         shots = make_shots;
        } else
            *elapsed = elapsed_l + weapon_used_time_;
        weapon_used_time_ = 0;
    } else
        *elapsed = elapsed_l;
    return shots;
}

void WeaponInstance::getInRangeAll(toDefineXYZ & cp,
   std::vector<ShootableMapObject *> & targets, uint8 mask,
   bool checkTileOnly, int maxr)
{
    // NOTE: no need to ignore owner_ here, as all in range are vulnerable

    bool selfState = isIgnored();
    setIsIgnored(true);

    g_Session.getMission()->getInRangeAll(&cp, targets, mask,
        checkTileOnly, maxr);

    setIsIgnored(selfState);
}

bool WeaponInstance::isReloading() {
    int time_for_shot = pWeaponClass_->timeForShot();
    int time_reload = pWeaponClass_->timeReload();
    int time_full_shot = time_for_shot + time_reload;
    return (weapon_used_time_ > time_for_shot
        && weapon_used_time_ <= time_full_shot);
}

void WeaponInstance::activate() {
    //if (main_type_ == Weapon::TimeBomb)
        activated_ = true;
}

void WeaponInstance::deactivate() {
    activated_ = false;
    if (main_type_ == Weapon::TimeBomb)
        weapon_used_time_ = 0;
}

void ShotClass::makeShot(bool rangeChecked, toDefineXYZ &cp, int anim_hit,
    std::vector <Weapon::ShotDesc> &all_shots, int anim_obj_hit,
    WeaponInstance * w)
{
    Mission *m = g_Session.getMission();
    for (std::vector <Weapon::ShotDesc>::iterator it_a = all_shots.begin();
         it_a != all_shots.end(); it_a++) {
        ShootableMapObject * smp = it_a->smo;
        PathNode pn = it_a->tpn;
        ShootableMapObject::DamageInflictType d;
        uint8 has_blocker = 0;
        if (rangeChecked)
            has_blocker = 1;
        else {
            assert(w != NULL);
            has_blocker = w->inRange(cp, &smp, &pn, true);
        }
        // TODO: set direction?
        d.ddir = -1;
        if (smp) {
            int txb = it_a->tp.x;
            int tyb = it_a->tp.y;
            smp->setDirection(txb - cp.x, tyb - cp.y, &(d.ddir));
        }

        d.dvalue = it_a->d.dvalue;
        d.dtype = it_a->d.dtype;
        d.d_owner = it_a->d.d_owner;
        if ((has_blocker & 22) != 0) {
            if ((has_blocker & 16) != 0) {
                if (anim_hit != SFXObject::sfxt_Unknown) {
                    SFXObject *so = new SFXObject(m->map(),
                        anim_hit);
                    so->setPosition(pn.tileX(), pn.tileY(), pn.tileZ(),
                        pn.offX(), pn.offY(), pn.offZ());
                    so->setTileVisZ();
                    so->correctZ();
                    m->addSfxObject(so);
                }
            } else if ((has_blocker & 6) != 0) {
                assert(smp != 0);
                // TODO: more information needed, handle return value
                // pass info to shot owner if exist
                if (smp->handleDamage(&d)) {
                    if (it_a->target_object == smp
                        && it_a->d.d_owner
#ifdef _DEBUG
                        && it_a->d.d_owner->majorType() == MapObject::mjt_Ped
#endif
                        && ((PedInstance *)it_a->d.d_owner)->isOurAgent())
                    {
                        m->incStatisticsHits();
                    }
                }
                if (anim_obj_hit != SFXObject::sfxt_Unknown) {
                    SFXObject *so = new SFXObject(m->map(),
                        anim_obj_hit);
                    int z = smp->visZ() * 128 + smp->offZ() + (smp->sizeZ() >> 1);
                    if (z > (m->mmax_z_ - 1) * 128)
                        z = (m->mmax_z_ - 1) * 128;
                    so->setPosition(smp->tileX(), smp->tileY(), z / 128,
                        smp->offX(), smp->offY(), z % 128);
                    so->setTileVisZ();
                    so->correctZ();
                    m->addSfxObject(so);
                }
            }
        } else if ((has_blocker & 8) != 0) {
            if (anim_hit != SFXObject::sfxt_Unknown) {
                SFXObject *so = new SFXObject(m->map(),
                    anim_hit);
                so->setPosition(pn.tileX(), pn.tileY(), pn.tileZ(), pn.offX(),
                    pn.offY(), pn.offZ());
                so->setTileVisZ();
                so->correctZ();
                m->addSfxObject(so);
            }
        } else if (has_blocker == 1) {
            if (smp == NULL) {
                if (anim_hit != SFXObject::sfxt_Unknown) {
                    SFXObject *so = new SFXObject(m->map(),
                        anim_hit);
                    so->setPosition(pn.tileX(), pn.tileY(), pn.tileZ(), pn.offX(),
                        pn.offY(), pn.offZ());
                    so->setTileVisZ();
                    so->correctZ();
                    m->addSfxObject(so);
                }
            } else {
                // TODO: more information needed, handle return value
                // pass info to shot owner if exist
                if (smp->handleDamage(&d)) {
                    if (it_a->target_object == smp
                        &&it_a->d.d_owner
#ifdef _DEBUG
                        && it_a->d.d_owner->majorType() == MapObject::mjt_Ped
#endif
                        && ((PedInstance *)it_a->d.d_owner)->isOurAgent())
                    {
                        m->incStatisticsHits();
                    }
                }
                if (anim_obj_hit != SFXObject::sfxt_Unknown) {
                    SFXObject *so = new SFXObject(m->map(),
                        anim_obj_hit);
                    int z = smp->visZ() * 128 + smp->offZ() + (smp->sizeZ() >> 1);
                    if (z > (m->mmax_z_ - 1) * 128)
                        z = (m->mmax_z_ - 1) * 128;
                    so->setPosition(smp->tileX(), smp->tileY(), z / 128,
                        smp->offX(), smp->offY(), z % 128);
                    so->setTileVisZ();
                    so->correctZ();
                    m->addSfxObject(so);
                }
            }
        }
    }
}

void WeaponInstance::getHostileInRange(toDefineXYZ * cp,
    ShootableMapObject * & target, uint8 mask, bool checkTileOnly, int maxr)
{
    bool ownerState, vehicleState, selfState = isIgnored();
    setIsIgnored(true);
 
    if (owner_) {
        ownerState = owner_->isIgnored();
        owner_->setIsIgnored(true);
        vehicleState = ((PedInstance *)owner_)->setVehicleIgnore(true);
    }

    bool found = false;
    double dist = -1;
    double d = -1;
    Mission * m = g_Session.getMission();

    if (mask & MapObject::mjt_Ped) {
        for (int i = 0; i < m->numPeds(); i++) {
            ShootableMapObject *p = m->ped(i);
            if (!p->isIgnored() && (((PedInstance *)owner_)->checkHostileIs(p)
                || ((PedInstance *)owner_)->isInHostilesFound(p))
                && m->inRangeCPos(cp, &p, NULL, false, checkTileOnly, maxr,
                &d) == 1)
                // TODO: inrange if checktileonly = false might return "7"
                // different handling for this we might shoot if blocker
                // is not friendly
            {
                if (found) {
                    if (d < dist) {
                        target = p;
                        dist = d;
                    }
                } else {
                    dist = d;
                    target = p;
                    found = true;
                }
            }
        }
    }
    if (mask & MapObject::mjt_Vehicle) {
        for (int i = 0; i < m->numVehicles(); i++) {
            ShootableMapObject *v = m->vehicle(i);
            if (!v->isIgnored() && (((PedInstance *)owner_)->checkHostileIs(v)
                || ((PedInstance *)owner_)->isInHostilesFound(v))
                && m->inRangeCPos(cp, &v, NULL, false, checkTileOnly, maxr,
                &d) == 1)
            {
                if (found) {
                    if (d < dist) {
                        target = v;
                        dist = d;
                    }
                } else {
                    dist = d;
                    target = v;
                    found = true;
                }
            }
        }
    }

    setIsIgnored(selfState);
    if (owner_) {
        owner_->setIsIgnored(ownerState);
        if (vehicleState)
            ((PedInstance *)owner_)->setVehicleIgnore(false);
    }
}

void WeaponInstance::getNonFriendInRange(toDefineXYZ * cp,
    ShootableMapObject * & target, bool checkTileOnly, int maxr)
{
    bool ownerState, vehicleState, selfState = isIgnored();
    setIsIgnored(true);
 
    if (owner_) {
        ownerState = owner_->isIgnored();
        owner_->setIsIgnored(true);
        vehicleState = ((PedInstance *)owner_)->setVehicleIgnore(true);
    }

    bool found = false;
    double dist = -1;
    double d = -1;
    Mission * m = g_Session.getMission();
    target = NULL;
    if (maxr == -1)
        maxr = range();

    for (int i = 0; i < m->numPeds(); i++) {
        ShootableMapObject *p = m->ped(i);
        if (!p->isIgnored()
            // TODO: inrange if checktileonly = false might return "7"
            // different handling for this we might shoot if blocker
            // is not friendly
            && !(((PedInstance *)owner_)->checkFriendIs((PedInstance *)p))
            && m->inRangeCPos(cp, &p, NULL, false, checkTileOnly, maxr,
            &d) == 1)
        {
            if (found) {
                if (d < dist) {
                    target = p;
                    dist = d;
                }
            } else {
                dist = d;
                target = p;
                found = true;
            }
        }
    }

    setIsIgnored(selfState);
    if (owner_) {
        owner_->setIsIgnored(ownerState);
        if (vehicleState)
            ((PedInstance *)owner_)->setVehicleIgnore(false);
    }
}

bool WeaponInstance::handleDamage(ShootableMapObject::DamageInflictType * d)
{
    // TODO: add damage handling for timebomb and flamer,
    // they will explode when shot
    if (health_ > 0)
        printf("weapon hit\n");
    return true;
}

void ShotClass::rangeDamageAnim(toDefineXYZ &cp, double dmg_rng,
    int max_anims, int rngdamg_anim)
{
    Mission *m = g_Session.getMission();
    toDefineXYZ base_pos_ = cp;
    cp.z += 16;
    if (cp.z > (m->mmax_z_ - 1) * 128)
        cp.z = (m->mmax_z_ - 1) * 128;
    // TODO: exclude flames on water, put these flames to the ground,
    // don't draw in air(, stairs problem?)
    dmg_rng *= 1.1;
    for (char i = 0; i < max_anims; i++) {
        // TODO: bad randoming for animations, something better needed
        toDefineXYZ target_pos = base_pos_;
        shotTargetRandomizer(&cp, &target_pos, 120.0, dmg_rng, true);
        PathNode pn = PathNode(target_pos.x / 256, target_pos.y / 256,
            target_pos.z / 128, target_pos.x % 256,
            target_pos.y % 256, target_pos.z % 128);
        m->inRangeCPos(&cp, NULL, &pn, true, true, dmg_rng);
        SFXObject *so = new SFXObject(m->map(), rngdamg_anim,
                           100 * (rand() % 16));
        so->setPosition(pn.tileX(), pn.tileY(), pn.tileZ(),
                        pn.offX(), pn.offY(), pn.offZ());
        so->setTileVisZ();
        m->addSfxObject(so);
    }
}
