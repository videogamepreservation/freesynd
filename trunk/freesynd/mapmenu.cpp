/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2010  Benoit Blancard <benblan@users.sourceforge.net>*
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

/*!
 * This structure holds the position of the various
 * blocks on the map.
 * The order in the array is the same as the one in
 * the block file.
 */
struct BlockDisplay {
    struct pos pos;
    struct pos logo_pos;
    struct pos line_start;
    struct pos line_end;
} g_BlocksDisplay[50] = {
    {{46, 18}, {14, 12}, {48, 32}, {76, 36}},
    {{90, 16}, {14, 12}, {48, 32}, {107, 40}},
    {{122, 6}, {226, 78}, {226, 78}, {181, 33}},
    {{218, 16}, {226, 78}, {245, 78}, {250, 40}},
    {{320, 22}, {289, 24}, {322, 52}, {344, 60}},
    {{370, 20}, {353, 12}, {387, 45}, {404, 62}},
    {{434, 10}, {353, 12}, {388, 34}, {458, 48}},
    {{482, 30}, {585, 69}, {583, 79}, {527, 53}},
    {{70, 36}, {13, 59}, {49, 73}, {100, 63}},
    {{290, 70}, {243, 93}, {279, 107}, {316, 97}},
    {{334, 54}, {289, 22}, {325, 57}, {360, 88}},
    {{372, 58}, {353, 12}, {381, 47}, {406, 90}},
    {{436, 74}, {353, 12}, {389, 45}, {452, 102}},
    {{434, 62}, {585, 69}, {584, 85}, {487, 81}},
    {{532, 76}, {585, 69}, {584, 89}, {541, 93}},
    {{160, 52}, {225, 77}, {224, 87}, {177, 69}},
    {{56, 76}, {13, 59}, {49, 83}, {74, 91}},
    {{76, 76}, {13, 59}, {49, 83}, {96, 93}},
    {{110, 76}, {149, 123}, {149, 123}, {117, 91}},
    {{140, 76}, {149, 123}, {163, 122}, {157, 91}},
    {{278, 124}, {243, 93}, {279, 123}, {300, 135}},
    {{288, 124}, {243, 93}, {279, 123}, {326, 153}},
    {{344, 116}, {421, 185}, {423, 183}, {385, 137}},
    {{398, 106}, {421, 185}, {435, 184}, {427, 129}},
    {{474, 98}, {565, 133}, {564, 143}, {511, 119}},
    {{56, 98}, {13, 127}, {49, 135}, {86, 113}},
    {{100, 106}, {149, 123}, {148, 133}, {107, 117}},
    {{198, 130}, {207, 165}, {217, 164}, {207, 137}},
    {{266, 140}, {207, 165}, {243, 177}, {284, 163}},
    {{336, 156}, {243, 209}, {279, 215}, {358, 167}},
    {{378, 146}, {421, 185}, {421, 184}, {401, 159}},
    {{440, 118}, {421, 185}, {449, 184}, {468, 143}},
    {{492, 136}, {565, 133}, {564, 149}, {509, 147}},
    {{66, 122}, {13, 127}, {49, 141}, {80, 135}},
    {{120, 172}, {61, 179}, {97, 195}, {138, 189}},
    {{280, 170}, {243, 209}, {279, 211}, {312, 181}},
    {{326, 166}, {285, 251}, {317, 250}, {352, 205}},
    {{370, 172}, {421, 185}, {420, 203}, {385, 203}},
    {{118, 206}, {89, 257}, {115, 256}, {131, 217}},
    {{142, 184}, {207, 165}, {206, 191}, {159, 211}},
    {{180, 196}, {243, 209}, {242, 225}, {195, 223}},
    {{328, 226}, {285, 251}, {321, 259}, {342, 249}},
    {{354, 212}, {412, 249}, {412, 259}, {363, 243}},
    {{502, 250}, {459, 253}, {495, 278}, {522, 285}},
    {{542, 246}, {597, 259}, {596, 275}, {569, 271}},
    {{542, 282}, {597, 259}, {596, 283}, {567, 293}},
    {{152, 230}, {215, 265}, {214, 269}, {173, 243}},
    {{148, 242}, {215, 265}, {214, 283}, {159, 281}},
    {{170, 252}, {215, 265}, {214, 277}, {179, 269}},
    {{504, 160}, {471, 197}, {507, 211}, {536, 207}}
};

extern int g_Colours[8];

/*!
 * Class constructor.
 * \param m The menu manager.
 */
MapMenu::MapMenu(MenuManager * m)
:  Menu(m, "map", "mmap.dat", "mmapout.dat"),
mapblk_data_(NULL), orig_pixels_(NULL), tick_count_(0)
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
		} while (blk_colours_[i] == g_App.getGameSession().getLogoColour());
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

/*!
 * Utility method to draw the mission selector on the map
 * depending on the current selection.<br/>
 * The selector consists of the player logo inside a box and
 * a line that links the logo to the selected region.
 */
void MapMenu::drawSelector()
{
    int logo_x = g_BlocksDisplay[g_Session.getSelectedBlockId()].logo_pos.x;
    int logo_y = g_BlocksDisplay[g_Session.getSelectedBlockId()].logo_pos.y;
    g_Screen.drawLogo(logo_x, logo_y, g_Session.getLogo(), g_Session.getLogoColour(),
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
    int blk_line_end_x = g_BlocksDisplay[g_Session.getSelectedBlockId()].line_end.x;
    int blk_line_end_y = g_BlocksDisplay[g_Session.getSelectedBlockId()].line_end.y;
    int blk_line_start_x = g_BlocksDisplay[g_Session.getSelectedBlockId()].line_start.x;
    int blk_line_start_y = g_BlocksDisplay[g_Session.getSelectedBlockId()].line_start.y;
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

void MapMenu::handleRender()
{
    if (orig_pixels_ == 0) {
        orig_pixels_ = new uint8[GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT];
        memcpy(orig_pixels_, g_Screen.pixels(),
               GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT);
    } else {
        g_Screen.blit(0, 0, GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, orig_pixels_);
    }

    // State of the Briefing button
    Block blk = g_Session.getBlock(g_Session.getSelectedBlockId());
    if (blk.finished) { // A mission is finished
        // Brief is available only if replay mission cheat is set
        if (g_Session.canReplayMission()) {
            showOption(KEY_F4);
        } else {
            hideOption(KEY_F4);
        }
    } else if (!blk.available) { // A mission is not finished but unavailable
        // Brief is available only if all missions enable cheat is set
        if (g_Session.isAllMissionEnabled()) {
            showOption(KEY_F4);
        } else {
            hideOption(KEY_F4);
        }
    } else {
        // Brief is available because mission is available and not finished
        showOption(KEY_F4);
    }

    for (int i = 0; i < 50; i++) {
        uint8 data[64 * 44];
        memcpy(data, mapblk_data_ + i * 64 * 44, 64 * 44);
        for (int j = 0; j < 64 * 44; j++)
            if (data[j] == 0)
                data[j] = 255;
            else
                data[j] = blk_colours_[i];
        g_Screen.scale2x(g_BlocksDisplay[i].pos.x, g_BlocksDisplay[i].pos.y, 64, 44,
                         data, 64);
    }

    drawSelector();

    // Draw country info
    // TODO: Grab countryinfo and display it
    char tmp[100];
    // Country name
    g_App.fonts().drawText(268, 312, blk.name, 0, false);

    // Population
#ifdef WIN_SECURE
    sprintf_s(tmp, 100, "%i", blk.population);
#else
    sprintf(tmp, "%i", blk.population);
#endif
    g_App.fonts().drawText(268, 332, tmp, 0, false);

    // TODO: Add tax adjustment buttons
    if (blk.finished) {
#ifdef WIN_SECURE
        sprintf_s(tmp, 100, "%i", blk.tax);
#else
        sprintf(tmp, "%i", blk.tax);
#endif
    } else {
        STR_CPY(tmp, "UNKNOWN");
    }

    g_App.fonts().drawText(268, 346, tmp, 0, false);

    // Own
    STR_CPY(tmp, "");
    g_App.fonts().drawText(268, 360, tmp, 0, false);

    g_System.showCursor();
}

void MapMenu::handleLeave() {
    g_System.hideCursor();
}

void MapMenu::handleMouseDown(int x, int y, int button)
{
    for (int i = 0; i < 50; i++) {
        if (x > g_BlocksDisplay[i].pos.x && x < g_BlocksDisplay[i].pos.x + 64 &&
            y > g_BlocksDisplay[i].pos.y && y < g_BlocksDisplay[i].pos.y + 44) {
            if (mapblk_data_
                [i * 64 * 44 + (y - g_BlocksDisplay[i].pos.y) / 2 * 64 +
                 (x - g_BlocksDisplay[i].pos.x) / 2] != 0) {
                g_Session.setSelectedBlockId(i);

                handleRender();
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
        g_Session.setSelectedBlockId(0);
    if (key == KEY_LEFT && (g_Session.getSelectedBlockId() > 0))
        g_Session.setSelectedBlockId(g_Session.getSelectedBlockId() - 1);
    if (key == KEY_RIGHT && g_Session.getSelectedBlockId() < 49)
        g_Session.setSelectedBlockId(g_Session.getSelectedBlockId() + 1);
    render();
}
