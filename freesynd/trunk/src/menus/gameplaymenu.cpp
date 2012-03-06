/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2007  Davor Ocelic <docelic@mail.inet.hr>			*
 *   Copyright (C) 2010  Benoit Blancard <benblan@users.sourceforge.net>*
 *   Copyright (C) 2010  Bohdan Stelmakh <chamel@users.sourceforge.net> *
 *                                                                      *
 *    This program is free software;  you can redistribute it and / or  *
 *  modify it  under the  terms of the  GNU General  Public License as  *
 *  published by the Free Software Foundation; either version 2 of the  *
 *  License, or (at your option) any later version.                     *
 *                                                                      *
 *    This program is  distributed in the hope that it will be useful,  *
 *  but WITHOUT  ANY WARRANTY;without even  the impliedwarranty of      *
 *  MERCHANTABILITY  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *
 *  General Public License for more details.                            *
 *                                                                      *
 *    You can view the GNU  General Public License, online, at the GNU  *
 *  project'sweb  site;  see <http://www.gnu.org/licenses/gpl.html>.  *
 *  The full text of the license is also included in the file COPYING.  *
 *                                                                      *
 ************************************************************************/

#include <stdio.h>
#include <assert.h>
#include "app.h"
#include "gameplaymenu.h"
#include "gfx/fliplayer.h"
#include "utils/file.h"

//#define NEW_ANIMATE_HANDLING
#ifdef _WIN32
#include <windows.h>
#endif

// The number of pixel of a scroll
const int SCROLL_STEP = 16;

GameplayMenu::GameplayMenu(MenuManager *m) :
Menu(m, MENU_GAMEPLAY, MENU_DEBRIEF, "", "mscrenup.dat"),
tick_count_(0), last_animate_tick_(0), last_motion_tick_(0),
last_motion_x_(320), last_motion_y_(240), mission_hint_ticks_(0), 
mission_hint_(0), mission_(NULL), world_x_(0),
world_y_(0), selected_agents_(0),
pointing_at_ped_(-1), pointing_at_vehicle_(-1), pointing_at_weapon_(-1) 
{
    scroll_x_ = 0;
    scroll_y_ = 0;
}

/*!
 * Scroll the map vertically or horizontally.
 * Each map has a min and max value for the world origin coords and this
 * method moves that point between those limits. If scrolling hits the
 * map border, the scrolling is made along that border.
 * \return True is a scroll is made
 */
bool GameplayMenu::scroll() {
    bool change = false;
    int ox, oy; // unused

    if (scroll_x_ != 0) {
        int newWorldX = world_x_ + scroll_x_;

        int tx =
            g_App.maps().screenToTileX(mission_->map(), newWorldX, world_y_, ox);
        int ty =
            g_App.maps().screenToTileY(mission_->map(), newWorldX, world_y_, oy);

        // Scroll to the right
        if (scroll_x_ > 0) {
            if (ty < mission_->minY()) {
                // we hit the upper right border of the map
                // so we scroll down until the far right corner
                int newWorldY = world_y_ + SCROLL_STEP;
                newWorldX += SCROLL_STEP;
                tx = g_App.maps().screenToTileX(mission_->map(), newWorldX, newWorldY, ox);
                ty = g_App.maps().screenToTileY(mission_->map(), newWorldX, newWorldY, oy);

                if (ty < mission_->minY() || tx > mission_->maxX()) {
                    // We hit the corner so don't scroll
                    return false;
                } else {
                    world_x_ = newWorldX;
                    world_y_ = newWorldY;
                    change = true;
                }
            } else if (tx > mission_->maxX()) {
                // we hit the lower right border of the map
                // so we scroll up until the far right corner
                int newWorldY = world_y_ - SCROLL_STEP;
                newWorldX += SCROLL_STEP;
                tx = g_App.maps().screenToTileX(mission_->map(), newWorldX, newWorldY, ox);
                ty = g_App.maps().screenToTileY(mission_->map(), newWorldX, newWorldY, oy);

                if (ty < mission_->minY() || tx > mission_->maxX()) {
                    return false;
                } else {
                    world_x_ = newWorldX;
                    world_y_ = newWorldY;
                    change = true;
                }
            } else {
                // This is a regular right scroll
                world_x_ = newWorldX;
                change = true;
            }

        } else if (scroll_x_ < 0) { // Scroll to the left
            if (tx < mission_->minX()) {
                // we hit the upper left border of the map
                // so we scroll down until the far left corner
                int newWorldY = world_y_ + SCROLL_STEP;
                newWorldX -= SCROLL_STEP;
                tx = g_App.maps().screenToTileX(mission_->map(), newWorldX, newWorldY, ox);
                ty = g_App.maps().screenToTileY(mission_->map(), newWorldX, newWorldY, oy);

                if (tx < mission_->minX() || ty > mission_->maxY()) {
                    return false;
                } else {
                    world_x_ = newWorldX;
                    world_y_ = newWorldY;
                    change = true;
                }
            } else if (ty > mission_->maxY()) {
                // we hit the lower left border of the map
                // so we scroll up until the far left corner
                int newWorldY = world_y_ - SCROLL_STEP;
                newWorldX -= SCROLL_STEP;
                tx = g_App.maps().screenToTileX(mission_->map(), newWorldX, newWorldY, ox);
                ty = g_App.maps().screenToTileY(mission_->map(), newWorldX, newWorldY, oy);

                if (tx < mission_->minX() || ty > mission_->maxY()) {
                    return false;
                } else {
                    world_x_ = newWorldX;
                    world_y_ = newWorldY;
                    change = true;
                }
            } else {
                world_x_ = newWorldX;
                change = true;
            }
        }
    }

    if (scroll_y_ != 0) {
        int newWorldY = world_y_ + scroll_y_;

        int tx =
            g_App.maps().screenToTileX(mission_->map(), world_x_, newWorldY, ox);
        int ty =
            g_App.maps().screenToTileY(mission_->map(), world_x_, newWorldY, oy);

        // Scroll down
        if (scroll_y_ > 0) {
            if (tx > mission_->maxX()) {
                // we hit the lower right border of the map
                // so we scroll down until the lower corner
                int newWorldX = world_x_ - 2*SCROLL_STEP;
                tx = g_App.maps().screenToTileX(mission_->map(), newWorldX, newWorldY, ox);
                ty = g_App.maps().screenToTileY(mission_->map(), newWorldX, newWorldY, oy);

                if (ty > mission_->maxY() || tx > mission_->maxX()) {
                    return false;
                } else {
                    world_x_ = newWorldX;
                    world_y_ = newWorldY;
                    change = true;
                }
            } else if (ty > mission_->maxY()) {
                // we hit the lower left border of the map
                // so we scroll down until the lower corner
                int newWorldX = world_x_ + 2*SCROLL_STEP;
                tx = g_App.maps().screenToTileX(mission_->map(), newWorldX, newWorldY, ox);
                ty = g_App.maps().screenToTileY(mission_->map(), newWorldX, newWorldY, oy);

                if (ty > mission_->maxY() || tx > mission_->maxX()) {
                    return false;
                } else {
                    world_x_ = newWorldX;
                    world_y_ = newWorldY;
                    change = true;
                }
            } else {
                world_y_ = newWorldY;
                change = true;
            }

        } else if (scroll_y_ < 0) { // Scroll up
            if (tx < mission_->minX()) {
                // we hit the upper right border of the map
                // so we scroll up until the upper corner
                int newWorldX = world_x_ + 2*SCROLL_STEP;
                tx = g_App.maps().screenToTileX(mission_->map(), newWorldX, newWorldY, ox);
                ty = g_App.maps().screenToTileY(mission_->map(), newWorldX, newWorldY, oy);

                if (ty < mission_->minY() || tx < mission_->minX()) {
                    return false;
                } else {
                    world_x_ = newWorldX;
                    world_y_ = newWorldY;
                    change = true;
                }
            } else if (ty < mission_->minY()) {
                // we hit the upper left border of the map
                // so we scroll up until the upper corner
                int newWorldX = world_x_ - 2*SCROLL_STEP;
                tx = g_App.maps().screenToTileX(mission_->map(), newWorldX, newWorldY, ox);
                ty = g_App.maps().screenToTileY(mission_->map(), newWorldX, newWorldY, oy);

                if (ty < mission_->minY() || tx < mission_->minX()) {
                    return false;
                } else {
                    world_x_ = newWorldX;
                    world_y_ = newWorldY;
                    change = true;
                }
            } else {
                world_y_ = newWorldY;
                change = true;
            }
        }
    }

    return change;
}

void GameplayMenu::handleTick(int elapsed)
{
    bool change = false;
    tick_count_ += elapsed;

    // Update stats
    mission_->getStatistics()->mission_duration += elapsed;

    // Checks mission objectives
    mission_->checkObjectives();

    // Scroll the map
    if (scroll_x_ != 0 || scroll_y_ != 0) {
        change = scroll();
        scroll_x_ = 0;
        scroll_y_ = 0;
    }

    if (tick_count_ - last_animate_tick_ > 33) {
        int diff = tick_count_ - last_animate_tick_;
        last_animate_tick_ = tick_count_;

        for (int i = 0; i < mission_->numSfxObjects(); i++) {
            change |= mission_->sfxObjects(i)->animate(diff);
            if (mission_->sfxObjects(i)->sfxLifeOver()) {
                mission_->delSfxObject(i);
                i--;
            }
        }

        for (int i = 0; i < mission_->numPeds(); i++)
            change |= mission_->ped(i)->animate(diff, mission_);

        for (int i = 0; i < mission_->numVehicles(); i++)
            change |= mission_->vehicle(i)->animate(diff);

        for (int i = 0; i < mission_->numWeapons(); i++)
            change |= mission_->weapon(i)->animate(diff);

        for (int i = 0; i < mission_->numStatics(); i++)
            change |= mission_->statics(i)->animate(diff, mission_);

        for (int i = 0; i < mission_->numPrjShots(); i++) {
            change |= mission_->prjShots(i)->animate(diff, mission_);
            if (mission_->prjShots(i)->prjsLifeOver()) {
                mission_->delPrjShot(i);
                i--;
            }
        }
    }

    selectable_agents_ = 0;
    for (int i = 0; i < 4; i++) {
        if (mission_->ped(i)) {
            if(mission_->ped(i)->agentIs() == PedInstance::Agent_Active)
            {
                if (mission_->ped(i)->health() > 0) {
                    selectable_agents_ |= 1 << i;
                } else {
                    if ((selected_agents_ & (1 << (i + 4))) != 0 )
                        selected_agents_ ^= 1 << (i + 4);
                    if ((selected_agents_ & (1 << i)) != 0 )
                        selected_agents_ ^= 1 << i;
                }
            }
        }
    }
    if (selectable_agents_ == 0)
        pressed_btn_select_all_ = false;
    if ((selected_agents_ & 0xFF) == 0) {
        for (int i = 0; i < 4; i++)
            if ((selectable_agents_ & (1 << i)) != 0) {
                    selected_agents_ = 1 << i;
                    break;
            }
    }

    if (change) {
        needRendering();
        // force pointing_at_ped / vehicle to update
        handleMouseMotion(last_motion_x_, last_motion_y_, 0, KMD_NONE);
    }

    drawMissionHint(elapsed);
}

bool GameplayMenu::isScrollLegal(int newScrollX, int newScrollY)
{
    int ox, oy;
    int tx =
        g_App.maps().screenToTileX(mission_->map(), newScrollX, newScrollY, ox);
    int ty =
        g_App.maps().screenToTileY(mission_->map(), newScrollX, newScrollY, oy);

    if (tx < mission_->minX())
        return false;

    if (ty < mission_->minY())
        return false;

    if (tx > mission_->maxX())
        return false;

    if (ty > mission_->maxY())
        return false;

    return true;
}

void GameplayMenu::improveScroll(int &newScrollX, int &newScrollY)
{
    int ox, oy;
    int tx =
        g_App.maps().screenToTileX(mission_->map(), newScrollX, newScrollY, ox);
    int ty =
        g_App.maps().screenToTileY(mission_->map(), newScrollX, newScrollY, oy);

    if (tx < mission_->minX())
        newScrollX++;

    if (ty < mission_->minY())
        newScrollY++;

    if (tx > mission_->maxX())
        newScrollX--;

    if (ty > mission_->maxY())
        newScrollY--;
}

void GameplayMenu::handleShow() {
    if (mission_ == NULL) {
        mission_ = g_Session.getMission();
        mission_->start();
        completed_ = false;
        g_App.music().playTrack(msc::TRACK_ASSASSINATE);

        char spal[20];
        sprintf(spal,"hpal0%i.dat",g_Session.getSelectedBlock().mis_id % 5 + 1);
		menu_manager_->setPalette(spal);
        g_Screen.clear(0);
        world_x_ = mission_->startX();
        world_y_ = mission_->startY();
        int count = 0;

        while (!isScrollLegal(world_x_, world_y_) && count < 100) {
            improveScroll(world_x_, world_y_);
            count++;
        }

        selected_agents_ = 0;
        selectable_agents_ = 0;

        for (int i = 0; i < 4; i++)
            if (mission_->ped(i)) {
                if(mission_->ped(i)->agentIs() == PedInstance::Agent_Active){
                    selectable_agents_ |= 1 << i;
                }
            }
        for (int i = 0; i < 4; i++)
            if ((selectable_agents_ & (1 << i)) != 0) {
                    selected_agents_ = 1 << i;
                    break;
            }

        g_System.usePointerCursor();
        g_System.showCursor();
        pressed_btn_select_all_ = false;
    }
}

int qanim = 200, qframe = 0;

void GameplayMenu::handleRender(DirtyList &dirtyList)
{
    g_Screen.clear(0);
    mission_->drawMap(world_x_, world_y_);
    g_Screen.drawRect(0,0, 129, GAME_SCREEN_HEIGHT);
    drawAgentSelectors();
    drawPerformanceMeters();
    drawSelectAllButton();
    drawMissionHint(0);
    drawWeaponSelectors();
    drawMiniMap();

#ifdef _DEBUG
    if (g_System.getKeyModState() & KMD_LALT) {
        if (isAgentSelected(0))
            mission_->ped(0)->showPath(world_x_, world_y_);

        if (isAgentSelected(1))
            mission_->ped(1)->showPath(world_x_, world_y_);

        if (isAgentSelected(2))
            mission_->ped(2)->showPath(world_x_, world_y_);

        if (isAgentSelected(3))
            mission_->ped(3)->showPath(world_x_, world_y_);
    }
    // drawing of different sprites
//    g_App.gameSprites().sprite(9 * 40 + 1)->draw(0, 0, 0, false, true);
#if 0
    // 1601 == start of weapons icons for sidebar
    // 1621 == start of selected weapons icons for sidebar
    // 1748 == start of agent selectors for sidebar
    // 1772 == start of selected agent selectors for sidebar
    g_Screen.clear(1);
    int x = 0, y = 0, my = 0;
    for (int i = 1756; i < g_App.gameSprites().spriteCount(); i++) {
        Sprite *s = g_App.gameSprites().sprite(i);

        if (y + s->height() > GAME_SCREEN_HEIGHT) {
            printf("last sprite %i\n", i - 1);
            break;
        }

        if (x + s->width() > GAME_SCREEN_WIDTH) {
            x = 0;
            y += my;
            my = 0;
        }

        s->draw(x, y, 0);
        x += s->width();
        if (s->height() > my)
            my = s->height();
    }
#endif
    // this is used in combination with keys
#if 0
    g_App.gameSprites().drawFrame(qanim, qframe, 320, 200);
#endif
#endif

#ifdef TRACK_FPS
	char tmp[100];
	static int current_time = 0;
    static int last_time = 0;
	static float fps = 0;
	static int frame = 0;

	current_time = g_System.getTicks();

	int elapsed = current_time - last_time;
	frame++;

	if (elapsed > 1000) {
		fps = (float) frame / ((float) elapsed / 1000.0f);
		frame = 0;
		last_time = current_time;
	}
	sprintf(tmp, "FPS : %.2f FRAMES PER SEC", fps);
	gameFont()->drawText(10, g_Screen.gameScreenHeight() - 15, tmp, 14);
#endif
}

void GameplayMenu::handleLeave()
{
    g_App.music().stopPlayback();

    g_System.hideCursor();
	menu_manager_->setDefaultPalette();
    mission_->end();
    
    tick_count_ = 0;
    last_animate_tick_ = 0;
    last_motion_tick_ = 0;
    last_motion_x_ = 320;
    last_motion_y_ = 240;
    mission_hint_ticks_ = 0;
    mission_hint_ = 0;
    world_x_ = 0;
    world_y_ = 0;
    selected_agents_ = 0;
    pointing_at_ped_ = -1;
    pointing_at_vehicle_ = -1;
    pointing_at_weapon_ = -1;
    mission_ = NULL;
    scroll_x_ = 0;
    scroll_y_ = 0;
}

void GameplayMenu::handleMouseMotion(int x, int y, int state, const int modKeys)
{
    last_motion_tick_ = tick_count_;
    last_motion_x_ = x;
    last_motion_y_ = y;

    if (last_motion_x_ < 5) {
        scroll_x_ = - SCROLL_STEP;
    } else if (last_motion_x_ > GAME_SCREEN_WIDTH - 5) {
        scroll_x_ = SCROLL_STEP;
    }

    if (last_motion_y_ < 5) {
        scroll_y_ = - SCROLL_STEP;
    } else if (last_motion_y_ > GAME_SCREEN_HEIGHT - 5) {
        scroll_y_ = SCROLL_STEP;
    }

    pointing_at_ped_ = -1;
#ifdef _DEBUG
    for (int i = 0; mission_ && i < mission_->numPeds(); i++) {
#else
    for (int i = 8; mission_ && i < mission_->numPeds(); i++) {
#endif
        PedInstance *p = mission_->ped(i);
        if (p->health() > 0 && p->map() != -1) {
            int px = p->screenX() - 10;
            int py = p->screenY() - (1 + p->visZ()) * TILE_HEIGHT/3
                - (p->offZ() * TILE_HEIGHT/3) / 128;

            if (x - 129 + world_x_ >= px && y + world_y_ >= py &&
                x - 129 + world_x_ < px + 21 && y + world_y_ < py + 34) {
                pointing_at_ped_ = i;
                break;
            }
        }
    }

    pointing_at_vehicle_ = -1;

    for (int i = 0; mission_ && i < mission_->numVehicles(); i++) {
        VehicleInstance *v = mission_->vehicle(i);
        if (v->health() > 0) {
            int px = v->screenX() - 20;
            int py = v->screenY() - 10 - v->visZ() * TILE_HEIGHT/3;

            if (x - 129 + world_x_ >= px && y + world_y_ >= py &&
                x - 129 + world_x_ < px + 40 && y + world_y_ < py + 32) {
                pointing_at_vehicle_ = i;
                break;
            }
        }
    }

    pointing_at_weapon_ = -1;

    for (int i = 0; mission_ && i < mission_->numWeapons(); i++) {
        WeaponInstance *w = mission_->weapon(i);

        if (w->map() != -1) {
            int px = w->screenX() - 10;
            int py = w->screenY() + 4 - w->visZ() * TILE_HEIGHT/3
                - (w->offZ() * TILE_HEIGHT/3) / 128;

            if (x - 129 + world_x_ >= px && y + world_y_ >= py &&
                x - 129 + world_x_ < px + 20 && y + world_y_ < py + 15) {
                pointing_at_weapon_ = i;
                break;
            }
        }
    }

    // I only want to show the cross-hair cursor if one of your selected agents can shoot
    // the target.  I know this isn't the same as the original game, but displaying a red
    // cross-hair is particularly difficult on multiple platforms, so this is a good
    // compromise.
    // Updated 2006/11/20, think I'll ignore range - quantumg
    bool shootable = false;

    if (pointing_at_ped_ != -1) {
        for (int i = 0; i < 4; i++)
            if (isAgentSelected(i)
                    && mission_->ped(i)->selectedWeapon())
                shootable = true;
    }

    if (pointing_at_vehicle_ != -1) {
        for (int i = 0; i < 4; i++)
            if (isAgentSelected(i))
                shootable = true;
    }

    if (!shootable)
        pointing_at_ped_ = pointing_at_vehicle_ = -1;

    if (pointing_at_ped_ != -1 || pointing_at_vehicle_ != -1)
        g_System.useTargetCursor();
    else if (pointing_at_weapon_ != -1)
        g_System.usePickupCursor();
    else
        g_System.usePointerCursor();
}

bool GameplayMenu::handleMouseDown(int x, int y, int button, const int modKeys)
{
    bool change = false; /* Indicator whether we need menu redraw */
    bool ctrl = false;  // Is control button pressed
    if (modKeys & KMD_CTRL) {
        ctrl = true;
    }

    if (button == 1) {
      /* Handle agent selection. Click on an agent changes selection
       * to it. If control key is pressed, add or removes agent from
       * current selection. */
        if (x < 128) {
            if (y < 46) {
                if (x < 64) {
                    if ((selectable_agents_ & (1 << 0)) != 0) {
                        selectAgent(0, ctrl);
                        change = true;
                    }
                } else {
                    if ((selectable_agents_ & (1 << 1)) != 0) {
                        selectAgent(1, ctrl);
                        change = true;
                    }
                }
            }

            if (y >= 42 + 48 + 10 && y < 42 + 48 + 10 + 46) {
                if (x < 64) {
                    if ((selectable_agents_ & (1 << 2)) != 0) {
                        selectAgent(2, ctrl);
                        change = true;
                    }
                } else {
                    if ((selectable_agents_ & (1 << 3)) != 0) {
                        selectAgent(3, ctrl);
                        change = true;
                    }
                }
            }

            /* The group-button click (in-between first and second row of
             * agents. A mouse click cycles between current selection and all
             * agents. If control is pressed, current selection is inverted. */
            if (y >= 42 + 48 && y < 42 + 48 + 10) {
                selectAllAgents();
                change = true;
            }
#if 0
            // TODO: click on minimap requires fixing
            int sy = 46 + 44 + 10 + 46 + 44 + 15 + 2 * 32;

            if (y > sy) {
                for (int j = 0; j < 16; j++)
                    for (int i = 0; i < 16; i++)
                        if (x > i * 8 && x < i * 8 + 8 && y > j * 8 + sy
                                && y < j * 8 + sy + 8) {
                            int t =
                                g_App.maps().map(mission_->map())->
                                tileAt(mm_tx_ + i, mm_ty_ + j, 1);
                            printf("%i\n", t);
                        }
            }
#endif
        } else {
            int ox, oy;
            int tx =
                g_App.maps().screenToTileX(mission_->map(), world_x_ + x - 129,
                        world_y_ + y, ox);
            int ty =
                g_App.maps().screenToTileY(mission_->map(), world_x_ + x - 129,
                        world_y_ + y, oy);

            for (int i = 0; i < 4; i++) {
                PedInstance *ped = mission_->ped(i);
                if (isAgentSelected(i)) {
                    if (pointing_at_weapon_ != -1) {
#ifdef NEW_ANIMATE_HANDLING
                        PedInstance::actionQueueGroupType as;
                        as.group_desc = PedInstance::gd_mExclusive;
                        ped->createActQPickUp(as,
                            mission_->weapon(pointing_at_weapon_));
                        as.main_act = as.actions.size() - 1;
                        if (modKeys & KMD_CTRL)
                            ped->addActQToQueue(as);
                        else
                            ped->setActQInQueue(as);
#else
                        ped->pickupWeapon(
                                mission_->weapon(pointing_at_weapon_));
#endif
                    } else if (pointing_at_vehicle_ != -1) {
                        if (ped->inVehicle()) {;
                            ped->inVehicle()->removeDriver(ped);
                            ped->leaveVehicle();
                        } else {
                            if (mission_->vehicle(pointing_at_vehicle_)->
                                    health() > 0)
                                ped->putInVehicle( mission_->vehicle(
                                    pointing_at_vehicle_));
                        }
                    } else if (ped->inVehicle()) {
                        if (ped == ped->inVehicle()->getDriver()) {
                            int stx = tx;
                            int sty = ty;
                            //int sox = ox;
                            //int soy = oy;
                            stx = tx * 256 + ox + 128 * ped->inVehicle()->tileZ();
                            //sox = stx % 256;
                            stx = stx / 256;
                            sty = ty * 256 + oy + 128 * ped->inVehicle()->tileZ();
                            //soy = sty % 256;
                            sty = sty / 256;
                            if (modKeys & KMD_CTRL) {
                                ped->inVehicle()->
                                    addDestinationV(stx, sty, 0,
                                    128, 128, 480);
                            } else {
                                ped->inVehicle()->
                                    setDestinationV(mission_, stx, sty, 0, 
                                    128, 128, 480);
                            }
                        }
                    } else {
                        int stx = tx;
                        int sty = ty;
                        int stz = 0;
                        int sox = ox;
                        int soy = oy;
                        if (!(mission_->getWalkable(stx, sty, stz, sox, soy)))
                            continue;
                        if (selectedAgentsCount() > 1) {
                            //TODO: current group position is like
                            // in original this can make non-tile
                            // oriented
                            //int sox = (i % 2) * (i - 2) * 16;
                            //int soy = ((i + 1) % 2) * (i - 1) * 8;

                            //this should be romoved if non-tile
                            //position needed
                            sox = 63 + 128 * (i % 2);
                            soy = 63 + 128 * (i >> 1);
                        }
#ifdef NEW_ANIMATE_HANDLING
                        PedInstance::actionQueueGroupType as;
                        PathNode tpn = PathNode(stx, sty, stz, sox, soy, 0);
                        ped->createActQWalking(as, &tpn, NULL);
                         as.main_act = as.actions.size() - 1;
                        as.group_desc = PedInstance::gd_mExclusive;
                        if (modKeys & KMD_CTRL)
                            ped->addActQToQueue(as);
                        else
                            ped->setActQInQueue(as);
#else
                        mission_->ped(i)->setDestinationP(mission_, stx, sty, stz,
                                sox, soy, 320);
#endif
                    }
                }
            }
        }
    } else if (button == 3) {
        // NOTE: this click will not generate firing animation,
        // no correct state handling for peds
        if (x >= 129) {
            int ox, oy;
            int tx =
                g_App.maps().screenToTileX(mission_->map(), world_x_ + x - 129,
                    world_y_ + y, ox);
            int ty =
                g_App.maps().screenToTileY(mission_->map(), world_x_ + x - 129,
                    world_y_ + y, oy);
            for (int i = 0; i < 4; i++) {
                PedInstance * pa = mission_->ped(i);
                if (isAgentSelected(i)
                        && pa->selectedWeapon()) {
                    if (pointing_at_ped_ != -1
                        && pa->selectedWeapon()->inflictDamage(
                            mission_->ped(pointing_at_ped_), NULL, NULL, true))
                    {
                        pa->startFiring();
                        pa->setTarget(mission_->ped(pointing_at_ped_));
                    } else if (pointing_at_vehicle_ != -1
                            && pa->selectedWeapon()->inflictDamage(
                            mission_->vehicle(pointing_at_vehicle_), NULL,
                            NULL, true))
                    {
                        pa->startFiring();
                        pa->setTarget(mission_->ped(pointing_at_vehicle_));
                    } else {
                        int stx = tx;
                        int sty = ty;
                        int stz = 0;
                        int sox = ox;
                        int soy = oy;
                        int oz = 0;
                        if (mission_->getShootableTile(stx, sty, stz,
                            sox, soy, oz))
                        {
                            PathNode pn(stx, sty, stz, sox, soy, oz);
                            //if (
                                pa->selectedWeapon()->inflictDamage(NULL, &pn, NULL, true)
                                //)
                                ;
                        }
                    }
                }
            }
        }
    }

    // handle weapon selectors
    if (x < 129) {
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < 4; i++) {
                if (x >= 32 * i && x < 32 * i + 32
                        && y >= 2 + 46 + 44 + 10 + 46 + 44 + 15 + j * 32
                        && y < 2 + 46 + 44 + 10 + 46 + 44 + 15 + j * 32 + 32) {
                    for (int a = 0; a < 4; a++) {
                        // TODO: when more then one agent is selecting weapon,
                        // the chosen ones should be same type and less ammo
                        // or best in rank
                        PedInstance *ped = mission_->ped(a);
                        if (isAgentSelected(a)) {
                            if (i + j * 4 < ped->numWeapons()) {
                                if (button == 1) {
                                    if (ped->selectedWeapon()
                                            == ped->weapon(i + j * 4))
                                        ped->setSelectedWeapon(-1);
                                    else {
                                        ped->setSelectedWeapon(
                                                i + j * 4);
                                    }
                                } else
#ifdef NEW_ANIMATE_HANDLING
                                {
                                    PedInstance::actionQueueGroupType as;
                                    as.main_act = 0;
                                    as.group_desc = PedInstance::gd_mExclusive;
                                    ped->createActQPutDown(as,
                                        ped->weapon(i + j * 4));
                                    if (modKeys & KMD_CTRL)
                                        ped->addActQToQueue(as);
                                    else
                                        ped->setActQInQueue(as);
                                }
#else
                                    ped->dropWeapon(i + j * 4);
#endif

                                change = true;
                            }
                        }
                    }
                }
            }
        }
    }

    if (change)
        //render();
		needRendering();

	return true;
}

void GameplayMenu::handleMouseUp(int x, int y, int button, const int modKeys) {
    if (button == 3) {
        for (int i = 0; i < 4; i++) {
            if (isAgentSelected(i)
                    && mission_->ped(i)->selectedWeapon()) {
                mission_->ped(i)->stopFiring();
            }
        }
    }
}

extern int topz;

bool GameplayMenu::handleUnknownKey(Key key, const int modKeys) {
    bool change = false; /* indicator whether menu should be redrawn */
    bool consumed = true;

    if (mission_ == NULL) {
        // Mission must not be null
        return false;
    }

    bool ctrl = false;
    if (modKeys & KMD_CTRL) {
        ctrl = true;
    }

#ifdef _DEBUG
	if (isLetterH(key.unicode)) {
        mission_->setStatus(Mission::COMPLETED);
        return true;
    }

    if (isLetterG(key.unicode)) {
        mission_->setStatus(Mission::FAILED);
        return true;
    }
#endif

    // SPACE is pressed when the mission failed or succeeded to return
    // to menu
	if (key.unicode == K_SPACE) {
        if (mission_->completed() || mission_->failed()) {
			// Do not display default leaving animation because 
			// a success/failed animation will be played
			leaveAnim_ = "";
            if (mission_->completed()) {
                g_Session.completeSelectedBlock();
                // Display success animation
				menu_manager_->gotoMenu(Menu::MENU_FLI_SUCCESS);
            }
            else if (mission_->failed()) {
                
				menu_manager_->gotoMenu(Menu::MENU_FLI_FAILED);
            }

            return true;
        }
	} else if (key.keyFunc == KFC_ESCAPE) {
        // Abort mission
        mission_->setStatus(Mission::ABORTED);
        // Return false so when can still go to parent menu with escape
        return false;
    }

    /* Handle agent selection by numeric keys. Key 0 cycles between current
     * selection and all 4 agents. Ctrl + 0 inverts selection.
     * Individual keys select the specified agent unless ctrl is pressed -
     * then they add/remove agent from current selection. */
	if (key.keyVirt == KVT_NUMPAD0) {
        /* This code is exactly the same as for clicking on "group-button"
         * as you can see above. */
        selectAllAgents(ctrl);
        change = true;
    }
    else if (key.keyVirt == KVT_NUMPAD1) {
        if ((selectable_agents_ & (1 << 0)) != 0) {
            selectAgent(0, ctrl);
            change = true;
        }
    }
    else if (key.keyVirt == KVT_NUMPAD2) {
        if ((selectable_agents_ & (1 << 1)) != 0) {
            selectAgent(1, ctrl);
            change = true;
        }
    }
    else if (key.keyVirt == KVT_NUMPAD3) {
        if ((selectable_agents_ & (1 << 2)) != 0) {
            selectAgent(2, ctrl);
            change = true;
        }
    }
    else if (key.keyVirt == KVT_NUMPAD4) {
        if ((selectable_agents_ & (1 << 3)) != 0) {
            selectAgent(3, ctrl);
            change = true;
        }
	} else if (key.keyFunc == KFC_LEFT) { // Scroll the map to the left
        scroll_x_ = -SCROLL_STEP;
    } else if (key.keyFunc == KFC_RIGHT) { // Scroll the map to the right
        scroll_x_ = SCROLL_STEP;
    } else if (key.keyFunc == KFC_UP) { // Scroll the map to the top
        scroll_y_ = -SCROLL_STEP;
    } else if (key.keyFunc == KFC_DOWN) { // Scroll the map to the bottom
        scroll_y_ = SCROLL_STEP;
    } else if (key.keyFunc == KFC_F1) { // Music Control
        g_App.music().toggleMusic();
    } else if (key.keyFunc == KFC_F2) { // Sound Control
        g_App.gameSounds().toggleSound();
    } else if (isLetterD(key.unicode)) { // all agents are killed with 'd'
        for (int i = 8; i < mission_->numPeds(); i++)
            mission_->ped(i)->setHealth(0);
    } else {
        consumed = false;
    }

#ifdef _DEBUG
#if 0
    if (key == KEY_i)
        mission_->ped(0)->setTileY(mission_->ped(0)->tileY() - 1);

    if (key == KEY_k)
        mission_->ped(0)->setTileY(mission_->ped(0)->tileY() + 1);

    if (key == KEY_j)
        mission_->ped(0)->setTileX(mission_->ped(0)->tileX() - 1);

    if (key == KEY_l)
        mission_->ped(0)->setTileX(mission_->ped(0)->tileX() + 1);

    if (key == KEY_PAGEUP) {
        if (mission_->ped(0)->tileZ() < mission_->mmax_z_)
            mission_->ped(0)->setTileZ(mission_->ped(0)->tileZ() + 1);
    }

    if (key == KEY_PAGEDOWN) {
        if (mission_->ped(0)->tileZ() > 0)
            mission_->ped(0)->setTileZ(mission_->ped(0)->tileZ() - 1);
    }

    if (key == KEY_o) {
        mission_->ped(0)->setOffZ(mission_->ped(0)->offZ() + 8);
        if (mission_->ped(0)->tileZ() >= mission_->mmax_z_) {
            mission_->ped(0)->setTileZ(mission_->mmax_z_ - 1);
            mission_->ped(0)->setOffZ(127);
        }
    }

    if (key == KEY_p) {
        mission_->ped(0)->setOffZ(mission_->ped(0)->offZ() - 8);
        if (mission_->ped(0)->tileZ() < 0) {
            mission_->ped(0)->setTileZ(0);
            mission_->ped(0)->setOffZ(0);
        }
    }
    if (mission_->ped(0)->offZ() != 0)
        mission_->ped(0)->setVisZ(mission_->ped(0)->tileZ() - 1);
    else
        mission_->ped(0)->setVisZ(mission_->ped(0)->tileZ());

    printf("%i %i %i\n", mission_->ped(0)->tileX(), mission_->ped(0)->tileY(),
        mission_->ped(0)->tileZ());
#endif

#if 0
    // used to see animations by number + frame
    if (key == KEY_a) {
        qanim--;
        if (qanim < 0)
            qanim = 0;
    }

    if (key == KEY_s) {
        qanim++;
        if (qanim > 1969)
            qanim = 1969;
    }

    if (key == KEY_q) {
        qanim -= 8;
        if (qanim < 0)
            qanim = 0;
    }

    if (key == KEY_w) {
        qanim += 8;
        if (qanim > 1969)
            qanim = 1969;
    }

    if (key == KEY_x) {
        qframe++;
        if (qframe > 30)
            qframe = 0;
    }

    if (key == KEY_z) {
        qframe--;
        if (qframe < 0)
            qframe = 0;
    }

    if (key == KEY_SPACE)
        printf("qanim %i qframe %i\n", qanim, qframe);
#endif

#if 0
    // when directional pathfinding will be implemented this will be used
    // for tests
    if (key == KEY_t) {
        mission_->vehicle(0)->setDirection(
                mission_->vehicle(0)->direction() + 1);
    }

    if (key == KEY_y) {
        mission_->vehicle(0)->setDirection(
                mission_->vehicle(0)->direction() - 1);
    }
#endif

#endif //_DEBUG

    if (change)
        needRendering();

    return consumed;
}

void GameplayMenu::drawAgentSelectors() {
    // 64x46
    g_App.gameSprites().sprite(isAgentSelected(0) ? 1772 : 1748)->draw(
            0, 0, 0);
    g_App.gameSprites().sprite(isAgentSelected(1) ? 1773 : 1749)->draw(
            64, 0, 0);

    // performance meters, select all button

    g_App.gameSprites().sprite(isAgentSelected(2) ? 1774 : 1752)->draw(
            0, 46 + 44 + 10, 0);
    g_App.gameSprites().sprite(isAgentSelected(3) ? 1775 : 1753)->draw(
            64, 46 + 44 + 10, 0);

    // draw health bars
    for (int a = 0; a < 4; a++) {
        int ydiff = 36 * mission_->ped(a)->health() / mission_->ped(a)->startHealth();
        g_Screen.drawRect(((a % 2) == 1 ? 64 : 0) + 51,
            (a > 1 ? 46 + 44 + 10 : 0) + 6 + 36 - ydiff, 7, ydiff, 12);
    }

    //draw animation within selectors
    mission_->ped(0)->drawSelectorAnim(32,38);
    mission_->ped(1)->drawSelectorAnim(96,38);
    mission_->ped(2)->drawSelectorAnim(32,138);
    mission_->ped(3)->drawSelectorAnim(96,138);
}

void GameplayMenu::drawPerformanceMeters() {
    // 64x46
    g_App.gameSprites().sprite(isAgentSelected(0) ? 1778 : 1754)->draw(
            0, 46, 0);
    g_App.gameSprites().sprite(isAgentSelected(1) ? 1778 : 1755)->draw(
            64, 46, 0);

    // select all button, agent selectors

    g_App.gameSprites().sprite(isAgentSelected(2) ? 1778 : 1754)->draw(
            0, 46 + 44 + 10 + 46, 0);
    g_App.gameSprites().sprite(isAgentSelected(3)? 1778 : 1755)->draw(
            64, 46 + 44 + 10 + 46, 0);
}

void GameplayMenu::drawSelectAllButton() {
    // 64x10
    if(pressed_btn_select_all_) {
        g_App.gameSprites().sprite(1792)->draw(0, 46 + 44, 0);
        g_App.gameSprites().sprite(1793)->draw(64, 46 + 44, 0);
    } else {
        g_App.gameSprites().sprite(1796)->draw(0, 46 + 44, 0);
        g_App.gameSprites().sprite(1797)->draw(64, 46 + 44, 0);
    }
}

void GameplayMenu::drawMissionHint(int elapsed) {

    elapsed += mission_hint_ticks_;
    int inc = elapsed / 45;
    mission_hint_ticks_ = elapsed % 45;

    g_App.gameSprites().sprite(1798)->draw(
        0, 46 + 44 + 10 + 46 + 44 - 1, 0);
    g_App.gameSprites().sprite(1799)->draw(
        64, 46 + 44 + 10 + 46 + 44 - 1, 0);

    mission_hint_ += inc;

    bool inversed = false;
    bool text_pw = (pointing_at_weapon_ != -1
        && mission_->weapon(pointing_at_weapon_)->map() != -1);

    std::string str;

    uint8 txtColor;

    if ((mission_hint_ > 20 && mission_hint_ < 41)
        || (mission_hint_ > 60))
    {
        for (int i = 0; i < 4; i++) {
            if (isAgentSelected(i)){
                if (mission_->ped(i)->speed()) {
                    str = g_App.menus().getMessage("HINT_GOING");
                } else {
                    str = g_App.menus().getMessage("HINT_OBSERVING");
                }
                if (mission_->ped(i)->wePickupWeapon()) {
                    str = g_App.menus().getMessage("HINT_PICKUP_WEAPON");
                }
                if (mission_->ped(i)->drawnAnim() == PedInstance::ad_HitAnim) {
                    str = g_App.menus().getMessage("HINT_HIT_BY_BULLET");
                }
            }
        }
        txtColor = 14;

        if (mission_hint_ > 79) {
            mission_hint_ = 0;
            return;
        }
    } else {

        inversed = (mission_hint_ % 5) > 2;
        txtColor = inversed ? 0 : 11;

        if (mission_) {
            mission_->objectiveMsg(str);
            if (mission_->failed()) {
                if (!completed_) {
                    completed_ = true;
                    g_App.gameSounds().play(snd::SPEECH_MISSION_FAILED);
                }

                str = g_App.menus().getMessage("HINT_MISSION_FAILED");
                text_pw = false;
            }

            if (mission_->completed()) {
                if (!completed_) {
                    completed_ = true;
                    g_App.gameSounds().play(snd::SPEECH_MISSION_COMPLETED);
                }

                str = g_App.menus().getMessage("HINT_MISSION_COMPLETE");
                text_pw = false;
            }

            if (mission_hint_ > 40 && mission_hint_ < 61)
                if (mission_->completed() || mission_->failed()) {
                    str = g_App.menus().getMessage("HINT_PRESS_SPACE");
                    text_pw = false;
                }
        }

        if (inversed && !text_pw) {
            g_Screen.drawRect(0, 46 + 44 + 10 + 46 + 44, 128, 12, 11);
        } else {
            if (text_pw) {
                str = mission_->weapon(pointing_at_weapon_)->name();
                txtColor = inversed ? 14 : 11;
            }
        }
    }

    int width = gameFont()->textWidth(str.c_str(), false, false);
    int x = 64 - width / 2;
	gameFont()->drawText(x, 46 + 44 + 10 + 46 + 44 + 2 - 1, str.c_str(), txtColor);
}

void GameplayMenu::drawWeaponSelectors() {
    PedInstance *p = NULL;

    if (isAgentSelected(3))
        p = mission_->ped(3);

    if (isAgentSelected(2))
        p = mission_->ped(2);

    if (isAgentSelected(1))
        p = mission_->ped(1);

    if (isAgentSelected(0))
        p = mission_->ped(0);

    if (p) {
        bool draw_pw = true;
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < 4; i++) {
                WeaponInstance *wi = NULL;
                int s = 1601;
                // NOTE: weapon selectors can be drawn by drawFrame instead
                // of using current draw(), animations are folowing:
                // 285,286 empty selector :: 287 persuadatron 289
                // 291 pistol 293 :: 295 gauss gun 297 :: 299 shotgun 301
                // 303 uzi 305 :: 307 minigun 309 :: 311 laser gun 313
                // 315 flamer 317 :: 319 long range 321 :: 323 scanner 325
                // 327 medikit 329 :: 331 time bomb 333 :: 343 access card 345
                // 351 energy shield 353

                if (i + j * 4 < p->numWeapons()) {
                    wi = p->weapon(i + j * 4);
                    s = wi->getWeaponClass()->selector();
                    if (p->selectedWeapon() && p->selectedWeapon() == wi)
                        s += 40;
                } else {
                    if (draw_pw && pointing_at_weapon_ != -1
                        && (mission_hint_ % 20) < 10
                        && mission_->weapon(pointing_at_weapon_)->map() != -1)
                    {
                        draw_pw = false;
                        wi = mission_->weapon(pointing_at_weapon_);
                        s = wi->getWeaponClass()->selector() + 40;
                    }
                }

                g_App.gameSprites().sprite(s)->draw(
                        32 * i, 2 + 46 + 44 + 10 + 46 + 44 + 15 + j * 32, 0);

                // draw ammo bars
                if (wi) {
                    int n;

                    if (wi->ammo() == -1)
                        continue;
                    if (wi->ammo() == 0)
                        n = 25;
                    else
                        n = 25 * wi->ammoRemaining() / wi->ammo();

                    g_Screen.drawRect(32 * i + 3, 46 + 44 + 10 + 46 + 44 + 15 + j * 32 + 23 + 2,
                        n, 5, 12);
                }
            }
        }
    } else {
        for (int j = 0; j < 2; j++)
            for (int i = 0; i < 4; i++) {
                int s = 1601;

                g_App.gameSprites().sprite(s)->draw(
                        32 * i, 2 + 46 + 44 + 10 + 46 + 44 + 15 + j * 32, 0);
            }

    }
}

void GameplayMenu::drawMiniMap() {
    if (mission_ == 0)
        return;

    int sy = 46 + 44 + 10 + 46 + 44 + 15 + 2 * 32 + 2;

    int ox, oy;
    Map *m = g_App.maps().map(mission_->map());
    int tx = m->screenToTileX(world_x_ + (GAME_SCREEN_WIDTH - 129) / 2,
            world_y_ + GAME_SCREEN_HEIGHT / 2, ox);
    int ty = m->screenToTileY(world_x_ + (GAME_SCREEN_WIDTH - 129) / 2,
            world_y_ + GAME_SCREEN_HEIGHT / 2, oy);

    for (int i = 0; i < 4; i++)
        if (isAgentSelected(i)) {
            tx = mission_->ped(i)->tileX();
            ty = mission_->ped(i)->tileY();
            ox = mission_->ped(i)->offX();
            oy = mission_->ped(i)->offY();
            break;
        }

    tx -= 8;
    ty -= 8;
    mm_tx_ = tx;
    mm_ty_ = ty;
    // every 32 of offset = 1 pixel
    ox = ox / 32;
    oy = oy / 32;

    uint8 minimap_layer[21*21*8*8];
    memset(minimap_layer, 0, 21*21*8*8);
    uint8 minimap_final_layer[128*128];

    for (int j = 0; j < 17; j++) {
        for (int i = 0; i < 17; i++) {
            uint8 gcolour = mission_->getMinimapColour(tx + i, ty + j);
            for (char inc = 0; inc < 8; inc ++) {
                memset(minimap_layer + (j + 2) * 8 * 8 * 21 + (i + 2) * 8
                    + inc * 8 * 21, gcolour, 8);
            }
        }
    }
    //g_Screen.blit(130, 10, 168, 168, minimap_layer);
    for (int j = 0; j < 128; j++) {
        memcpy(minimap_final_layer + 128 * j, minimap_layer + (8 * 8 * 21) * 2
            + (j + oy) * 8 * 21 + (8 * 2) + ox, 128);
    }
    g_Screen.blit(0, sy, 128, 128, minimap_final_layer);
        // TODO: draw icons for units, weapons, etc

}

int GameplayMenu::selectedAgentsCount() {
    int agents = 0;

    for (int i = 0; i < 4; i++)
        if (isAgentSelected(i))
            agents++;

    return agents;
}

/*!
 * Select the given agent.
 * \param agentNo Agent id
 * \param addToGroup If true, agent is added to the current selection.
 * If not, the selection is emptied and filled with the new agent.
 */
void GameplayMenu::selectAgent(unsigned int agentNo, bool addToGroup) {
    if (addToGroup)
        selected_agents_ ^= 1 << agentNo;
    else
        selected_agents_ = 1 << agentNo;
    pressed_btn_select_all_ = false;
}

/*!
 * Selects all agents.
 * \param invert If true, selects only the agents that were not already
 * selected.
 */
void GameplayMenu::selectAllAgents(bool invert) {
    if (invert) {
        if ((selectable_agents_ & (1 << 0)) != 0)
            selected_agents_ ^= 1;
        if ((selectable_agents_ & (1 << 1)) != 0)
            selected_agents_ ^= 2;
        if ((selectable_agents_ & (1 << 2)) != 0)
            selected_agents_ ^= 4;
        if ((selectable_agents_ & (1 << 3)) != 0)
            selected_agents_ ^= 8;
        pressed_btn_select_all_ = false;
    } else {
        if ((selected_agents_ & 0xF0) != 0) {
            selected_agents_ = selected_agents_ >> 4;
            pressed_btn_select_all_ = false;
        } else {
            if (selectable_agents_ != 0) {
                selected_agents_ = (selected_agents_ << 4) | selectable_agents_;
                pressed_btn_select_all_ = true;
            } else
                pressed_btn_select_all_ = false;
        }
    }
}
