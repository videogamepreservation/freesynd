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

#include <stdio.h>
#include <assert.h>
#include "fontmanager.h"

FontManager::FontManager()
{
    for (int i = 0; i < 4; i++)
        dark_fonts_[i] = light_fonts_[i] = NULL;
}

FontManager::~FontManager()
{
    for (int i = 0; i < 4; i++) {
        delete dark_fonts_[i];
        delete light_fonts_[i];
    }
}

bool FontManager::loadFont(SpriteManager * sprites, EFontSize size, bool dark,
                           int offset, char base)
{
    assert(sprites);

    if (dark) {
        dark_fonts_[size] = new Font();
        dark_fonts_[size]->setSpriteManager(sprites, offset, base);
    } else {
        light_fonts_[size] = new Font();
        light_fonts_[size]->setSpriteManager(sprites, offset, base);
    }

    return true;
}

void FontManager::drawText(int x, int y, const char *text, int size,
                           bool dark, bool x2)
{
    assert(size < 4);
    if (dark) {
        assert(dark_fonts_[size]);

        dark_fonts_[size]->drawText(x, y, text, x2);
    } else {
        assert(light_fonts_[size]);

        light_fonts_[size]->drawText(x, y, text, x2);
    }
}

void FontManager::drawText(int x, int y, const char *text, EFontSize size,
                           bool dark, bool x2)
{
    if (dark) {
        assert(dark_fonts_[size]);

        dark_fonts_[size]->drawText(x, y, text, x2);
    } else {
        assert(light_fonts_[size]);

        light_fonts_[size]->drawText(x, y, text, x2);
    }
}

int FontManager::textWidth(const char *text, EFontSize size, bool x2)
{
    return dark_fonts_[size]->textWidth(text, x2);
}

int FontManager::textHeight(EFontSize size, bool x2)
{
    return dark_fonts_[size]->textHeight(x2);
}
