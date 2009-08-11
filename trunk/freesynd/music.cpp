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

#include "music.h"

#ifdef HAVE_SDL_MIXER
Music::Music():music_data_(NULL),rw_(NULL)
{
}

Music::~Music()
{
    if (music_data_) {
        Mix_FreeMusic(music_data_);
    }

    if (rw_)
        SDL_FreeRW(rw_);
}

void Music::play(int loops) const
{
    Mix_PlayMusic(music_data_, loops);
}

void Music::playFadeIn(int loops, int ms) const
{
    Mix_FadeInMusic(music_data_, loops, ms);
}

void Music::stop() const
{
    Mix_HaltMusic();
    Mix_RewindMusic();
}

void Music::stopFadeOut(int ms) const
{
    while (!Mix_FadeOutMusic(ms) && Mix_PlayingMusic()) {
        SDL_Delay(100);
    }
    Mix_RewindMusic();
}

void Music::setVolume(int volume)
{
    Mix_VolumeMusic(volume);
}

bool Music::loadMusic(uint8 * musicData, int size)
{
    SDL_RWops *rw = SDL_RWFromMem(musicData, size);
    if (!rw) {
        error("Failed creating SDL_RW buffer from memory");
        return false;
    }
    Mix_Music *newmusic = Mix_LoadMUS_RW(rw);

    if (!newmusic) {
        error("Failed loading music from SDL_RW buffer");
        SDL_FreeRW(rw);
        return false;
    }
    if (music_data_)
        Mix_FreeMusic(music_data_);

    if (rw_) {
        SDL_FreeRW(rw_);
        rw_ = NULL;
    }

    music_data_ = newmusic;
    rw_ = rw;

    return true;
}

bool Music::loadMusicFile(const char *fname)
{
	Mix_Music *newmusic = Mix_LoadMUS(fname);

	if (!newmusic) {
        error("Failed loading music from file");
        return false;
	}
    if (music_data_)
        Mix_FreeMusic(music_data_);

    if (rw_) {
        SDL_FreeRW(rw_);
        rw_ = NULL;
    }

    music_data_ = newmusic;

    return true;
}


#endif                          // HAVE_SDL_MIXER
