/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2012  Benoit Blancard <benblan@users.sourceforge.net>*
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
#include "utils/timer.h"

class Mission;
class MissionBriefing;

/*!
 * Base class Renderer for minimap.
 * A minimap renderer can display a minimap at a given position on the screen.
 * The minimap is rendered with a resolution that depends on the zoom level.
 * The resolution is the number of pixel used to render one tile.
 * The renderer also stores the coords for the top left corner of the minimap
 * in the map coordinates. Moving the minimap depends on the subclass.
 */
class MinimapRenderer {
public:
    /*!
     * Enumeration for the available zoom levels.
     * The higher is the zoom, the more pixels is used to
     * render a tile.
     */
    enum EZoom {
        ZOOM_X1 = 6,
        ZOOM_X2 = 4,
        ZOOM_X3 = 2,
        ZOOM_X4 = 0
    };

    //! update the class with elapsed time
    bool handleTick(int elapsed) { return false; }

    //! Render the minimap at the given point
    void render(uint16 mm_x, uint16 mm_y) {}
protected:
    void setZoom(EZoom zoom);
     //! called when zoom changes
    virtual void updateRenderingInfos() = 0;
protected:
    /*! The size in pixel of the minimap. Minimap is a square.*/
    static const int kMiniMapSizePx;

    /*! Number of pixel used to draw a map tile.*/
    uint8 pixpertile_;
    /*! Current zoom level.*/
    EZoom zoom_;
    /*! Tile X coord on the map for the top left corner of the minimap.*/
    uint16 mm_tx_;
    /*! Tile Y coord on the map for the top left corner of the minimap.*/
    uint16 mm_ty_;
};

/*!
 * Renderer for minimap.
 * This class is used to display a minimap in the briefing menu.
 */
class BriefMinimapRenderer : public MinimapRenderer {
public:

    //! Class Constructor.
    BriefMinimapRenderer();
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
    //! If minimap is too far right or down, align with right or down border
    void clipMinimapToRightAndDown();

protected:
    /*! A timer to control the blinking on the minimap.*/
    fs_utils::Timer mm_timer;
    /*! Helps controling blinking.*/
    int minimap_blink_;
    /*!
     * Total number of tiles displayed in the minimap. 
     * same for width and height.
     */
    uint16 mm_maxtile_;
    /*! The scrolling step : depends on the zoom level.*/
    uint8 scroll_step_;
    /*! The mission that contains the minimap.*/
    Mission *p_mission_;
    /*! IF true, enemies are drawn on the minimap.*/
    bool b_draw_enemies_;
};


/*!
 * Renderer for minimap.
 * This class is used to display a minimap in the gameplay menu.
 */
class GamePlayMinimapRenderer : public MinimapRenderer {
public:
    //! Constructor for the class
    GamePlayMinimapRenderer();

    //! Reset the class with a new mission
    void init(Mission *pMission, bool b_scannerEnabled);

    //! Sets the minimap center on the given tile
    void centerOn(uint16 tileX, uint16 tileY, int offX, int offY);

    //! Tells whether scanner is on or off.
    void setScannerEnabled(bool b_enabled);

    //! Render the minimap
    void render(uint16 mm_x, uint16 mm_y);
protected:
    //! called when zoom changes
    void updateRenderingInfos();
    //! Draw all visible cars
    void drawCars(uint8 * a_minimap);
    /*!
     * Returns X coord of the given tile relatively
     * to the top letf corner of the minimap in pixel.
     */
    int mapToMiniMapX(int tileX, int offX)
    {
    	return ((tileX - mm_tx_) * pixpertile_) + (offX / (256 / pixpertile_));
    }
    /*!
     * Returns Y coord of the given tile relatively
     * to the top letf corner of the minimap in pixel.
     */
    int mapToMiniMapY(int tileY, int offY)
    {
    	return ((tileY - mm_ty_) * pixpertile_) + (offY / (256 / pixpertile_));
    }
    static const uint32 minimap_buffer_width_ = 19*8;
private:
    /*! The mission that contains the minimap.*/
    Mission *p_mission_;
    /*!
     * Total number of tiles displayed in the minimap. 
     * same for width and height.
     */
    uint16 mm_maxtile_;
    /*! Offset for the tile.*/
    int offset_x_;
    /*! Offset for the tile.*/
    int offset_y_;
    /*! Coords in pixels of the cross.*/
    int cross_x_;
    /*! Coords in pixels of the cross.*/
    int cross_y_;
};

#endif  // MENUS_MINIMAPRENDERER_H_
