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
#include "file.h"
#include "loadingmenu.h"
#include "mapmenu.h"
#include "gameplaymenu.h"
#ifdef _WIN32
#include <windows.h>
#endif

// The number of pixel of a scroll
const int SCROLL_STEP = 16;

GameplayMenu::GameplayMenu(MenuManager *m, LoadingMenu *loading,
        MapMenu *mapMenu) :
Menu(m, "Gameplay", "", ""), loading_(loading), map_menu_(mapMenu),
tick_count_(0), last_animate_tick_(0), last_motion_tick_(0),
last_motion_x_(320), last_motion_y_(240), mission_(0), world_x_(0),
world_y_(0), selected_agents_(0), ctrl_(false), alt_(false),
pointing_at_ped_(-1), pointing_at_vehicle_(-1), 
mission_hint_ticks_(0), mission_hint_(0)
{
    setParentMenu("debrief");
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

    // Scroll the map
    if (scroll_x_ != 0 || scroll_y_ != 0) {
        change = scroll();
        scroll_x_ = 0;
        scroll_y_ = 0;
    }

    if (tick_count_ - last_animate_tick_ > 33) {
        int diff = tick_count_ - last_animate_tick_;
        last_animate_tick_ = tick_count_;

        for (int i = 0; i < mission_->numPeds(); i++)
            change |= mission_->ped(i)->animate(diff, mission_);

        for (int i = 0; i < mission_->numVehicles(); i++)
            change |= mission_->vehicle(i)->animate(diff);

        for (int i = 0; i < mission_->numWeapons(); i++)
            change |= mission_->weapon(i)->animate(diff);

        for (int i = 0; i < mission_->numStatics(); i++)
            change |= mission_->statics(i)->animate(diff);
    }

    if (change) {
        show(false);
        // force pointing_at_ped / vehicle to update
        handleMouseMotion(last_motion_x_, last_motion_y_, 0);
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

int qanim = 1085, qframe = 0;

void GameplayMenu::handleShow()
{
    if (mission_ == NULL) {
        mission_ = loading_->mission();
        mission_->start();
        completed_ = false;
        g_App.music().playTrack(MusicManager::TRACK_ASSASSINATE);

        // TODO: choose the right game palette.
        g_App.setPalette("hpal02.dat");
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
                if(mission_->ped(i)->isAsAgent() == PedInstance::Agent_Active){
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
    }

    g_Screen.clear(0);
    mission_->drawMap(world_x_, world_y_);
    drawAgentSelectors();
    drawPerformanceMeters();
    drawSelectAllButton();
    drawMissionHint(0);
    drawWeaponSelectors();
    drawMiniMap();

    if (ctrl_) {
        if (isAgentSelected(0))
            mission_->ped(0)->showPath(world_x_, world_y_);

        if (isAgentSelected(1))
            mission_->ped(1)->showPath(world_x_, world_y_);

        if (isAgentSelected(2))
            mission_->ped(2)->showPath(world_x_, world_y_);

        if (isAgentSelected(3))
            mission_->ped(3)->showPath(world_x_, world_y_);
    }
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
#if 0
    g_App.gameSprites().drawFrame(qanim, qframe, 320, 200);
#endif
}

void GameplayMenu::handleLeave()
{
    g_System.hideCursor();
    g_App.setPalette("mselect.pal");
    mission_->end();
    mission_ = NULL;
    g_App.music().stopPlayback();
}

void GameplayMenu::handleMouseMotion(int x, int y, int state)
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

    if (alt_)
        return;

    pointing_at_ped_ = -1;

    for (int i = 8; mission_ && i < mission_->numPeds(); i++) {
        PedInstance *p = mission_->ped(i);
        int px = p->screenX() - 20;
        int py = p->screenY() - 30;

        if (x - 129 + world_x_ >= px && y + world_y_ >= py &&
            x - 129 + world_x_ < px + 40 && y + world_y_ < py + 32) {
            pointing_at_ped_ = i;
            break;
        }
    }

    pointing_at_vehicle_ = -1;

    for (int i = 0; mission_ && i < mission_->numVehicles(); i++) {
        VehicleInstance *v = mission_->vehicle(i);
        int px = v->screenX() - 20;
        int py = v->screenY() - 30;

        if (x - 129 + world_x_ >= px && y + world_y_ >= py &&
            x - 129 + world_x_ < px + 40 && y + world_y_ < py + 32) {
            pointing_at_vehicle_ = i;
            break;
        }
    }

    pointing_at_weapon_ = -1;

    for (int i = 0; mission_ && i < mission_->numWeapons(); i++) {
        WeaponInstance *w = mission_->weapon(i);

        if (w->map() != -1) {
            int px = w->screenX() - 10;
            int py = w->screenY() - 10;

            if (x - 129 + world_x_ >= px && y + world_y_ >= py &&
                x - 129 + world_x_ < px + 20 && y + world_y_ < py + 20) {
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
                    && mission_->ped(i)->selectedWeapon()
                    && mission_->ped(pointing_at_ped_)->health() > 0)
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

void GameplayMenu::handleMouseDown(int x, int y, int button)
{
    bool change = false; /* Indicator whether we need menu redraw */

    if (button == 1) {
      /* Handle agent selection. Click on an agent changes selection
       * to it. If control key is pressed, add or removes agent from
       * current selection. */
        if (x < 128) {
            if (y < 42) {
                if (x < 64) {
                    if ((selectable_agents_ & (1 << 0)) != 0) {
                        selectAgent(0);
                        change = true;
                    }
                }
                else {
                    if ((selectable_agents_ & (1 << 1)) != 0) {
                        selectAgent(1);
                        change = true;
                    }
                }
            }

            if (y >= 42 + 48 + 10 && y < 42 + 48 + 10 + 42) {
                if (x < 64) {
                    if ((selectable_agents_ & (1 << 2)) != 0) {
                        selectAgent(2);
                        change = true;
                    }
                }
                else {
                    if ((selectable_agents_ & (1 << 3)) != 0) {
                        selectAgent(3);
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
        }
        else {
            int ox, oy;
            int tx =
                g_App.maps().screenToTileX(mission_->map(), world_x_ + x - 129,
                        world_y_ + y, ox);
            int ty =
                g_App.maps().screenToTileY(mission_->map(), world_x_ + x - 129,
                        world_y_ + y, oy);

            for (int i = 0; i < 4; i++) {
                if (isAgentSelected(i)) {
                    if (pointing_at_weapon_ != -1) {
                        mission_->ped(i)->pickupWeapon(
                                mission_->weapon(pointing_at_weapon_));
                    }
                    else if (pointing_at_vehicle_ != -1) {
                        if (mission_->ped(i)->inVehicle())
                            mission_->ped(i)->leaveVehicle();
                        else {
                            if (mission_->vehicle(pointing_at_vehicle_)->
                                    health() > 0)
                                mission_->ped(i)->putInVehicle(
                                        mission_->vehicle(
                                                pointing_at_vehicle_));
                        }
                    }
                    else if (mission_->ped(i)->inVehicle()) {
                        if (ctrl_)
                            mission_->ped(i)->inVehicle()->
                                addDestination(tx, ty, 0,
                                128, 128, 320);
                        else
                            mission_->ped(i)->inVehicle()->
                                setDestinationV(tx, ty, 0, 
                                128, 128, 480);
                    }
                    else {
                        if (ctrl_)
                            mission_->ped(i)->addDestination(tx, ty, 0,
                                    ox, oy, 320);
                        else {
                            if (selectedAgentsCount() > 1) {
                                //TODO: current group position is like
                                // in original this can make non-tile
                                // oriented
                                //int dx = (i % 2) * (i - 2) * 16;
                                //int dy = ((i + 1) % 2) * (i - 1) * 8;

                                int dx = 0; int dy = 0;
                                int offset_x = world_x_ + x - 129;
                                int offset_y = world_y_ + y;

                                tx = g_App.maps().screenToTileX(
                                        mission_->map(),
                                        offset_x,
                                        offset_y, ox);
                                ty = g_App.maps().screenToTileY(
                                        mission_->map(),
                                        offset_x,
                                        offset_y, oy);

                                //this should be romoved if non-tile
                                //position needed
                                ox = 63 + 128 * (i % 2);
                                oy = 63 + 128 * (i >> 1);
                            }

                            mission_->ped(i)->setDestinationP(tx, ty, 0,
                                    ox, oy, 320);
                        }
                    }
                }
            }
        }
    }
    else if (button == 3) {
        for (int i = 0; i < 4; i++)
            if (isAgentSelected(i)
                    && mission_->ped(i)->selectedWeapon()) {
                if (pointing_at_ped_ != -1
                        && mission_->ped(i)->inRange(
                                mission_->ped(pointing_at_ped_))) {
                    mission_->ped(i)->setTarget(
                            mission_->ped(pointing_at_ped_));
                }
                else if (pointing_at_vehicle_ != -1
                        && mission_->ped(i)->inRange(
                                mission_->vehicle(pointing_at_vehicle_))) {
                    mission_->ped(i)->setTarget(
                            mission_->vehicle(pointing_at_vehicle_));
                }
                else if (pointing_at_ped_ == -1
                        && pointing_at_vehicle_ == -1) {
                    mission_->ped(i)->setTarget(world_x_ + x - 129,
                            world_y_ + y);
                }
                WeaponInstance *w = mission_->ped(i)->selectedWeapon();
                mission_->ped(i)->setHitDamage(w->shot());
            }
    }

    // handle weapon selectors
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < 4; i++) {
            if (x >= 32 * i && x < 32 * i + 32
                    && y >= 46 + 44 + 10 + 46 + 44 + 15 + j * 32
                    && y < 46 + 44 + 10 + 46 + 44 + 15 + j * 32 + 32) {
                for (int a = 0; a < 4; a++) {
                    if (isAgentSelected(a)) {
                        if (i + j * 4 < mission_->ped(a)->numWeapons()) {
                            if (button == 1) {
                                if (mission_->ped(a)->selectedWeapon()
                                        == mission_->ped(a)->weapon(i + j * 4))
                                    mission_->ped(a)->setSelectedWeapon(-1);
                                else {
                                    mission_->ped(a)->setSelectedWeapon(
                                            i + j * 4);
                                }
                            }
                            else
                                mission_->ped(a)->dropWeapon(i + j * 4);

                            change = true;
                        }
                    }
                }
            }
        }
    }

    if (change)
        show(false);
}

void GameplayMenu::handleMouseUp(int x, int y, int button) {
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

void GameplayMenu::handleUnknownKey(Key key, KeyMod mod, bool pressed) {
    bool change = false; /* indicator whether menu should be redrawn */

    // SPACE is pressed when the mission failed or succeeded to return
    // to menu
    if (key == KEY_SPACE && mission_) {
        if (mission_->completed()) {
            map_menu_->setBlkColour(g_App.currentBlk(),
                    g_App.getGameSession().getLogoColour());
            menu_manager_->changeCurrentMenu("misswin");
            return;
        }
        else if (mission_->failed()) {
            menu_manager_->changeCurrentMenu("misslose");
            return;
        }
    }

    if (key == KEY_LCTRL)
        ctrl_ = pressed;
    else if (key == KEY_RCTRL)
        ctrl_ = pressed;

    if (key == KEY_LALT) {
        alt_ = pressed;
        printf("%i\n", pointing_at_ped_);

        if (pointing_at_ped_ != -1 && !alt_) {
            int ox, oy;
            int tx = g_App.maps().screenToTileX(mission_->map(),
                    last_motion_x_ - 129 + world_x_,
                    last_motion_y_ + world_y_, ox);
            int ty = g_App.maps().screenToTileY(mission_->map(),
                    last_motion_x_ - 129 + world_x_,
                    last_motion_y_ + world_y_, oy);
            mission_->ped(pointing_at_ped_)->setTileX(tx);
            mission_->ped(pointing_at_ped_)->setTileY(ty);
            mission_->ped(pointing_at_ped_)->setOffX(ox);
            mission_->ped(pointing_at_ped_)->setOffY(oy);
        }
    }


    /* Handle agent selection by numeric keys. Key 0 cycles between current
     * selection and all 4 agents. Ctrl + 0 inverts selection.
     * Individual keys select the specified agent unless ctrl is pressed -
     * then they add/remove agent from current selection. */
    if (key == KEY_0 || key == KEY_KP0 || key == KEY_BACKQUOTE) {
        /* This code is exactly the same as for clicking on "group-button"
         * as you can see above. */
        selectAllAgents();
        change = true;
    }
    else if (key == KEY_1 || key == KEY_KP1) {
        if ((selectable_agents_ & (1 << 0)) != 0) {
            selectAgent(0);
            change = true;
        }
    }
    else if (key == KEY_2 || key == KEY_KP2) {
        if ((selectable_agents_ & (1 << 1)) != 0) {
            selectAgent(1);
            change = true;
        }
    }
    else if (key == KEY_3 || key == KEY_KP3) {
        if ((selectable_agents_ & (1 << 2)) != 0) {
            selectAgent(2);
            change = true;
        }
    }
    else if (key == KEY_4 || key == KEY_KP4) {
        if ((selectable_agents_ & (1 << 3)) != 0) {
            selectAgent(3);
            change = true;
        }
    }

    // Scroll the map with the direction keys
    if (key == KEY_LEFT) {
        scroll_x_ = -SCROLL_STEP;
    } else if (key == KEY_RIGHT) {
        scroll_x_ = SCROLL_STEP;
    }

    if (key == KEY_UP) {
        scroll_y_ = -SCROLL_STEP;
    } else if (key == KEY_DOWN) {
        scroll_y_ = SCROLL_STEP;
    }

    // Music Control
    if (key == KEY_F1) {
        g_App.music().toggleMusic();
    }

    // Sound Control
    if (key == KEY_F2) {
        g_App.gameSounds().toggleSound();
    }

#if 0
    Ped *ped = g_App.peds().ped(0);

    if (key == KEY_q)
        ped->init(ped->head() - 1, ped->torso(), ped->legs());

    if (key == KEY_w)
        ped->init(ped->head() + 1, ped->torso(), ped->legs());

    if (key == KEY_a)
        ped->init(ped->head(), ped->torso() - 20, ped->legs());

    if (key == KEY_s)
        ped->init(ped->head(), ped->torso() + 20, ped->legs());

    if (key == KEY_z)
        ped->init(ped->head(), ped->torso(), ped->legs() - 1);

    if (key == KEY_x)
        ped->init(ped->head(), ped->torso(), ped->legs() + 1);

    if (key == KEY_c)
        ped->init(ped->head(), ped->torso(), ped->torso() + 100);

    if (key == KEY_SPACE)
        printf("%i %i %i\n", ped->head(), ped->torso(), ped->legs());
#endif

#if 0
    static int n = 0;

    if (key == KEY_a && n > 0)
        n--;

    if (key == KEY_s && n < mission_->numPeds() - 1)
        n++;

    if (key == KEY_SPACE) {
        PedInstance *p = mission_->ped(n);
        p->animate();
    }

    if (key == KEY_p)
        printf("%d\n", n);
#endif

#if 0
    if (key == KEY_UP)
        mission_->ped(0)->setTileY(mission_->ped(0)->tileY() - 1);

    if (key == KEY_DOWN)
        mission_->ped(0)->setTileY(mission_->ped(0)->tileY() + 1);

    if (key == KEY_LEFT)
        mission_->ped(0)->setTileX(mission_->ped(0)->tileX() - 1);

    if (key == KEY_RIGHT)
        mission_->ped(0)->setTileX(mission_->ped(0)->tileX() + 1);

    printf("%i %i\n", mission_->ped(0)->tileX(), mission_->ped(0)->tileY());
#endif

#if 0
    if (key == KEY_LEFT)
        inc--;

    if (key == KEY_RIGHT)
        inc++;

    if (key == KEY_SPACE)
        printf("%i\n", inc);
#endif

#ifdef _DEBUG

    if (key == KEY_PAGEUP)
        mission_->ped(0)->setTileZ(mission_->ped(0)->tileZ() + 1);

    if (key == KEY_PAGEDOWN) {
        if (mission_->ped(0)->tileZ() > 0)
            mission_->ped(0)->setTileZ(mission_->ped(0)->tileZ() - 1);
    }
    
    if (key == KEY_o)
        topz++;

    if (key == KEY_l)
        topz--;

    if (key == KEY_d) {
        for (int i = 8; i < mission_->numPeds(); i++)
            mission_->ped(i)->setHealth(0);
    }

    if (key == KEY_a) {
        qanim--;

        if (qanim < 0)
            qanim = 0;
    }

    if (key == KEY_s)
        qanim++;

    if (key == KEY_g)
        qanim -= 8;

    if (key == KEY_h)
        qanim += 8;

    if (key == KEY_x)
        qframe++;

    if (key == KEY_z)
        qframe--;

    if (key == KEY_SPACE)
        printf("qanim %i qframe %i\n", qanim, qframe);

    if (key == KEY_t) {
        mission_->vehicle(0)->setDirection(
                mission_->vehicle(0)->direction() + 1);
    }

    if (key == KEY_y) {
        mission_->vehicle(0)->setDirection(
                mission_->vehicle(0)->direction() - 1);
    }

#endif //_DEBUG

    if (change)
        show(false);
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
        for (int j = 36 - 36 * mission_->ped(a)->health()
                / mission_->ped(a)->startHealth(); j < 36; j++) {
            for (int i = 0; i < 7; i++) {
                g_Screen.setPixel(((a % 2) == 1 ? 64 : 0) + 51 + i,
                        (a > 1 ? 46 + 44 + 10 : 0) + 6 + j, 12);
            }
        }
    }

    //draw animation within selectors
    mission_->ped(0)->drawSelectorAnim(32,38);
    mission_->ped(1)->drawSelectorAnim(96,38);
    mission_->ped(2)->drawSelectorAnim(32,138);
    mission_->ped(3)->drawSelectorAnim(96,138);
}

void GameplayMenu::drawPerformanceMeters() {
    // 64x44
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
    g_App.gameSprites().sprite((selected_agents_ & 15) ==
        15 ? 1792 : 1796)->draw(0, 46 + 44, 0);
    g_App.gameSprites().sprite((selected_agents_ & 15) ==
        15 ? 1793 : 1797)->draw(64, 46 + 44, 0);
}

static int drawChar(int x, int y, char ch, uint8 color) {
    if (ch == ' ')
        return 3;

    Sprite *s = g_App.menuSprites().sprite(665 + ch - 'A');
    uint8 *data = new uint8[s->width() * s->height()];
    s->data(data);

    for (int i = 0; i < s->width() * s->height(); i++)
        data[i] = (data[i] == 252 ? color : 255);

    g_Screen.blit(x, y, s->width(), s->height(), data);
    delete[] data;
    return s->width();
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

    const char *str = "";

    uint8 txtColor;

    if ((mission_hint_ > 20 && mission_hint_ < 41)
        ||(mission_hint_ > 60)) {

        for (int i = 0; i < 4; i++) {
            if (isAgentSelected(i)){
                if (mission_->ped(i)->speed()) {
                    str = "GOING";
                } else {
                    str = "OBSERVING";
                }
                if (mission_->ped(i)->wePickupWeapon()) {
                    str = "PICKUP WEAPON";
                }
                if (mission_->ped(i)->getDrawnAnim() == PedInstance::HitAnim) {
                    str = "HIT BY BULLET";
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

        if (inversed) {
            for (int j = 0; j < 11; j++) {
                for (int i = 0; i < 128; i++)
                    g_Screen.setPixel(i, 46 + 44 + 10 + 46 + 44 + j , 11);
            }
        }

        if (mission_) {
            // TODO: check these
            switch (mission_->objective()) {
            case 1:
                str = "PERSUADE";
                break;
            case 2:
                str = "ASSASSINATE";
                break;
            case 3:
                str = "PROTECT";
                break;
            case 5:
                str = "GET WEAPON";
                break;
            case 10:
            case 11:
                str = "ELIMINATE";
                break;
            case 14:
                str = "RESCUE";
                break;
            case 15:
                str = "USE VEHICLE";
                break;
            default:
                break;
            }

            if (mission_->failed()) {
                if (!completed_) {
                    completed_ = true;
                    g_App.gameSounds().play(snd::SPEECH_MISSION_FAILED);
                }

                str = "MISSION FAILED";
            }

            if (mission_->completed()) {
                if (!completed_) {
                    completed_ = true;
                    g_App.gameSounds().play(snd::SPEECH_MISSION_COMPLETED);
                }

                str = "MISSION COMPLETE";
            }

            if (mission_->completed() || mission_->failed())
                str = "PRESS SPACE";
        }
    }

    int x = 0;
    const char *t = str;

    while (*t)
        x += g_App.menuSprites().sprite(665 + *t++ - 'A')->width() - 1;

    x = 64 - x / 2;

    while (*str)
        x += drawChar(x, 46 + 44 + 10 + 46 + 44 + 2 - 1, *str++, txtColor) - 1;
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
        for (int j = 0; j < 2; j++)
            for (int i = 0; i < 4; i++) {
                WeaponInstance *w = NULL;
                int s = 1601;

                if (i + j * 4 < p->numWeapons()) {
                    w = p->weapon(i + j * 4);
                    s = w->selector();
                }

                if (p->selectedWeapon() && p->selectedWeapon() == w)
                    s += 40;

                g_App.gameSprites().sprite(s)->draw(
                        32 * i, 2 + 46 + 44 + 10 + 46 + 44 + 15 + j * 32, 0);

                // draw ammo bars
                if (w) {
                    int n;

                    if (w->ammo() == 0)
                        n = 25;
                    else
                        n = 25 * w->ammoRemaining() / w->ammo();

                    for (int m = 0; m < n; m++)
                        for (int k = 0; k < 5; k++)
                            g_Screen.setPixel(32 * i + 3 + m,
                                    46 + 44 + 10 + 46 + 44 + 15 + j * 32 + 23
                                        + k + 2,
                                    12);
                }
            }
    }
}

int mcolors_[] = {
    8,  7,  7,  7,
    7,  7, 10, 10,
   10, 10,  0, 10,
   15, 15, 10, 10,
   0,
};

void GameplayMenu::drawMiniMap() {
    if (mission_ == 0)
        return;

    int sy = 46 + 44 + 10 + 46 + 44 + 15 + 2 * 32 + 2;

    int ox, oy;
    int tx = g_App.maps().map(mission_->map())->screenToTileX(
            world_x_ + (GAME_SCREEN_WIDTH - 129) / 2,
            world_y_ + GAME_SCREEN_HEIGHT / 2, ox);
    int ty = g_App.maps().map(mission_->map())->screenToTileY(
            world_x_ + (GAME_SCREEN_WIDTH - 129) / 2,
            world_y_ + GAME_SCREEN_HEIGHT / 2, oy);

    for (int i = 0; i < 4; i++)
        if (isAgentSelected(i)) {
            tx = mission_->ped(i)->tileX();
            ty = mission_->ped(i)->tileY();
            break;
        }

    tx -= 8;
    ty -= 8;
    mm_tx_ = tx;
    mm_ty_ = ty;

    for (int j = 0; j < 16; j++)
        for (int i = 0; i < 16; i++) {
            // TODO: still needs tweaking
            int t =
                g_App.maps().map(mission_->map())->tileAt(tx + i, ty + j, 0);

            uint8 ground[8 * 8];
            memset(ground, mcolors_[g_App.walkdata_[t]], sizeof(ground));
            g_Screen.blit(i * 8, j * 8 + sy, 8, 8, ground);
        }

        // TODO: draw icons for units, weapons, etc
}

int GameplayMenu::selectedAgentsCount() {
    int agents = 0;

    for (int i = 0; i < 4; i++)
        if (isAgentSelected(i))
            agents++;

    return agents;
}

void GameplayMenu::selectAgent(unsigned int agentNo) {
    if (ctrl_)
        selected_agents_ ^= 1 << agentNo;
    else
        selected_agents_ = 1 << agentNo;
}

void GameplayMenu::selectAllAgents() {
    if (ctrl_) {
        if ((selectable_agents_ & (1 << 0)) != 0)
            selected_agents_ ^= 1;
        if ((selectable_agents_ & (1 << 1)) != 0)
            selected_agents_ ^= 2;
        if ((selectable_agents_ & (1 << 2)) != 0)
            selected_agents_ ^= 4;
        if ((selectable_agents_ & (1 << 3)) != 0)
            selected_agents_ ^= 8;
    }
    else {
        if ((selected_agents_ & selectable_agents_) == selectable_agents_)
            selected_agents_ = selected_agents_ >> 4;
        else
            selected_agents_ = (selected_agents_ << 4) | selectable_agents_;
    }
}
