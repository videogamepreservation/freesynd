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

#include <cstdio>

#include "fliplayer.h"
#include "screen.h"
#include "utils/log.h"
#include "app.h"

#if 0 // TMN: Data for playing samples during intro video. Hardcoded from intro.exe.

//#include <pshpack1.h>
struct sample_timings {
\tint16 m0;
\tuint8 m1, m2, m3, m4, m5, m6;
};
//#include <poppack.h>

const sample_timings g_rg_sample_offsets_and_timings[] = {
\t{   0, 0x4C, 0x12, 0x00, 0x00, 0x00 }, // read data from FLC (0x12     (18)     bytes)
\t{   1, 0x4C, 0xB2, 0xF2, 0x08, 0x00 }, // read data from FLC (0x08f2b2 (586418) bytes)
\t{   1, 0x45, 0x00, 0x00, 0x00, 0x00 }, // load "data/isnds-0.dat"
\t{   1, 0x41, 0x01, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (1 = 10ms)
\t{   1, 0x53, 0x12, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x12
\t{  15, 0x54, 0x01, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{  19, 0x41, 0x06, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (6 = 60ms)
\t{  19, 0x4D, 0x00, 0x00, 0x00, 0x00 }, // start new sequence (sequence # 0)
\t{  34, 0x53, 0x01, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x01
\t{  39, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{  44, 0x54, 0x02, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{  60, 0x53, 0x02, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x02
\t{  62, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{  67, 0x54, 0x03, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{  85, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{  90, 0x54, 0x04, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{  91, 0x41, 0x06, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (6 = 60ms)
\t{ 117, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{ 119, 0x4C, 0xFA, 0xD6, 0x04, 0x00 }, // read data from FLC (0x04d6fa (317178) bytes)
\t{ 119, 0x41, 0x06, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (6 = 60ms)
\t{ 121, 0x54, 0x05, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{ 135, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{ 138, 0x54, 0x06, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{ 153, 0x54, 0x07, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{ 165, 0x54, 0x08, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{ 169, 0x53, 0x0B, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0B
\t{ 178, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{ 180, 0x54, 0x09, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{ 190, 0x53, 0x0A, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0A
\t{ 200, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{ 217, 0x4C, 0x16, 0x6E, 0x02, 0x00 }, // read data from FLC (0x026e16 (159254) bytes)
\t{ 217, 0x41, 0x06, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (6 = 60ms)
\t{ 218, 0x53, 0x03, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x03
\t{ 231, 0x54, 0x0A, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{ 267, 0x4C, 0xC0, 0x52, 0x04, 0x00 }, // read data from FLC (0x0452c0 (283328) bytes)
\t{ 267, 0x41, 0x0C, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (12 = 120ms)
\t{ 268, 0x53, 0x0C, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0C
\t{ 271, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{ 291, 0x54, 0x0B, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{ 305, 0x41, 0x06, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (6 = 60ms)
\t{ 316, 0x4C, 0x84, 0x7C, 0x0A, 0x00 }, // read data from FLC (0x0a7c84 (687236) bytes)
\t{ 317, 0x41, 0x0C, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (12 = 120ms)
\t{ 318, 0x53, 0x0D, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0D
\t{ 333, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{ 333, 0x53, 0x0D, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0D
\t{ 343, 0x53, 0x0D, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0D
\t{ 354, 0x53, 0x0D, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0D
\t{ 359, 0x54, 0x0C, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{ 367, 0x41, 0x07, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (7 = 70ms)
\t{ 406, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{ 414, 0x4C, 0x78, 0x2D, 0x02, 0x00 }, // read data from FLC (0x22d78 (142712) bytes)
\t{ 415, 0x41, 0x06, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (6 = 60ms)
\t{ 427, 0x53, 0x05, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x05
\t{ 439, 0x4C, 0x0A, 0x29, 0x01, 0x00 }, // read data from FLC (0x01290a (76042) bytes)
\t{ 439, 0x41, 0x78, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (120 = 1200ms)
\t{ 440, 0x41, 0x0C, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (12 = 120ms)
\t{ 442, 0x54, 0x0D, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{ 449, 0x53, 0x11, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x11
\t{ 454, 0x53, 0x11, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x11
\t{ 459, 0x53, 0x0F, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0F
\t{ 467, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{ 470, 0x53, 0x10, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x10
\t{ 472, 0x4C, 0x42, 0xE2, 0x07, 0x00 }, // read data from FLC (0x07e242 (516674) bytes)
\t{ 473, 0x54, 0x0E, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{ 473, 0x41, 0x0A, 0x00, 0x00, 0x00 }, // wait 1/100 seconds
\t{ 489, 0x53, 0x07, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x07
\t{ 502, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{ 504, 0x41, 0x08, 0x00, 0x00, 0x00 }, // wait 1/100 seconds
\t{ 520, 0x53, 0x08, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x08
\t{ 528, 0x41, 0x0C, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (12 = 120ms)
\t{ 532, 0x53, 0x09, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x09
\t{ 541, 0x41, 0x08, 0x00, 0x00, 0x00 }, // wait 1/100 seconds
\t{ 542, 0x53, 0x08, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x08
\t{ 548, 0x54, 0x0F, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{ 548, 0x53, 0x07, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x07
\t{ 562, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{ 565, 0x41, 0x0C, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (12 = 120ms)
\t{ 568, 0x53, 0x06, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x06
\t{ 569, 0x54, 0x10, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{ 577, 0x53, 0x07, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x07
\t{ 580, 0x53, 0x04, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x04
\t{ 583, 0x53, 0x07, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x07
\t{ 589, 0x53, 0x04, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x04
\t{ 592, 0x53, 0x07, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x07
\t{ 602, 0x53, 0x04, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x04
\t{ 606, 0x53, 0x07, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x07
\t{ 610, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{ 617, 0x53, 0x04, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x04
\t{ 620, 0x53, 0x07, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x07
\t{ 627, 0x4C, 0xAC, 0x5D, 0x00, 0x00 }, // read data from FLC (0x5dac (23980) bytes)
\t{ 628, 0x41, 0x28, 0x00, 0x00, 0x00 }, // wait 1/100 seconds
\t{ 629, 0x53, 0x0E, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0E
\t{ 635, 0x4C, 0x5C, 0x31, 0x01, 0x00 }, // read data from FLC (0x01315c (78172) bytes)
\t{ 635, 0x41, 0x01, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (1 = 10ms)
\t{ 636, 0x53, 0x00, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x00
\t{ 637, 0x45, 0x01, 0x00, 0x00, 0x00 }, // load "data/isnds-1.dat"
\t{ 638, 0x41, 0x06, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (6 = 60ms)
\t{ 639, 0x54, 0x11, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{ 641, 0x53, 0x0B, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0B
\t{ 646, 0x53, 0x07, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x07
\t{ 654, 0x53, 0x05, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x05
\t{ 662, 0x53, 0x06, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x06
\t{ 673, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{ 674, 0x53, 0x04, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x04
\t{ 679, 0x53, 0x08, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x08
\t{ 691, 0x53, 0x09, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x09
\t{ 704, 0x53, 0x0A, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0A
\t{ 709, 0x53, 0x0C, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0C
\t{ 719, 0x53, 0x0A, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0A
\t{ 725, 0x53, 0x0A, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0A
\t{ 727, 0x53, 0x0C, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0C
\t{ 733, 0x53, 0x0A, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0A
\t{ 738, 0x53, 0x0A, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0A
\t{ 741, 0x53, 0x0C, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0C
\t{ 744, 0x53, 0x0A, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0A
\t{ 749, 0x53, 0x0A, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0A
\t{ 755, 0x53, 0x0C, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0C
\t{ 761, 0x53, 0x0D, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0D
\t{ 786, 0x4C, 0xCE, 0x90, 0x04, 0x00 }, // read data from FLC (0x0490ce (299214) bytes)
\t{ 787, 0x41, 0x06, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (6 = 60ms)
\t{ 788, 0x53, 0x01, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x01
\t{ 807, 0x53, 0x02, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x02
\t{ 837, 0x4C, 0xF8, 0x03, 0x01, 0x00 }, // read data from FLC (0x0103f8 (66552) bytes)
\t{ 848, 0x54, 0x12, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{ 869, 0x4C, 0x78, 0x92, 0x00, 0x00 }, // read data from FLC (0x9278 (37496) bytes)
\t{ 870, 0x41, 0x0C, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (12 = 120ms)
\t{ 871, 0x53, 0x03, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x03
\t{ 892, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{ 902, 0x4C, 0x10, 0x17, 0x05, 0x00 }, // read data from FLC, 0x051710 (333584) bytes
\t{ 903, 0x41, 0x0A, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (10 = 100ms)
\t{ 903, 0x53, 0x0E, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0E
\t{ 904, 0x54, 0x13, 0x00, 0x00, 0x00 }, // draw subtitle string, index 
\t{ 917, 0x53, 0x07, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x07
\t{ 920, 0x41, 0x0B, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (11 = 110ms)
\t{ 929, 0x58, 0x00, 0x00, 0x00, 0x00 }, // stop current sequence
\t{ 931, 0x53, 0x0F, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x0F
\t{ 935, 0x53, 0x10, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x10
\t{ 938, 0x53, 0x10, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x10
\t{ 941, 0x53, 0x10, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x10
\t{ 944, 0x53, 0x10, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x10
\t{ 947, 0x53, 0x10, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x10
\t{ 950, 0x53, 0x10, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x10
\t{ 953, 0x53, 0x10, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x10
\t{ 956, 0x53, 0x10, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x10
\t{ 959, 0x53, 0x10, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x10
\t{ 960, 0x54, 0x00, 0x00, 0x00, 0x00 }, // clear subtitle string area
\t{ 963, 0x53, 0x11, 0x00, 0x00, 0x00 }, // maybe play VOC index 0x11
\t{ 997, 0x41, 0x78, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (120 = 1200ms)
\t{ 998, 0x41, 0x78, 0x00, 0x00, 0x00 }, // wait 1/100 seconds (120 = 1200ms)
\t{  -1, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

#endif

FliPlayer::~FliPlayer() {
    if (offscreen_) {
        delete[] offscreen_;
        offscreen_ = NULL;
    }
}

void FliPlayer::loadFliData(uint8 *data) {
    fli_data_ = data;

    fli_info_.size = READ_LE_UINT32(fli_data_);
    fli_data_ += 4;
    fli_info_.type = READ_LE_UINT16(fli_data_);
    fli_data_ += 2;
    fli_info_.numFrames = READ_LE_UINT16(fli_data_);
    fli_data_ += 2;
    fli_info_.width = READ_LE_UINT16(fli_data_);
    fli_data_ += 2;
    fli_info_.height = READ_LE_UINT16(fli_data_);
    fli_data_ += 2;

    if (fli_info_.type != 0xAF12) {     //simple check to verify it is indeed a (Bullfrog) FLI
\t\tFSERR(Log::k_FLG_GFX, "FliPlayer", "loadFliData()", ("Attempted to load non-FLI data (type = 0x%04X)\n", fli_info_.type));
        fli_info_.width = fli_info_.height = 100;
        fli_info_.numFrames = 0;
        return;
    }

    assert(fli_info_.width == 320 && fli_info_.height == 200);
    if (offscreen_)
        delete[] offscreen_;
    offscreen_ = new uint8[fli_info_.width * fli_info_.height];

    memset(palette_, 0, sizeof(palette_));
}

bool FliPlayer::isValidChunk(uint16 type) {
    //Even though it may be a valid chunk type, only return true if we know how to deal with it
    switch (type) {
    case 4:                    //COLOR_256
    case 7:                    //DELTA_FLC (FLI_SS2)
    case 15:                   //BYTE_RUN
    case 0xF1FA:               //FRAME_TYPE
        return true;

    default:
\t\tFSERR(Log::k_FLG_GFX, "FliPlayer", "isValidChunk()", ("ERROR: isValidChunk(0x%04X) is NOT a valid chunk\n", type));
        return false;
    }
}

ChunkHeader FliPlayer::readChunkHeader(uint8 * mem) {
    ChunkHeader head;
    head.size = READ_LE_UINT32(mem + 0);
    head.type = READ_LE_UINT16(mem + 4);
    return head;
}

FrameTypeChunkHeader FliPlayer::readFrameTypeChunkHeader(ChunkHeader chunkHead,
        uint8 *&mem) {
    FrameTypeChunkHeader head;

    head.header = chunkHead;
    mem += 6;
    head.numChunks = READ_LE_UINT16(mem);
    mem += 2;
    head.delay = READ_LE_UINT16(mem);
    mem += 2;
    head.reserved = READ_LE_UINT16(mem);
    mem += 2;
    head.widthOverride = READ_LE_UINT16(mem);
    mem += 2;
    head.heightOverride = READ_LE_UINT16(mem);
    mem += 2;

    return head;
}

void FliPlayer::decodeByteRun(uint8 *data) {
    uint8 *ptr = (uint8 *) offscreen_;
    while ((ptr - offscreen_) < (fli_info_.width * fli_info_.height)) {
        uint8 chunks = *data++;
        while (chunks--) {
            int8 count = *data++;
            if (count > 0) {
                while (count--) {
                    *ptr++ = *data;
                }
                data++;
            } else {
                uint8 copyBytes = -count;
                memcpy(ptr, data, copyBytes);
                ptr += copyBytes;
                data += copyBytes;
            }
        }
    }
}

#define OP_PACKETCOUNT      0
#define OP_UNDEFINED        1
#define OP_LASTPIXEL        2
#define OP_LINESKIPCOUNT    3

void FliPlayer::decodeDeltaFLC(uint8 *data) {
    uint16 linesInChunk = READ_LE_UINT16(data);
    data += 2;
    uint16 currentLine = 0;
    uint16 packetCount = 0;

    while (linesInChunk--) {
        uint16 opcode;

        // First process all the opcodes.
        do {
            opcode = READ_LE_UINT16(data);
            data += 2;

            switch ((opcode >> 14) & 3) {
            case OP_PACKETCOUNT:
                packetCount = opcode;
                break;
            case OP_UNDEFINED:
                break;
            case OP_LASTPIXEL:
                *(uint8 *) (offscreen_ + (currentLine * fli_info_.width) +
                            (fli_info_.width - 1)) = (opcode & 0xFF);
                break;
            case OP_LINESKIPCOUNT:
                currentLine += -(int16) opcode;
                break;
            }
        } while (((opcode >> 14) & 3) != OP_PACKETCOUNT);

        uint16 column = 0;

        //Now interpret the RLE data
        while (packetCount--) {
            column += *data++;
            int8 rleCount = (int8) * data++;

            if (rleCount > 0) {
                memcpy((void *) (offscreen_ +
                                 (currentLine * fli_info_.width) + column),
                       data, rleCount * 2);
                data += rleCount * 2;
                column += rleCount * 2;
            }
            else if (rleCount < 0) {
                uint16 dataWord = *(uint16 *) data;
                data += 2;
                for (int i = 0; i < -(int16) rleCount; ++i) {
                    *(uint16 *) (offscreen_ +
                                 (currentLine * fli_info_.width) + column +
                                 (i * 2)) = dataWord;
                }

                column += (-(int16) rleCount) * 2;
            }
            else {            // End of cutscene ?
                return;
            }
        }

        currentLine++;
    }
}


#define FRAME_TYPE  0xF1FA

bool FliPlayer::decodeFrame() {
    FrameTypeChunkHeader frameHeader;
    ChunkHeader cHeader = readChunkHeader(fli_data_);
    do {
        switch (cHeader.type) {
        case 4:
            setPalette(fli_data_ + 6);
            g_System.setPalette8b3(palette_);
            break;
        case 7:
            decodeDeltaFLC(fli_data_ + 6);
            break;
        case 15:
            decodeByteRun(fli_data_ + 6);
            break;
        case FRAME_TYPE:
            frameHeader = readFrameTypeChunkHeader(cHeader, fli_data_);
            fli_info_.numFrames--;
            //printf("Frames Remaining: %d\n", fli_info_.numFrames);
            break;
        default:
            break;
        }

        if (cHeader.type != FRAME_TYPE)
            fli_data_ += cHeader.size;

        cHeader = readChunkHeader(fli_data_);

    } while (isValidChunk(cHeader.type) && cHeader.type != FRAME_TYPE);

    return isValidChunk(cHeader.type);

}

void FliPlayer::setPalette(uint8 *mem) {
    uint16 numPackets = READ_LE_UINT16(mem);
    mem += 2;

    if (0 == READ_LE_UINT16(mem)) {     //special case
        mem += 2;
        for (int i = 0; i < 256; ++i)
            for (int j = 0; j < 3; ++j)
                palette_[i * 3 + j] =
                    (mem[i * 3 + j] << 2) | (mem[i * 3 + j] & 3);
    }
    else {
        uint8 palPos = 0;

        while (numPackets--) {
            palPos += *mem++;
            uint8 change = *mem++;

            for (int i = 0; i < change; ++i)
                for (int j = 0; j < 3; ++j)
                    palette_[(palPos + i) * 3 + j] =
                        (mem[i * 3 + j] << 2) | (mem[i * 3 + j] & 3);

            palPos += change;
            mem += (change * 3);
        }
    }
}

void FliPlayer::copyCurrentFrameToScreen() {
    g_Screen.scale2x(0, 0, fli_info_.width, fli_info_.height, offscreen(),
                     0, false);
}

bool FliPlayer::play(bool intro, Font *pIntroFont) {
    if (!fli_data_)
        return false;

    g_Screen.clear(0);
\tint cur_frame = 0;
    while (hasFrames()) {
\t\t// Consumes events now so they won't be piled up after the animation
\t\tg_App.menus().handleEvents();

        if (!decodeFrame())
            break;
        copyCurrentFrameToScreen();

        cur_frame++;

        g_System.updateScreen();
        g_System.delay(1000 / (intro ? 10 : 15));      //fps
    }

    //clear the backscreen
    //bzero(Screen::pixels(), GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT);

    return true;
}
