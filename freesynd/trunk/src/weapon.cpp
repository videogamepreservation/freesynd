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
    unsigned int w_shot_property) {

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
    if (owner_) {
        if (pWeaponClass_->getWeaponType() == Weapon::EnergyShield) {
            int ammoused = getShots(elapsed, pWeaponClass_->timeForShot(),
                pWeaponClass_->timeReload()) * pWeaponClass_->ammoPerShot();
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
        } else if (pWeaponClass_->getWeaponType() == Weapon::MediKit) {
            if (owner_->health() != owner_->startHealth()) {
                owner_->setHealth(owner_->startHealth());
                ammo_remaining_ = 0;
                ((PedInstance *)owner_)->selectNextWeapon();
                weapon_used_time_ = 0;
            }
        }
    } else if (weapon_used_time_ != 0) {
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

void WeaponInstance::shotTargetRandomizer(PathNode * cp, PathNode * tp,
                                          double angle)
{
    if (angle == 0)
        return;
    int cx = cp->tileX() * 256 + cp->offX();
    int cy = cp->tileY() * 256 + cp->offY();
    int cz = cp->tileZ() * 128 + cp->offZ();

    int tx = tp->tileX() * 256 + tp->offX();
    int ty = tp->tileY() * 256 + tp->offY();
    int tz = tp->tileZ() * 128 + tp->offZ();
    double dtx = (double)(tx - cx);
    double dty = (double)(ty - cy);
    double dtz = (double)(tz - cz);
    double dist_cur = sqrt(dtx * dtx + dty * dty + dtz * dtz);
    if (dist_cur == 0)
        return;

    double PI = 3.14159265;
    double angx = 0;
    angle /= (180.0 / PI);
    angx = acos(dtx/dist_cur);
    double angy = 0;
    angy = acos(dty/dist_cur);
    double angz = 0;
    angz = acos(dtz/dist_cur);

    double set_sign = 1;
    if (rand() % 2 == 1)
        set_sign = -1;
    angx = angx + (angle / 2 * (double)(rand() % 100) / 100.0) * set_sign;
    int gtx = cx + (int)(cos(angx) * dist_cur);

    set_sign = 1;
    if (rand() % 2 == 1)
        set_sign = -1;
    angy = angy + (angle / 2 * (double)(rand() % 100) / 100.0) * set_sign;
    int gty = cy + (int)(cos(angy) * dist_cur);
    set_sign = 1;
    if (rand() % 2 == 1)
        set_sign = -1;
    angz = angz + (angle / 2 * (double)(rand() % 100) / 100.0) * set_sign;
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

    int max_x = g_App.getGameSession().getMission()->mmax_x_ * 256 - 1;
    int max_y = g_App.getGameSession().getMission()->mmax_y_ * 256 - 1;
    int max_z = g_App.getGameSession().getMission()->mmax_z_ * 128 - 1;
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

    tp->setTileXYZ(gtx / 256, gty / 256, gtz / 128);
    tp->setOffXYZ(gtx % 256, gty % 256, gtz % 128);
}

#if 0
class ProjectileShot {
public:
    ProjectileShot(PathNode &cp, PathNode &tp, MapObject::DamageType dt,
        int d_value, int d_range, ShootableMapObject * ignrd_obj = NULL);
    bool animate(int elapsed, Mission *m);
    bool lifeOver() { return life_over_; }

protected:
    toDefineXYZ cur_pos_;
    toDefineXYZ target_pos_;
    toDefineXYZ base_pos_;
    MapObject::DamageType dmg_type_;
    int dmg_value_;
    int dmg_range_;
    double dist_max_;
    double dist_passed_;
    toDefineXYZ last_anim_pos_;
    double cur_dist_;
    // per second, current 200
    double speed_;
    bool life_over_;
    // owner should be ignored
    ShootableMapObject * ignored_obj_;
    double inc_x_;
    double inc_y_;
    double inc_z_;
};

ProjectileShot::ProjectileShot(PathNode &cp, PathNode &tp, MapObject::DamageType dt,
        int d_value, int d_range, ShootableMapObject * ignrd_obj)
{
    cur_pos_.x = cp.tileX() * 256 + cp.offX();
    cur_pos_.y = cp.tileY() * 256 + cp.offY();
    cur_pos_.z = cp.tileZ() * 128 + cp.offZ();
    target_pos_.x = tp.tileX() * 256 + tp.offX();
    target_pos_.y = tp.tileY() * 256 + tp.offY();
    target_pos_.z = tp.tileZ() * 128 + tp.offZ();
    base_pos_ = cur_pos_;
    dmg_type_ = dt;
    dmg_value_ = d_value;
    dmg_range_ = d_range;
    dist_max_ = 10000;
    dist_passed_ = 0;
    speed_ = 4000;
    life_over_ = false;
    last_anim_pos_.x = -1;
    last_anim_pos_.y = -1;
    last_anim_pos_.z = -1;
    double diffx = (double)(cur_pos_.x - target_pos_.x);
    double diffy = (double)(cur_pos_.y - target_pos_.y);
    double diffz = (double)(cur_pos_.z - target_pos_.z);
    cur_dist_ = sqrt(diffx * diffx + diffy * diffy + diffz * diffz);
    if (cur_dist_ != 0) {
        inc_x_ = diffx / cur_dist_;
        inc_y_ = diffy / cur_dist_;
        inc_z_ = diffz / cur_dist_;
    }
    ignored_obj_ = ignrd_obj;
}

bool ProjectileShot::animate(int elapsed, Mission *m) {

    bool max_range = false;
    if (cur_dist_ != 0) {
        double inc_dist = speed_ * elapsed / 1000;
        if ((cur_dist_ + inc_dist) > dist_max_) {
            inc_dist = dist_max_ - cur_dist_;
            max_range = true;
        }
        bool ignored_state;

        if (ignored_obj_) {
            ignored_state = ignored_obj_->isIgnored();
            ignored_obj_->setIsIgnored(true);
        }

        target_pos_.x = (int)(cur_pos_.x + inc_x_ * speed_);
        if (target_pos_.x < 0) {
            target_pos_.x = 0;
            max_range = true;
        } else if (target_pos_.x >= m->mmax_x_ * 256) {
            target_pos_.x = m->mmax_x_ * 256 - 1;
            max_range = true;
        }
        target_pos_.y = (int)(cur_pos_.y + inc_y_ * speed_);
        if (target_pos_.y < 0) {
            target_pos_.y = 0;
            max_range = true;
        } else if (target_pos_.y >= m->mmax_y_ * 256) {
            target_pos_.y = m->mmax_y_ * 256 - 1;
            max_range = true;
        }
        target_pos_.z = (int)(cur_pos_.z + inc_z_ * speed_);
        if (target_pos_.z < 0) {
            target_pos_.z = 0;
            max_range = true;
        } else if (target_pos_.z >= m->mmax_z_ * 128) {
            target_pos_.z = m->mmax_z_ * 128 - 1;
            max_range = true;
        }

        if (ignored_obj_) {
            ignored_obj_->setIsIgnored(ignored_state);
        }
    }

    if (cur_dist_ == 0 && !max_range) {
    }
    return true;
}
#endif

bool WeaponInstance::inflictDamage(ShootableMapObject * tobj, PathNode * tp,
    int elapsed, bool ignoreBlocker)
{

    if (ammo_remaining_ == 0)
        return false;

    // TODO: if owner exists these two values should change(IPA, mods)
    int time_for_shot = pWeaponClass_->timeForShot();
    int time_reload = pWeaponClass_->timeReload();
    int shots = getShots(elapsed, time_reload, time_for_shot);

    if (pWeaponClass_->dmgType() == MapObject::dmg_None) {
        return false;
    } else if (pWeaponClass_->dmgType() == MapObject::dmg_Heal) {
        // NOTE: not only self-healing in future?
        return false;
    } else if (pWeaponClass_->dmgType() == MapObject::dmg_Mental) {
        return false;
    }

    ShootableMapObject * smp = tobj;
    PathNode pn, cp;
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
        cp.setTileXYZ(owner_->tileX(), owner_->tileY(), owner_->visZ());
        cp.setOffXYZ(owner_->offX(), owner_->offY(), owner_->offZ());
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
        cp.setTileXYZ(tile_x_, tile_y_, vis_z_);
        cp.setOffXYZ(off_x_, off_y_, off_z_);
    }
    if ((has_blocker & 6) != 0) {
        if (!ignoreBlocker)
            return false;
    } else if((has_blocker & 1) == 0)
        if (!ignoreBlocker)
            return false;

    this->playSound();
    DamageInflictType d;
    // 90% accuracy
    double accuracy = 0.9;

    // angle is used to generate shot with randomizer
    // TODO: add angle per weapon(precision is higher for smaller angle)
    double angle = 30;

    angle = angle * (1 - accuracy);

    ShotDesc base_shot;
    base_shot.smo = NULL;
    base_shot.d.dtype = pWeaponClass_->dmgType();
    base_shot.d.dvalue =  pWeaponClass_->damagePerShot();
    if (tobj) {
        base_shot.tp.setTileXYZ(tobj->tileX(), tobj->tileY(),
            tobj->visZ() + 1);
        base_shot.tp.setOffXYZ(tobj->offX(), tobj->offY(), tobj->offZ());
    } else {
        base_shot.tp.setTileXYZ(tp->tileX(), tp->tileY(),
            tp->tileZ());
        base_shot.tp.setOffXYZ(tp->offX(), tp->offY(), tp->offZ());
    }
    unsigned int shot_prop = pWeaponClass_->shotProperty();
    std::vector <ShotDesc> all_shots;
    all_shots.reserve(20);

    // TODO: define this somewhere
    int mask = MapObject::mt_Ped | MapObject::mt_Vehicle
        | MapObject::mt_Static | MapObject::mt_Weapon;
    if (shot_prop & Weapon::spe_CreatesProjectile) {
        // TODO: create projectile, 1 - spe_PointToPoint,
        // many - spe_PointToManyPoints
        return false;
    }

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

    if (shot_prop & Weapon::spe_NoTarget) {
        std::vector <ShotDesc> gen_shots;
        if (shot_prop & Weapon::spe_PointToPoint) {
        } else if (shot_prop & Weapon::spe_PointToManyPoints) {
        } else {
            base_shot.tp = cp;
            gen_shots.push_back(base_shot);
        }
        if (shot_prop & Weapon::spe_RangeDamageOnReach) {
            for (unsigned int i = 0; i < gen_shots.size(); i++) {
                std::vector<ShootableMapObject *> all_targets;

                getInRangeAll(gen_shots[i].tp, all_targets, mask,
                    true, 512);
                for (unsigned int indx = 0; indx < all_targets.size();
                    indx++)
                {
                    ShootableMapObject * smo = all_targets[indx];
                    ShotDesc sd;
                    sd.tp = PathNode(smo->tileX(), smo->tileY(),
                        smo->tileZ(), smo->offX(), smo->offY(),
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
        std::vector <ShotDesc> gen_shots;
        switch((shot_prop & (Weapon::spe_PointToPoint
            | Weapon::spe_PointToManyPoints)))
        {
            case Weapon::spe_PointToPoint:
                for (int i = 0; i < ammoused; i++) {
                    gen_shots.push_back(base_shot);
                    shotTargetRandomizer(&cp, &(gen_shots.back().tp), angle);
                }
                if (shot_prop & Weapon::spe_RangeDamageOnReach) {
                    for (unsigned int i = 0; i < gen_shots.size(); i++) {
                        std::vector<ShootableMapObject *> all_targets;

                        getInRangeAll(gen_shots[i].tp, all_targets, mask,
                            true, 512);
                        for (unsigned int indx = 0; indx < all_targets.size();
                            indx++)
                        {
                            ShootableMapObject * smo = all_targets[indx];
                            ShotDesc sd;
                            sd.tp = PathNode(smo->tileX(), smo->tileY(),
                                smo->tileZ(), smo->offX(), smo->offY(),
                                smo->offZ());
                            sd.d = gen_shots[i].d;
                            sd.smo = smo;
                            all_shots.push_back(sd);
                        }
                    }
                } else {
                    all_shots = gen_shots;
                }
                break;
            case Weapon::spe_PointToManyPoints:
                for (unsigned short i = 0; i < ammoused; i++) {
                    gen_shots.push_back(base_shot);
                    shotTargetRandomizer(&cp, &(gen_shots.back().tp), angle);
                }
                if (shot_prop & Weapon::spe_RangeDamageOnReach) {
                } else {
                    all_shots = gen_shots;
                }
                break;
        }
    }

    for (unsigned short i = 0; i < all_shots.size(); i++) {
        smp = all_shots[i].smo;
        pn = all_shots[i].tp;
        if (shot_prop & Weapon::spe_RangeDamageOnReach)
            has_blocker = 1;
        else
            has_blocker = inRange(&smp, &pn, true);
        d.ddir = -1;
        txb = pn.tileX() * 256 + pn.offX();
        tyb = pn.tileY() * 256 + pn.offY();
        setDirection(txb - xb, tyb - yb, &(d.ddir));

        d.dvalue = all_shots[i].d.dvalue;
        d.dtype = all_shots[i].d.dtype;
        if ((has_blocker & 6) != 0) {
            if ((has_blocker & 4) != 0) {
                SFXObject *so = new SFXObject(g_Session.getMission()->map(),
                    SFXObject::sfxt_BulletHit);
                so->setPosition(pn.tileX(), pn.tileY(), pn.tileZ(), pn.offX(),
                    pn.offY(), pn.offZ());
                so->setVisZ(pn.tileZ());
                g_Session.getMission()->addSfxObject(so);
            } else if ((has_blocker & 2) != 0) {
                if (smp) {
                    if (smp->handleDamage(&d)) {
                    }
                    SFXObject *so = new SFXObject(g_Session.getMission()->map(),
                        SFXObject::sfxt_BulletHit);
                    so->setPosition(smp->tileX(), smp->tileY(), smp->tileZ() + 1,
                        smp->offX(), smp->offY(), smp->offZ());
                    so->setVisZ(smp->visZ() + 1);
                    g_Session.getMission()->addSfxObject(so);
                }
            }
        } else if (has_blocker == 8) {
            SFXObject *so = new SFXObject(g_Session.getMission()->map(),
                SFXObject::sfxt_BulletHit);
            so->setPosition(pn.tileX(), pn.tileY(), pn.tileZ(), pn.offX(),
                pn.offY(), pn.offZ());
            so->setVisZ(pn.tileZ());
            g_Session.getMission()->addSfxObject(so);
        } else if (has_blocker == 1) {
            if (smp)
                smp->handleDamage(&d);
            SFXObject *so = new SFXObject(g_Session.getMission()->map(),
                SFXObject::sfxt_BulletHit);
            so->setPosition(pn.tileX(), pn.tileY(), pn.tileZ(), pn.offX(),
                pn.offY(), pn.offZ());
            so->setVisZ(pn.tileZ());
            g_Session.getMission()->addSfxObject(so);
        }
    }
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
    bool ownerState;
    toDefineXYZ cxyz;

    if (owner_) {
        ownerState = owner_->isIgnored();
        owner_->setIsIgnored(true);
        cxyz.x = owner_->tileX() * 256 + owner_->offX();
        cxyz.y = owner_->tileY() * 256 + owner_->offY();
        cxyz.z = owner_->visZ() * 128 + owner_->offZ() + 128;
    } else {
        cxyz.x = tile_x_ * 256 + off_x_;
        cxyz.y = tile_y_ * 256 + off_y_;
        cxyz.z = vis_z_ * 128 + off_z_ + 1;
    }

    uint8 block_mask = g_App.getGameSession().getMission()->inRangeCPos(
        &cxyz, t, pn, setBlocker, checkTileOnly, maxr);
    if (owner_)
        owner_->setIsIgnored(ownerState);

    return block_mask;
}

int WeaponInstance::getShots(int elapsed, int tForReload, int tForShot) {
    int time_full_shot = tForShot + tForReload;
    if (elapsed == -1)
        elapsed = time_full_shot;
    if (weapon_used_time_ >= tForShot) {
        weapon_used_time_ += elapsed;
        if (weapon_used_time_ >= time_full_shot) {
            weapon_used_time_ -= time_full_shot;
        } else
            return false;
    } else
        weapon_used_time_ += elapsed;

    int shots = weapon_used_time_ / time_full_shot;
    weapon_used_time_ %= time_full_shot;
    if (weapon_used_time_ >= tForShot)
        shots++;
    return shots;
}

void WeaponInstance::getInRangeOne(PathNode & cp,
   ShootableMapObject * & target, uint8 mask, bool checkTileOnly, int maxr)
{
    // TODO: this function should do auto-targetting, check for hostile needed
    Mission *m = g_App.getGameSession().getMission();
    bool found = false;
    double dist = -1;
    double d = -1;;
    toDefineXYZ cpXYZ;
    cpXYZ.x = cp.tileX() * 256 + cp.offX();
    cpXYZ.y = cp.tileY() * 256 + cp.offY();
    cpXYZ.z = cp.tileZ() * 128 + cp.offZ();
    bool ownerState;
    if (owner_) {
        ownerState = owner_->isIgnored();
        owner_->setIsIgnored(true);
        cpXYZ.z += 128;
    } else {
        cpXYZ.z += 1;
    }

    if (mask & MapObject::mt_Ped) {
        for (int i = 0; i < m->numPeds(); i++) {
            ShootableMapObject *ped = m->ped(i);
            if (!ped->isIgnored())
                if (m->inRangeCPos(&cpXYZ, &ped, NULL, false, checkTileOnly, maxr,
                    &d) == 1)
                {
                    if (found) {
                        if (d < dist) {
                            target = ped;
                            dist = d;
                        }
                    } else {
                        dist = d;
                        target = ped;
                        found = true;
                    }
                }
        }
    }
    if (mask & MapObject::mt_Static) {
        for (int i = 0; i < m->numStatics(); i++) {
            ShootableMapObject *st = m->statics(i);
            if (!st->isIgnored())
                if (m->inRangeCPos(&cpXYZ, &st, NULL, false, checkTileOnly, maxr,
                    &d) == 1)
                {
                    if (found) {
                        if (d < dist) {
                            target = st;
                            dist = d;
                        }
                    } else {
                        dist = d;
                        target = st;
                        found = true;
                    }
                }
        }
    }
    if (mask & MapObject::mt_Vehicle) {
        for (int i = 0; i < m->numVehicles(); i++) {
            ShootableMapObject *v = m->vehicle(i);
            if (!v->isIgnored())
                if (m->inRangeCPos(&cpXYZ, &v, NULL, false, checkTileOnly, maxr,
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
    if (mask & MapObject::mt_Weapon) {
        for (int i = 0; i < m->numWeapons(); i++) {
            ShootableMapObject *w = m->weapon(i);
            if (!w->isIgnored())
                if (m->inRangeCPos(&cpXYZ, &w, NULL, false, checkTileOnly, maxr,
                    &d) == 1)
                {
                    if (found) {
                        if (d < dist) {
                            target = w;
                            dist = d;
                        }
                    } else {
                        dist = d;
                        target = w;
                        found = true;
                    }
                }
        }
    }
    if (owner_)
        owner_->setIsIgnored(ownerState);
}

void WeaponInstance::getInRangeAll(PathNode & cp,
   std::vector<ShootableMapObject *> & targets, uint8 mask,
   bool checkTileOnly, int maxr)
{
    // NOTE: no need to ignore owner_ here, as all in range are vulnerable
    toDefineXYZ cpXYZ;
    cpXYZ.x = cp.tileX() * 256 + cp.offX();
    cpXYZ.y = cp.tileY() * 256 + cp.offY();
    cpXYZ.z = cp.tileZ() * 128 + cp.offZ() + 1;
    Mission *m = g_App.getGameSession().getMission();

    if (mask & MapObject::mt_Ped) {
        for (int i = 0; i < m->numPeds(); i++) {
            ShootableMapObject *ped = m->ped(i);
            if (!ped->isIgnored())
                if (m->inRangeCPos(&cpXYZ, &ped, NULL, false, checkTileOnly,
                    maxr) == 1)
                {
                    targets.push_back(ped);
                }
        }
    }
    if (mask & MapObject::mt_Static) {
        for (int i = 0; i < m->numStatics(); i++) {
            ShootableMapObject *st = m->statics(i);
            if (!st->isIgnored())
                if (m->inRangeCPos(&cpXYZ, &st, NULL, false, checkTileOnly,
                    maxr) == 1)
                {
                    targets.push_back(st);
                }
        }
    }
    if (mask & MapObject::mt_Vehicle) {
        for (int i = 0; i < m->numVehicles(); i++) {
            ShootableMapObject *v = m->vehicle(i);
            if (!v->isIgnored())
                if (m->inRangeCPos(&cpXYZ, &v, NULL, false, checkTileOnly,
                    maxr) == 1)
                {
                    targets.push_back(v);
                }
        }
    }
    if (mask & MapObject::mt_Weapon) {
        for (int i = 0; i < m->numWeapons(); i++) {
            ShootableMapObject *w = m->weapon(i);
            if (!w->isIgnored())
                if (m->inRangeCPos(&cpXYZ, &w, NULL, false, checkTileOnly,
                    maxr) == 1)
                {
                    targets.push_back(w);
                }
        }
    }
}
