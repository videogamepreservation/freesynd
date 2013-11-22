/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
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

#ifndef GAMEPLAYMENU_H
#define GAMEPLAYMENU_H

#include "agentselectorrenderer.h"
#include "maprenderer.h"
#include "minimaprenderer.h"
#include "squadselection.h"
#include "core/gameevent.h"

class Mission;
class IPAStim;

/*!
 * Gameplay Menu class.
 */
class GameplayMenu : public Menu, GameEventListener {
public:
    GameplayMenu(MenuManager *m);
    //! Update the menu state
    void handleTick(int elapsed);
    void handleShow();
    void handleRender(DirtyList &dirtyList);
    void handleLeave();

protected:
    bool handleUnknownKey(Key key, const int modKeys);

    void handleMouseMotion(int x, int y, int state, const int modKeys);
    bool handleMouseDown(int x, int y, int button, const int modKeys);
    void handleMouseUp(int x, int y, int button, const int modKeys);
    //! Handles the user's click on weapon selector
    void handleClickOnWeaponSelector(int x, int y, int button, const int modKeys);
    //! sets IPA level for defined agent, selector
    void setIPAForAgent(size_t slot, IPAStim::IPAType ipa_type, int percentage);
    //! Handles the user's click on the map
    void handleClickOnMap(int x, int y, int button, const int modKeys);
    //! Handles the user's click on the minimap
    void handleClickOnMinimap(int x, int y);

    void drawSelectAllButton();
    void drawMissionHint(int elapsed);
    void drawWeaponSelectors();
    //! Scroll the map horizontally.
    bool scrollOnX();
    //! Scroll the map vertically.
    bool scrollOnY();
    //void improveScroll(int &newScrollX, int &newScrollY);
    void initWorldCoords();
    //! Selects/deselects an agent
    void selectAgent(size_t agentNo, bool addToGroup);
    //! Selects/deselects all agents
    void selectAllAgents();
    //! Reacts to a weapon selection/deselection
    void handleWeaponSelection(uint8 weapon_idx, bool ctrl);

    //! Deselect agent if he died
    void updateSelectionForDeadAgent(PedInstance *p_ped);
    //! updates visual markers for our agents
    void updtAgentsMarker();

    void stopShootingEvent(void);
    //! Centers the minimap on the selection leader
    void centerMinimapOnLeader();
    //! Animate the minimap
    void updateMinimap(int elapsed);
    //! Update the select all button state
    void updateSelectAll();
    //! Update the target value for adrenaline etc for an agent
    void updateIPALevelMeters(int elapsed);

    //! Handles game events
    void handleGameEvent(GameEvent evt);

protected:
    /*! Origin of the minimap on the screen.*/
    static const int kMiniMapScreenX;
    /*! Origin of the minimap on the screen.*/
    static const int kMiniMapScreenY;

    int tick_count_, last_animate_tick_;
    int last_motion_tick_, last_motion_x_, last_motion_y_;
    int mission_hint_ticks_, mission_hint_;
    Mission *mission_;
    /*! Holds the X coordinate of the screen origin (top left corner) in the world coordinate.*/
    int world_x_;
    /*! Holds the Y coordinate of the screen origin (top left corner) in the world coordinate.*/
    int world_y_;
    /*! Holds the amount of scroll on the X axis.*/
    int scroll_x_;
    /*! Holds the amount of scroll on the Y axis.*/
    int scroll_y_;
    /*! Agent selection manager.*/
    SquadSelection selection_;
    /*! Object mouse cursor is above*/ 
    ShootableMapObject *target_;
    /*! This renderer is in charge of drawing the map.*/
    MapRenderer map_renderer_;
    /*! This renderer is in charge of drawing the minimap.*/
    GamePlayMinimapRenderer mm_renderer_;
    /*! This renderer is in charge of drawing the IPA meters.*/
    AgentSelectorRenderer agt_sel_renderer_;
    bool pressed_btn_select_all_;
    struct ShootingEvents {
        bool shooting_;
        bool agents_shooting[4];
        uint32 ids[4];
        void clear() {
            shooting_ = false;
            agents_shooting[0] = false;
            agents_shooting[1] = false;
            agents_shooting[2] = false;
            agents_shooting[3] = false;
            ids[0] = 0;
            ids[1] = 0;
            ids[2] = 0;
            ids[3] = 0;
        }
    } shooting_events_;

    // when ipa is manipulated this represents
    struct IPA_manipulation {
        // ipa that is manipulated
        int8 ipa_chng;
        // agent to base IPA's update on
        uint8 agent_used;
    } ipa_chng_;
};

#endif
