/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2007  Davor Ocelic <docelic@mail.inet.hr>			*
 *                                                                      *
 *    This program is free software;  you can redistribute it and / or  *
 *  modify it  under the  terms of the  GNU General  Public License as  *
 *  published by the Free Software Foundation; either version 2 of the  *
 *  License, or (at your option) any later version.                     *
 *                                                                      *
 *    This program is  distributed in the hope that it will be useful,  *
 *but WITHOUT  ANY WARRANTY;without even  the impliedwarranty of  *
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

GameplayMenu::GameplayMenu(MenuManager *m, LoadingMenu *loading,
        MapMenu *mapMenu) :
Menu(m, "Gameplay", "", ""), loading_(loading), map_menu_(mapMenu),
tick_count_(0), last_animate_tick_(0), last_motion_tick_(0),
last_motion_x_(320), last_motion_y_(240), mission_(0), scroll_x_(0),
scroll_y_(0), selected_agents_(1), ctrl_(false), alt_(false),
pointing_at_ped_(-1), pointing_at_vehicle_(-1)
{
    setParentMenu("debrief");
}

int inc = 16;

void GameplayMenu::handleTick(int elapsed)
{
    bool change = false;
    tick_count_ += elapsed;

    if (last_motion_x_ < 5) {
        int newScrollX = scroll_x_ - inc;

        if (newScrollX < 0)
            newScrollX = 0;

        if (isScrollLegal(newScrollX, scroll_y_)) {
            scroll_x_ = newScrollX;
            change = true;
        }
    }

    if (last_motion_y_ < 5) {
        int newScrollY = scroll_y_ - inc;

        if (newScrollY < 0)
            newScrollY = 0;

        if (isScrollLegal(scroll_x_, newScrollY)) {
            scroll_y_ = newScrollY;
            change = true;
        }
    }

    if (last_motion_x_ > GAME_SCREEN_WIDTH - 5) {
        int newScrollX = scroll_x_ + inc;

        if (newScrollX > mission_->mapWidth() - GAME_SCREEN_WIDTH)
            newScrollX = mission_->mapWidth() - GAME_SCREEN_WIDTH;

        if (isScrollLegal(newScrollX, scroll_y_)) {
            scroll_x_ = newScrollX;
            change = true;
        }
    }

    if (last_motion_y_ > GAME_SCREEN_HEIGHT - 5) {
        int newScrollY = scroll_y_ + inc;

        if (newScrollY > mission_->mapHeight() - GAME_SCREEN_HEIGHT)
            newScrollY = mission_->mapHeight() - GAME_SCREEN_HEIGHT;

        if (isScrollLegal(scroll_x_, newScrollY)) {
            scroll_y_ = newScrollY;
            change = true;
        }
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

    for (int i = 0; i < 4; i++)
        if (mission_->ped(i) && mission_->ped(i)->selectedWeapon()) {
            WeaponInstance *w = mission_->ped(i)->selectedWeapon();

            if (w->ammoRemaining() == 0) {
                mission_->ped(i)->selectNextWeapon();
            }
        }

    if (change) {
        show(false);
        // force pointing_at_ped / vehicle to update
        handleMouseMotion(last_motion_x_, last_motion_y_, 0);
    }

    drawMissionHint();
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
        scroll_x_ = mission_->startX();
        scroll_y_ = mission_->startY();
        int count = 0;

        while (!isScrollLegal(scroll_x_, scroll_y_) && count < 100) {
            improveScroll(scroll_x_, scroll_y_);
            count++;
        }

        selected_agents_ = 0;

        for (int i = 0; i < 4; i++)
            if (mission_->ped(i)) {
                selected_agents_ = 1 << i;
                break;
            }
    }

    g_Screen.clear(0);
    drawAgentSelectors();
    drawPerformanceMeters();
    drawSelectAllButton();
    drawMissionHint();
    drawWeaponSelectors();
    drawMiniMap();
    mission_->drawMap(scroll_x_, scroll_y_);

    if (ctrl_) {
        if (isAgentSelected(0))
            mission_->ped(0)->showPath(scroll_x_, scroll_y_);

        if (isAgentSelected(1))
            mission_->ped(1)->showPath(scroll_x_, scroll_y_);

        if (isAgentSelected(2))
            mission_->ped(2)->showPath(scroll_x_, scroll_y_);

        if (isAgentSelected(3))
            mission_->ped(3)->showPath(scroll_x_, scroll_y_);
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
    g_System.usePointerCursor();
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

    if (alt_)
        return;

    pointing_at_ped_ = -1;

    for (int i = 8; mission_ && i < mission_->numPeds(); i++) {
        PedInstance *p = mission_->ped(i);
        int px = p->screenX() - 20;
        int py = p->screenY() - 30;

        if (x - 129 + scroll_x_ >= px && y + scroll_y_ >= py &&
            x - 129 + scroll_x_ < px + 40 && y + scroll_y_ < py + 32) {
            pointing_at_ped_ = i;
            break;
        }
    }

    pointing_at_vehicle_ = -1;

    for (int i = 0; mission_ && i < mission_->numVehicles(); i++) {
        VehicleInstance *v = mission_->vehicle(i);
        int px = v->screenX() - 20;
        int py = v->screenY() - 30;

        if (x - 129 + scroll_x_ >= px && y + scroll_y_ >= py &&
            x - 129 + scroll_x_ < px + 40 && y + scroll_y_ < py + 32) {
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

            if (x - 129 + scroll_x_ >= px && y + scroll_y_ >= py &&
                x - 129 + scroll_x_ < px + 20 && y + scroll_y_ < py + 20) {
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
                    selectAgent(0);
                    change = true;
                }
                else {
                    selectAgent(1);
                    change = true;
                }
            }

            if (y >= 42 + 48 + 10 && y < 42 + 48 + 10 + 42) {
                if (x < 64) {
                    selectAgent(2);
                    change = true;
                }
                else {
                    selectAgent(3);
                    change = true;
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
                g_App.maps().screenToTileX(mission_->map(), scroll_x_ + x - 129,
                        scroll_y_ + y, ox);
            int ty =
                g_App.maps().screenToTileY(mission_->map(), scroll_x_ + x - 129,
                        scroll_y_ + y, oy);

            int spred = 64;
            for (int i = 0; i < 4; i++)
                if (isAgentSelected(i))
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
                                addDestination(tx, ty, 0, ox, oy, 320);
                        else
                            mission_->ped(i)->inVehicle()->
                                setDestination(tx, ty, 0, ox, oy, 320);
                    }
                    else {
                        if (ctrl_)
                            mission_->ped(i)->addDestination(tx, ty, 0,
                                    ox - spred, oy - spred, 320);
                        else {
                            if (selectedAgentsCount() > 1) {
                                int dx = (i % 2) * (i - 2) * 16;
                                int dy = ((i + 1) % 2) * (i - 1) * 8;

                                int offset_x = scroll_x_ + x - 129;
                                int offset_y = scroll_y_ + y;

                                // find center of tile
                                offset_x = offset_x - (offset_x % TILE_WIDTH)
                                        + TILE_WIDTH / 2;
                                offset_y = offset_y - (offset_y % TILE_HEIGHT)
                                        + TILE_HEIGHT / 2;

                                tx = g_App.maps().screenToTileX(
                                        mission_->map(),
                                        offset_x + dx,
                                        offset_y + dy, ox);
                                ty = g_App.maps().screenToTileY(
                                        mission_->map(),
                                        offset_x + dx,
                                        offset_y + dy, oy);
                            }

                            mission_->ped(i)->setDestination(tx, ty, 0,
                                    ox - spred, oy - spred, 320);
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
                                mission_->ped(pointing_at_ped_)))
                    mission_->ped(i)->setTarget(
                            mission_->ped(pointing_at_ped_));
                else if (pointing_at_vehicle_ != -1
                        && mission_->ped(i)->inRange(
                                mission_->vehicle(pointing_at_vehicle_)))
                    mission_->ped(i)->setTarget(
                            mission_->vehicle(pointing_at_vehicle_));
                else if (pointing_at_ped_ == -1
                        && pointing_at_vehicle_ == -1)
                    mission_->ped(i)->setTarget(scroll_x_ + x - 129,
                            scroll_y_ + y);
                WeaponInstance *w = mission_->ped(i)->selectedWeapon();
                mission_->ped(i)->setHitDamage(w->shot());
            }
    }

    // handle weapon selectors
    for (int j = 0; j < 2; j++)
        for (int i = 0; i < 4; i++)
            if (x >= 32 * i && x < 32 * i + 32
                    && y >= 46 + 44 + 10 + 46 + 44 + 15 + j * 32
                    && y < 46 + 44 + 10 + 46 + 44 + 15 + j * 32 + 32)
                for (int a = 0; a < 4; a++)
                    if (isAgentSelected(a)) {
                        if (i + j * 4 < mission_->ped(a)->numWeapons()) {
                            if (button == 1) {
                                if (mission_->ped(a)->selectedWeapon()
                                        == mission_->ped(a)->weapon(i + j * 4))
                                    mission_->ped(a)->setSelectedWeapon(-1);
                                else
                                    mission_->ped(a)->setSelectedWeapon(
                                            i + j * 4);
                            }
                            else {
                                mission_->ped(a)->dropWeapon(i + j * 4);
                            }

                            change = true;
                        }
                    }

    if (change)
        show(false);
}

void GameplayMenu::handleMouseUp(int x, int y, int button)
{
    if (button == 3) {
        for (int i = 0; i < 4; i++)
            if (isAgentSelected(i)
                    && mission_->ped(i)->selectedWeapon())
                mission_->ped(i)->stopFiring();
    }
}

extern int topz;

void GameplayMenu::handleUnknownKey(Key key, KeyMod mod, bool pressed)
{
    bool change = false; /* indicator whether menu should be redrawn */

    if (key == KEY_SPACE && mission_) {
        if (mission_->completed()) {
            map_menu_->setBlkColour(map_menu_->currentBlk(),
                    g_App.logoColour());
            menu_manager_->changeCurrentMenu("misswin");
            return;
        }
        else if (mission_->failed()) {
            menu_manager_->changeCurrentMenu("misslose");
            return;
        }
    }

    if (key == KEY_LCTRL) {
        ctrl_ = pressed;
    }
    else if (key == KEY_RCTRL) {
        ctrl_ = pressed;
    }

    if (key == KEY_LALT) {
        alt_ = pressed;
        printf("%i\n", pointing_at_ped_);

        if (pointing_at_ped_ != -1 && !alt_) {
            int ox, oy;
            int tx = g_App.maps().screenToTileX(mission_->map(),
                    last_motion_x_ - 129 + scroll_x_,
                    last_motion_y_ + scroll_y_, ox);
            int ty = g_App.maps().screenToTileY(mission_->map(),
                    last_motion_x_ - 129 + scroll_x_,
                    last_motion_y_ + scroll_y_, oy);
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
        selectAgent(0);
        change = true;
    }
    else if (key == KEY_2 || key == KEY_KP2) {
        selectAgent(1);
        change = true;
    }
    else if (key == KEY_3 || key == KEY_KP3) {
        selectAgent(2);
        change = true;
    }
    else if (key == KEY_4 || key == KEY_KP4) {
        selectAgent(3);
        change = true;
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

    if (key == KEY_p) {
        printf("%d\n", n);
    }
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

    if (key == KEY_LEFT)
        mission_->ped(0)->setDirection(mission_->ped(0)->direction() + 1);

    if (key == KEY_RIGHT)
        mission_->ped(0)->setDirection(mission_->ped(0)->direction() - 1);

    if (key == KEY_UP)
        mission_->ped(0)->setSpeed(16);

    if (key == KEY_DOWN)
        mission_->ped(0)->setSpeed(0);

    if (key == KEY_PAGEUP)
        mission_->ped(0)->setTileZ(mission_->ped(0)->tileZ() + 1);

    if (key == KEY_PAGEDOWN) {
        if (mission_->ped(0)->tileZ() > 0)
            mission_->ped(0)->setTileZ(mission_->ped(0)->tileZ() - 1);
    }
    //if (key == KEY_SPACE)
    //    printf("dir %i speed %i weapon %i\n", mission_->ped(0)->direction(), mission_->ped(0)->speed(), mission_->ped(0)->selectedWeapon()->index());

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

    if (key == KEY_s) {
        qanim++;
    }

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

    if (key == KEY_t)
        mission_->vehicle(0)->setDirection(
                mission_->vehicle(0)->direction() + 1);

    if (key == KEY_y)
        mission_->vehicle(0)->setDirection(
                mission_->vehicle(0)->direction() - 1);

    if (change)
        show(false);
}

void GameplayMenu::drawAgentSelectors()
{
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
    for (int a = 0; a < 4; a++)
        for (int j = 36 - 36 * mission_->ped(a)->health()
                / mission_->ped(a)->startHealth(); j < 36; j++)
            for (int i = 0; i < 7; i++)
                g_Screen.setPixel(((a % 2) == 1 ? 64 : 0) + 51 + i,
                        (a > 1 ? 46 + 44 + 10 : 0) + 6 + j, 12);
}

void GameplayMenu::drawPerformanceMeters()
{
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

void GameplayMenu::drawSelectAllButton()
{
    // 64x10
    g_App.gameSprites().sprite((selected_agents_ & 15) ==
        15 ? 1792 : 1796)->draw(0, 46 + 44, 0);
    g_App.gameSprites().sprite((selected_agents_ & 15) ==
        15 ? 1793 : 1797)->draw(64, 46 + 44, 0);
}

static int drawChar(int x, int y, char ch, bool inversed)
{
    if (ch == ' ')
        return 3;

    Sprite *s = g_App.menuSprites().sprite(665 + ch - 'A');
    uint8 *data = new uint8[s->width() * s->height()];
    s->data(data);

    for (int i = 0; i < s->width() * s->height(); i++)
        data[i] = (data[i] == 252 ? (inversed ? 0 : 11) : 255);

    g_Screen.blit(x, y, s->width(), s->height(), data);
    delete[] data;
    return s->width();
}

void GameplayMenu::drawMissionHint()
{
    static int mission_hint_ = 0;
    mission_hint_++;
    int speed = 100;

    if ((mission_hint_ % speed) != 0)
        return;

    bool inversed = false;

    if ((((mission_hint_ / speed) % 9) % 2) == 1)
        inversed = true;

    for (int j = 0; j < 14; j++)
        for (int i = 0; i < 128; i++)
            g_Screen.setPixel(i, 46 + 44 + 10 + 46 + 44 + j, 0);

    for (int j = inversed ? 1 : 0; j < 7; j++)
        for (int i = 0; i < 64; i++)
            g_Screen.setPixel(i * 2 + 1, 46 + 44 + 10 + 46 + 44 + j * 2, 8);

    if ((mission_hint_ / speed) % 10 == 0
            || (mission_hint_ / speed) % 10 == 9)
        return;

    if (inversed) {
        for (int j = 0; j < 11; j++)
            for (int i = 0; i < 128; i++)
                g_Screen.setPixel(i, 46 + 44 + 10 + 46 + 44 + j + 1, 11);
    }

    const char *str = "";

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
                g_App.gameSounds().sound(Sound::SPEECH_MISSION_FAILED)->play();
            }

            str = "MISSION FAILED";
        }

        if (mission_->completed()) {
            if (!completed_) {
                completed_ = true;
                g_App.gameSounds().sound(Sound::SPEECH_MISSION_COMPLETED)->
                        play();
            }

            str = "MISSION COMPLETE";
        }

        if ((mission_hint_ / speed) % 9 > 3
                && (mission_hint_ / speed) % 9 < 6) {
            if (mission_->completed() || mission_->failed())
                str = "PRESS SPACE";
            else
                for (int i = 0; i < 4; i++)
                    if (mission_->ped(i)->speed())
                        str = "GOING";
        }
    }

    int x = 0;
    const char *t = str;

    while (*t)
        x += g_App.menuSprites().sprite(665 + *t++ - 'A')->width() - 1;

    x = 64 - x / 2;

    while (*str)
        x += drawChar(x, 46 + 44 + 10 + 46 + 44 + 2, *str++, inversed) - 1;
}

void GameplayMenu::drawWeaponSelectors()
{
    // TODO: there should be a divider above these
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
                        32 * i, 46 + 44 + 10 + 46 + 44 + 15 + j * 32, 0);

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
                                        + k,
                                    12);
                }
            }
    }
}

int mcolors[] = {
    0, 0, 0, 0, 0, 8, 8, 8, 8, 8,       // 0
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 10
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 20
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 30
    8, 15, 8, 8, 8, 8, 8, 8, 8, 8,      // 40
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 50
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 60
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 70
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 80
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 90
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 100
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 110
    8, 8, 8, 8, 8, 7, 8, 8, 8, 8,       // 120
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 130
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 140
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 150
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 160
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 170
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 180
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 190
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 200
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 210
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 220
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 230
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 240
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,       // 250
};

void GameplayMenu::drawMiniMap()
{
    if (mission_ == 0)
        return;

    int sy = 46 + 44 + 10 + 46 + 44 + 15 + 2 * 32;

    int ox, oy;
    int tx = g_App.maps().map(mission_->map())->screenToTileX(
            scroll_x_ + (GAME_SCREEN_WIDTH - 129) / 2,
            scroll_y_ + GAME_SCREEN_HEIGHT / 2, ox);
    int ty = g_App.maps().map(mission_->map())->screenToTileY(
            scroll_x_ + (GAME_SCREEN_WIDTH - 129) / 2,
            scroll_y_ + GAME_SCREEN_HEIGHT / 2, oy);

    for (int i = 0; i < 4; i++)
        if (isAgentSelected(i)) {
            tx = mission_->ped(i)->tileX();
            ty = mission_->ped(i)->tileY();
        }

    tx -= 8;
    ty -= 8;
    mm_tx_ = tx;
    mm_ty_ = ty;

    for (int j = 0; j < 16; j++)
        for (int i = 0; i < 16; i++) {
            int t =
                g_App.maps().map(mission_->map())->tileAt(tx + i, ty + j, 1);

            if (t > 5) {
                uint8 ground[8 * 8];
                memset(ground, mcolors[t], sizeof(ground));
                g_Screen.blit(i * 8, j * 8 + sy, 8, 8, ground);
                // TODO: needs work
            }
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

void GameplayMenu::selectAgent(unsigned int agentNo)
{
    if (ctrl_)
        selected_agents_ ^= 1 << agentNo;
    else
        selected_agents_ = 1 << agentNo;
}

void GameplayMenu::selectAllAgents()
{
    if (ctrl_) {
        selected_agents_ ^= 1;
        selected_agents_ ^= 2;
        selected_agents_ ^= 4;
        selected_agents_ ^= 8;
    }
    else {
        if ((selected_agents_ & 15) == 15)
            selected_agents_ = selected_agents_ >> 4;
        else
            selected_agents_ = (selected_agents_ << 4) | 15;
    }
}
