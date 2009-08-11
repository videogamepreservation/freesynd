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
#include "musicmanager.h"
#include "xmidi.h"
#include "config.h"
#include "file.h"

#ifndef HAVE_SDL_MIXER_1_2_7
#ifdef _WIN32
#pragma message("Music playback requires SDL_mixer 1.2.7 or later to function")
#else
#warning "Music playback requires SDL_mixer 1.2.7 or later to function"
#endif
#endif

MusicManager::MusicManager():is_playing_(false)
{
}

MusicManager::~MusicManager()
{
#ifdef HAVE_SDL_MIXER_1_2_7
    for (unsigned int i = 0; i < tracks_.size(); ++i) {
        delete tracks_[i];
    }
    tracks_.clear();
#endif
}

void MusicManager::loadMusic()
{
#ifdef HAVE_SDL_MIXER_1_2_7
    XMidi xmidi;
    std::vector < XMidi::Midi > tracks;
    int size;
    uint8 *data;

#if USE_INTRO_MP3
    tracks_.push_back(new Music);
#if USE_OGG_INSTEAD_OF_MP3
	tracks_.back()->loadMusicFile("intro.ogg");
#else
	tracks_.back()->loadMusicFile("intro.mp3");
#endif
#else
    data = File::loadFile("INTRO.XMI", size);
    tracks = xmidi.convertXMidi(data, size);
    for (unsigned int i = 0; i < tracks.size(); ++i) {
        tracks_.push_back(new Music);
        tracks_.back()->loadMusic(tracks[i].data_, tracks[i].size_);
    }
    delete[] data;
#endif

    data = File::loadFile("SYNGAME.XMI", size);
    tracks = xmidi.convertXMidi(data, size);
    for (unsigned int i = 0; i < tracks.size(); ++i) {
		if (i == 0) {
#if USE_ASSASSINATE_MP3
			tracks_.push_back(new Music);
#if USE_OGG_INSTEAD_OF_MP3
			tracks_.back()->loadMusicFile("assassinate.ogg");
#else
			tracks_.back()->loadMusicFile("assassinate.mp3");
#endif
#else
			tracks_.push_back(new Music);
			tracks_.back()->loadMusic(tracks[i].data_, tracks[i].size_);
#endif
		} else {
			tracks_.push_back(new Music);
			tracks_.back()->loadMusic(tracks[i].data_, tracks[i].size_);
		}
    }
    delete[] data;
#endif
}

void MusicManager::playTrack(MusicTrack track, int loops)
{
#ifdef HAVE_SDL_MIXER_1_2_7
    if (is_playing_) {
        tracks_.at(current_track_)->stopFadeOut();
    }
    tracks_.at(track)->play(loops);
    current_track_ = track;
    is_playing_ = true;
#endif
}

void MusicManager::stopPlayback()
{
#ifdef HAVE_SDL_MIXER_1_2_7
    tracks_.at(current_track_)->stop();
    is_playing_ = false;
#endif
}
