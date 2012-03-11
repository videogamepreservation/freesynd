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

#ifndef MINIMAPRENDERER_H
#define MINIMAPRENDERER_H

#include <map>

#include "common.h"

class Mission;
class MissionBriefing;

/*!
 * Renderer for minimap.
 * This class is used to display a minimap in the briefing menu.
 */
class MinimapRenderer {
public:
    enum EZoom {
        ZOOM_X1 = 6,
        ZOOM_X2 = 4,
        ZOOM_X3 = 2,
        ZOOM_X4 = 0
    };

    //! Class Constructor.
    MinimapRenderer();
    //! Reset the class with a new mission
    void init(Mission *pMission, EZoom zoom, bool drawEnemies);

    //! update the class with elapsed time
    bool handleTick(int elapsed);

    //! Render the minimap
    void render();

    //! Sets all parameters that depend on enhancement level
    void zoomOut();
    //! Sets the flag whether to draw enemies on the minimap or not
    void setDrawEnemies(bool draw) { b_draw_enemies_ = draw; }

    //! Scrolls the minimap to the right
    void scrollRight();
    //! Scrolls the minimap to the left
    void scrollLeft();
    //! Scrolls the minimap to the top
    void scrollUp();
    //! Scrolls the minimap to the bottom
    void scrollDown();
    
protected:
    /*! X coord in the minimap system.*/
    uint16 minimap_scroll_x_;
    /*! Y coord in the minimap system.*/
    uint16 minimap_scroll_y_;
    /*! A counter to control the blinking on the minimap.*/
    int minimap_blink_ticks_;
    /*! Helps controling blinking.*/
    int minimap_blink_;
    /*! The scrolling step : depends on the enhancement level.*/
    uint8 scroll_step_;
    /*! The mission that contains the minimap.*/
    Mission *p_mission_;
    /*! Current zoom level.*/
    EZoom zoom_;
    /*! IF true, enemies are drawn on the minimap.*/
    bool b_draw_enemies_;
};

#endif
