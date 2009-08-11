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
#ifndef AUDIO_H
#define AUDIO_H

#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif
#include <string>
#include "common.h"
#include "config.h"

#ifdef HAVE_SDL_MIXER
#ifdef _WIN32
#include <SDL_mixer.h>
#else
#include <SDL/SDL_mixer.h>
#endif

/*!
 * Audio configuration class.
 */
class Audio {
  public:
    static bool init(int frequency = MIX_DEFAULT_FREQUENCY,
                     Uint16 format = MIX_DEFAULT_FORMAT,
                     int channels = MIX_DEFAULT_CHANNELS,
                     int chunksize = 1024);
    static bool quit();
    static void error(std::string const &message);
};

#endif                          // HAVE_SDL_MIXER

#endif
