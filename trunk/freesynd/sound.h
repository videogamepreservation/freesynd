/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
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

#ifndef SOUND_H
#define SOUND_H

#include "common.h"
#include "audio.h"

#ifdef HAVE_SDL_MIXER

/*!
 * Sound class.
 */
class Sound : public Audio {
public:
    Sound();
    ~Sound();

    void play(int loops = 0) const;
    void stop() const;
    bool setVolume(int volume);
    bool loadSound(uint8 *soundData, uint32 size);

	// These enum values match the indices in the vector containing the samples
    // so don't mess up the order in which they are in.
    enum InGameSample {
		SHOTGUN = 0,
        PISTOL,
        LASER,
        FLAME,
        FLAMING_DEATH,
        GLASS_BREAKING,
        EXPLOSION,
        UZI,
        LONGRANGE,
        MINIGUN,
        PERSUADE,
        TRACKING_PONG,
        SPEECH_SELECTED,    // seems a little fast
                            // BS: yeah, wonder if the sample rate data is
                            // stored in the "bogus" sounds in the data file?
        GAUSSGUN,
        SPEECH_MISSION_COMPLETED,
        SPEECH_MISSION_FAILED,
        DOOR,
        TIMEBOMB,
        DOOR_2,
        MENU_UP,
        MENU_CHANGE,
        FIREWORKS,
        SPEECH_NO,
        UNRECOGNIZED_9,
        UNRECOGNIZED_10,   // sounds like glass breaking?
        APPLAUSE,
        APPLAUSE_ZOOM,
        FIREWORKS_APPLAUSE,
        EXPLOSION_BIG,
        MENU_AFTER_MISSION,
        UNRECOGNIZED_12,
        NO_SOUND = -1
    };

protected:
     Mix_Chunk *sound_data_;
};

#endif                          // HAVE_SDL_MIXER

#endif
