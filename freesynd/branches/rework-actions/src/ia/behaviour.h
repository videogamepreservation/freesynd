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

#include "utils/timer.h"

class Mission;
class PedInstance;
/*!
 * A behaviour is used to determine default actions for ped.
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
        kBehvEvtAgentHit
    };

    Behaviour(PedInstance *pPed) { pThisPed_ = pPed; }
    virtual ~Behaviour() {}

    virtual void execute(int elapsed, Mission *pMission) = 0;

    virtual void handleBehaviourEvent(BehaviourEvent evtType) = 0;

protected:
    /*! The ped that use this behaviour.*/
    PedInstance *pThisPed_;
};

/*!
 * This behaviour if for development purpose.
 */
class NopeBehaviour : public Behaviour {
public:
    NopeBehaviour(PedInstance *pPed) : Behaviour(pPed) {}

    void execute(int elapsed, Mission *pMission) {}

    void handleBehaviourEvent(BehaviourEvent evtType) {}
};

/*!
 * This class defines commons behaviours for all agents (good or bad).
 */
class AgentBehaviour : public Behaviour {
public:
    //! Distance under whom an agent is close enough to persuade a ped
    static const int kMaxDistanceForPersuadotron;
    //! Amount of health regenerated each period
    static const int kRegeratesHealthStep;

    AgentBehaviour(PedInstance *pPed);

    void execute(int elapsed, Mission *pMission);

    void handleBehaviourEvent(BehaviourEvent evtType);

protected:
    /*! Flag to indicate whether ped can regenerate his health.*/
    bool doRegenerates_;
    /*! Flag to indicate an agent can use his persuadotron.*/
    bool doUsePersuadotron_;
    //! used for health regeneration
    fs_utils::Timer healthTimer_;
};

#endif // IA_BEHAVIOUR_H_