/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2010  Benoit Blancard <benblan@users.sourceforge.net>*
 *   Copyright (C) 2010  Bohdan Stelmakh <chamel@users.sourceforge.net> *
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

#ifndef BRIEFMENU_H
#define BRIEFMENU_H

#include "menus/minimaprenderer.h"

class MissionBriefing;

/*!
 * Brief Menu class. 
 */
class BriefMenu : public Menu {
public:
    BriefMenu(MenuManager *m);
    ~BriefMenu();

    void handleTick(int elapsed);
    void handleShow();
    void handleRender(DirtyList &dirtyList);
    void handleLeave();
    void handleAction(const int actionId, void *ctx, const int modKeys);
    
protected:
    static const int MINIMAP_X;
    static const int MINIMAP_Y;
    static const int MINIMAP_WIDTH;
    static const int MINIMAP_HEIGHT;

    void updateClock();
    void redrawMiniMap();

	bool handleMouseDown(int x, int y, int button, const int modKeys);

protected:
    int start_line_;

    /*! Id of the text widget for time.*/
    int txtTimeId_;
    /*! Id of the text widget for money.*/
    int txtMoneyId_;
    /*! Id of the text widget for info.*/
    int txtInfoId_;
    /*! Id of the text widget for enhance.*/
    int txtEnhId_;
    /*! Id of the infos button.*/
    int infosButId_;
    /*! Id of the enhance button.*/
    int enhButId_;
    /*! Id of the scroll next button.*/
    int nextButId_;
    /*! Id of the scroll previous button.*/
    int prevButId_;
    /*! Briefing informations on current mission.*/
	MissionBriefing *p_briefing_;
    /*! Class that draws the minimap.*/
    MinimapRenderer mm_renderer_;
};

#endif
