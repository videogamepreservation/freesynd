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
#include "app.h"
#include "file.h"
#include "mapmenu.h"

struct pos {
    int x;
    int y;
};

struct Block {
    const char *name;
    int population;

    struct pos pos;
    struct pos logo_pos;
    struct pos line_start;
    struct pos line_end;
} g_Blocks[50] = {
    {"ALASKA", 46000000, {46, 18}, {14, 12}, {48, 32}, {76, 36}},
    {"NORTHWEST TERRITORIES", 56000000, {90, 16}, {14, 12}, {48, 32}, {107, 40}},
    {"NORTHEAST TERRITORIES", 58000000, {122, 6}, {226, 78}, {226, 78}, {181, 33}},
    {"GREENLAND", 40000000, {218, 16}, {226, 78}, {245, 78}, {250, 40}},
    {"SCANDINAVIA", 54000000, {320, 22}, {289, 24}, {322, 52}, {344, 60}},
    {"URALS", 40000000, {370, 20}, {353, 12}, {387, 45}, {404, 62}},
    {"SIBERIA", 54000000, {434, 10}, {353, 12}, {388, 34}, {458, 48}},
    {"KAMCHATKA", 56000000, {482, 30}, {585, 69}, {583, 79}, {527, 53}},
    {"YUKON", 58000000, {70, 36}, {13, 59}, {49, 73}, {100, 63}},
    {"WESTERN EUROPE", 48000000, {290, 70}, {243, 93}, {279, 107}, {316, 97}},
    {"CENTRAL EUROPE", 50000000, {334, 54}, {289, 22}, {325, 57}, {360, 88}},
    {"EASTERN EUROPE", 52000000, {372, 58}, {353, 12}, {381, 47}, {406, 90}},
    {"KAZAKHSTAN", 42000000, {436, 74}, {353, 12}, {389, 45}, {452, 102}},
    {"MONGOLIA", 52000000, {434, 62}, {585, 69}, {584, 85}, {487, 81}},
    {"FAR EAST", 42000000, {532, 76}, {585, 69}, {584, 89}, {541, 93}},
    {"NEWFOUNDLAND", 44000000, {160, 52}, {225, 77}, {224, 87}, {177, 69}},
    {"CALIFORNIA", 46000000, {56, 76}, {13, 59}, {49, 83}, {74, 91}},
    {"ROCKIES", 56000000, {76, 76}, {13, 59}, {49, 83}, {96, 93}},
    {"MID WEST", 58000000, {110, 76}, {149, 123}, {149, 123}, {117, 91}},
    {"NEW ENGLAND", 40000000, {140, 76}, {149, 123}, {163, 122}, {157, 91}},
    {"ALGERIA", 50000000, {278, 124}, {243, 93}, {279, 123}, {300, 135}},
    {"LYBIA", 40000000, {288, 124}, {243, 93}, {279, 123}, {326, 153}},
    {"IRAQ", 50000000, {344, 116}, {421, 185}, {423, 183}, {385, 137}},
    {"IRAN", 52000000, {398, 106}, {421, 185}, {435, 184}, {427, 129}},
    {"CHINA", 54000000, {474, 98}, {565, 133}, {564, 143}, {511, 119}},
    {"COLORADO", 40000000, {56, 98}, {13, 127}, {49, 135}, {86, 113}},
    {"SOUTHERN STATES", 42000000, {100, 106}, {149, 123}, {148, 133}, {107, 117}},
    {"ATLANTIC ACCELERATOR", 44000000, {198, 130}, {207, 165}, {217, 164}, {207, 137}},
    {"MAURITANIA", 58000453, {266, 140}, {207, 165}, {243, 177}, {284, 163}},
    {"SUDAN", 43999510, {336, 156}, {243, 209}, {279, 215}, {358, 167}},
    {"ARABIA", 54000471, {378, 146}, {421, 185}, {421, 184}, {401, 159}},
    {"INDIA", 55999864, {440, 118}, {421, 185}, {449, 184}, {468, 143}},
    {"PACIFIC RIM", 57999593, {492, 136}, {565, 133}, {564, 149}, {509, 147}},
    {"MEXICO", 43999722, {66, 122}, {13, 127}, {49, 141}, {80, 135}},
    {"COLOMBIA", 45999547, {120, 172}, {61, 179}, {97, 195}, {138, 189}},
    {"NIGERIA", 48000204, {280, 170}, {243, 209}, {279, 211}, {312, 181}},
    {"ZAIRE", 57999525, {326, 166}, {285, 251}, {317, 250}, {352, 205}},
    {"KENYA", 47999574, {370, 172}, {421, 185}, {420, 203}, {385, 203}},
    {"PERU", 57999719, {118, 206}, {89, 257}, {115, 256}, {131, 217}},
    {"VENEZUALA", 40000488, {142, 184}, {207, 165}, {206, 191}, {159, 211}},
    {"BRAZIL", 41999785, {180, 196}, {243, 209}, {242, 225}, {195, 223}},
    {"SOUTH AFRICA", 48000138, {328, 226}, {285, 251}, {321, 259}, {342, 249}},
    {"MOZAMBIQUE", 45999915, {354, 212}, {412, 249}, {412, 259}, {363, 243}},
    {"WESTERN AUSTRALIA", 47999716, {502, 250}, {459, 253}, {495, 278}, {522, 285}},
    {"NORTHERN TERRITORIES", 41999837, {542, 246}, {597, 259}, {596, 275}, {569, 271}},
    {"NEW SOUTH WALES", 47999878, {542, 282}, {597, 259}, {596, 283}, {567, 293}},
    {"PARAGUAY", 58000207, {152, 230}, {215, 265}, {214, 269}, {173, 243}},
    {"ARGENTINA", 40000352, {148, 242}, {215, 265}, {214, 283}, {159, 281}},
    {"URUGUAY", 57999681, {170, 252}, {215, 265}, {214, 277}, {179, 269}},
    {"INDONESIA", 47999794, {504, 160}, {471, 197}, {507, 211}, {536, 207}}
};

extern int g_Colours[8];

MapMenu::MapMenu(MenuManager * m)
:  Menu(m, "map", "mmap.dat", "mmapout.dat"),
mapblk_data_(NULL), orig_pixels_(NULL), cur_blk_(9), tick_count_(0)
{
    addOption(53, 352, "BRIEF", 1, KEY_F4, "brief");
    addOption(535, 352, "MENU", 1, KEY_F5, "main");
    addStatic(194, 332, "POP", 1, false);
    addStatic(194, 346, "TAX", 1, false);
    addStatic(194, 360, "OWN", 1, false);

    setParentMenu("main");

    // 64 x 44 x 50
    mapblk_data_ = File::loadFile("mmapblk.dat", mapblk_size_);
    for (int i = 0; i < 50; i++) {
        do {
            int index = rand() % (sizeof(g_Colours) / sizeof(int));
            blk_colours_[i] = g_Colours[index];
		} while (blk_colours_[i] == g_App.logoColour());
    }
}

MapMenu::~MapMenu()
{
    delete[] mapblk_data_;
    if (orig_pixels_)
        delete[] orig_pixels_;
}

void MapMenu::handleTick(int elapsed)
{
    tick_count_ += elapsed;
    if (tick_count_ > 200) {
        static int count = 0;
        tick_count_ = count++;
        drawSelector();
        tick_count_ = 0;
    }
}

void MapMenu::drawSelector()
{
    int logo_x = g_Blocks[cur_blk_].logo_pos.x;
    int logo_y = g_Blocks[cur_blk_].logo_pos.y;
    g_Screen.drawLogo(logo_x, logo_y, g_App.logo(), g_App.logoColour(),
                      true);

    // Draw box enclosing logo
    uint8 box[18 * 18];
    memset(box, 255, 18 * 18);
    for (int i = 0; i < 18; i++) {
        box[i] = 252;
        box[i + 17 * 18] = 252;
    }
    for (int j = 0; j < 18; j++) {
        box[j * 18] = 252;
        box[j * 18 + 17] = 252;
    }
    g_Screen.scale2x(logo_x - 2, logo_y - 2, 18, 18, box);

    // Draw line between country and logobox
    int blk_line_end_x = g_Blocks[cur_blk_].line_end.x;
    int blk_line_end_y = g_Blocks[cur_blk_].line_end.y;
    int blk_line_start_x = g_Blocks[cur_blk_].line_start.x;
    int blk_line_start_y = g_Blocks[cur_blk_].line_start.y;
    g_Screen.drawLine(blk_line_start_x, blk_line_start_y, blk_line_end_x,
                      blk_line_end_y, 252, 5, tick_count_ % 10);
    g_Screen.drawLine(blk_line_start_x, blk_line_start_y - 1,
                      blk_line_end_x, blk_line_end_y - 1, 252, 5,
                      tick_count_ % 10);
    g_Screen.drawLine(blk_line_start_x, blk_line_start_y, blk_line_end_x,
                      blk_line_end_y, 4, 5, tick_count_ % 10 + 5);
    g_Screen.drawLine(blk_line_start_x, blk_line_start_y - 1,
                      blk_line_end_x, blk_line_end_y - 1, 4, 5,
                      tick_count_ % 10 + 5);
}

void MapMenu::handleShow()
{
    if (orig_pixels_ == 0) {
        orig_pixels_ = new uint8[GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT];
        memcpy(orig_pixels_, g_Screen.pixels(),
               GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT);
    } else {
        g_Screen.blit(0, 0, GAME_SCREEN_WIDTH, 345, orig_pixels_);
    }

    for (int i = 0; i < 50; i++) {
        uint8 data[64 * 44];
        memcpy(data, mapblk_data_ + i * 64 * 44, 64 * 44);
        for (int j = 0; j < 64 * 44; j++)
            if (data[j] == 0)
                data[j] = 255;
            else
                data[j] = blk_colours_[i];
        g_Screen.scale2x(g_Blocks[i].pos.x, g_Blocks[i].pos.y, 64, 44,
                         data, 64);
    }

    drawSelector();

    // Draw country info
    // TODO: Grab countryinfo and display it
    char tmp[100];
    g_App.fonts().drawText(268, 312, g_Blocks[cur_blk_].name, 0, false);  // countryname
    sprintf(tmp, "%i", g_Blocks[cur_blk_].population);
    g_App.fonts().drawText(268, 332, tmp, 0, false);    // pop
    // if (owns_ountry) {
    // TODO: Add tax adjustment buttons
    strcpy(tmp, "UNKNOWN");
    g_App.fonts().drawText(268, 346, tmp, 0, false);    // tax
    strcpy(tmp, "");
    g_App.fonts().drawText(268, 360, tmp, 0, false);    // own
    // }
}

void MapMenu::handleMouseDown(int x, int y, int button)
{
    for (int i = 0; i < 50; i++) {
        if (x > g_Blocks[i].pos.x && x < g_Blocks[i].pos.x + 64 &&
            y > g_Blocks[i].pos.y && y < g_Blocks[i].pos.y + 44) {
            if (mapblk_data_
                [i * 64 * 44 + (y - g_Blocks[i].pos.y) / 2 * 64 +
                 (x - g_Blocks[i].pos.x) / 2] != 0) {
                cur_blk_ = i;
                handleShow();
                return;
            }
        }
    }
}

void MapMenu::handleOption(Key key)
{
}

void MapMenu::handleUnknownKey(Key key, KeyMod mod, bool pressed)
{
    if (key == KEY_0)
        cur_blk_ = 0;
    if (key == KEY_LEFT && cur_blk_ > 0)
        cur_blk_--;
    if (key == KEY_RIGHT && cur_blk_ < 49)
        cur_blk_++;
    show(false);
}
