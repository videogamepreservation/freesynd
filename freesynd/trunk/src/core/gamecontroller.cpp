/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2010  Benoit Blancard <benblan@users.sourceforge.net>*
 *   Copyright (C) 2010  Bohdan Stelmakh <chamel@users.sourceforge.net> *
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

#include "core/gamecontroller.h"

GameController::GameController() {
}

GameController::~GameController() {
    game_listeners_.clear();
    mission_listeners_.clear();
}

bool GameController::reset() {
    return false;
}

/*! 
 * Adds a listener to the list of listeners for a stream.
 * \param pListener The listener
 * \param stream The stream
 */
void GameController::addListener(GameEventListener *pListener, GameEvent::EEventStream stream) {
    if (pListener) {
        if (stream == GameEvent::kGame) {
            // Check if listener has already subscribed
            for (std::list < GameEventListener * >::iterator it = game_listeners_.begin();
             it != game_listeners_.end(); it++) {
                 if (pListener == *it) {
                     return;
                 }
            }
            game_listeners_.push_back(pListener);
        } else if (stream == GameEvent::kMission) {
            // Check if listener has already subscribed
            for (std::list < GameEventListener * >::iterator it = mission_listeners_.begin();
             it != mission_listeners_.end(); it++) {
                 if (pListener == *it) {
                     return;
                 }
            }
            mission_listeners_.push_back(pListener);
        }
    }
}

//! Removes the listener from the given stream of events
void GameController::removeListener(GameEventListener *pListener, GameEvent::EEventStream stream) {
    if (pListener) {
        if (stream == GameEvent::kGame) {
            for (std::list < GameEventListener * >::iterator it = game_listeners_.begin();
             it != game_listeners_.end(); it++) {
                 if (pListener == *it) {
                     game_listeners_.erase(it);
                     return;
                 }
            }
        } else if (stream == GameEvent::kMission) {
            for (std::list < GameEventListener * >::iterator it = mission_listeners_.begin();
             it != mission_listeners_.end(); it++) {
                 if (pListener == *it) {
                     mission_listeners_.erase(it);
                     return;
                 }
            }
        }
    }
}
//! Sends the event to the listeners
void GameController::fireGameEvent(GameEvent & evt) {
    switch (evt.stream) {
    case GameEvent::kGame:
        for (std::list < GameEventListener * >::iterator it = game_listeners_.begin();
                        it != game_listeners_.end(); it++) {
            (*it)->handleGameEvent(evt);
        }
        break;
    case GameEvent::kMission:
        for (std::list < GameEventListener * >::iterator it = mission_listeners_.begin();
                        it != mission_listeners_.end(); it++) {
            (*it)->handleGameEvent(evt);
        }
        break;
    }
}