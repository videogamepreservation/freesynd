/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
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

#ifndef MODEL_OBJECTIVEDESC_H
#define MODEL_OBJECTIVEDESC_H

#include "common.h"
#include "mapobject.h"
#include "core/gameevent.h"

/*!
 * Defines all objectives possible types.
 */
enum ObjectiveType {
    objv_None = 0x0,
    //! Setup control over object where possible to lose this control
    objv_AquireControl = 0x0001,
    //! Leave control over object where possible to lose this control
    objv_LoseControl = 0x0002,
    //! Obtain inventory object
    objv_PickUpObject = 0x0008,
    /*!
     * Object of defined subtype (of type) should be destroyed
     * defined by indx
     */
    objv_DestroyObject = 0x0010,
    //! Use of object untill condition is met
    objv_UseObject = 0x0020,
    objv_PutDownObject = 0x0040,
    //! Objects should be at defined location
    objv_ReachLocation = 0x0080,
    objv_FollowObject = 0x0100,
    //! Should wait some time
    objv_Wait = 0x0200,
    objv_AttackLocation = 0x0400,
    objv_Protect = 0x8000,
};

/*!
 * Defines all possible status for the current objective.
 */
enum EObjectiveStatus {
    //! Objective is not started
    kNotStarted,
    //! Objective is started
    kStarted,
    //! Objective has failed
    kFailed,
    //! Objective is completed
    kCompleted
};

/*!
 * An ObjectiveDesc structure holds the elements defining an objective.
 */
class ObjectiveDesc {
public:
    virtual ~ObjectiveDesc() {};
    //! type of objective
    ObjectiveType type;
    // MapObject::MajorTypeEnum
    uint8 targettype;
    // (objGroupDefMasks)
    uint32 targetsubtype;
    union {
        // index within vector of data
        uint16 targetindx;
        uint16 grpid;
    } indx_grpid;

    /*! Status of the objective.*/
    EObjectiveStatus status;
    /*!
     * Status of the objective.
     * Possible values :
     * - 00 : not defined
     * - 0b : has sub objective
     * - 1b : refers to all objects of subtype
     * - 2b : completed
     * - 3b : failed
     * - 4b : check previous objectives for fail
     * - 5b - is subobjective
     */
    uint32 condition;
    // indx for sub objective
    uint16 subobjindx;
    toDefineXYZ pos_xyz;
    //! This message should be set during objective definition
    std::string msg;
    uint16 nxtobjindx;

    ObjectiveDesc(ObjectiveType aType) {
        type = aType;
        targettype = (MapObject::MajorTypeEnum)MapObject::mjt_Undefined;
        targetsubtype = 0;
        indx_grpid.targetindx = 0;
        status = kNotStarted;
        condition = 0;
        subobjindx = 0;
        pos_xyz.x = 0;
        pos_xyz.y = 0;
        pos_xyz.z = 0;
        nxtobjindx = 0;
    }

    /*!
     * Return true if objective is cleared (succeeded or failed)
     */
    bool isTerminated() {
        return ((condition & 12) != 0) || status == kCompleted || status == kFailed;
    }

    /*!
     * This method declares the objective as 'started'.
     * Returns an event that should be send to all listeners interested by
     * it.
     * The returned event depends on the type of objective.
     */
    GameEvent start() {
        status = kStarted;
        GameEvent evt;
        evt.type_ = GameEvent::kNone;
        evt.pCtxt_ = NULL;
        handleStart(evt);
        return evt;
    }

    /*!
     * This method ends an objective with a 'failed' or 'succeeded' status.
     * Returns an event that should be send to all listeners interested by
     * it.
     * The returned event depends on the type of objective.
     */
    GameEvent end(bool succeeded) {
        status = succeeded ? kCompleted : kFailed;
        GameEvent evt;
        evt.type_ = GameEvent::kNone;
        evt.pCtxt_ = NULL;
        handleEnd(evt);
        return evt;
    }
protected:
    virtual void handleStart(GameEvent &evt) {}

    virtual void handleEnd(GameEvent &evt) {}
};

/*!
 * A TargetObjective defines an objective with a MapObject as target.
 */
class TargetObjective : public ObjectiveDesc {
public:
    TargetObjective(ObjectiveType aType, MapObject * pMapObject) : ObjectiveDesc(aType) {
        p_target_ = pMapObject;
    }

    MapObject * target() { return p_target_; }
protected:
    void handleStart(GameEvent &evt) {
        evt.type_ = GameEvent::kObjTargetSet;
        evt.pCtxt_ = p_target_;
    }

    void handleEnd(GameEvent &evt) {
        evt.type_ = GameEvent::kObjTargetCleared;
        evt.pCtxt_ = p_target_;
    }

protected:
    MapObject *p_target_;
};

/*!
 * A LocationObjective defines an objective with a Location as target.
 */
class LocationObjective : public ObjectiveDesc {
public:
    LocationObjective(ObjectiveType aType) : ObjectiveDesc(aType) {}
};

#endif // MODEL_OBJECTIVEDESC_H
