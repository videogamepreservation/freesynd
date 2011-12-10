/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2011  Joey Parrish  <joey.parrish@gmail.com>         *
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
#ifndef AGENTMANAGER_H
#define AGENTMANAGER_H

#include "common.h"
#include "agent.h"
#include "utils/seqmodel.h"
#include "utils/portablefile.h"

extern const char * const g_AgentNames[];
extern const int g_NumAgentNames;

/*!
 * Agent Manager class.
 *
 * Provides methods for managing player's agents.
 */
class AgentManager {
public:
    /*! Max number of agents in cryo chamber.*/
    static const int MAX_AGENT;

    AgentManager();
    ~AgentManager();

    void loadAgents();
    void reset(bool onlyWomen = false);
    void destroyAgentSlot(int n);

    Agent *agent(int n) {
        assert(n < MAX_AGENT);
        return agents_.get(n);
    }

    SequenceModel * getAgents() { return &agents_; }

    //! Save instance to file
    bool saveToFile(PortableFile &file);
    //! Load instance from file
    bool loadFromFile(PortableFile &infile, const FormatVersion& v);

protected:
    /*!
     * All available agents.
     */
    VectorModel<Agent *> agents_;
    int nextName_;
};

#endif
