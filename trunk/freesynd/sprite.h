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

#ifndef SPRITE_H
#define SPRITE_H

#include "common.h"

const int TABENTRY_SIZE = 6;

/*!
 * Sprite class.
 */
class Sprite {
    int width_;
    int height_;
    int stride_;
    uint8 *sprite_data_;

public:
    enum Format {
        FMT_BLOCKS,
        FMT_RLE
    };

    Sprite();
    virtual ~Sprite();

    void loadSpriteFromPNG(const char *filename);
    bool loadSprite(uint8 *tabData, uint8 *spriteData, uint32 offset,
            bool rle = false);
    void draw(int x, int y, int z, bool flipped = false, bool x2 = false);

    int width() const { return width_; }
    int height() const { return height_; }

    void data(uint8 *spr_data) const;
};

#endif
