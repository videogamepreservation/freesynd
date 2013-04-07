/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2011  Benoit Blancard <benblan@users.sourceforge.net>*
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

#ifndef GAMEVENT_H
#define GAMEVENT_H

class GameEvent {
public:
    enum EEventType {
        kNone,
        GE_AGENT,
        GE_SEARCH,
        /*! Sent when evacuation order arrives.*/
        kObjEvacuate,
        /*! Sent when an objective with a MapObject target arrives.*/
        kObjTargetSet,
        /*! Sent when an objective with a MapObject target is finished.*/
        kObjTargetCleared,
        /*! Send when all the objectives of a mission are completed.*/
        kObjSucceed
    };

    EEventType type_;
    void *pCtxt_;
};

class GameEventListener {
public:
    virtual ~GameEventListener() {}
    virtual void handleGameEvent(GameEvent evt) = 0;
};

#endif //GAMEVENT_H
