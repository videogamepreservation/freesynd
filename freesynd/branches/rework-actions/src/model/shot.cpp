/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2010  Bohdan Stelmakh <chamel@users.sourceforge.net> *
 *   Copyright (C) 2013  Benoit Blancard <benblan@users.sourceforge.net>*
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

#include <map>

#include "model/shot.h"
#include "mission.h"
#include "ped.h"
#include "vehicle.h"

/*!
 * Returns all ShootableMapObject that are alive and in range of
 * weapon who generated this shot.
 * Every object found is added to the objInRangeVec vector.
 * \param pMission Mission data
 * \param originLocW Origin of shot
 * \param objInRangeVec Result list
 * \param includeShooter if true, owner of the shot will be include in the result list
 */
void Shot::getAllShootablesWithinRange(Mission *pMission,
                                                    toDefineXYZ &originLocW,
                                                    std::vector<ShootableMapObject *> &objInRangeVec,
                                                    bool includeShooter) {
    // Look at all peds
    for (size_t i = 0; i < pMission->numPeds(); ++i) {
        ShootableMapObject *p = pMission->ped(i);
        if (p->isAlive()) {
            if (p == dmg_.d_owner && !includeShooter) {
                // sometimes we don't want to include the shooter
                continue;
            }
            if (pMission->inRangeCPos(&originLocW, &p, NULL, false, true, dmg_.range) == 1) {
                objInRangeVec.push_back(p);
            }
        }
    }

    for (size_t i = 0; i < pMission->numStatics(); ++i) {
        ShootableMapObject *st = pMission->statics(i);
        if (!st->isIgnored())
            if (pMission->inRangeCPos(&originLocW, &st, NULL, false, true, dmg_.range) == 1) {
                objInRangeVec.push_back(st);
            }
    }

    // look at all vehicles
    for (size_t i = 0; i < pMission->numVehicles(); ++i) {
        ShootableMapObject *v = pMission->vehicle(i);
        if (dmg_.d_owner->majorType() == MapObject::mjt_Ped) {
            PedInstance *pPed = dynamic_cast<PedInstance *>(dmg_.d_owner);
            if (v == pPed->inVehicle()) {
                // don't take vehicle if the owner of the shoot is a ped and if he's
                // in the vehicle (ie hope ped has opened the window)
                continue;
            }
        }
        if (pMission->inRangeCPos(&originLocW, &v, NULL, false, true, dmg_.range) == 1) {
                objInRangeVec.push_back(v);
        }
    }

    // look at all weapons except the weapon that generated the shot
    for (size_t i = 0; i < pMission->numWeapons(); ++i) {
        ShootableMapObject *w = pMission->weapon(i);
        if (w != dmg_.pWeapon &&
            pMission->inRangeCPos(&originLocW, &w, NULL, false, true, dmg_.range) == 1) {
                objInRangeVec.push_back(w);
        }
    }
}

void InstantImpactShot::inflictDamage(Mission *pMission) {
    toDefineXYZ originLocW; // origin of shooting
    dmg_.d_owner->convertPosToXYZ(&originLocW);
    // get how much impacts does the weapon generate
    int nbImpacts = dmg_.pWeapon->getWeaponClass()->shotsPerAmmo();
    // Get all potential reachable objects
    std::vector<ShootableMapObject *> objInRangeLst;
    getAllShootablesWithinRange(pMission, originLocW, objInRangeLst, false);
    // If there are many impacts, a target can be hit by several impacts
    // so this map stores number of impacts for a target
    std::map<ShootableMapObject *, int> hitsByObject;

    for (int i = 0; i < nbImpacts; ++i) {
        PathNode impactLocT = dmg_.aimedLoc;
        if (nbImpacts > 1) {
            // When multiple impacts, they're spread
            diffuseImpact(pMission, originLocW, impactLocT);
        }

        // Verify if shot hit something or was blocked by a tile
        ShootableMapObject *pTargetHit = checkHitTarget(objInRangeLst, originLocW, impactLocT);
        if (pTargetHit != NULL) {
            hitsByObject[pTargetHit] = hitsByObject[pTargetHit] + 1;
        }
        // creates impact sprite
        createImpactAnimation(pMission, pTargetHit, impactLocT);
    }

    // finally distribute damage
    std::map<ShootableMapObject *, int>::iterator it;
    for (it = hitsByObject.begin(); it != hitsByObject.end(); it++) {
        dmg_.dvalue = (*it).second * dmg_.pWeapon->getWeaponClass()->damagePerShot();
        (*it).first->handleHit(dmg_);
    }
}

void InstantImpactShot::diffuseImpact(Mission *pMission, toDefineXYZ &originLocW, PathNode &impactLocT) {
    double angle = dmg_.pWeapon->getWeaponClass()->shotAngle();
    if (angle == 0)
        return;

    angle *= (double)(69 + (rand() & 0x1F)) / 100.0;
    int cx = originLocW.x;
    int cy = originLocW.y;
    int cz = originLocW.z;

    toDefineXYZ impactLocW;
    impactLocT.convertPosToXYZ(&impactLocW);
    int tx = impactLocW.x;
    int ty = impactLocW.y;
    int tz = impactLocW.z;
    double dtx = (double)(tx - cx);
    double dty = (double)(ty - cy);
    double dtz = (double)(tz - cz);
    double dist_cur = 0.0;
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

    set_sign = 1.0;
    if (rand() % 100 < 50)
        set_sign = -1.0;
    angz += (angle * (double)(rand() % 100) / 200.0) * set_sign;

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

    int max_x = (pMission->mmax_x_ - 1) * 256;
    int max_y = (pMission->mmax_y_ - 1) * 256;
    int max_z = (pMission->mmax_z_ - 1) * 128;
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

    impactLocT.setTileXYZ(gtx / 256, gty / 256, gtz / 128);
    impactLocT.setOffXYZ(gtx % 256, gtx % 256, gtz % 128);
}

/*!
 *
 */
ShootableMapObject *InstantImpactShot::checkHitTarget(std::vector<ShootableMapObject *> objInRangeLst, toDefineXYZ &originLocW, PathNode &impactLocT) {
    ShootableMapObject *pTarget = NULL;
    // TODO reimplement to use objInRangeLst
    uint8 has_blocker = dmg_.pWeapon->checkRangeAndBlocker(originLocW, &pTarget, &impactLocT, true);
    return pTarget;
}

/*!
 * Creates the impact animation based on the type of target hit.
 * Some weapons can have no animations for an impact.
 */
void InstantImpactShot::createImpactAnimation(Mission *pMission, ShootableMapObject * pTargetHit, PathNode &impactLocT) {
    int impactAnimId = 
        (pTargetHit != NULL ? 
            dmg_.pWeapon->getWeaponClass()->impactAnims()->objectHit :
            dmg_.pWeapon->getWeaponClass()->impactAnims()->groundHit);

    if (impactAnimId != SFXObject::sfxt_Unknown) {
        SFXObject *so = new SFXObject(pMission->map(), impactAnimId);
        so->setPosition(impactLocT);
        so->correctZ();
        pMission->addSfxObject(so);
    }
}

/*!
 * Convenient method to create explosions.
 * \param pMission Mission data
 * \param pOwner What's at the origin of the explosion
 * \param The range for damage 
 * \param The value of the damage
 */
void Explosion::createExplosion(Mission *pMission, ShootableMapObject *pOwner, double range, int dmgValue) {
    ShootableMapObject::DamageInflictType dmg;
    if (pOwner && pOwner->majorType() == MapObject::mjt_Weapon) {
        // It's a bomb that exploded
        dmg.pWeapon = dynamic_cast<WeaponInstance *>(pOwner);
        dmg.d_owner = NULL;
    } else {
        dmg.pWeapon = NULL;
        dmg.d_owner = pOwner;
    }
    
    dmg.dtype = MapObject::dmg_Explosion;
    dmg.range = range;
    dmg.dvalue =  dmgValue;
    pOwner->convertPosToXYZ(&(dmg.originLocW));
    dmg.originLocW.z += 8;

    Explosion explosion(dmg);
    explosion.inflictDamage(pMission);
}

void Explosion::inflictDamage(Mission *pMission) {
    std::vector<ShootableMapObject *> objInRangeLst;

    getAllShootablesWithinRange(pMission, dmg_.originLocW, objInRangeLst, true);

    for (std::vector<ShootableMapObject *>::iterator it = objInRangeLst.begin();
        it != objInRangeLst.end(); it++)
    {
        ShootableMapObject *smo = *it;
        smo->handleHit(dmg_);

        SFXObject *so = new SFXObject(pMission->map(), SFXObject::sfxt_ExplosionBall);
        so->setPosition(smo->tileX(), smo->tileY(), smo->tileZ(), smo->offX(),
            smo->offY(), smo->offZ());
        so->correctZ();
        pMission->addSfxObject(so);
    }
    rangeDamageAnim(pMission, dmg_.originLocW, dmg_.range, SFXObject::sfxt_ExplosionFire);
}

/*! Draws animation of impact/explosion
 * @param cp current position (center)
 * @param dmg_rng effective range for drawing
 * @param rngdamg_anim animation to be used for drawing
 */
void Explosion::rangeDamageAnim(Mission *pMission, toDefineXYZ &cp, double dmg_rng,
    int rngdamg_anim)
{
    toDefineXYZ base_pos = cp;
    cp.z += 4;
    if (cp.z > (pMission->mmax_z_ - 1) * 128)
        cp.z = (pMission->mmax_z_ - 1) * 128;
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

            uint8 block_mask = pMission->inRangeCPos(&cp, NULL, &pn, true, true, dmg_rng);
            if (block_mask != 32) {
                SFXObject *so = new SFXObject(pMission->map(), rngdamg_anim,
                                100 * (rand() % 16));
                so->setPosition(pn.tileX(), pn.tileY(), pn.tileZ(),
                                pn.offX(), pn.offY(), pn.offZ());
                pMission->addSfxObject(so);
            }
        }
        angle_inc /= 2.0;
    }
}
