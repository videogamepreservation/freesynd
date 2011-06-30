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

#include "utils/file.h"
#include "font.h"
#include "screen.h"

void Font::setSpriteManager(SpriteManager *sprites, int offset, char base) {
    sprites_ = sprites;
    offset_ = offset;
    base_ = base;
}

void Font::drawText(int x, int y, const char *text, bool x2) {
    int sc = x2 ? 2 : 1;
    int ox = x;
    for (const char *c = text; *c; ++c) {
        if (*c == ' ') {
            x += sprites_->sprite('A' - base_ + offset_)->width() * sc - sc;
            continue;
        }
        if (*c == '\n') {
            x = ox;
            y += textHeight() - sc;
            continue;
        }
        Sprite *s = sprites_->sprite(*c - base_ + offset_);
        if (s) {
            if (*c == ':')
                s->draw(x, y + sc, 0, false, x2);
            else if (*c == '.')
                s->draw(x, y + 4 * sc, 0, false, x2);
            else if (*c == '-')
                s->draw(x, y + 2 * sc, 0, false, x2);
            else
                s->draw(x, y, 0, false, x2);
            x += s->width() * sc - sc;
        }
    }
}

int Font::textWidth(const char *text, bool x2) {
    int sc = x2 ? 2 : 1;
    int x = 0;
    for (const char *c = text; *c; ++c) {
        if (*c == ' ') {
            x += sprites_->sprite('A' - base_ + offset_)->width() * sc - sc;
            continue;
        }
        Sprite *s = sprites_->sprite(*c - base_ + offset_);
        if (s) {
            x += s->width() * sc - sc;
        }
    }
    return x;
}

int Font::textHeight(bool x2) {
    int sc = x2 ? 2 : 1;
    return sprites_->sprite('A' - base_ + offset_)->height() * sc;
}

HChar::HChar():width_(0), height_(0), bits_(0) {
}

HChar::~HChar() {
    if (bits_)
        delete[] bits_;
}

void HChar::set(int w, int h, uint8 *data) {
    width_ = w;
    height_ = h;
    if (data) {
        int stride = w < 9 ? 1 : 2;
        bits_ = new bool[width_ * height_];
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                if (data[y * stride + (x / 8)] & (128 >> (x % 8)))
                    bits_[width_ * y + x] = true;
                else
                    bits_[width_ * y + x] = false;
            }
        }
    }
}

int HChar::draw(int x, int y, uint8 color) {
    if (bits_) {
        for (int j = 0; j < height_; j++) {
            for (int i = 0; i < width_; i++) {
                if (bits_[width_ * j + i])
                    g_Screen.setPixel(x + i, y + j, color);
            }
        }
    }
    return width_;
}

HFont::HFont() {

}

HFont::~HFont() {

}

void HFont::load() {
    int size;
    uint8 *data = File::loadOriginalFile("hfnt01.dat", size);
    for (int i = 0; i < 128; i++) {
        if (data[i * 5] || data[i * 5 + 1]) {
            if (data[i * 5] == 0xff && data[i * 5 + 1] == 0xff)
                characters[i].set(data[i * 5 + 2], data[i * 5 + 3], NULL);
            else
                characters[i].set(data[i * 5 + 2], data[i * 5 + 3],
                                  data +
                                  (data[i * 5] | (data[i * 5 + 1] << 8)));
        }
    }
    delete[] data;
}

void HFont::drawText(int x, int y, const char *str, uint8 color) {
    while (*str) {
        if (characters.find(*str) != characters.end())
            x += characters[*str].draw(x, y, color);

        str++;
    }
}
