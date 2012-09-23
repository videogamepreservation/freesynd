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

#ifndef MENUS_MINIMAPRENDERER_H_
#define MENUS_MINIMAPRENDERER_H_

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
    /*!
     * Enumeration for the available zoom levels.
     */
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

    //! Render the minimap at the given point
    void render(uint16 mm_x, uint16 mm_y);

    //! Sets all parameters that depend on zooming level
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
    //! Finds the minimap location
    void initMinimapLocation();
    void updateRenderingInfos();
    //! Checks if minimap is still within the map borders.
    void checkBorders();

protected:
    /*! Tile X coord on the map for the top left corner of the minimap.*/
    uint16 mm_tx_;
    /*! Tile Y coord on the map for the top left corner of the minimap.*/
    uint16 mm_ty_;
    /*!
     * Total number of tiles displayed in the minimap. 
     * same for width and height.
     */
    uint16 mm_maxtile_;
    /*! Number of pixel used to draw a map tile.*/
    uint8 pixpertile_;
    /*! A counter to control the blinking on the minimap.*/
    int minimap_blink_ticks_;
    /*! Helps controling blinking.*/
    int minimap_blink_;
    /*! The scrolling step : depends on the zoom level.*/
    uint8 scroll_step_;
    /*! The mission that contains the minimap.*/
    Mission *p_mission_;
    /*! Current zoom level.*/
    EZoom zoom_;
    /*! IF true, enemies are drawn on the minimap.*/
    bool b_draw_enemies_;
};


/*!
 * Renderer for minimap.
 * This class is used to display a minimap in the gameplay menu.
 */
class GamePlayMinimapRenderer {
public:
    //! Constructor for the class
    GamePlayMinimapRenderer();

    //! Reset the class with a new mission
    void init(Mission *pMission);

    //! Sets the minimap center on the given tile
    void centerOn(uint16 tileX, uint16 tileY, int offX, int offY);

    //! Render the minimap
    void render(uint16 mm_x, uint16 mm_y);

private:
    /*! The size in pixel of the minimap. Minimap is a square.*/
    static const int kMiniMapSizePx;
    /*! The number of pixel on the minimap for a map tile.*/
    static const int kPixelPerTile;

    /*! The mission that contains the minimap.*/
    Mission *p_mission_;
    /*! Tile X coord on the map for the top left corner of the minimap.*/
    uint16 mm_tx_;
    /*! Tile Y coord on the map for the top left corner of the minimap.*/
    uint16 mm_ty_;
    /*! Offset for the tile.*/
    int offset_x_;
    /*! Offset for the tile.*/
    int offset_y_;
};

#endif  // MENUS_MINIMAPRENDERER_H_
