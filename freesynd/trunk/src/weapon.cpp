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
    int w_ammo_per_shot, int w_time_for_shot, int w_time_reload,
    unsigned int w_shot_property, int w_hit_anim, int w_obj_hit_anim,
    int w_trace_anim, int w_rd_anim, int w_range_dmg, double w_shot_angle,
    double w_shot_accuracy)
{

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
    shot_property_ = w_shot_property;
    anims_.hit_anim = w_hit_anim;
    anims_.obj_hit_anim = w_obj_hit_anim;
    anims_.trace_anim = w_trace_anim;
    anims_.rd_anim = w_rd_anim;
    range_dmg_ = w_range_dmg;
    shot_angle_ = w_shot_angle;
    shot_accuracy_ = w_shot_accuracy;
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
    owner_ = NULL;
    activated_ = false;
}

bool WeaponInstance::animate(int elapsed) {
    Weapon::WeaponType wt = pWeaponClass_->getWeaponType();
    if (owner_) {
        if (wt == Weapon::EnergyShield) {
            int ammoused = getShots(elapsed)
                * pWeaponClass_->ammoPerShot();
            if (((PedInstance *)owner_)->selectedWeapon()
                && ((PedInstance *)owner_)->selectedWeapon() == this)
            {
                if (ammoused >= ammo_remaining_) {
                    ammo_remaining_ = 0;
                    ((PedInstance *)owner_)->selectNextWeapon();
                } else
                    ammo_remaining_ -= ammoused;
            } else if (ammo_remaining_ != 0
                && ammo_remaining_ != pWeaponClass_->ammo())
            {
                ammo_remaining_ += ammoused;
                if (ammo_remaining_ > pWeaponClass_->ammo())
                    ammo_remaining_ = pWeaponClass_->ammo();
            }
        } else if (wt == Weapon::MediKit) {
            if (owner_->health() != owner_->startHealth()) {
                owner_->setHealth(owner_->startHealth());
                ammo_remaining_ = 0;
                ((PedInstance *)owner_)->selectNextWeapon();
                weapon_used_time_ = 0;
            }
        }
    } else if (weapon_used_time_ != 0 || activated_) {
        weapon_used_time_ += elapsed;
        if (weapon_used_time_ > (pWeaponClass_->timeForShot()
            + pWeaponClass_->timeReload()))
        {
            if (activated_ && wt == Weapon::TimeBomb
                && inflictDamage(NULL, NULL))
            {
                deactivate();
                map_ = -1;
                setIsIgnored(true);

                Mission *m = g_Session.getMission();
                int max_anims = 16 + rand() % 8;
                toDefineXYZ cur_pos = {tile_x_ * 256 + off_x_,
                    tile_y_ * 256 + off_y_, vis_z_ * 128 + off_z_};
                // TODO: make a function, also used in projectileshot::animate
                SFXObject *so = new SFXObject(m->map(),
                    pWeaponClass_->anims()->hit_anim);
                so->setPosition(cur_pos.x / 256, cur_pos.y / 256, cur_pos.z / 128,
                    cur_pos.x % 256, cur_pos.y % 256, cur_pos.z % 128);
                so->setTileVisZ();
                m->addSfxObject(so);
                toDefineXYZ base_pos = cur_pos;
                cur_pos.z += 16;
                if (cur_pos.z > (m->mmax_z_ - 1) * 128)
                    cur_pos.z = (m->mmax_z_ - 1) * 128;
                // TODO: exclude explosionfires on water
                for (int i = 0; i < max_anims; i++) {
                    toDefineXYZ target_pos = base_pos;
                    shotTargetRandomizer(&cur_pos, &target_pos, 120.0,
                        pWeaponClass_->rangeDmg() + 32.0, true);
                    PathNode pn(target_pos.x / 256, target_pos.y / 256,
                        target_pos.z / 128, target_pos.x % 256,
                        target_pos.y % 256, target_pos.z % 128);
                    m->inRangeCPos(&cur_pos, NULL, &pn, true, true, 384 + 32);
                    so = new SFXObject(m->map(),
                        pWeaponClass_->anims()->rd_anim);
                    so->setPosition(pn.tileX(), pn.tileY(), pn.tileZ(),
                        pn.offX(), pn.offY(), pn.offZ());
                    so->setTileVisZ();
                    m->addSfxObject(so);
                }
                return true;
            }
            weapon_used_time_ = 0;
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
    // TODO: the generated points are in rect should be in circle
    if (angle == 0)
        return;
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

ProjectileShot::ProjectileShot(toDefineXYZ &cp, toDefineXYZ &tp,
    MapObject::DamageType dt, int d_value, int d_range,
    Weapon::HitAnims *panims, ShootableMapObject * ignrd_obj, int range_max,
    ShootableMapObject * w_owner)
{
    cur_pos_ = cp;
    target_pos_ = tp;
    base_pos_ = cur_pos_;
    dmg_type_ = dt;
    dmg_value_ = d_value;
    dmg_range_ = d_range;
    anims_ = *panims;
    dist_max_ = range_max;
    dist_passed_ = 0;
    // TODO: define speed somewhere in weapon, also animation shift by Z,
    // animation step
    if (dmg_range_ == 0) {
        speed_ = 1500;
        last_anim_dist_ = 128;
    } else {
        speed_ = 4000;
        last_anim_dist_ = 0;
    }
    life_over_ = false;
    double diffx = (double)(target_pos_.x - cur_pos_.x);
    double diffy = (double)(target_pos_.y - cur_pos_.y);
    double diffz = (double)(target_pos_.z - cur_pos_.z);
    cur_dist_ = sqrt(diffx * diffx + diffy * diffy + diffz * diffz);
    if (cur_dist_ != 0) {
        inc_x_ = diffx / cur_dist_;
        inc_y_ = diffy / cur_dist_;
        inc_z_ = diffz / cur_dist_;
    }
    cur_dist_ = 0;
    ignored_obj_ = ignrd_obj;
    owner_ = w_owner;
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
    uint8 block_mask = m->inRangeCPos(
        &cur_pos_, &smo, &pn, true, false, (int)inc_dist);
    if (block_mask == 1) {
        if (reached_pos.x == target_pos_.x
            && reached_pos.y == target_pos_.y
            && reached_pos.z == target_pos_.z)
        {
            draw_impact = true;
            self_remove = true;
        }
    } else if (block_mask != 0) {
        reached_pos.x = pn.tileX() * 256 + pn.offX();
        reached_pos.y = pn.tileY() * 256 + pn.offY();
        reached_pos.z = pn.tileZ() * 128 + pn.offZ();
        draw_impact = true;
        self_remove = true;
    }
    double anim_d = 64;
    if (dmg_range_ == 0)
        anim_d = 32;
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
            sd.d.dtype = dmg_type_;
            sd.d.dvalue = dmg_value_;
            sd.smo = smo;
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
                sd.d.dtype = dmg_type_;
                sd.d.dvalue = dmg_value_;
                sd.smo = smo;
                all_shots.push_back(sd);
            }
            makeShot(true, cp, anims_.hit_anim, all_shots,
                anims_.obj_hit_anim);
            int max_flames = 16 + rand() % 8;
            base_pos_ = cur_pos_;
            cur_pos_.z += 16;
            if (cur_pos_.z > (m->mmax_z_ - 1) * 128)
                cur_pos_.z = (m->mmax_z_ - 1) * 128;
            // TODO: exclude flames on water
            for (int i = 0; i < max_flames; i++) {
                target_pos_ = base_pos_;
                shotTargetRandomizer(&cur_pos_, &target_pos_, 120.0,
                    (double)dmg_range_ + 32.0, true);
                pn = PathNode(target_pos_.x / 256, target_pos_.y / 256,
                    target_pos_.z / 128, target_pos_.x % 256,
                    target_pos_.y % 256, target_pos_.z % 128);
                m->inRangeCPos(&cur_pos_, NULL, &pn, true, true,
                    dmg_range_ + 32);
                so = new SFXObject(m->map(), anims_.rd_anim,
                    100 * (rand() % 16));
                so->setPosition(pn.tileX(), pn.tileY(), pn.tileZ(),
                    pn.offX(), pn.offY(), pn.offZ());
                so->setTileVisZ();
                m->addSfxObject(so);
            }
        }
    }
    if (self_remove)
        life_over_ = self_remove;
    return true;
}

bool WeaponInstance::inflictDamage(ShootableMapObject * tobj, PathNode * tp,
    int elapsed, bool ignoreBlocker)
{
    if (ammo_remaining_ == 0)
        return false;

    int shots = getShots(elapsed);
    if (shots == 0)
        return false;

    if (pWeaponClass_->dmgType() == MapObject::dmg_None) {
        return false;
    } else if (pWeaponClass_->dmgType() == MapObject::dmg_Heal) {
        // NOTE: not only self-healing in future?
        return false;
    } else if (pWeaponClass_->dmgType() == MapObject::dmg_Mental) {
        return false;
    }

    ShootableMapObject * smp = tobj;
    PathNode pn;
    toDefineXYZ cp;
    if(tp)
        pn = *tp;
    uint8 has_blocker = 1;
    if (tobj || tp)
        has_blocker = inRange(&smp, &pn);
    int xb = 0;
    int yb = 0;
    int txb = 0;
    int tyb = 0;
    if (owner_) {
        xb = owner_->tileX() * 256 + owner_->offX();
        yb = owner_->tileY() * 256 + owner_->offY();
        cp.x = xb;
        cp.y = yb;
        cp.z = owner_->visZ() * 128 + owner_->offZ()
            + (owner_->sizeZ() >> 1);
        if (cp.z > (g_Session.getMission()->mmax_z_ - 1) * 128)
            cp.z = (g_Session.getMission()->mmax_z_ - 1) * 128;
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
        cp.z = vis_z_ * 128 + off_z_;
    }
    if ((has_blocker & 14) != 0) {
        if (!ignoreBlocker)
            return false;
    } else if((has_blocker & 1) == 0)
        if (!ignoreBlocker)
            return false;

    this->playSound();
    // 90% accuracy agent * acurracy weapon
    double accuracy = 0.9 * pWeaponClass_->shotAcurracy();

    // angle is used to generate random shot with randomizer
    double angle = pWeaponClass_->shotAngle();

    angle *= (1 - accuracy);

    Weapon::ShotDesc base_shot;
    base_shot.smo = NULL;
    base_shot.d.dtype = pWeaponClass_->dmgType();
    base_shot.d.dvalue =  pWeaponClass_->damagePerShot();
    if (tobj) {
        base_shot.tp.x = tobj->tileX() * 256 + tobj->offX();
        base_shot.tp.y = tobj->tileY() * 256 + tobj->offY();
        base_shot.tp.z = tobj->visZ() * 128 + tobj->offZ() + (tobj->sizeZ() >> 1);
        if (base_shot.tp.z > (g_Session.getMission()->mmax_z_ - 1) * 128)
            base_shot.tp.z = (g_Session.getMission()->mmax_z_ - 1) * 128;
        base_shot.tpn.setTileXYZ(tobj->tileX(), tobj->tileY(),
            base_shot.tp.z / 128);
        base_shot.tpn.setOffXYZ(tobj->offX(), tobj->offY(),
            base_shot.tp.z % 128);
    } else if (tp) {
        base_shot.tp.x = tp->tileX() * 256 + tp->offX();
        base_shot.tp.y = tp->tileY() * 256 + tp->offY();
        base_shot.tp.z = tp->tileZ() * 128 + tp->offZ() + 1;
        if (base_shot.tp.z > (g_Session.getMission()->mmax_z_ - 1) * 128)
            base_shot.tp.z = (g_Session.getMission()->mmax_z_ - 1) * 128;
        base_shot.tpn = *tp;
        base_shot.tpn.setTileZ(base_shot.tp.z / 128);
        base_shot.tpn.setOffZ(base_shot.tp.z % 128);
    } else {
        base_shot.tp = cp;
        base_shot.tp.z++;
        if (base_shot.tp.z > (g_Session.getMission()->mmax_z_ - 1) * 128)
            base_shot.tp.z = (g_Session.getMission()->mmax_z_ - 1) * 128;
        base_shot.tpn = PathNode(cp.x / 256, cp.y / 256, base_shot.tp.z / 128,
            cp.x % 256, cp.y % 256, base_shot.tp.z % 128);
    }
    unsigned int shot_prop = pWeaponClass_->shotProperty();
    std::vector <Weapon::ShotDesc> all_shots;
    all_shots.reserve(20);

    int mask = Weapon::stm_AllObjects;

    int ammoused = 1;
    if (shot_prop & Weapon::spe_UsesAmmo) {
        if (pWeaponClass_->ammo() > 0) {
            ammoused = shots * pWeaponClass_->ammoPerShot();
                if (ammoused > ammo_remaining_) {
                    ammoused = ammo_remaining_;
                    ammo_remaining_ = 0;
                } else
                    ammo_remaining_ -= ammoused;
        }
    }

    bool range_damage = (shot_prop & Weapon::spe_RangeDamageOnReach) != 0;
    if (shot_prop & Weapon::spe_CreatesProjectile) {
        for (int i = 0; i < ammoused; i++) {
            Weapon::ShotDesc shot_new = base_shot;
            //shotTargetRandomizer(&cp, &(shot_new.tp), angle);
            ProjectileShot *prjs = new ProjectileShot(cp, shot_new.tp,
                shot_new.d.dtype, shot_new.d.dvalue, pWeaponClass_->rangeDmg(),
                pWeaponClass_->anims(), owner_, range());
            g_Session.getMission()->addPrjShot(prjs);
        }
        return true;
    }
    if (shot_prop & Weapon::spe_NoTarget) {
        std::vector <Weapon::ShotDesc> gen_shots;
        if (shot_prop & Weapon::spe_PointToPoint) {
        } else if (shot_prop & Weapon::spe_PointToManyPoints) {
        } else {
            gen_shots.push_back(base_shot);
        }
        if (range_damage) {
            for (unsigned int i = 0; i < gen_shots.size(); i++) {
                std::vector<ShootableMapObject *> all_targets;

                getInRangeAll(gen_shots[i].tp, all_targets, mask,
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
                    sd.d = gen_shots[i].d;
                    sd.smo = smo;
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

                        getInRangeAll(gen_shots[i].tp, all_targets, mask,
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
                            sd.d = gen_shots[i].d;
                            sd.smo = smo;
                            all_shots.push_back(sd);
                        }
                        if (gen_shots[i].smo == NULL) {
                            // drawing anim at point of impact
                            all_shots.push_back(gen_shots[i]);
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
    // TODO: make this instant
    makeShot(range_damage, cp, pWeaponClass_->anims()->hit_anim, all_shots,
        pWeaponClass_->anims()->obj_hit_anim, this);
    return true;
}

/*!
 * Plays the sound associated with that weapon.
 */
void WeaponInstance::playSound() {
    g_App.gameSounds().play(pWeaponClass_->getSound());
}

/*
* returns mask where bits are:
* 0b - target in range(1); 1b - blocker is object, "t" and "pn" are set(2)
* 2b - blocker tile, "pn" is set(4), 3b - reachable point set
*/
uint8 WeaponInstance::inRange(ShootableMapObject ** t, PathNode * pn,
                         bool setBlocker, bool checkTileOnly, int maxr)
{
    // NOTE: too many calculations of type tile*tilesize + off,
    // optimize this if possible everywhere, in freesynd

    if (maxr == -1)
        maxr = range();

    double d = 0;
    bool ownerState, selfState;
    toDefineXYZ cxyz;

    selfState = isIgnored();
    setIsIgnored(true);
    if (owner_) {
        ownerState = owner_->isIgnored();
        owner_->setIsIgnored(true);
        cxyz.x = owner_->tileX() * 256 + owner_->offX();
        cxyz.y = owner_->tileY() * 256 + owner_->offY();
        cxyz.z = owner_->visZ() * 128 + owner_->offZ() + (owner_->sizeZ() >> 1);
    } else {
        cxyz.x = tile_x_ * 256 + off_x_;
        cxyz.y = tile_y_ * 256 + off_y_;
        cxyz.z = vis_z_ * 128 + off_z_ + 1;
    }

    uint8 block_mask = g_Session.getMission()->inRangeCPos(
        &cxyz, t, pn, setBlocker, checkTileOnly, maxr);

    setIsIgnored(selfState);
    if (owner_)
        owner_->setIsIgnored(ownerState);

    return block_mask;
}

int WeaponInstance::getShots(int elapsed) {
    int time_for_shot = pWeaponClass_->timeForShot();
    int time_reload = pWeaponClass_->timeReload();
#if 0
    // TODO: if owner exists these two values should change(IPA, mods)
    if (owner_)
#endif
    int time_full_shot = time_for_shot + time_reload;
    if (elapsed == -1)
        elapsed = time_full_shot;
    if (weapon_used_time_ >= time_for_shot) {
        weapon_used_time_ += elapsed;
        if (weapon_used_time_ >= time_full_shot) {
            weapon_used_time_ -= time_full_shot;
        } else
            return false;
    } else
        weapon_used_time_ += elapsed;

    int shots = weapon_used_time_ / time_full_shot;
    weapon_used_time_ %= time_full_shot;
    if (weapon_used_time_ >= time_for_shot)
        shots++;
    return shots;
}

void WeaponInstance::getInRangeOne(toDefineXYZ & cp,
   ShootableMapObject * & target, uint8 mask, bool checkTileOnly, int maxr)
{
    // TODO: this function should do auto-targetting, check for hostile needed
    toDefineXYZ cpXYZ;
    cpXYZ.x = cp.x;
    cpXYZ.y = cp.y;
    cpXYZ.z = cp.z;
    bool ownerState, selfState;
    selfState = isIgnored();
    setIsIgnored(true);
    if (owner_) {
        ownerState = owner_->isIgnored();
        owner_->setIsIgnored(true);
        cpXYZ.z += (owner_->sizeZ() >> 1);
    } else {
        cpXYZ.z += 1;
    }
    
    g_Session.getMission()->getInRangeOne(&cpXYZ, target, mask,
        checkTileOnly, maxr);

    setIsIgnored(selfState);
    if (owner_)
        owner_->setIsIgnored(ownerState);
}

void WeaponInstance::getInRangeAll(toDefineXYZ & cp,
   std::vector<ShootableMapObject *> & targets, uint8 mask,
   bool checkTileOnly, int maxr)
{
    // NOTE: no need to ignore owner_ here, as all in range are vulnerable
    toDefineXYZ cpXYZ;
    cpXYZ.x = cp.x;
    cpXYZ.y = cp.y;
    cpXYZ.z = cp.z + 16;

    bool selfState;
    selfState = isIgnored();
    setIsIgnored(true);

    g_Session.getMission()->getInRangeAll(&cpXYZ, targets, mask,
        checkTileOnly, maxr);

    setIsIgnored(selfState);
}

bool WeaponInstance::isReloading() {
    int time_for_shot = pWeaponClass_->timeForShot();
    int time_reload = pWeaponClass_->timeReload();
    int time_full_shot = time_for_shot + time_reload;
    return (weapon_used_time_ > time_for_shot
        && weapon_used_time_ <= time_for_shot);
}

void WeaponInstance::activate() {
    if (pWeaponClass_->getWeaponType() == Weapon::TimeBomb)
        activated_ = true;
}

void WeaponInstance::deactivate() {
    activated_ = false;
    if (pWeaponClass_->getWeaponType() == Weapon::TimeBomb)
        weapon_used_time_ = 0;
}

void ShotClass::makeShot(bool rangeChecked, toDefineXYZ &cp, int anim_hit,
    std::vector <Weapon::ShotDesc> &all_shots, int anim_obj_hit,
    WeaponInstance * w)
{
    for (unsigned short i = 0; i < all_shots.size(); i++) {
        ShootableMapObject * smp = all_shots[i].smo;
        PathNode pn = all_shots[i].tpn;
        MapObject::DamageInflictType d;
        uint8 has_blocker = 0;
        if (rangeChecked)
            has_blocker = 1;
        else {
            assert(w != NULL);
            has_blocker = w->inRange(&smp, &pn, true);
        }
        d.ddir = -1;
        if (smp) {
            int txb = all_shots[i].tp.x;
            int tyb = all_shots[i].tp.y;
            smp->setDirection(txb - cp.x, tyb - cp.y, &(d.ddir));
        }

        d.dvalue = all_shots[i].d.dvalue;
        d.dtype = all_shots[i].d.dtype;
        if ((has_blocker & 6) != 0) {
            if ((has_blocker & 4) != 0) {
                if (anim_hit != SFXObject::sfxt_Unknown) {
                    SFXObject *so = new SFXObject(g_Session.getMission()->map(),
                        anim_hit);
                    so->setPosition(pn.tileX(), pn.tileY(), pn.tileZ(),
                        pn.offX(), pn.offY(), pn.offZ());
                    so->setTileVisZ();
                    so->correctZ();
                    g_Session.getMission()->addSfxObject(so);
                }
            } else if ((has_blocker & 2) != 0) {
                assert(smp != 0);
                if (anim_obj_hit!= SFXObject::sfxt_Unknown) {
                    if (smp->handleDamage(&d)) {
                    }
                    SFXObject *so = new SFXObject(g_Session.getMission()->map(),
                        anim_obj_hit);
                    so->setPosition(smp->tileX(), smp->tileY(), smp->tileZ() + 1,
                        smp->offX(), smp->offY(), smp->offZ());
                    so->setTileVisZ();
                    so->correctZ();
                    g_Session.getMission()->addSfxObject(so);
                }
            }
        } else if (has_blocker == 8) {
            if (anim_hit != SFXObject::sfxt_Unknown) {
                SFXObject *so = new SFXObject(g_Session.getMission()->map(),
                    anim_hit);
                so->setPosition(pn.tileX(), pn.tileY(), pn.tileZ(), pn.offX(),
                    pn.offY(), pn.offZ());
                so->setTileVisZ();
                so->correctZ();
                g_Session.getMission()->addSfxObject(so);
            }
        } else if (has_blocker == 1) {
            if (smp == NULL && anim_hit != SFXObject::sfxt_Unknown) {
                SFXObject *so = new SFXObject(g_Session.getMission()->map(),
                    anim_hit);
                so->setPosition(pn.tileX(), pn.tileY(), pn.tileZ(), pn.offX(),
                    pn.offY(), pn.offZ());
                so->setTileVisZ();
                so->correctZ();
                g_Session.getMission()->addSfxObject(so);
            }
            if (smp) {
                smp->handleDamage(&d);
                if (anim_obj_hit!= SFXObject::sfxt_Unknown) {
                    SFXObject *so = new SFXObject(g_Session.getMission()->map(),
                        anim_obj_hit);
                    so->setPosition(pn.tileX(), pn.tileY(), pn.tileZ() + 1,
                        pn.offX(), pn.offY(), pn.offZ());
                    so->setTileVisZ();
                    so->correctZ();
                    g_Session.getMission()->addSfxObject(so);
                }
            }
        }
    }
}
