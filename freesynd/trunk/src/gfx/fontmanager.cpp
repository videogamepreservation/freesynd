/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2011  Joey Parrish  <joey.parrish@gmail.com>         *
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
        menuFonts_[i] = NULL;
}

FontManager::~FontManager()
{
    for (int i = 0; i < 4; i++) {
        delete menuFonts_[i];
    }
}

bool FontManager::loadFont(SpriteManager * sprites, EFontSize size,
                           int darkOffset, int lightOffset, char base, const std::string& valid_chars)
{
    assert(sprites);

	menuFonts_[size] = new Font();
    menuFonts_[size]->setSpriteManager(sprites, darkOffset, lightOffset, base, valid_chars);

    return true;
}

void FontManager::drawText(int x, int y, const char *text, bool dos, EFontSize size,
                           bool dark, bool x2,
                           bool changeColor, uint8 fromColor, uint8 toColor)
{
    assert(menuFonts_[size]);

    menuFonts_[size]->drawText(x, y, text, dos, !dark, x2, changeColor, fromColor, toColor);
}

int FontManager::textWidth(const char *text, bool dos, EFontSize size, bool x2)
{
    return menuFonts_[size]->textWidth(text, dos, x2);
}

int FontManager::textHeight(EFontSize size, bool x2)
{
    return menuFonts_[size]->textHeight(x2);
}
