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

#include "sound.h"

#ifdef HAVE_SDL_MIXER

Sound::Sound():sound_data_(NULL)
{
}

Sound::~Sound()
{
    if (sound_data_) {
        Mix_FreeChunk(sound_data_);
    }
}



void Sound::play(int loops) const
{
    int ret = Mix_PlayChannel(0, sound_data_, loops);
    if (ret < 0) {
        error("Failed to play sound on channel 0.");
    }
}

void Sound::stop() const
{
    Mix_HaltChannel(0);
}

bool Sound::setVolume(int volume)
{
    int ret = Mix_VolumeChunk(sound_data_, volume);
    if (ret < 0) {
        error("Failed setting volume on Sound.");
        return false;
    }
    return true;
}


bool Sound::loadSound(uint8 * soundData, uint32 size)
{
    SDL_RWops *rw = SDL_RWFromMem(soundData, size);
    if (!rw) {
        error("Failed creating SDL_RW buffer from memory");
        return false;
    }
    Mix_Chunk *newsound = Mix_LoadWAV_RW(rw, 1);

    if (!newsound) {
        error("Failed loading sound from SDL_RW buffer");
        return false;
    }
    if (sound_data_ != 0)
        Mix_FreeChunk(sound_data_);
    sound_data_ = newsound;
    return true;
}

#endif                          // HAVE_SDL_MIXER
