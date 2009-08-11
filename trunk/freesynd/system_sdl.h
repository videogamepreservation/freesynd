/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
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

#ifndef SYSTEM_SDL_H
#define SYSTEM_SDL_H

#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif

#include "common.h"
#include "system.h"

/*!
 * System SDL class.
 */
class SystemSDL : public System {
  public:
    SystemSDL(int depth = 32);
    ~SystemSDL();

    void updateScreen();
    void handleEvents();
    void delay(int msec);

    void setPalette6b3(const uint8 *pal, int cols = 256);
    void setPalette8b3(const uint8 *pal, int cols = 256);
    void setColor(uint8 index, uint8 r, uint8 g, uint8 b);

    void hideCursor();
    void showCursor();
    void usePointerCursor();
    void useTargetCursor();
    void usePickupCursor();

  protected:
    int depth_;
    bool scale_;

    SDL_Surface *screen_surf_;
    SDL_Surface *temp_surf_;
};

#endif
