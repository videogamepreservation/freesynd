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
#include "utils/log.h"
#include "weapon.h"
#include "ped.h"
#include "vehicle.h"
#include "mission.h"
#include "model/shot.h"

#define Z_SHIFT_TO_AIR   4

Weapon::Weapon(WeaponType w_type, ConfigFile &conf)
{
    type_ = w_type;
    submittedToSearch_ = false;

    switch(w_type) {
        case Weapon::Pistol:
            idx_ = Weapon::Pistol_Anim;
            sample_ = snd::PISTOL;
            dmg_type_ = MapObject::dmg_Bullet;
            shot_property_ = Weapon::wspt_Pistol;
            impactAnims_.groundHit = SFXObject::sfxt_BulletHit;
            impactAnims_.objectHit = SFXObject::sfxt_BulletHit;
            impactAnims_.trace_anim = SFXObject::sfxt_Unknown;
            impactAnims_.rd_anim = SFXObject::sfxt_Unknown; 
            break;
        case Weapon::Minigun:
            idx_ = Weapon::Minigun_Anim;
            sample_ = snd::MINIGUN;
            dmg_type_ = MapObject::dmg_Bullet;
            shot_property_ = Weapon::wspt_Minigun;
            impactAnims_.groundHit = SFXObject::sfxt_BulletHit;
            impactAnims_.objectHit = SFXObject::sfxt_BulletHit;
            impactAnims_.trace_anim = SFXObject::sfxt_Unknown;
            impactAnims_.rd_anim = SFXObject::sfxt_Unknown; 
            break;
        case Weapon::Flamer:
            idx_ = Weapon::Flamer_Anim;
            sample_ = snd::FLAME;
            dmg_type_ = MapObject::dmg_Burn;
            shot_property_ = Weapon::wspt_Flamer;
            impactAnims_.groundHit = SFXObject::sfxt_FlamerFire;
            impactAnims_.objectHit = SFXObject::sfxt_FlamerFire;
            impactAnims_.trace_anim = SFXObject::sfxt_FlamerFire;
            impactAnims_.rd_anim = SFXObject::sfxt_Unknown; 
            break;
        case Weapon::LongRange:
            idx_ = Weapon::LongRange_Anim;
            sample_ = snd::LONGRANGE;
            dmg_type_ = MapObject::dmg_Bullet;
            shot_property_ = Weapon::wspt_LongRange;
            impactAnims_.groundHit = SFXObject::sfxt_BulletHit;
            impactAnims_.objectHit = SFXObject::sfxt_BulletHit;
            impactAnims_.trace_anim = SFXObject::sfxt_Unknown;
            impactAnims_.rd_anim = SFXObject::sfxt_Unknown; 
            break;
        case Weapon::EnergyShield:
            idx_ = Weapon::EnergyShield_Anim;
            sample_ = snd::NO_SOUND;
            dmg_type_ = MapObject::dmg_None;
            shot_property_ = Weapon::wspt_EnergyShield;
            impactAnims_.groundHit = SFXObject::sfxt_Unknown;
            impactAnims_.objectHit = SFXObject::sfxt_Unknown;
            impactAnims_.trace_anim = SFXObject::sfxt_Unknown;
            impactAnims_.rd_anim = SFXObject::sfxt_Unknown; 
            break;
        case Weapon::Uzi:
            idx_ = Weapon::Uzi_Anim;
            sample_ = snd::UZI;
            dmg_type_ = MapObject::dmg_Bullet;
            shot_property_ = Weapon::wspt_Uzi;
            impactAnims_.groundHit = SFXObject::sfxt_BulletHit;
            impactAnims_.objectHit = SFXObject::sfxt_BulletHit;
            impactAnims_.trace_anim = SFXObject::sfxt_Unknown;
            impactAnims_.rd_anim = SFXObject::sfxt_Unknown; 
            break;
        case Weapon::Laser:
            idx_ = Weapon::Laser_Anim;
            sample_ = snd::LASER;
            dmg_type_ = MapObject::dmg_Laser;
            shot_property_ = Weapon::wspt_Laser;
            impactAnims_.groundHit = SFXObject::sfxt_Fire_LongSmoke;
            impactAnims_.objectHit = SFXObject::sfxt_Unknown;
            impactAnims_.trace_anim = SFXObject::sfxt_Unknown;
            impactAnims_.rd_anim = SFXObject::sfxt_Unknown; 
            break;
        case Weapon::GaussGun:
            idx_ = Weapon::Gauss_Anim;
            sample_ = snd::GAUSSGUN;
            dmg_type_ = MapObject::dmg_Explosion;
            shot_property_ = Weapon::wspt_GaussGun;
            impactAnims_.groundHit = SFXObject::sfxt_ExplosionFire;
            impactAnims_.objectHit = SFXObject::sfxt_ExplosionBall;
            impactAnims_.trace_anim = SFXObject::sfxt_Smoke;
            impactAnims_.rd_anim = SFXObject::sfxt_LargeFire; 
            break;
        case Weapon::Shotgun:
            idx_ = Weapon::Shotgun_Anim;
            sample_ = snd::SHOTGUN;
            dmg_type_ = MapObject::dmg_Bullet;
            shot_property_ = Weapon::wspt_Shotgun;
            impactAnims_.groundHit = SFXObject::sfxt_BulletHit;
            impactAnims_.objectHit = SFXObject::sfxt_BulletHit;
            impactAnims_.trace_anim = SFXObject::sfxt_Unknown;
            impactAnims_.rd_anim = SFXObject::sfxt_Unknown; 
            break;
        case Weapon::MediKit:
            idx_ = Weapon::Unarmed_Anim;
            sample_ = snd::NO_SOUND;
            dmg_type_ = MapObject::dmg_Heal;
            shot_property_ = Weapon::wspt_MediKit;
            impactAnims_.groundHit = SFXObject::sfxt_Unknown;
            impactAnims_.objectHit = SFXObject::sfxt_Unknown;
            impactAnims_.trace_anim = SFXObject::sfxt_Unknown;
            impactAnims_.rd_anim = SFXObject::sfxt_Unknown; 
            break;
        case Weapon::Scanner:
            idx_ = Weapon::Unarmed_Anim;
            sample_ = snd::NO_SOUND;
            dmg_type_ = MapObject::dmg_None;
            shot_property_ = Weapon::wspt_Scanner;
            impactAnims_.groundHit = SFXObject::sfxt_Unknown;
            impactAnims_.objectHit = SFXObject::sfxt_Unknown;
            impactAnims_.trace_anim = SFXObject::sfxt_Unknown;
            impactAnims_.rd_anim = SFXObject::sfxt_Unknown; 
            break;
        case Weapon::AccessCard:
            idx_ = Weapon::Unarmed_Anim;
            sample_ = snd::NO_SOUND;
            dmg_type_ = MapObject::dmg_None;
            shot_property_ = Weapon::wspt_AccessCard;
            impactAnims_.groundHit = SFXObject::sfxt_Unknown;
            impactAnims_.objectHit = SFXObject::sfxt_Unknown;
            impactAnims_.trace_anim = SFXObject::sfxt_Unknown;
            impactAnims_.rd_anim = SFXObject::sfxt_Unknown; 
            break;
        case Weapon::TimeBomb:
            idx_ = Weapon::Unarmed_Anim;
            sample_ = snd::EXPLOSION;
            dmg_type_ = MapObject::dmg_Explosion;
            shot_property_ = Weapon::wspt_TimeBomb;
            impactAnims_.groundHit = SFXObject::sfxt_ExplosionFire;
            impactAnims_.objectHit = SFXObject::sfxt_ExplosionBall;
            impactAnims_.trace_anim = SFXObject::sfxt_Unknown;
            impactAnims_.rd_anim = SFXObject::sfxt_ExplosionFire; 
            break;
        case Weapon::Persuadatron:
            idx_ = Weapon::Unarmed_Anim;
            sample_ = snd::PERSUADE;
            dmg_type_ = MapObject::dmg_Persuasion;
            shot_property_ = Weapon::wspt_Persuadatron;
            impactAnims_.groundHit = SFXObject::sfxt_Unknown;
            impactAnims_.objectHit = SFXObject::sfxt_Unknown;
            impactAnims_.trace_anim = SFXObject::sfxt_Unknown;
            impactAnims_.rd_anim = SFXObject::sfxt_Unknown; 
            break;
        default:
#if _DEBUG
            printf("unknown weapon loaded(%i), NULL passed", w_type);
#endif
            break;
    }

    // initialize other properties
    initFromConfig(w_type, conf);
}

void Weapon::initFromConfig(WeaponType w_type, ConfigFile &conf) {
    const char *pattern = "weapon.%d.%s";
    char propName[25];

    try {
        sprintf(propName, pattern, w_type, "name");
        name_ = g_Ctx.getMessage(conf.read<std::string>(propName));

        sprintf(propName, pattern, w_type, "icon.small");
        small_icon_ = conf.read<int>(propName);
        sprintf(propName, pattern, w_type, "icon.big");
        big_icon_ = conf.read<int>(propName);
        sprintf(propName, pattern, w_type, "cost");
        cost_ = conf.read<int>(propName);
        sprintf(propName, pattern, w_type, "ammo.nb");
        ammo_ = conf.read<int>(propName);
        sprintf(propName, pattern, w_type, "ammo.price");
        ammo_cost_ = conf.read<int>(propName);
        sprintf(propName, pattern, w_type, "range");
        range_ = conf.read<int>(propName);
        sprintf(propName, pattern, w_type, "rank");
        rank_ = conf.read<int>(propName);
        sprintf(propName, pattern, w_type, "anim");
        anim_ = conf.read<int>(propName);
        sprintf(propName, pattern, w_type, "ammopershot");
        ammo_per_shot_ = conf.read<int>(propName);
        sprintf(propName, pattern, w_type, "timeforshot");
        time_for_shot_ = conf.read<int>(propName);
        sprintf(propName, pattern, w_type, "timereload");
        time_reload_ = conf.read<int>(propName);
        sprintf(propName, pattern, w_type, "damagerange");
        range_dmg_ = conf.read<int>(propName);
        sprintf(propName, pattern, w_type, "shotangle");
        shot_angle_ = conf.read<double>(propName);
        sprintf(propName, pattern, w_type, "shotaccuracy");
        shot_accuracy_ = conf.read<double>(propName);
        sprintf(propName, pattern, w_type, "shotspeed");
        shot_speed_ = conf.read<int>(propName);
        sprintf(propName, pattern, w_type, "dmg_per_shot");
        dmg_per_shot_ = conf.read<int>(propName);
        sprintf(propName, pattern, w_type, "shots_per_ammo");
        shots_per_ammo_ = conf.read<int>(propName);
        sprintf(propName, pattern, w_type, "weight");
        weight_ = conf.read<int>(propName);
        sprintf(propName, pattern, w_type, "auto.fire_rate");
        fireRate_ = conf.read<int>(propName, 0);
    } catch (...) {
        FSERR(Log::k_FLG_GAME, "Weapon", "initFromConfig", ("Cannot load weapon %d : %s\n", w_type, propName))
    }
}

/*!
 * Creates a new instance of Weapon instance for the given weapon class.
 * \param pWeaponClass Class of weapon
 * \return an instance of WeaponInstance
 */
WeaponInstance *WeaponInstance::createInstance(Weapon *pWeaponClass) {
    return new WeaponInstance(pWeaponClass);
}

WeaponInstance::WeaponInstance(Weapon * w) : ShootableMapObject(-1),
    bombSoundTimer(w->timeReload()), bombExplosionTimer(w->timeForShot()),
    flamerTimer_(180) {
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
    pFlamerShot_ = NULL;
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
        } else if (main_type_ == Weapon::TimeBomb) {
            if (bombSoundTimer.update(elapsed)) {
                g_App.gameSounds().play(snd::TIMEBOMB);
            }
            
            is_ignored_ = true;
            if (bombExplosionTimer.update(elapsed)) {
                ShootableMapObject::DamageInflictType dmg;
                fire(g_Session.getMission(), dmg, elapsed);
                return true;
            }
            is_ignored_ = false;
            time_consumed_ = true;
        }

        updtWeaponUsedTime(elapsed);
    } else if (weapon_used_time_ != 0) {
        updtWeaponUsedTime(elapsed);
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
    double dist_cur = 0.0;
    if (dist_new != -1.0)
        dist_cur = dist_new;
    else
        dist_cur = sqrt(dtx * dtx + dty * dty + dtz * dtz);
    if (dist_cur == 0.0)
        return;

    angle /= (180.0 / PI);
    double angx = acos(dtx/dist_cur);
    double angy = acos(dty/dist_cur);
    double angz = acos(dtz/dist_cur);

    double set_sign = 1.0;
    if (rand() % 100 < 50)
        set_sign = -1.0;
    double diff_ang = (angle * (double)(rand() % 100) / 200.0) * set_sign;
    angx += diff_ang;
    angle -= fabs(diff_ang);
    int gtx = cx + (int)(cos(angx) * dist_cur);

    set_sign = 1.0;
    if (rand() % 100 < 50)
        set_sign = -1.0;
    diff_ang = (angle * (double)(rand() % 100) / 200.0) * set_sign;
    angy += diff_ang;
    angle -= fabs(diff_ang);
    int gty = cy + (int)(cos(angy) * dist_cur);
    if (!exclude_z) {
        set_sign = 1.0;
        if (rand() % 100 < 50)
            set_sign = -1.0;
        angz += (angle * (double)(rand() % 100) / 200.0) * set_sign;
    }
    int gtz = cz + (int)(cos(angz) * dist_cur);

    if (gtx < 0) {
        if (cos(angx) == 0.0) {
            gtx = 0;
        } else {
            dist_cur -= fabs((double)gtx / cos(angx));
            gtx = 0;
            gty = cy + (int)(cos(angy) * dist_cur);
            gtz = cz + (int)(cos(angz) * dist_cur);
        }
    }
    if (gty < 0) {
        if (cos(angy) == 0.0) {
            gty = 0;
        } else {
            dist_cur -= fabs((double)gty / cos(angy));
            gty = 0;
            gtx = cx + (int)(cos(angx) * dist_cur);
            gtz = cz + (int)(cos(angz) * dist_cur);
        }
    }
    if (gtz < 0) {
        if (cos(angz) == 0.0) {
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
        if (cos(angx) == 0.0) {
            gtx = max_x;
        } else {
            dist_cur -= fabs((double)(gtx - max_x) / cos(angx));
            gtx = max_x;
            gty = cy + (int)(cos(angy) * dist_cur);
            gtz = cz + (int)(cos(angz) * dist_cur);
        }
    }
    if (gty > max_y) {
        if (cos(angy) == 0.0) {
            gty = max_y;
        } else {
            dist_cur -= fabs((double)(gty - max_y) / cos(angy));
            gty = max_y;
            gtx = cx + (int)(cos(angx) * dist_cur);
            gtz = cz + (int)(cos(angz) * dist_cur);
        }
    }
    if (gtz > max_z) {
        if (cos(angx) == 0.0) {
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
    //TODO: check tobj completed action

    // for weapons that are not using ammo, ammo_remaining_ = -1
    if (ammo_remaining_ == 0) {
        if (make_shots)
            *make_shots = 0;
        return 1;
    }

    // NOTE: this block of code defines direction for shooter and damaged person
    // it is placed before shots calculation to set direction for shooter even
    // if there is not enough time for shot
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
        cp.z = owner_->tileZ() * 128 + owner_->offZ()
            + (owner_->sizeZ() >> 1);
        if (cp.z > (m->mmax_z_ - 1) * 128)
            return 128;
        if (m->isTileSolid(cp.x / 256, cp.y / 256, cp.z / 128, cp.x % 256,
            cp.y % 256, cp.z % 128))
        {
            return 256;
        }
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
        // if off_z_ != 0 tile_z_ = tile_z_ + Z_SHIFT_TO_AIR, from this assumed that
        // tile above is "air", if Z_SHIFT_TO_AIR will not be added, tile will be "solid",
        // trajectory checking will return "failed" response at start
        cp.z = tile_z_ * 128 + off_z_ + Z_SHIFT_TO_AIR;
        if (cp.z > (m->mmax_z_ - 1) * 128)
            cp.z = (m->mmax_z_ - 1) * 128;
    }

    unsigned int shot_prop = shotProperty();

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
        has_blocker = checkRangeAndBlocker(cp, &smp, &pn);
    if ((has_blocker & 62) != 0) {
        if (!ignoreBlocker) {
            if (make_shots)
                *make_shots = 0;
            if ((has_blocker & 6) != 0)
                has_blocker = 16;
            else if ((has_blocker & 16) != 0)
                has_blocker = 32;
            else if ((has_blocker & 8) != 0)
                has_blocker = 64;
            // if has_blocker == 32
            return 64;
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


    // TODO: fix this, remove auto targetting for perseudatron,
    // use find non friend in ped animate
    // perseudatron skipping, due to no target

    double accuracy = pWeaponClass_->shotAcurracy();
    if (main_type_ != Weapon::Persuadatron)
        this->playSound();
    if (owner_ && owner_->majorType() == MapObject::mjt_Ped)
    {
        if (pWeaponClass_->dmgType() != MapObject::dmg_Persuasion
            && ((PedInstance *)owner_)->isOurAgent())
        {
            m->incStatisticsShots((int32)shots);
        }
        ((PedInstance *)owner_)->getAccuracy(accuracy);
    }

    // angle is used to generate random shot with randomizer
    double angle = pWeaponClass_->shotAngle();

    angle *= accuracy;

    Weapon::ShotDesc base_shot;
    base_shot.smo = NULL;
    base_shot.d.dtype = pWeaponClass_->dmgType();
    base_shot.d.dvalue =  pWeaponClass_->damagePerShot();
    base_shot.d.d_owner = owner_;
    base_shot.target_object = tobj;
    if (tobj) {
        base_shot.tp.x = tobj->tileX() * 256 + tobj->offX();
        base_shot.tp.y = tobj->tileY() * 256 + tobj->offY();
        base_shot.tp.z = tobj->tileZ() * 128 + tobj->offZ() + (tobj->sizeZ() >> 1);
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

        if (shot_prop & Weapon::spe_ShootsWhileNoTarget) {
            std::vector <Weapon::ShotDesc> gen_shots;
            if (shot_prop & Weapon::spe_PointToPoint) {/*
                if (!tobj) {
                    getNonFriendInRange(&cp, smp, false);
                    if (smp) {
                        gen_shots.push_back(base_shot);
                        gen_shots.back().smo = smp;
                        this->playSound();
                    }
                }*/
                if (tobj && tobj->majorType() == MapObject::mjt_Ped) {
                    if (((PedInstance *)owner_)->isFriendWith(
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
                for (unsigned int i = 0; i < gen_shots.size(); ++i) {
                    std::vector<ShootableMapObject *> all_targets;

                    Weapon::ShotDesc &sdc = gen_shots[i];
                    checkRangeAndBlocker(cp, &sdc.smo, &sdc.tpn, true);
                    sdc.tpn.convertPosToXYZ(&sdc.tp);
                    getInRangeAll(sdc.tp, all_targets, mask,
                        true, pWeaponClass_->rangeDmg());
                    for (unsigned int indx = 0; indx < all_targets.size();
                        ++indx)
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
            if (shot_prop & Weapon::spe_PointToManyPoints)
                ammoused *= getWeaponClass()->shotsPerAmmo();
            for (int i = 0; i < ammoused; ++i) {
                gen_shots.push_back(base_shot);
                Weapon::ShotDesc &last_one = gen_shots.back();
                shotTargetRandomizer(&cp, &(last_one.tp), angle);
                last_one.tpn.setTileXYZ(last_one.tp.x / 256,
                    last_one.tp.y / 256, last_one.tp.z / 128);
                last_one.tpn.setOffXYZ(last_one.tp.x % 256,
                    last_one.tp.y % 256, last_one.tp.z % 128);
            }
            if (range_damage) {
                for (unsigned int i = 0; i < gen_shots.size(); ++i) {
                    std::vector<ShootableMapObject *> all_targets;

                    Weapon::ShotDesc &sdc = gen_shots[i];
                    checkRangeAndBlocker(cp, &sdc.smo, &sdc.tpn, true);
                    sdc.tpn.convertPosToXYZ(&sdc.tp);
                    getInRangeAll(sdc.tp, all_targets, mask,
                        true, pWeaponClass_->rangeDmg());
                    for (unsigned int indx = 0; indx < all_targets.size();
                        ++indx)
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
        }
        makeShot(range_damage, cp, pWeaponClass_->impactAnims()->groundHit, all_shots,
            pWeaponClass_->impactAnims()->objectHit, this);
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

uint8 WeaponInstance::checkRangeAndBlocker(toDefineXYZ & cp, ShootableMapObject ** t,
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
        cxyz.z = owner_->tileZ() * 128 + owner_->offZ() + (owner_->sizeZ() >> 1);
    } else {
        cxyz.x = tile_x_ * 256 + off_x_;
        cxyz.y = tile_y_ * 256 + off_y_;
        cxyz.z = tile_z_ * 128 + off_z_ + Z_SHIFT_TO_AIR;
    }

    return checkRangeAndBlocker(cxyz, t, pn, setBlocker, checkTileOnly, maxr);
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
    if (shotProperty() & Weapon::spe_UsesAmmo) {
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

void WeaponInstance::activate() {
    activated_ = true;
}

void WeaponInstance::deactivate() {
    activated_ = false;
    if (main_type_ == Weapon::TimeBomb)
        weapon_used_time_ = 0;
}

/*!
 * Use weapon and decrease ammo.
 * This method is used only for shooting weapons and Medikit.
 * \param pMission Mission data
 * \param dmg Information on the damage to perform
 * \param elapsed Time since last frame
 */
void WeaponInstance::fire(Mission *pMission, ShootableMapObject::DamageInflictType &dmg, int elapsed) {
    if (getWeaponType() == Weapon::MediKit) {
        dmg.d_owner->resetHealth();
    } else if (getWeaponType() == Weapon::GaussGun) {
        GaussGunShot *pShot = new GaussGunShot(dmg);
        pMission->addPrjShot(pShot);
    } else if (getWeaponType() == Weapon::Flamer) {
        // when targeting a point with the flamer, the point of impact
        // circles around the target.
        // We use the weapon's direction field to store a logical direction
        // which will give the current moving point of impact
        toDefineXYZ xyz;
        dmg.aimedLoc.convertPosToXYZ(&xyz);
        
        switch(direction()) {
        case 0:
            xyz.y += 160;
            break;
        case 1:
            xyz.x += 96;
            xyz.y += 96;
            break;
        case 2:
            xyz.x += 160;
            break;
        case 3:
            xyz.x += 96;
            xyz.y -= 96;
            break;
        case 4:
            xyz.y -= 160;
            break;
        case 5:
            xyz.x -= 96;
            xyz.y -= 96;
            break;
        case 6:
            xyz.x -= 160;
            break;
        case 7:
            xyz.x -= 96;
            xyz.y += 96;
            break;
        }

        dmg.aimedLoc.setTileX(xyz.x / 256);
        dmg.aimedLoc.setTileY(xyz.y / 256);
        dmg.aimedLoc.setOffXY(xyz.x % 256, xyz.y % 256);

        FlamerShot *pFlamerShot = new FlamerShot(dmg);
        pMission->addPrjShot(pFlamerShot);

        // Change direction for next time
        if (flamerTimer_.update(elapsed)) {
            setDirection((direction() + 1) % 8);
        }
    }  else if (getWeaponType() == Weapon::TimeBomb) {
        map_ = -1;
        health_ = 0;
        deactivate();
        Explosion::createExplosion(pMission, this, 
            (double)pWeaponClass_->rangeDmg(), pWeaponClass_->damagePerShot());
    } else {
        // For other weapons, damage are done immediatly because projectile speed
        // is too high to draw them
        InstantImpactShot shot(dmg);
        shot.inflictDamage(pMission);
    }

    ammo_remaining_ -= pWeaponClass_->ammoPerShot();
    if (ammo_remaining_ < 0) {
        ammo_remaining_ = 0;
    }
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
            has_blocker = w->checkRangeAndBlocker(cp, &smp, &pn, true);
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
                    int z = smp->tileZ() * 128 + smp->offZ() + (smp->sizeZ() >> 1);
                    if (z > (m->mmax_z_ - 1) * 128)
                        z = (m->mmax_z_ - 1) * 128;
                    so->setPosition(smp->tileX(), smp->tileY(), z / 128,
                        smp->offX(), smp->offY(), z % 128);
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
                    int z = smp->tileZ() * 128 + smp->offZ() + (smp->sizeZ() >> 1);
                    if (z > (m->mmax_z_ - 1) * 128)
                        z = (m->mmax_z_ - 1) * 128;
                    so->setPosition(smp->tileX(), smp->tileY(), z / 128,
                        smp->offX(), smp->offY(), z % 128);
                    so->correctZ();
                    m->addSfxObject(so);
                }
            }
        } else if (has_blocker == 32)
            return;
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
        for (size_t i = 0; i < m->numPeds(); i++) {
            ShootableMapObject *p = m->ped(i);
            if (!p->isIgnored() && (((PedInstance *)owner_)->isHostileTo(p)
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
        for (size_t i = 0; i < m->numVehicles(); i++) {
            ShootableMapObject *v = m->vehicle(i);
            if (!v->isIgnored() && (((PedInstance *)owner_)->isHostileTo(v)
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

    for (size_t i = 0; i < m->numPeds(); i++) {
        ShootableMapObject *p = m->ped(i);
        if (!p->isIgnored()
            // TODO: inrange if checktileonly = false might return "7"
            // different handling for this we might shoot if blocker
            // is not friendly
            && !(((PedInstance *)owner_)->isFriendWith((PedInstance *)p))
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

void WeaponInstance::handleHit(ShootableMapObject::DamageInflictType & d)
{
    // When a bomb is hit, it explodes
    if (main_type_ == Weapon::TimeBomb && health_ > 0) {
        is_ignored_ = true;
        // we pass the given DamageInflictType just for the compiler
        // as it is not used by the fire method for a Bomb
        // same for elapsed
        fire(g_Session.getMission(), d, 0);
    }
}

void WeaponInstance::updtWeaponUsedTime(int elapsed) {
    if (time_consumed_) {
        time_consumed_ = false;
    } else if (weapon_used_time_ != 0 ) {
        weapon_used_time_ += elapsed;
        if (weapon_used_time_ >= (pWeaponClass_->timeForShot()
            + pWeaponClass_->timeReload()))
        {
            weapon_used_time_ = 0;
        }
    }
}

/*! Draws animation of impact/explosion
 * @param cp current position (center)
 * @param dmg_rng effective range for drawing
 * @param rngdamg_anim animation to be used for drawing
 */
void ShotClass::rangeDamageAnim(toDefineXYZ &cp, double dmg_rng,
    int rngdamg_anim)
{
    Mission *m = g_Session.getMission();
    toDefineXYZ base_pos = cp;
    cp.z += Z_SHIFT_TO_AIR;
    if (cp.z > (m->mmax_z_ - 1) * 128)
        cp.z = (m->mmax_z_ - 1) * 128;
    // TODO: exclude flames on water, put these flames to the ground,
    // don't draw in air(, stairs problem?)
    double angle_inc = PI;
    const uint8 waves = (int)dmg_rng / 144 + 1;

    for (uint8 i = 0; i < waves; i++) {
        double base_angle = 0.0;
        if (rand() % 100 > 74)
            base_angle += angle_inc;

        for (int j = 0; j < (4 << i); j++) {
            double x = (double)(144 * i) * cos(base_angle);
            double y = (double)(144 * i) * sin(base_angle);
            base_angle += angle_inc;
            PathNode pn = PathNode((base_pos.x + (int)x) / 256,
                (base_pos.y + (int)y) / 256,
                base_pos.z / 128, (base_pos.x + (int)x) % 256,
                (base_pos.y + (int)y) % 256, base_pos.z % 128);

            uint8 block_mask = m->inRangeCPos(&cp, NULL, &pn, true, true, dmg_rng);
            if (block_mask != 32) {
                SFXObject *so = new SFXObject(m->map(), rngdamg_anim,
                                100 * (rand() % 16));
                so->setPosition(pn.tileX(), pn.tileY(), pn.tileZ(),
                                pn.offX(), pn.offY(), pn.offZ());
                m->addSfxObject(so);
            }
        }
        angle_inc /= 2.0;
    }
}
