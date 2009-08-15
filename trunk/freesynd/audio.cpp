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
#include "audio.h"
#include <iostream>

#ifdef HAVE_SDL_MIXER

bool Audio::quit(void) {
    if (Mix_QuerySpec(0, 0, 0) > 0) {
        Mix_HaltMusic();
        Mix_HaltChannel(-1);
        Mix_CloseAudio();
        return true;
    }
    return false;
}

bool Audio::init(int frequency, Uint16 format, int channels, int chunksize) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        error("Failed initialize SDL Audio module.");
        return false;
    }

    if (Mix_QuerySpec(0, 0, 0) > 0) {
        error("SDL Mixer has already been initialized.");
        return true;
    }

    if (Mix_OpenAudio(frequency, format, channels, chunksize) < 0) {
        error("Failed to initialize SDL Mixer.");
        return false;
    }
    return true;
}

void Audio::error(std::string const &message) {
    std::cerr << "SDL_Mixer error: " << Mix_GetError() << std::endl
        << "Message: " << message << std::endl;
}

#endif                          // HAVE_SDL_MIXER
