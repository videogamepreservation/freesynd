/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2013  Bohdan Stelmakh <chamel@users.sourceforge.net> *
 *   Copyright (C) 2014  Benoit Blancard <benblan@users.sourceforge.net>*
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

#ifndef IA_BEHAVIOUR_H_
#define IA_BEHAVIOUR_H_

#include <list>

#include "utils/timer.h"

class Mission;
class PedInstance;
class BehaviourComponent;

/*!
 * A Behaviour drives the ped's reactions.
 * It is composed of a set of components, each one 
 * responsible for an aspect of the reaction.
 * Behaviour reacts to events and modify the ped's
 * actions.
 */
class Behaviour {
public:
    /*!
     * List of events that may affect behaviours.
     */
    enum BehaviourEvent {
        //! An agent has activated his Persuadotron
        kBehvEvtPersuadotronActivated,
        //! An agent has deactivated his Persuadotron
        kBehvEvtPersuadotronDeactivated,
        //! An ped has been hit
        kBehvEvtHit
    };

    virtual ~Behaviour();

    void setOwner(PedInstance *pPed) { pThisPed_ = pPed; }
    //! Adds a component to the behaviour
    void addComponent(BehaviourComponent *pComp);

    virtual void execute(int elapsed, Mission *pMission);

    virtual void handleBehaviourEvent(BehaviourEvent evtType);
protected:
    /*! The ped that use this behaviour.*/
    PedInstance *pThisPed_;
    /*! List of behaviour components.*/
    std::list <BehaviourComponent *> compLst_;
};

/*!
 * Abstract class that represent an aspect of a behaviour.
 * A component may be disabled according to certain types of events.
 */
class BehaviourComponent {
public:
    BehaviourComponent() { enabled_ = true; }
    virtual ~BehaviourComponent() {}

    bool isEnabled() { return enabled_; }
    void setEnabled(bool val) { enabled_ = val; }

    virtual void execute(int elapsed, Mission *pMission, PedInstance *pPed) = 0;

    virtual void handleBehaviourEvent(Behaviour::BehaviourEvent evtType, PedInstance *pPed){};

protected:
    bool enabled_;
};

/*!
 * This class defines commons behaviours for all agents (good or bad).
 * It is responsible for :
 * - regenerating life for agents that are hurt
 */
class CommonAgentBehaviourComponent : public BehaviourComponent {
public:
    //! Amount of health regenerated each period
    static const int kRegeratesHealthStep;

    CommonAgentBehaviourComponent(PedInstance *pPed);

    void execute(int elapsed, Mission *pMission, PedInstance *pPed);

    void handleBehaviourEvent(Behaviour::BehaviourEvent evtType, PedInstance *pPed);
private:
    /*! Flag to indicate whether ped can regenerate his health.*/
    bool doRegenerates_;
    //! used for health regeneration
    fs_utils::Timer healthTimer_;
};

/*!
 * Component for user of Persuadotron. Only our agents use this component.
 */
class PersuaderBehaviourComponent : public BehaviourComponent {
public:
    //! Amount of health regenerated each period
    static const int kMaxDistanceForPersuadotron;

    PersuaderBehaviourComponent();

    void execute(int elapsed, Mission *pMission, PedInstance *pPed);

    void handleBehaviourEvent(Behaviour::BehaviourEvent evtType, PedInstance *pPed);
private:
    /*! Flag to indicate an agent can use his persuadotron.*/
    bool doUsePersuadotron_;
};

#endif // IA_BEHAVIOUR_H_