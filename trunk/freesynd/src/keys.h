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

#ifndef KEYS_H
#define KEYS_H

/*! 
 * All available key codes.
 */
enum Key {
    KEY_UNKNOWN = 0,
    KEY_ESCAPE = 1,
    KEY_BACKSPACE = 2,
    KEY_SPACE = 3,

    KEY_BACKQUOTE = 4,
    KEY_PLUS = 5,
    KEY_COMMA = 6,
    KEY_MINUS = 7,
    KEY_PERIOD = 8,
    KEY_SLASH = 9,

    // Arrows + Home/End pad
    KEY_UP = 10,
    KEY_DOWN = 11,
    KEY_RIGHT = 12,
    KEY_LEFT = 13,
    KEY_INSERT = 14,
    KEY_HOME = 15,
    KEY_END = 16,
    KEY_PAGEUP = 17,
    KEY_PAGEDOWN = 18,

    // Function keys
    KEY_F1 = 19,
    KEY_F2 = 20,
    KEY_F3 = 21,
    KEY_F4 = 22,
    KEY_F5 = 23,
    KEY_F6 = 24,
    KEY_F7 = 25,
    KEY_F8 = 26,
    KEY_F9 = 27,
    KEY_F10 = 28,
    KEY_F11 = 29,
    KEY_F12 = 30,

    KEY_0 = 40,
    KEY_1 = 41,
    KEY_2 = 42,
    KEY_3 = 43,
    KEY_4 = 44,
    KEY_5 = 45,
    KEY_6 = 46,
    KEY_7 = 47,
    KEY_8 = 48,
    KEY_9 = 49,

    KEY_a = 50,
    KEY_b = 51,
    KEY_c = 52,
    KEY_d = 53,
    KEY_e = 54,
    KEY_f = 55,
    KEY_g = 56,
    KEY_h = 57,
    KEY_i = 58,
    KEY_j = 59,
    KEY_k = 60,
    KEY_l = 61,
    KEY_m = 62,
    KEY_n = 63,
    KEY_o = 64,
    KEY_p = 65,
    KEY_q = 66,
    KEY_r = 67,
    KEY_s = 68,
    KEY_t = 69,
    KEY_u = 70,
    KEY_v = 71,
    KEY_w = 72,
    KEY_x = 73,
    KEY_y = 74,
    KEY_z = 75
};

//! Valid key modifiers
enum KeyMod {
    KMD_NONE = 0x0000,
    KMD_LSHIFT = 0x0001,
    KMD_RSHIFT = 0x0002,
    KMD_LCTRL = 0x0040,
    KMD_RCTRL = 0x0080,
    KMD_LALT = 0x0100,
    KMD_RALT = 0x0200
};

const int KMD_CTRL = KMD_LCTRL | KMD_RCTRL;
const int KMD_SHIFT = KMD_LSHIFT | KMD_RSHIFT;
const int KMD_ALT = KMD_LALT | KMD_RALT;

#endif
