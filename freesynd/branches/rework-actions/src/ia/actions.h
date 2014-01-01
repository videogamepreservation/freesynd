/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2013  Bohdan Stelmakh <chamel@users.sourceforge.net> *
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

#ifndef IA_ACTIONS_H_
#define IA_ACTIONS_H_

#include "path.h"

class Mission;
class PedInstance;
class ShootableMapObject;
class WeaponInstance;
class Vehicle;
class VehicleInstance;

namespace fs_actions {

    /*!
     * List the different process by which action can be created.
     */
    enum CreatOrigin {
        //! Origin unknown
        kOrigUnknow = 0,
        //! By script
        kOrigScript = 1,
        //! From default action list
        kOrigDefault = 2,
        //! By another action group
        kOrigAction = 3,
        //! From user input
        kOrigUser = 4,
        //! Something happened that generated this action
        kOrigEvent = 5
    };

    /*!
     * The Action class is an abstract class representing an action that a ped can do
     * like walking, driving a car, pick up someting or shoot.
     * Every action has following attributes :
     *   - status (from not started to finished), 
     *   - origin : can be created from game file or after a player action. When
     *      an action is finished it is removed from the list except for scripted actions
     *      which are replayed.
     * Subclasses must implement the execute() method.
     */
    class Action {
    public:
        //! Possible status of an action.
        enum ActionStatus {
            //! Initial status for action
            kActStatusNotStarted,
            //! When action is running
            kActStatusRunning,
            //! When action needs to wait for the end of animation to complete
            kActStatusWaitForAnim,
            //! When action needs to wait for some time to complete
            kActStatusWaitForTime,
            //! When action ends with success
            kActStatusSucceeded,
            //! When action ends with failure
            kActStatusFailed
        };

        //! Constructor for the class
        Action(CreatOrigin origin);
        //! Destructor of the class
        virtual ~Action() { } 

        //! Entry point to execute the action
        virtual bool execute(int elapsed, Mission *pMission, PedInstance *pPed) = 0;

        //! Returns true if action is waiting for animation to end
        bool isWaitingForAnimation() { return status_ == kActStatusWaitForAnim; }
        //! Returns true if action has succeeded or failed
        bool isFinished() { return status_ == kActStatusSucceeded || status_ == kActStatusFailed; }

        //! Sets the status to Running
        void setRunning() { status_ = kActStatusRunning; }
        //! Sets the status to WaitForTime
        void setWaitingForTime() { status_ = kActStatusWaitForTime; }
        //! Sets the status to Succeeded
        void setSucceeded() { status_ = kActStatusSucceeded; }
        //! Sets the status to Failed
        void setFailed() { status_ = kActStatusFailed; }
        
    protected:
        CreatOrigin origin_;
        /*! This is the status of the action.*/
        ActionStatus status_;
    };

    /*!
     * MovementAction (not happy with the name) are every action a Ped can make except shooting.
     * A Ped can chain multiple MovementActions.
     * Normaly, a ped can shoot while doing movement actions except if current action is exclusive.
     * For example, ped cannot shoot while dropping his weapon on the ground.
     * Some actions are not possible while ped is in car (like walking) and other are (like driving).
     * The targetState field stores the state that the ped will have when action is realized.
     *
     * MovementAction exposes 2 methods for subclasses to implement :
     * - doStart() : method called one time when action is first started
     * - doExecute() : method called every time to do the action implementation
     */
    class MovementAction : public Action {
    public:
        //! Constructor for the class
        MovementAction(CreatOrigin origin, bool isExclusive = false, bool canExecVehicle = false);
        //! Destructor of the class
        virtual ~MovementAction() { pNext_ = NULL; }

        //! Entry point to execute the action
        bool execute(int elapsed, Mission *pMission, PedInstance *pPed);

        //! Returns true if action is exclusive
        bool isExclusive() { return isExclusive_; }
        //! Returns true if the action can be executed while ped is in a vehicle
        bool canExecInVehicle() { return canExecInVehicle_; }
        //! A completed action can be removed only if not created by scenario files
        bool canRemove() { return origin_ != kOrigScript; }

        //! Return the next action in the list
        MovementAction *next() { return pNext_; }
        //! Set the next action
        void setNext(MovementAction *pAction) { pNext_ = pAction; }
    protected:
        //! Subclasses must implement this method to do somthing at the begining of the action
        virtual void doStart(Mission *pMission, PedInstance *pPed) {}
        //! Sublasses must implement this method to realize this action
        virtual bool doExecute(int elapsed, Mission *pMission, PedInstance *pPed) = 0;
    protected:
        /*! When this flag is set, the ped cannot shoot.*/
        bool isExclusive_;
        /*! This flag tells that the action can be executed when ped is in a vehicule.*/
        bool canExecInVehicle_;
        /*! Store the state the ped will have when executing the action.*/
        uint8 targetState_;
        /*! Next action in the chain.*/
        MovementAction *pNext_;
    };

    /*!
     * This action is used to move a ped to a given point by walking.
     */
    class WalkAction : public MovementAction {
    public:
        //! Walt to given point
        WalkAction(CreatOrigin origin, PathNode pn, int speed = -1);
        //! Walk to given object
        WalkAction(CreatOrigin origin, ShootableMapObject *smo, int speed = -1);

    protected:
        void doStart(Mission *pMission, PedInstance *pPed);
        bool doExecute(int elapsed, Mission *pMission, PedInstance *pPed);
    protected:
        /*! Where to walk to.*/
        PathNode dest_;
        /*! Speed used to walk to destination.*/
        int newSpeed_;
    };

    /*!
     * This action is used to make a ped follow another ped.
     * Both must be walking.
     * This action is only available to player.
     */
    class FollowAction : public MovementAction {
    public:
        //! Walt to given point
        FollowAction(PedInstance *pTarget);

    protected:
        void doStart(Mission *pMission, PedInstance *pPed);
        bool doExecute(int elapsed, Mission *pMission, PedInstance *pPed);

        //! Update the target's last position
        void updateLastTargetPos();
    protected:
        /*! This constant defines the closest distance a ped can
         * approach a target when following.*/
        static const int kFollowDistance;

        /*! The ped to follow.*/
        PedInstance *pTarget_;
        /*! To keep track of target position and see if it has moved.*/
        PathNode targetLastPos_;
    };

    /*!
     * This action allows an agent to drop a weapon (identified by its
     * position in the agent's inventory) on the ground.
     * It is only available for our agents (so it's a player action).
     */
    class PutdownWeaponAction : public MovementAction {
    public:
        PutdownWeaponAction(uint8 weaponIdx);

    protected:
        void doStart(Mission *pMission, PedInstance *pPed);
        bool doExecute(int elapsed, Mission *pMission, PedInstance *pPed);
    protected:
        /*! Index of the weapon to drop in the ped's inventory.*/
        uint8 weaponIdx_;
    };

    /*!
     * This action allows an agent to pick a weapon up from the ground
     * and to add it in his inventory.
     * It is only available for our agents (so it's a player action).
     */
    class PickupWeaponAction : public MovementAction {
    public:
        PickupWeaponAction(WeaponInstance *pWeapon);

    protected:
        void doStart(Mission *pMission, PedInstance *pPed);
        bool doExecute(int elapsed, Mission *pMission, PedInstance *pPed);
    protected:
        /*! The weapon to pick up.*/
        WeaponInstance *pWeapon_;
    };

    /*!
     * This action is used to get a ped in a vehicle.
     */
    class EnterVehicleAction : public MovementAction {
    public:
        EnterVehicleAction(Vehicle *pVehicle);

    protected:
        void doStart(Mission *pMission, PedInstance *pPed);
        bool doExecute(int elapsed, Mission *pMission, PedInstance *pPed);
    protected:
        /*! The vehicle to enter.*/
        Vehicle *pVehicle_;
    };

    /*!
     * This action is used to drive a vehicle to a point.
     */
    class DriveVehicleAction : public MovementAction {
    public:
        DriveVehicleAction(CreatOrigin origin, VehicleInstance *pVehicle, PathNode &dest);

    protected:
        void doStart(Mission *pMission, PedInstance *pPed);
        bool doExecute(int elapsed, Mission *pMission, PedInstance *pPed);
    protected:
        /*! Vehicle to drive.*/
        VehicleInstance *pVehicle_;
        /*! Destination point.*/
        PathNode dest_;
    };

    /*!
     * This action is used to shoot. A ped can only have a single shoot action
     * at a given time.
     */
    class ShootAction : public Action {
    public:
        ShootAction(CreatOrigin origin, PathNode &aimedAt);

        //! Entry point to execute the action
        bool execute(int elapsed, Mission *pMission, PedInstance *pPed);
        //! Stop shooting (mainly used with AutomaticShootAction)
        virtual void stop() {};
    protected:
        bool doShoot(int elapsed, Mission *pMission, PedInstance *pPed);
    protected:
        //! Where the player aimed with the mouse
        PathNode aimedAt_;
        //! Time to wait between two shoot actions
        int timeToWait_;
    };

    /*!
     * This action is used to shoot with automatic gun.
     */
    class AutomaticShootAction : public ShootAction {
    public:
        AutomaticShootAction(CreatOrigin origin, PathNode &dest);

        void stop();
    protected:
        bool doExecute(int elapsed, Mission *pMission, PedInstance *pPed);

    };

    /*!
     * A behaviour is used to determine default actions for ped.
     */
    class Behaviour {
        public:
        virtual void execute(Mission *pMission, PedInstance *pPed) = 0;
    };

    class NopeBehaviour : public Behaviour {
    public:
        void execute(Mission *pMission, PedInstance *pPed);
    };
}

#endif // IA_ACTIONS_H_
