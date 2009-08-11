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

#ifndef SYSTEM_H
#define SYSTEM_H

#include "common.h"

// This file contains the abstract interface that all systems/ports should implement.

class Sprite;

struct System : public Singleton<System> {
    virtual ~System() {}
    virtual void updateScreen() = 0;
    virtual void handleEvents() = 0;
    virtual void delay(int msec) = 0;

    virtual void setPalette6b3(const uint8 *pal, int cols = 256) = 0;
    virtual void setPalette8b3(const uint8 *pal, int cols = 256) = 0;
    virtual void setColor(uint8 index, uint8 r, uint8 g, uint8 b) = 0;

    virtual void hideCursor() = 0;
    virtual void showCursor() = 0;
    virtual void usePointerCursor() = 0;
    virtual void useTargetCursor() = 0;
    virtual void usePickupCursor() = 0;

};

#define g_System    System::singleton()

#endif
