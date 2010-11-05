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

#ifndef MAPMENU_H
#define MAPMENU_H

#include <assert.h>

//! Displays the mission selection map.
/*!
 * This menu displays a world map on which the player
 * selects the next mission to play.
 */
class MapMenu : public Menu {
public:
    MapMenu(MenuManager *m);
    ~MapMenu();

    void handleTick(int elapsed);
    void handleShow();
    void handleRender();
    void handleLeave();
    void handleMouseDown(int x, int y, int button, const int modKeys);
    void handleOption(Key key, const int modKeys);
    void handleUnknownKey(Key key, const int modKeys);

protected:
    static const int OWN_LBL_STATIC_ID;
    static const int COUNTRY_STATIC_ID;
    static const int POP_STATIC_ID;
    static const int OWN_STATIC_ID;
    static const int TAX_VALUE_STATIC_ID;
    static const int TIME_STATIC_ID;
    static const int TAX_PCT_STATIC_ID;
    static const int TAX_INC_OPT_ID;
    static const int TAX_DEC_OPT_ID;

    int mapblk_size_;
    /*! Contains the images of the differents blocks.*/
    uint8 *mapblk_data_;
    /*!
     * Contains the pixels on screen just after the opening animation.
     * Used to reset the screen.
     */
    uint8 *orig_pixels_;
    /*! A counter for the blinking line of the selector.*/
    int select_tick_count_;
    /*! A counter for the blinking available blocks. */
    int blk_tick_count_;
    /*! */
    bool blink_status_;

    //! Utility method to draw the mission selector
    void drawSelector();
    //! Utility method to update mission informations
    void handleBlockSelected();
    //! Update the game time display
    void updateClock();
};

#endif
