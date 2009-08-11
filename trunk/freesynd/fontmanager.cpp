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
bool FontManager::loadFont(SpriteManager * sprites, int size, bool dark,
                           int offset, char base)
{
    assert(sprites);
    assert(size < 4);

    if (dark) {
        dark_fonts_[size] = new Font();
        dark_fonts_[size]->setSpriteManager(sprites, offset, base);
    } else {
        light_fonts_[size] = new Font();
        light_fonts_[size]->setSpriteManager(sprites, offset, base);
    }

    return true;
}

void FontManager::loadWidgets(SpriteManager * sprites)
{
    assert(sprites);

    dark_widgets_[0] = sprites->sprite(7);
    light_widgets_[0] = sprites->sprite(10);
    dark_widgets_[1] = sprites->sprite(5);
    light_widgets_[1] = sprites->sprite(8);
    dark_widgets_[2] = sprites->sprite(6);
    light_widgets_[2] = sprites->sprite(9);

    light_widgets_[3] = sprites->sprite(1);
    light_widgets_[4] = sprites->sprite(2);
    light_widgets_[5] = sprites->sprite(3);
    light_widgets_[6] = sprites->sprite(4);
}

void FontManager::drawText(int x, int y, const char *text, int size,
                           bool dark, bool x2)
{
    int sc = x2 ? 2 : 1;
    assert(size < 4);
    if (dark) {
        assert(dark_fonts_[size]);
        if (*text >= '\001' && *text <= '\003') {
            Sprite *widget = dark_widgets_[*text - 1];
            widget->draw(x, y - 2 * sc, 0, false, true);
            text++;
            x += widget->width() * sc + 4 * sc;
        }
        dark_fonts_[size]->drawText(x, y, text, x2);
    } else {
        assert(light_fonts_[size]);
        if (*text >= '\001' && *text <= '\007') {
            Sprite *widget = light_widgets_[*text - 1];
            widget->draw(x, y - 4, 0, false, true);
            text++;
            x += widget->width() * sc + 4 * sc;
        }
        light_fonts_[size]->drawText(x, y, text, x2);
    }
}

int FontManager::textWidth(const char *text, int size, bool x2)
{
    return dark_fonts_[size]->textWidth(text, x2);
}

int FontManager::textHeight(int size, bool x2)
{
    return dark_fonts_[size]->textHeight(x2);
}
