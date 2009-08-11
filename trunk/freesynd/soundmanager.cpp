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
#include "soundmanager.h"
#include "file.h"
#include "config.h"

SoundManager::SoundManager():tabentry_startoffset_(58), tabentry_offset_(32)
{
}

SoundManager::~SoundManager()
{
	for(std::vector< Sound * >::iterator it = sounds_.begin(); it != sounds_.end(); ++it)
	{
		delete *it;
	}
}

Sound *SoundManager::sound(int n)
{
    return sounds_.at(n);
}

Sound *SoundManager::sound(Sound::InGameSample sample)
{
	if (sample == Sound::NO_SOUND)
		return NULL;
    return sounds_.at(sample);
}


void SoundManager::loadSounds(SampleSet set)
{
#ifdef HAVE_SDL_MIXER
    int tabSize, size;
    uint8 *tabData, *data;
    switch (set) {
    case SAMPLES_INTRO:
        {
            tabData = File::loadFile("ISNDS-0.TAB", tabSize);
            data = File::loadFile("ISNDS-0.DAT", size);
            loadSounds(tabData, tabSize, data);
            delete[] tabData;
            delete[] data;
            tabData = File::loadFile("ISNDS-1.TAB", tabSize);
            data = File::loadFile("ISNDS-1.DAT", size);
            loadSounds(tabData, tabSize, data);
            delete[] tabData;
            delete[] data;
        }
        break;
    case SAMPLES_GAME:
        {
            tabData = File::loadFile("SOUND-0.TAB", tabSize);
            data = File::loadFile("SOUND-0.DAT", size);
            loadSounds(tabData, tabSize, data);
            delete[] tabData;
            delete[] data;
            tabData = File::loadFile("SOUND-1.TAB", tabSize);
            data = File::loadFile("SOUND-1.DAT", size);
            loadSounds(tabData, tabSize, data);
            delete[] tabData;
            delete[] data;
        }
        break;
    default:
        break;
    }
#endif
}

bool SoundManager::loadSounds(uint8 * tabData, int tabSize,
                              uint8 * soundData)
{
#ifdef HAVE_SDL_MIXER
    assert(tabData);
    assert(soundData);
    tabData += tabentry_startoffset_;

    int soundnum = 0;
    uint32 offset = 0;

    for (int i = 0;
         i < tabSize - (2 * tabentry_offset_); i += tabentry_offset_) {
        uint32 soundsize = READ_LE_UINT32(tabData);

        // Samples with size < 144 are bogus
        if (soundsize > 144) {
            ++soundnum;
            sounds_.push_back(new Sound());
            uint8 *sample = new uint8[soundsize];
            memcpy(sample, soundData, soundsize);
            sounds_.back()->loadSound(sample, soundsize);
        }
        soundData += soundsize;
        offset += soundsize;

        tabData += tabentry_offset_;
    }
    return true;
#else
    return false;
#endif
}
