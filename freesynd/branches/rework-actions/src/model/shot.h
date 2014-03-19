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

#ifndef MODEL_SHOT_H_
#define MODEL_SHOT_H_

#include <vector>

#include "path.h"
#include "mapobject.h"

class Mission;
class WeaponInstance;
class PedInstance;

class Shot {
public:
    Shot(ShootableMapObject::DamageInflictType &dmg) {
        dmg_ = dmg;
    }
    virtual ~Shot() {}

    virtual bool animate(int elapsed, Mission *m) = 0;
    virtual bool isLifeOver() = 0;

    virtual void inflictDamage(Mission *pMission) = 0;

    ShootableMapObject::DamageInflictType & getAttributes() { return dmg_; }
protected:
    //!
    void getAllShootablesWithinRange(Mission *pMission, toDefineXYZ &originLocW, std::vector<ShootableMapObject *> &objInRangeLst, bool includeShooter);
protected:
    // The damage that will be inflicted by this shot
    ShootableMapObject::DamageInflictType dmg_;
};

/*!
 * This class represents a shot where the impacts are immediatly
 * made. Weapons who use this type of shot are pistol, minigun, Shotgun,
 * uzi, laser, long range.
 */
class InstantImpactShot : public Shot {
public:
    InstantImpactShot(ShootableMapObject::DamageInflictType &dmg) : Shot(dmg) {}

    virtual bool animate(int elapsed, Mission *m) { return false; }
    virtual bool isLifeOver() { return true; }

    void inflictDamage(Mission *pMission);
private:
    //! 
    void diffuseImpact(Mission *m, toDefineXYZ &originLocW, PathNode &impactLocT);
    //!
    ShootableMapObject *checkHitTarget(std::vector<ShootableMapObject *> objInRangeLst, toDefineXYZ &originLocW, PathNode &impactLocT);
    //!
    void createImpactAnimation(Mission *pMission, ShootableMapObject * pTargetHit, PathNode &impactLocT);
};

/*!
 * This class represents an explosion caused by a time bomb, an agent's
 * suicide or a car explosion.
 */
class Explosion : public Shot {
public:
    //! Creates an explosion
    static void createExplosion(Mission *pMission, ShootableMapObject *pOwner, double range, int dmgValue = 16);

    Explosion(ShootableMapObject::DamageInflictType &dmg) : Shot(dmg) {};

    bool animate(int elapsed, Mission *m) { return false; }
    bool isLifeOver() { return true; }

    void inflictDamage(Mission *pMission);
private:
    void generateFlameWaves(Mission *pMission, toDefineXYZ &cp, double dmg_rng, int rngdamg_anim);
};

#endif // MODEL_SHOT_H_
