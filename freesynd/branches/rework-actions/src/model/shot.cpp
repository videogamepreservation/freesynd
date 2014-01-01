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

InstantImpactShot::InstantImpactShot(Mission *pMission, ShotAttributes &att) :
    Shot(att) {
}

void InstantImpactShot::inflictDamage(Mission *pMission) {
    toDefineXYZ originLocW; // origin of shooting
    attributes_.pShooter->convertPosToXYZ(&originLocW);
    // get how much impacts does the weapon generate
    int nbImpacts = attributes_.pWeapon->getWeaponClass()->shotsPerAmmo();
    // Get all potential reachable objects
    std::vector<ShootableMapObject *> objInRangeLst;
    getAllShootablesWithinRange(pMission, originLocW, objInRangeLst);
    // If there are many impacts, a target can be hit by several impacts
    // so this map stores number of impacts for a target
    std::map<ShootableMapObject *, int> hitsByObject;

    for (int i = 0; i < nbImpacts; ++i) {
        PathNode impactLocT = attributes_.impactLoc;
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
        SFXObject *sfxImpact = new SFXObject(pMission->map(), attributes_.hitAnimId);
        sfxImpact->setPosition(impactLocT);
        sfxImpact->correctZ();
        pMission->addSfxObject(sfxImpact);
    }

    // finally distribute damage
    std::map<ShootableMapObject *, int>::iterator it;
    for (it = hitsByObject.begin(); it != hitsByObject.end(); it++) {
        attributes_.damage.dvalue = (*it).second * attributes_.pWeapon->getWeaponClass()->damagePerShot();
        (*it).first->handleDamage(&(attributes_.damage));
    }
}

void InstantImpactShot::diffuseImpact(Mission *pMission, toDefineXYZ &originLocW, PathNode &impactLocT) {
    double angle = attributes_.pWeapon->getWeaponClass()->shotAngle();
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
 * Returns all ShootableMapObject that are alive and in range of
 * weapon who generated this shot.
 * Every object found is added to the objInRangeVec vector.
 * \param pMission Mission data
 * \param originLocW Origin of shot
 * \param objInRangeVec Result list
 */
void InstantImpactShot::getAllShootablesWithinRange(Mission *pMission,
                                                    toDefineXYZ &originLocW,
                                                    std::vector<ShootableMapObject *> &objInRangeVec) {
    int range = attributes_.pWeapon->getWeaponClass()->range();
    
    // Look at all peds except the shooter
    for (size_t i = 0; i < pMission->numPeds(); ++i) {
        ShootableMapObject *p = pMission->ped(i);
        if (p->isAlive() && p != attributes_.pShooter) {
            if (pMission->inRangeCPos(&originLocW, &p, NULL, false, true, range) == 1) {
                objInRangeVec.push_back(p);
            }
        }
    }

    for (size_t i = 0; i < pMission->numStatics(); ++i) {
        ShootableMapObject *st = pMission->statics(i);
        if (!st->isIgnored())
            if (pMission->inRangeCPos(&originLocW, &st, NULL, false, true, range) == 1) {
                objInRangeVec.push_back(st);
            }
    }

    // look at all vehicules except the vehicle in which is the shoot
    for (size_t i = 0; i < pMission->numVehicles(); ++i) {
        ShootableMapObject *v = pMission->vehicle(i);
        if (v != attributes_.pShooter->inVehicle() &&
            pMission->inRangeCPos(&originLocW, &v, NULL, false, true, range) == 1) {
                objInRangeVec.push_back(v);
        }
    }

    // look at all weapons except the weapon that generated the shot
    for (size_t i = 0; i < pMission->numWeapons(); ++i) {
        ShootableMapObject *w = pMission->weapon(i);
        if (w != attributes_.pWeapon &&
            pMission->inRangeCPos(&originLocW, &w, NULL, false, true, range) == 1) {
                objInRangeVec.push_back(w);
        }
    }
}

/*!
 *
 */
ShootableMapObject *InstantImpactShot::checkHitTarget(std::vector<ShootableMapObject *> objInRangeLst, toDefineXYZ &originLocW, PathNode &impactLocT) {
    ShootableMapObject *pTarget = NULL;
    uint8 has_blocker = attributes_.pWeapon->checkRangeAndBlocker(originLocW, &pTarget, &impactLocT, true);
    return pTarget;
}