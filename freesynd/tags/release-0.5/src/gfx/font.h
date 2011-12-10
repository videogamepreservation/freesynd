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

#ifndef FONT_H
#define FONT_H

#include "common.h"
#include "spritemanager.h"
#include <map>

/*!
 * Font class.
 */
class Font {
public:
    Font() {}
    virtual ~Font() {}

    void setSpriteManager(SpriteManager *sprites, int offset = 0,
            char base = '!');
    void drawText(int x, int y, const char *text, bool x2 = true);
    int textWidth(const char *text, bool x2 = true);
    int textHeight(bool x2 = true);

protected:
    SpriteManager *sprites_;
    int offset_;
    char base_;
};

class HChar {
public:
    HChar();
    ~HChar();

    void set(int w, int h, uint8 *data);

    int draw(int x, int y, uint8 color);

protected:
    int width_;
    int height_;
    bool *bits_;
};

class HFont {
public:
    HFont();
    ~HFont();

    void load();

    void drawText(int x, int y, const char *str, uint8 color);

protected:
    std::map<char, HChar> characters;
};

#endif
