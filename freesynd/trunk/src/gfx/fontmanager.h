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

#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include "common.h"
#include "font.h"

class SpriteManager;

/*!
 * Font manager class.
 */
class FontManager {
public:
    /*! Size of font : 1 is the smaller.*/
    enum EFontSize {
        SIZE_1 = 0,
        SIZE_2 = 1,
        SIZE_3 = 2,
        SIZE_4 = 3
    };

    FontManager();
    ~FontManager();

    bool loadFont(SpriteManager *sprites, EFontSize size, int darkOffset, int lightOffset,
            char base, const std::string& valid_chars);

    void drawText(int x, int y, const char *text, bool dos, EFontSize size, bool dark,
            bool x2 = true, bool changeColor = false, uint8 fromColor = 0, uint8 toColor = 0);
    int textWidth(const char *text, bool dos, EFontSize size, bool x2 = true);
    int textHeight(EFontSize size, bool x2 = true);

protected:
    Font * menuFonts_[4];
};

#endif
