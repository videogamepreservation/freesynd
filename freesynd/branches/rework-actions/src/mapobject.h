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

#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include <math.h>
#include <list>
#include "path.h"
#include "pathsurfaces.h"

class Mission;
class WeaponInstance;

/*!
 * Map object class.
 */
class MapObject {
public:
    /*!
     * Express the nature of a MapObject.
     */
    enum ObjectNature {
        kNatureUndefined = 0,
        kNaturePed = 1,
        kNatureWeapon = 2,
        kNatureStatic = 4,
        kNatureVehicle = 8
    };

public:
    MapObject(int m, ObjectNature nature);
    virtual ~MapObject() {}

    //! Return the nature of the object
    ObjectNature nature() { return nature_; }
    //! For debug purpose
    const char* natureName();

    virtual void draw(int x, int y) = 0;
    enum DamageType {
        dmg_None = 0x0000,
        dmg_Bullet = 0x0001,
        dmg_Laser = 0x0002,
        dmg_Burn = 0x0004,
        dmg_Explosion = 0x0008,
        dmg_Collision = 0x0010, // By car or door
        dmg_Physical = (dmg_Bullet | dmg_Laser | dmg_Burn | dmg_Explosion | dmg_Collision),
        dmg_Persuasion = 0x0020,
        dmg_Heal = 0x0040,
        dmg_Panic = 0x0080,
        dmg_Mental = (dmg_Persuasion | dmg_Panic),
        dmg_All = 0xFFFF
    };

    typedef enum {
        ddmg_Invulnerable = dmg_None,
        ddmg_Ped = dmg_All,
        ddmg_PedPanicImmune = dmg_All ^ dmg_Panic,
        ddmg_PedWithEnergyShield = dmg_Explosion | dmg_Collision | dmg_Heal | dmg_Panic,
        ddmg_Vehicle = dmg_Bullet | dmg_Laser | dmg_Burn | dmg_Explosion,
        ddmg_StaticTree = dmg_Laser | dmg_Burn | dmg_Explosion,
        ddmg_StaticWindow = dmg_Bullet | dmg_Explosion,
        ddmg_StaticGeneral = dmg_Laser | dmg_Explosion,
        ddmg_WeaponBomb = dmg_Bullet | dmg_Laser | dmg_Explosion
    } DefDamageType;

    void setPosition(int tile_x, int tile_y, int tile_z, int off_x = 0,
            int off_y = 0, int off_z = 0) {
        tile_x_ = tile_x;
        tile_y_ = tile_y;
        tile_z_ = tile_z;
        off_x_ = off_x;
        off_y_ = off_y;
        off_z_ = off_z;
    }

    /*!
     * Set the position of the object to be the given one.
     * \param pos New object position
     */
    void setPosition(const PathNode &pos) {
        tile_x_ = pos.tileX();
        tile_y_ = pos.tileY();
        tile_z_ = pos.tileZ();
        off_x_ = pos.offX();
        off_y_ = pos.offY();
        off_z_ = pos.offZ();
    }

    /*!
     * Set the position of the object to be the given one.
     * \param pos New object position
     */
    void setPosition(const toDefineXYZ &pos) {
        setPosition(pos.x / 256, pos.y / 256, pos.z / 128, pos.x % 256,
                    pos.y % 256, pos.z % 128 );
    }

    int tileX() { return tile_x_; }
    int tileY() { return tile_y_; }
    int tileZ() { return tile_z_; }

    void setTileX(int x) { tile_x_ = x; }
    void setTileY(int y) { tile_y_ = y; }
    void setTileZ(int z) { tile_z_ = z; }

    int offX() { return off_x_; }
    int offY() { return off_y_; }
    int offZ() { return off_z_; }

    void setOffX(int n);
    void setOffY(int n);
    void setOffZ(int n);

    int sizeX() { return size_x_;}
    int sizeY() { return size_y_;}
    int sizeZ() { return size_z_;}

    void setSizeX(int size_x) { size_x_ = size_x;}
    void setSizeY(int size_y) { size_y_ = size_y;}
    void setSizeZ(int size_z) { size_z_ = size_z;}

    int screenX();
    int screenY();

    virtual int map() { return map_; }
    void setMap(int new_map) { map_ = new_map; }

    bool samePosition(MapObject * other) {
        return other->tile_x_ == tile_x_
                && other->tile_y_ == tile_y_
                && other->off_x_ == off_x_
                && other->off_y_ == off_y_
                && other->off_z_ == off_z_;
    }

    /*!
     * Return true if the distance between this object and the given object
     * is less than the given distance.
     * \param pObject The other object.
     * \param distance
     */
    bool isCloseTo(MapObject *pObject, int distance) {
        int cx = tile_x_ * 256 + off_x_ - (pObject->tile_x_ * 256 + pObject->off_x_);
        int cy = tile_y_ * 256 + off_y_ - (pObject->tile_y_ * 256 + pObject->off_y_);
        int cz = tile_z_ * 128 + off_z_ - (pObject->tile_z_ * 128 + pObject->off_z_);

        return (cx * cx + cy * cy + cz * cz) < (distance * distance);
    }

    /*!
     * Return true if the distance between this object and the given location
     * is less than the given distance.
     * \param loc The location.
     * \param distance
     */
    bool isCloseTo(const toDefineXYZ &loc, int32 distance) {
        int cx = tile_x_ * 256 + off_x_ - (loc.x);
        int cy = tile_y_ * 256 + off_y_ - (loc.y);
        int cz = tile_z_ * 128 + off_z_ - (loc.z);

        return (cx * cx + cy * cy + cz * cz) < (distance * distance);
    }

    double distanceTo(MapObject *t) {
        int cx = tile_x_ * 256 + off_x_ - (t->tile_x_ * 256 + t->off_x_);
        int cy = tile_y_ * 256 + off_y_ - (t->tile_y_ * 256 + t->off_y_);
        int cz = tile_z_ * 128 + off_z_ - (t->tile_z_ * 128 + t->off_z_);
        return sqrt((double) (cx * cx + cy * cy + cz * cz));
    }

    double distanceToPosXYZ(toDefineXYZ *xyz) {
        int cx = tile_x_ * 256 + off_x_ - (xyz->x);
        int cy = tile_y_ * 256 + off_y_ - (xyz->y);
        int cz = tile_z_ * 128 + off_z_  - (xyz->z);
        return sqrt((double) (cx * cx + cy * cy + cz * cz));
    }

    double distanceToPosSz(toDefineXYZ *xyz) {
        int cx = tile_x_ * 256 + off_x_ - (xyz->x);
        int cy = tile_y_ * 256 + off_y_ - (xyz->y);
        int cz = tile_z_ * 128 + off_z_ + (size_z_ >> 1) - (xyz->z);
        return sqrt((double) (cx * cx + cy * cy + cz * cz));
    }

    void convertPosToXYZ(toDefineXYZ *xyz) {
        xyz->x = tile_x_ * 256 + off_x_;
        xyz->y = tile_y_ * 256 + off_y_;
        xyz->z = tile_z_ * 128 + off_z_;
    }

    void convertPosToXY(int *x, int *y) {
        *x = tile_x_ * 256 + off_x_;
        *y = tile_y_ * 256 + off_y_;
    }

    virtual bool animate(int elapsed);

    void setFramesPerSec(int framesPerSec)
    {
        frames_per_sec_ = framesPerSec;
    }
    int getFramesPerSec() { return frames_per_sec_; }

    typedef struct{
        // 0 - can go; 1 - wait; 2 - stop
        char wayFree;
        int tilex;
        int tiley;
        int tilez;
        //! create range by x, should be 0 if not used
        int xadj;
        //! create range by y, should be 0 if not used
        int yadj;
        MapObject * pathBlocker;
    }FreeWay;

    virtual bool isPathBlocker() {
        return false;
    }

    void setRcvDamageDef(DefDamageType rcvDamageDef) {
        rcv_damage_def_ = rcvDamageDef;
    }
    DefDamageType getRcvDamageDef() {
        return rcv_damage_def_;
    }

    void setFrame(int frame) { frame_ = frame;}
    void setFrameFromObject(MapObject *m) {
        frame_ = m->frame_;
    }

    void setDirection(int dir);
    void setDirection(int posx, int posy, int * dir = NULL);

    int direction() { return dir_;}
    int getDirection(int snum = 8);

    void setTimeShowAnim(int t) {
        frame_ = 0;
        time_show_anim_ = t;
        time_showing_anim_ = 0;
    }
    bool leftTimeShowAnim(int t) {
        if (time_show_anim_ == -1)
            return true;
        time_showing_anim_ += t;
        return time_show_anim_ > time_showing_anim_;
    }

    bool isIgnored() { return is_ignored_; }
    void setIsIgnored(bool is_ignored = false) { is_ignored_ = is_ignored; }

    bool isBlocker(toDefineXYZ * startXYZ, toDefineXYZ * endXYZ,
               double * inc_xyz);

    void setStateMasks(unsigned int state) {
        state_ = state;
    }
    unsigned int stateMasks() { return state_; }

    void offzOnStairs(uint8 twd);

#ifdef _DEBUG
    uint32 getDebugID() { return debugId_; }
#endif

protected:
    //! the nature of this object
    ObjectNature nature_;
    /*!
     * Tile based coordinates, tile_x_ and tile_y_ have 256
     * tile_z_ has 128 base
     */
    int tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_;
    //! these are not true sizes, but halfs of full size by respective coord
    int size_x_, size_y_, size_z_;
    //! if equal -1 object is not on map and should not be drawn
    int map_;
    //! animation frame changing
    int frame_;
    /*!
     * Time left, if frame needs to be drawn every 'n' milliseconds
     * elapsed time % 'n' = time left
     */
    int elapsed_carry_;
    //! how often this frame should be drawn per seccond
    int frames_per_sec_;
    DefDamageType rcv_damage_def_;
    //! objects direction
    int dir_;
    //! looped animations, time to show them is set here, if = -1 show forever
    int time_show_anim_;
    //! looped animations, playing time
    int time_showing_anim_;
    //! object is not included in view/shot trajectory calculation
    bool is_ignored_;
    /*! animation to draw can, change this varible should provide check
     * whether first frame was drawn successfully, only if successful
     * MapObject::animate should be called
     */
    bool is_frame_drawn_;
    uint32 state_;

    void addOffs(int &x, int &y);
#ifdef _DEBUG
private:
    static uint32 debugIdCnt;
    uint32 debugId_;
#endif
};

/*!
 * SFXObject map object class.
 */
class SFXObject : public MapObject {
public:
    /*!
     * Type of SfxObject.
     */
    enum SfxTypeEnum {
        sfxt_Unknown = 0,
        sfxt_BulletHit = 1,
        sfxt_FlamerFire = 2,
        sfxt_Smoke = 3,
        sfxt_Fire_LongSmoke = 4,
        sfxt_ExplosionFire = 5,
        sfxt_ExplosionBall = 6,
        sfxt_LargeFire = 7,
        sfxt_SelArrow = 8,
        sfxt_AgentFirst = 9,
        sfxt_AgentSecond = 10,
        sfxt_AgentThird = 11,
        sfxt_AgentFourth = 12
    };

    SFXObject(int m, SfxTypeEnum type, int t_show = 0, bool managed = false);
    virtual ~SFXObject() {}

    bool sfxLifeOver() { return sfx_life_over_; }
    //! Return true if object is managed by another object
    bool isManaged() { return managed_; }
    //! Set whether animation should loop or not
    void setLoopAnimation(bool flag) { loopAnimation_ = flag; }
    //! Reset animation
    void reset();

    void draw(int x, int y);
    bool animate(int elapsed);
    void correctZ();
    void setDrawAllFrames(bool daf) {
        if (daf != draw_all_frames_) {
            draw_all_frames_ = daf;
            frame_ = 0;
        }
    }
protected:
    /*! The type of SfxObject.*/
    SfxTypeEnum type_;
    int anim_;
    bool sfx_life_over_;
    // to draw all frames or first frame only
    bool draw_all_frames_;
    //! Tells if the animation should restart automatically after ending
    bool loopAnimation_;
    int elapsed_left_;
    //! True means the life of the object is managed by something else than gameplaymenu
    bool managed_;
};

/*!
 * Shootable map object class.
 */
class ShootableMapObject : public MapObject {
public:
    /*!
     * This structure holds informations on the damage inflicted to a ShootableMapObject.
     */
    struct DamageInflictType {
        //! The type of damage
        DamageType dtype;
        //! Range of damage
        double range;
        //! The value of the damage
        int dvalue;
        //! direction damage comes from, should be angle 256 degree based
        int ddir;
        //! Location of aimed point
        PathNode aimedLoc;
        //! Location of origin of shot
        toDefineXYZ originLocW;
        //! The object that inflicted the damage
        ShootableMapObject * d_owner;
        //! The weapon that generated this damage
        WeaponInstance *pWeapon;
    };

public:
    ShootableMapObject(int m, ObjectNature nature);
    virtual ~ShootableMapObject() {}

    int health() { return health_; }

    // NOTE: objects that are not used or have negative health from start
    // shoud have -1; objects destroyed during gameplay 0
    void setHealth(int n) {
        if (n > 255)
            n = 255;

        if (n < -1)
            n = -1;

        health_ = n;
    }

    int startHealth() { return start_health_; }

    void setStartHealth(int n) {
        if (n > 255)
            n = 255;

        if (n < -1)
            n = -1;

        start_health_ = n;
    }

    /*!
     * Add a certain amount to health.
     * \return true if health reached max
     * \param amount how much to increase health
     */
    bool increaseHealth(int amount) {
        health_ += amount;
        if (health_ > start_health_) {
            health_ = start_health_;
        }

        return health_ == start_health_;
    }

    /*!
     * Remove a certain amount of health.
     */
    void decreaseHealth(int amount) {
        health_ -= amount;
        if (health_ <= 0) {
            health_ = 0;
        }
    }
    /*!
     * Reset current ped's health to starting health.
     */
    void resetHealth() {
        health_ = start_health_;
    }

    /*!
     * Method called when object is hit by a weapon shot.
     * \param d Damage description
     */
    virtual void handleHit(DamageInflictType &d) {
        // TODO : remove handleDamage()
        handleDamage(&d);
    }

    virtual bool handleDamage(ShootableMapObject::DamageInflictType * d) {
        if (health_ <= 0 || rcv_damage_def_ == MapObject::ddmg_Invulnerable
            || (d->dtype & rcv_damage_def_) == 0)
            return false;
        health_ -= d->dvalue;
        health_ = 0;
        return true;
    }
    virtual bool isExcluded() { return health_ <= 0; }
    bool isAlive() { return health_ > 0; }
    bool isDead() { return health_ <= 0; }

  protected:
    int health_, start_health_;
};

/*!
 * Shootable movable map object class.
 */
class ShootableMovableMapObject : public ShootableMapObject {
public:
    ShootableMovableMapObject(int m, ObjectNature nature);
    virtual ~ShootableMovableMapObject() {}

    void setSpeed(int new_speed) {
        speed_ = new_speed;
    }

    int speed() {
        return speed_;
    }
    void setBaseSpeed(int bs) {
        base_speed_ = bs;
    }
    /*!
     * Clear path to destination and sets speed to 0.
     */
    void clearDestination() {
        dest_path_.clear();
        speed_ = 0;
    }

    //! Set the destination to reach at given speed
    virtual bool setDestination(Mission *m, PathNode &node, int newSpeed = -1) = 0;

    //! checks whether final destination is same as pn
    bool checkFinalDest(PathNode& pn);
    bool isMoving() { return speed_ != 0 || !dest_path_.empty();}
    //! Returns true if object currently has a destination point (ie it's arrived)
    bool hasDestination() { return !dest_path_.empty(); }
    //! checks whether current position is same as pn
    bool checkCurrPos(PathNode &pn);
    //! checks whether current position is same as pn, tile only
    bool checkCurrPosTileOnly(PathNode &pn);

    FreeWay hold_on_;

protected:
    int speed_, base_speed_;
    //! on reaching this distance object should stop
    int dist_to_pos_;
    std::list<PathNode> dest_path_;

    bool updatePlacement(int nOffX, int nOffY);

    virtual bool walkable(int x, int y, int z) = 0;
};

/*!
 * Static map object class.
 */
class Static : public ShootableMapObject {
public:
    /*! Const for subtype 1 of Static.*/
    static const int kStaticSubtype1;
    /*! Const for subtype 2 of Static.*/
    static const int kStaticSubtype2;

    enum StaticType {
        // NOTE: should be the same name as Class
        smt_None = 0,
        smt_Advertisement,
        smt_Semaphore,
        smt_Door,
        smt_LargeDoor,
        smt_Tree,
        smt_Window,
        smt_AnimatedWindow,
        smt_NeonSign
    };
public:
    static Static *loadInstance(uint8 *data, int m);
    virtual ~Static() {}

    //! Return the type of statics
    StaticType type() { return type_; }
    //! Set the sub type of statics
    void setSubType(int objSubType) { subType_ = objSubType; }
    //! Return the type of statics
    int subType() { return subType_; }

    virtual bool animate(int elapsed, Mission *obj) {
        return MapObject::animate(elapsed);
    }

    typedef enum {
        sttdoor_Closed = 0,
        sttdoor_Closing,
        sttdoor_Open,
        sttdoor_Opening
    }stateDoors;

    typedef enum {
        stttree_Healthy = 0,
        stttree_Burning,
        stttree_Damaged
    }stateTrees;

    //semaphore, 4 animations + damaged
    typedef enum {
        sttsem_Stt0 = 0,
        sttsem_Stt1,
        sttsem_Stt2,
        sttsem_Stt3,
        sttsem_Damaged
    }stateSemaphores;
    
    typedef enum {
        sttwnd_Closed = 0,
        sttwnd_Open,
        sttwnd_Breaking,
        sttwnd_Damaged
    }stateWindows;

    typedef enum {
        sttawnd_LightOff = 0,
        sttawnd_LightSwitching,
        sttawnd_PedAppears,
        sttawnd_ShowPed,
        sttawnd_PedDisappears,
        sttawnd_LightOn
    }stateAnimatedWindows;

protected:
    Static(int m, StaticType type) :
            ShootableMapObject(m, MapObject::kNatureStatic) {
        type_ = type;
        subType_ = kStaticSubtype1;
    }

protected:
    /*! Type of statics.*/
    StaticType type_;
    /*! Sub division for statics of same type.*/
    int subType_;
};

/*!
 * Door map object class.
 */
class Door : public Static {
public:
    Door(int m, int anim, int closingAnim, int openAnim, int openingAnim);
    virtual ~Door() {}

    void draw(int x, int y);
    bool animate(int elapsed, Mission *obj);
    bool isPathBlocker();

protected:
    int anim_, closing_anim_, open_anim_, opening_anim_;
};

/*!
 * LargeDoor map object class.
 */
class LargeDoor : public Static {
public:
    LargeDoor(int m, int anim, int closingAnim, int openingAnim);
    virtual ~LargeDoor() {}

    void draw(int x, int y);
    bool animate(int elapsed, Mission *obj);
    bool isPathBlocker();

protected:
    int anim_, closing_anim_, opening_anim_;
};
/*!
 * Tree map object class.
 */
class Tree : public Static {
public:
    Tree(int m, int anim, int burningAnim, int damagedAnim);
    virtual ~Tree() {}

    void draw(int x, int y);
    bool animate(int elapsed, Mission *obj);
    bool handleDamage(ShootableMapObject::DamageInflictType *d);

protected:
    int anim_, burning_anim_, damaged_anim_;
};

/*!
 * Window map object class.
 */
class WindowObj : public Static {
public:
    WindowObj(int m, int anim, int openAnim, int breakingAnim,
              int damagedAnim);
    virtual ~WindowObj() {}

    bool animate(int elapsed, Mission *obj);
    void draw(int x, int y);
    bool handleDamage(ShootableMapObject::DamageInflictType *d);

protected:
    int anim_, open_anim_, breaking_anim_, damaged_anim_;
};

/*!
 * EtcObj map object class.
 */
class EtcObj : public Static {
public:
    EtcObj(int m, int anim, int burningAnim, int damagedAnim, StaticType type = smt_None);
    virtual ~EtcObj() {}

    void draw(int x, int y);

protected:
    int anim_, burning_anim_, damaged_anim_;
};

/*!
 * NeonSign map object class.
 */
class NeonSign : public Static {
public:
    NeonSign(int m, int anim);
    virtual ~NeonSign() {}

    void draw(int x, int y);

protected:
    int anim_;
};

/*!
 * Semaphore map object class.
 * That thing that bounces on crossroad.
 */
class Semaphore : public Static {
public:
    Semaphore(int m, int anim, int damagedAnim);
    virtual ~Semaphore() {}

    bool animate(int elapsed, Mission *obj);
    bool handleDamage(ShootableMapObject::DamageInflictType *d);
    void draw(int x, int y);

protected:
    int anim_, damaged_anim_;
    /*! used to make animation of movement up/down,
     * when damaged, stores time not consumed for movement down
     */
    int elapsed_left_smaller_;
    /*! animation color rotation,
     * when damaged, stores target Z distance to fall
     */
    int elapsed_left_bigger_;
    //! switch for moving up or down
    int up_down_;
};

/*!
 * AnimatedWindow map object class.
 */
class AnimWindow : public Static {
public:
    AnimWindow(int m, int anim);
    virtual ~AnimWindow() {}

    bool animate(int elapsed, Mission *obj);
    void draw(int x, int y);

protected:
    int anim_;
};

#endif
