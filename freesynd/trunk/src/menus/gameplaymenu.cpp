/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2007  Davor Ocelic <docelic@mail.inet.hr>            *
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
 *  project'sweb  site;  see <http://www.gnu.org/licenses/gpl.html>.    *
 *  The full text of the license is also included in the file COPYING.  *
 *                                                                      *
 ************************************************************************/

#include <stdio.h>
#include <assert.h>
#include "app.h"
#include "gameplaymenu.h"
#include "gfx/fliplayer.h"
#include "utils/file.h"

#ifdef _WIN32
#include <windows.h>
#endif

// The number of pixel of a scroll
const int SCROLL_STEP = 16;

const int GameplayMenu::kMiniMapScreenX = 0;
const int GameplayMenu::kMiniMapScreenY = 46 + 44 + 10 + 46 + 44 + 15 + 2 * 32 + 2;

GameplayMenu::GameplayMenu(MenuManager *m) :
Menu(m, MENU_GAMEPLAY, MENU_DEBRIEF, "", "mscrenup.dat"),
tick_count_(0), last_animate_tick_(0), last_motion_tick_(0),
last_motion_x_(320), last_motion_y_(240), mission_hint_ticks_(0), 
mission_hint_(0), mission_(NULL), world_x_(0),
world_y_(0), selection_(),
target_(NULL),
mm_renderer_()
{
    scroll_x_ = 0;
    scroll_y_ = 0;
    shooting_events_.clear();
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
            mission_->get_map()->screenToTileX(newWorldX, world_y_, ox);
        int ty =
            mission_->get_map()->screenToTileY(newWorldX, world_y_, oy);

        // Scroll to the right
        if (scroll_x_ > 0) {
            if (ty < mission_->minY()) {
                // we hit the upper right border of the map
                // so we scroll down until the far right corner
                int newWorldY = world_y_ + SCROLL_STEP;
                newWorldX += SCROLL_STEP;
                tx = mission_->get_map()->screenToTileX(newWorldX, newWorldY, ox);
                ty = mission_->get_map()->screenToTileY(newWorldX, newWorldY, oy);

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
                tx = mission_->get_map()->screenToTileX(newWorldX, newWorldY, ox);
                ty = mission_->get_map()->screenToTileY(newWorldX, newWorldY, oy);

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
                tx = mission_->get_map()->screenToTileX(newWorldX, newWorldY, ox);
                ty = mission_->get_map()->screenToTileY(newWorldX, newWorldY, oy);

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
                tx = mission_->get_map()->screenToTileX(newWorldX, newWorldY, ox);
                ty = mission_->get_map()->screenToTileY(newWorldX, newWorldY, oy);

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
            mission_->get_map()->screenToTileX(world_x_, newWorldY, ox);
        int ty =
            mission_->get_map()->screenToTileY(world_x_, newWorldY, oy);

        // Scroll down
        if (scroll_y_ > 0) {
            if (tx > mission_->maxX()) {
                // we hit the lower right border of the map
                // so we scroll down until the lower corner
                int newWorldX = world_x_ - 2*SCROLL_STEP;
                tx = mission_->get_map()->screenToTileX(newWorldX, newWorldY, ox);
                ty = mission_->get_map()->screenToTileY(newWorldX, newWorldY, oy);

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
                tx = mission_->get_map()->screenToTileX(newWorldX, newWorldY, ox);
                ty = mission_->get_map()->screenToTileY(newWorldX, newWorldY, oy);

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
                tx = mission_->get_map()->screenToTileX(newWorldX, newWorldY, ox);
                ty = mission_->get_map()->screenToTileY(newWorldX, newWorldY, oy);

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
                tx = mission_->get_map()->screenToTileX(newWorldX, newWorldY, ox);
                ty = mission_->get_map()->screenToTileY(newWorldX, newWorldY, oy);

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

void GameplayMenu::improveScroll(int &newScrollX, int &newScrollY)
{
    int ox, oy;
    int tx =
        mission_->get_map()->screenToTileX(newScrollX, newScrollY, ox);
    int ty =
        mission_->get_map()->screenToTileY(newScrollX, newScrollY, oy);

    if (tx < mission_->minX())
        tx = mission_->minX();

    if (ty < mission_->minY())
        ty = mission_->minY();

    if (tx > mission_->maxX())
        tx = mission_->maxX();

    if (ty > mission_->maxY())
        ty = mission_->maxY();

    // recalculating new screen coords
    newScrollX = mission_->get_map()->tileToScreenX(tx, ty,
        mission_->mmax_z_ + 1, 0, 0);
    newScrollY = mission_->get_map()->tileToScreenY(tx, ty,
        mission_->mmax_z_ + 1, 0, 0);
}

/*!
 * Called before the menu is first shown.
 * Place to do some initialising.
 */
void GameplayMenu::handleShow() {
    mission_ = g_Session.getMission();
    mission_->start();

    // init menu internal state
    pressed_btn_select_all_ = false;
    world_x_ = mission_->startX();
    world_y_ = mission_->startY();
    
    // set graphic palette
    menu_manager_->setPaletteForMission(g_Session.getSelectedBlock().mis_id);
    g_Screen.clear(0);
    
    // place the screen at the good starting position
    improveScroll(world_x_, world_y_);

    // init selection to the first selectable agent
    selection_.setSquad(mission_->getSquad());
    updtAgentsMarker();

    // Reset the minimap
    mm_renderer_.init(mission_, mission_->getSquad()->hasScanner());
    centerMinimapOnLeader();

    // Change cursor to game cursor
    g_System.usePointerCursor();
    g_System.showCursor();

    // play game track
    g_App.music().playTrack(msc::TRACK_ASSASSINATE);
}

int qanim = 200, qframe = 0;

void GameplayMenu::handleTick(int elapsed)
{
    if (paused_)
        return;
    bool change = false;
    tick_count_ += elapsed;

    if (!mission_->completed() && !mission_->failed()) {
        // Update stats
        mission_->getStatistics()->mission_duration += elapsed;

        // Checks mission objectives
        mission_->checkObjectives();
    }

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

    updateSelectionForDeadAgents();

    updateMinimap(elapsed);

    updateIPALevelMeters(elapsed);

    if (change) {
        needRendering();
        // force target to update
        handleMouseMotion(last_motion_x_, last_motion_y_, 0, KMD_NONE);
    }

    drawMissionHint(elapsed);
}

void GameplayMenu::handleRender(DirtyList &dirtyList)
{
    g_Screen.clear(0);
    mission_->drawMap(world_x_, world_y_);
    g_Screen.drawRect(0,0, 129, GAME_SCREEN_HEIGHT);
    agt_sel_renderer_.render(selection_, mission_->getSquad());
    drawSelectAllButton();
    drawMissionHint(0);
    drawWeaponSelectors();
    mm_renderer_.render(kMiniMapScreenX, kMiniMapScreenY);

#ifdef _DEBUG
    if (g_System.getKeyModState() & KMD_LALT) {
        for (SquadSelection::Iterator it = selection_.begin();
            it != selection_.end(); ++it) {
            (*it)->showPath(world_x_, world_y_);
        }
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
    selection_.clear();
    mission_->removeListeners();

    tick_count_ = 0;
    last_animate_tick_ = 0;
    last_motion_tick_ = 0;
    last_motion_x_ = 320;
    last_motion_y_ = 240;
    mission_hint_ticks_ = 0;
    mission_hint_ = 0;
    world_x_ = 0;
    world_y_ = 0;
    target_ = NULL;
    mission_ = NULL;
    scroll_x_ = 0;
    scroll_y_ = 0;
    shooting_events_.clear();
    paused_ = false;
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

    bool inrange = false;
    target_ = NULL;

    if (x > 128) {
#ifdef _DEBUG
        for (int i = 0; mission_ && i < mission_->numPeds(); i++) {
#else
        for (int i = 8; mission_ && i < mission_->numPeds(); i++) {
#endif
            PedInstance *p = mission_->ped(i);
            if (p->isAlive() && p->map() != -1) {
                int px = p->screenX() - 10;
                int py = p->screenY() - (1 + p->tileZ()) * TILE_HEIGHT/3
                    - (p->offZ() * TILE_HEIGHT/3) / 128;

                if (x - 129 + world_x_ >= px && y + world_y_ >= py &&
                    x - 129 + world_x_ < px + 21 && y + world_y_ < py + 34)
                {
                    for (SquadSelection::Iterator it = selection_.begin();
                         it != selection_.end(); ++it)
                    {
                        WeaponInstance * wi = (*it)->selectedWeapon();
                        target_ = p;
                        if (wi && wi->canShoot()
                            && wi->inRangeNoCP(&target_) == 1)
                        {
                            inrange = true;
                        }
                    }
                    break;
                }
            }
        }

        for (int i = 0; mission_ && i < mission_->numVehicles(); i++) {
            VehicleInstance *v = mission_->vehicle(i);
            if (v->isAlive()) {
                int px = v->screenX() - 20;
                int py = v->screenY() - 10 - v->tileZ() * TILE_HEIGHT/3;

                if (x - 129 + world_x_ >= px && y + world_y_ >= py &&
                    x - 129 + world_x_ < px + 40 && y + world_y_ < py + 32)
                {
                    for (SquadSelection::Iterator it = selection_.begin();
                         it != selection_.end(); ++it)
                    {
                        WeaponInstance * wi = (*it)->selectedWeapon();
                        target_ = v;
                        if (wi && wi->canShoot()
                            && wi->inRangeNoCP(&target_)== 1)
                        {
                            inrange = true;
                        }
                    }
                    break;
                }
            }
        }

        for (int i = 0; mission_ && i < mission_->numWeapons(); i++) {
            WeaponInstance *w = mission_->weapon(i);

            if (w->map() != -1) {
                int px = w->screenX() - 10;
                int py = w->screenY() + 4 - w->tileZ() * TILE_HEIGHT/3
                    - (w->offZ() * TILE_HEIGHT/3) / 128;

                if (x - 129 + world_x_ >= px && y + world_y_ >= py &&
                    x - 129 + world_x_ < px + 20 && y + world_y_ < py + 15)
                {
                    target_ = w;
                    break;
                }
            }
        }
#if 0
#ifdef _DEBUG
        for (int i = 0; mission_ && i < mission_->numStatics(); i++) {
            Static *s = mission_->statics(i);

            if (s->map() != -1) {
                int px = s->screenX() - 10;
                int py = s->screenY() + 4 - s->tileZ() * TILE_HEIGHT/3
                    - (s->offZ() * TILE_HEIGHT/3) / 128;

                if (x - 129 + world_x_ >= px && y + world_y_ >= py &&
                    x - 129 + world_x_ < px + 20 && y + world_y_ < py + 15)
                {
                    target_ = s;
                    break;
                }
            }
        }
#endif
#endif
    }

    if (target_) {
#if 0
#ifdef _DEBUG
        if (modKeys & KMD_ALT) {
            printf("target id  : %i == majorType : %i\n",
                target_->getDebugID(), target_->majorType());
        }
#endif
#endif
        if (target_->useTargetCursor() ) {
            if (inrange)
                g_System.useTargetRedCursor();
            else
                g_System.useTargetCursor();
        } else if (target_->majorType() == MapObject::mjt_Weapon)
            g_System.usePickupCursor();
    } else {
        if (x > 128)
            g_System.usePointerCursor();
        else
            g_System.usePointerYellowCursor();
    }

    if (x < 129)
        stopShootingEvent();
    if (shooting_events_.shooting_) {
        int ox, oy;
        int tx =
            mission_->get_map()->screenToTileX(world_x_ + x - 129,
                world_y_ + y, ox);
        int ty =
            mission_->get_map()->screenToTileY(world_x_ + x - 129,
                world_y_ + y, oy);
        for (int i = 0; i < 4; i++) {
            if (shooting_events_.agents_shooting[i]) {
                PedInstance * pa = mission_->ped(i);
                PathNode *pn = NULL;
                if (target_) {
                    int tilez = target_->tileZ() * 128 + target_->offZ()
                        + (target_->sizeZ() >> 1);
                    int offz = tilez % 128;
                    tilez /= 128;
                    pn = new PathNode(target_->tileX(), target_->tileY(), tilez,
                        target_->offX(), target_->offY(), offz);
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
                        pn = new PathNode(stx, sty, stz, sox, soy, oz);
#if 0
                        printf("shooting at\n x = %i, y=%i, z=%i\n",
                            stx, sty, stz);
                        printf("shooting pos\n ox = %i, oy=%i, oz=%i\n",
                            sox, soy, oz);
#endif
                    }
                }
                if (pn) {
                    pa->updtActGFiring(shooting_events_.ids[i], pn,
                        NULL);
                    delete pn;
                }
            }
        }
    }

}

bool GameplayMenu::handleMouseDown(int x, int y, int button, const int modKeys)
{
    if (paused_)
        return true;
    if (button == 3) {
        stopShootingEvent();
    }

    if (x < 129) {
        bool ctrl = false;  // Is control button pressed
        if (modKeys & KMD_CTRL) {
            ctrl = true;
        }

        // First check if player has clicked on agent selectors
        SelectorEvent selEvt;
        if (agt_sel_renderer_.hasClickedOnAgentSelector(x, y, selEvt)) {
            switch (selEvt.eventType) {
            case SelectorEvent::kSelectAgent:
                // Handle agent selection. Click on an agent changes selection
                // to it. If control key is pressed, add or removes agent from
                // current selection.
                selectAgent(selEvt.agentSlot, ctrl);
                break;
            case SelectorEvent::kSelectIpa:
                handleClickOnIPA(selEvt, button, modKeys);
                break;
            case SelectorEvent::kNone:
                break;
            }
        } else if (y >= 42 + 48 && y < 42 + 48 + 10) {
            // User clicked on the select all button
            selectAllAgents();
        }
        else if (y >= 2 + 46 + 44 + 10 + 46 + 44 + 15
                 && y < 2 + 46 + 44 + 10 + 46 + 44 + 15 + 64)
        {
            // user clicked on the weapon selector
            handleClickOnWeaponSelector(x, y, button, modKeys);
        } else if ( y > kMiniMapScreenY && button == 1) {
            handleClickOnMinimap(x, y);
        }
    } else {
        // User clicked on the map
        handleClickOnMap(x, y, button, modKeys);
    }

    return true;
}

/*!
 * The user has clicked on the weapon selector.
 * \param x Mouse X coord
 * \param y Mouse Y coord
 * \param button Mouse button that was clicked
 * \param modKeys System keys states
 */
void GameplayMenu::handleClickOnWeaponSelector(int x, int y, int button,
    const int modKeys)
{
    int w_num = ((y - (2 + 46 + 44 + 10 + 46 + 44 + 15)) / 32) * 4
            + x / 32;
    bool weapon_is_selected = false;
    PedInstance *pLeader = mission_->ped(selection_.getLeaderSlot());
    if (pLeader->isAlive()) {
        if (w_num < pLeader->numWeapons()) {
            if (button == 1) {
                WeaponInstance *wi = pLeader->weapon(w_num);
                bool deselect_curr = false;

                if (pLeader->selectedWeapon() == wi)
                    deselect_curr = true;

                for (SquadSelection::Iterator it = selection_.begin();
                     it != selection_.end(); ++it)
                {
                    PedInstance *ped = *it;
                    if (deselect_curr)
                        ped->setSelectedWeapon(-1);
                    else {
                        if (pLeader == ped) {
                            pLeader->setSelectedWeapon(w_num);
                        } else {
                            WeaponInstance *pWI = ped->selectedWeapon();
                            if ((pWI && pWI->getWeaponType() != wi->getWeaponType())
                                 || !pWI)
                            {
                                PedInstance::pedWeaponToUse pw_to_use;
                                pw_to_use.desc = 3;
                                pw_to_use.wpn.wpn_type = wi->getWeaponType();
                                ped->selectRequiredWeapon(&pw_to_use);
                            }
                        }
                    }
                    weapon_is_selected = true;
                } // end for
            } else {
                PedInstance::actionQueueGroupType as;
                as.main_act = 0;
                as.group_desc = PedInstance::gd_mExclusive;
                as.origin_desc = 4;
                pLeader->createActQPutDown(as, pLeader->weapon(w_num));
                if (modKeys & KMD_CTRL)
                    pLeader->addActQToQueue(as);
                else
                    pLeader->setActQInQueue(as);
            }
        }
    }
    // redraw weapon selector
    if (weapon_is_selected)
        g_App.gameSounds().play(snd::SPEECH_SELECTED);
    addDirtyRect(0, 207, 128, 64);
}

void GameplayMenu::handleClickOnIPA(const SelectorEvent & evt, int button, const int modKeys)
{
    PedInstance *ped = mission_->ped(evt.agentSlot);
    switch(evt.IpaType)
    {
        case IPAStim::Adrenaline:
            ped->adrenaline_->setAmount(evt.percentage);
            break;
        case IPAStim::Perception:
            ped->perception_->setAmount(evt.percentage);
            break;
        case IPAStim::Intelligence:
            ped->intelligence_->setAmount(evt.percentage);
            break;
    }
}

void GameplayMenu::updateIPALevelMeters(int elapsed)
{
    for (int agent = 0; agent < 4; agent++) {
        PedInstance *ped = mission_->ped(agent);
        ped->updtIPATime(elapsed);
    }
}

void GameplayMenu::handleClickOnMap(int x, int y, int button, const int modKeys) {
    MapTilePoint mapPt_base = mission_->get_map()->screenToTilePoint(world_x_ + x - 129,
                    world_y_ + y);

    if (button == 1) {
        for (int i = 0; i < 4; i++) {
            PedInstance *ped = mission_->ped(i);
            MapTilePoint mapPt(mapPt_base);
            if (selection_.isAgentSelected(i)) {
                PedInstance::actionQueueGroupType as;
                bool action = false;
                if (target_) {
                    switch (target_->majorType()) {
                        case MapObject::mjt_Ped:
                            // TODO : Should we also follow a vehicle if it is
                            // being driven / has a driver?
                            ped->createActQFollowing(as, target_, 0, 192);
                            action = true;
                            break;
                         case MapObject::mjt_Weapon:
                            ped->createActQPickUp(as, target_);
                            action = true;
                            break;
                         case MapObject::mjt_Vehicle:
                            if (ped->inVehicle() == target_) {
                                ped->createActQLeaveCar(as, target_);
                                action = true;
                            } else if (target_->isAlive())
                            {
                                ped->createActQGetInCar(as, target_);
                                action = true;
                            }
                            break;
                         default:
                             break;
                    }
                } else if (ped->inVehicle()) {
                    if (ped == ped->inVehicle()->getDriver())
                    {
                        int stx = mapPt.tx;
                        int sty = mapPt.ty;
                        //int sox = ox;
                        //int soy = oy;
                        stx = mapPt.tx * 256 + mapPt.ox + 128 * (ped->inVehicle()->tileZ() - 1);
                        //sox = stx % 256;
                        stx = stx / 256;
                        sty = mapPt.ty * 256 + mapPt.oy + 128 * (ped->inVehicle()->tileZ() - 1);
                        //soy = sty % 256;
                        sty = sty / 256;
                        PathNode tpn = PathNode(stx, sty, 0, 128, 128);
                        ped->createActQUsingCar(as, &tpn, ped->inVehicle());
                        action = true;
                    }
                }
                
                if (action) {
                    as.main_act = as.actions.size() - 1;
                    as.group_desc = PedInstance::gd_mStandWalk;
                    as.origin_desc = 4;
                    if (modKeys & KMD_CTRL)
                        ped->addActQToQueue(as);
                    else
                        ped->setActQInQueue(as);
                } else {
                    bool isForGroup = selection_.size() > 1;
                    bool addDestPt = (modKeys & KMD_CTRL) != 0;
                    if (!mission_->getWalkable(mapPt))
                        break;
                    if (!setDestinationPoint(mapPt, isForGroup, addDestPt, i, ped)) {
                        // could not set destination point -> we can stop
                        // TODO: if one agent can't move there, all should stop?
                        break;
                    }
                }
            } // end of if selected
        } // end of for
    } else if (button == 3) {
        // TODO: use directly mapPt_base?
        MapTilePoint mapPt(mapPt_base);
        PathNode *pn = NULL;
        if (target_) {
            int tilez = target_->tileZ() * 128 + target_->offZ() + (target_->sizeZ() >> 1);
            int offz = tilez % 128;
            tilez /= 128;
            pn = new PathNode(target_->tileX(), target_->tileY(), tilez,
                target_->offX(), target_->offY(), offz);
        } else {
            int stx = mapPt.tx;
            int sty = mapPt.ty;
            int stz = 0;
            int sox = mapPt.ox;
            int soy = mapPt.oy;
            int oz = 0;
            if (mission_->getShootableTile(stx, sty, stz,
                sox, soy, oz))
            {
                pn = new PathNode(stx, sty, stz, sox, soy, oz);
#if 0
                printf("shooting at\n x = %i, y=%i, z=%i\n",
                        stx, sty, stz);
                printf("shooting pos\n ox = %i, oy=%i, oz=%i\n",
                        sox, soy, oz);
#endif
            }
        }
        for (int i = 0; i < 4; i++) {
            if (selection_.isAgentSelected(i)) {
                PedInstance * pa = mission_->ped(i);
                PedInstance::actionQueueGroupType as;
                as.main_act = 0;
                as.group_desc = PedInstance::gd_mFire;
                as.origin_desc = 4;
                if (pn) {
                    if (modKeys & KMD_CTRL) {
                        if (pa->createActQFiring(as, pn, NULL, true))
                            pa->addActQToQueue(as);
                    } else {
                        if (pa->createActQFiring(as, pn, NULL, true))
                        {
                            shooting_events_.agents_shooting[i] = true;
                            shooting_events_.shooting_ = true;
                            pa->setActQInQueue(as, &shooting_events_.ids[i]);
                        }
                    }
                }
            }
        }
        if (pn)
            delete pn;
    }
}

void GameplayMenu::handleClickOnMinimap(int x, int y) {
    MapTilePoint pt_base = mm_renderer_.minimapToMapPoint(x - kMiniMapScreenX, y - kMiniMapScreenY);
    for (size_t i = 0; i < 4; i++) {
        MapTilePoint pt(pt_base);
        if (selection_.isAgentSelected(i)) {
            PedInstance *ped = mission_->ped(i);
            bool isForGroup = selection_.size() > 1;
            pt.tz = ped->tileZ();
            if (!mission_->getWalkableClosestByZ(pt))
                break;
            if (!setDestinationPoint(pt, isForGroup, false, i, ped)) {
                break;
            }
        }
    }
}

bool GameplayMenu::setDestinationPoint(const MapTilePoint &mapPt, bool isForGroup, bool addDestPt, size_t agentNo, PedInstance *p_ped) {
    
    MapTilePoint tmpPt(mapPt);

    if (isForGroup) {
        //TODO: current group position is like
        // in original this can make non-tile
        // oriented
        //int sox = (i % 2) * (i - 2) * 16;
        //int soy = ((i + 1) % 2) * (i - 1) * 8;

        //this should be romoved if non-tile
        //position needed
        tmpPt.ox = 63 + 128 * (agentNo % 2);
        tmpPt.oy = 63 + 128 * (agentNo >> 1);
    }

    PedInstance::actionQueueGroupType as;
    PathNode tpn = PathNode(tmpPt.tx, tmpPt.ty, tmpPt.tz, tmpPt.ox, tmpPt.oy, 0);
    p_ped->createActQWalking(as, &tpn, NULL);
    as.main_act = as.actions.size() - 1;
    as.group_desc = PedInstance::gd_mStandWalk;
    as.origin_desc = 4;
    if (addDestPt)
        p_ped->addActQToQueue(as);
    else
        p_ped->setActQInQueue(as);

    return true;
}

void GameplayMenu::stopShootingEvent(void )
{
    if (shooting_events_.shooting_) {
        shooting_events_.shooting_ = false;
        for (int i = 0; i < 4; i++) {
            if (shooting_events_.agents_shooting[i]) {
                shooting_events_.agents_shooting[i] = false;
                mission_->ped(i)->updtActGFiringShots(shooting_events_.ids[i], 1);
            }
        }
    }
}


void GameplayMenu::handleMouseUp(int x, int y, int button, const int modKeys) {
    // TODO: all mouse continued actions should stop, when paused
    //if (paused_)
        //return;
    if (button == 3) {
        stopShootingEvent();
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

    // Pause/unpause game
    if (isLetterP(key.unicode)) {
        if (paused_) {
            paused_ = false;
        } else {
            paused_ = true;
            // TODO: translate all paused texts
            std::string str_paused = g_App.menus().getMessage("GAME_PAUSED");
            MenuFont *font_used = getMenuFont(FontManager::SIZE_1);
            int txt_width = font_used->textWidth(str_paused.c_str(), false);
            int txt_posx = g_Screen.gameScreenWidth() / 2 - txt_width / 2;
            int txt_height = font_used->textHeight(false);
            int txt_posy = g_Screen.gameScreenHeight() / 2 - txt_height / 2;
            
            g_Screen.drawRect(txt_posx - 10, txt_posy - 5,
                txt_width + 20, txt_height + 10);
            gameFont()->drawText(txt_posx, txt_posy, str_paused.c_str(), 9);
        }
        return true;
    }

    if (paused_)
        return true;

    bool ctrl = false;
    if (modKeys & KMD_CTRL) {
        ctrl = true;
    }

#ifdef _DEBUG
    if (isLetterH(key.unicode)) {
        mission_->endWithStatus(Mission::COMPLETED);
        return true;
    }

    if (isLetterG(key.unicode)) {
        mission_->endWithStatus(Mission::FAILED);
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
                // Display success animation
                menu_manager_->gotoMenu(Menu::MENU_FLI_SUCCESS);
            }
            else if (mission_->failed()) {
                
                menu_manager_->gotoMenu(Menu::MENU_FLI_FAILED_MISSION);
            }

            return true;
        }
    } else if (key.keyFunc == KFC_ESCAPE) {
        // Abort mission
        mission_->endWithStatus(Mission::ABORTED);
        // Return false so when can still go to parent menu with escape
        return false;
    }

    /* Handle agent selection by numeric keys. Key 0 cycles between one agent
     * selection and all 4 agents.
     * Individual keys select the specified agent unless ctrl is pressed -
     * then they add/remove agent from current selection. */
    if (key.keyVirt == KVT_NUMPAD0) {
        /* This code is exactly the same as for clicking on "group-button"
         * as you can see above. */
        selectAllAgents();
    }
    else if (key.keyVirt == KVT_NUMPAD1) {
        selectAgent(0, ctrl);
    }
    else if (key.keyVirt == KVT_NUMPAD2) {
        selectAgent(1, ctrl);
    }
    else if (key.keyVirt == KVT_NUMPAD3) {
        selectAgent(2, ctrl);
    }
    else if (key.keyVirt == KVT_NUMPAD4) {
        selectAgent(3, ctrl);
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
    } else if (isLetterD(key.unicode)) { // selected agents are killed with 'd'
        if (ctrl) {
            for (int i = 0; i < 4; i++) {
                PedInstance *pAgent = mission_->getSquad()->member(i);
                if (pAgent && selection_.isAgentSelected(i)) {
                    pAgent->kill();
                }
            }
        }
    } else {
        consumed = false;
    }

#ifdef _DEBUG
#if 0
    static int sound_num = 20;
    if (key.unicode == 'i') {
     g_App.gameSounds().play((snd::InGameSample)sound_num);
     printf("sn %i\n", sound_num);
     sound_num++;
     if (sound_num == 33)
         sound_num = 20;
    }
#endif
#if 1
    if (key.unicode == 'm') {
        for (SquadSelection::Iterator it = selection_.begin();
                            it != selection_.end(); ++it) {
            PedInstance *ped = *it;
            PedInstance::actionQueueGroupType as;
            ped->createActQWalking(as, NULL, NULL, 160, 1024);
            as.main_act = as.actions.size() - 1;
            as.group_desc = PedInstance::gd_mStandWalk;
            as.origin_desc = 4;
            if (modKeys & KMD_CTRL)
                ped->addActQToQueue(as);
            else
                ped->setActQInQueue(as);
        }
    }
#endif
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
    bool text_pw = (target_ && target_->majorType() == MapObject::mjt_Weapon
        && target_->map() != -1);

    std::string str;

    uint8 txtColor;

    if ((mission_hint_ > 20 && mission_hint_ < 41)
        || (mission_hint_ > 60))
    {
        for (SquadSelection::Iterator it = selection_.begin();
                            it != selection_.end(); ++it) {
            if ((*it)->isMoving()) {
                str = g_App.menus().getMessage("HINT_GOING");
            } else {
                str = g_App.menus().getMessage("HINT_OBSERVING");
            }
            if ((*it)->wePickupWeapon()) {
                str = g_App.menus().getMessage("HINT_PICKUP_WEAPON");
            }
            if ((*it)->drawnAnim() == PedInstance::ad_HitAnim) {
                str = g_App.menus().getMessage("HINT_HIT_BY_BULLET");
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
                str = g_App.menus().getMessage("HINT_MISSION_FAILED");
                text_pw = false;
            }

            if (mission_->completed()) {
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
                str = ((WeaponInstance *)target_)->name();
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

    p = mission_->ped(selection_.getLeaderSlot());

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
                } else if (draw_pw) {
                    if (target_ && target_->majorType() == MapObject::mjt_Weapon
                        && (mission_hint_ % 20) < 10
                        && target_->map() != -1)
                    {
                        wi = (WeaponInstance *)target_;
                        draw_pw = false;
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

/*!
 * Select the given agent.
 * \param agentNo Agent id
 * \param addToGroup If true, agent is added to the current selection.
 * If not, the selection is emptied and filled with the new agent.
 */
void GameplayMenu::selectAgent(size_t agentNo, bool addToGroup) {
    // TODO: when agent deselected and he was shooting, he should stop
    if (selection_.selectAgent(agentNo, addToGroup)) {
        updateSelectAll();
        centerMinimapOnLeader();
        updtAgentsMarker();
        g_App.gameSounds().play(snd::SPEECH_SELECTED);
        
        // redraw agent selectors
        addDirtyRect((agentNo % 2) * 65, (agentNo / 2) * 90 , 64, 46);
    }
}

/*!
 * Selects all agents.
 * \param invert If true, selects only the agents that were not already
 * selected.
 */
void GameplayMenu::selectAllAgents() {
    bool prv_state = pressed_btn_select_all_;
    pressed_btn_select_all_ = !pressed_btn_select_all_;
    selection_.selectAllAgents(pressed_btn_select_all_);
    updateSelectAll();
    if (pressed_btn_select_all_ != prv_state) {
        g_App.gameSounds().play(snd::SPEECH_SELECTED);
        // redraw all agent selectors
        addDirtyRect(0, 0, 128, 180);
    }
}

/*!
 * Make the current leader marker blinks.
 * All other agents not.
 */
void GameplayMenu::updtAgentsMarker()
{
    for (size_t i = AgentManager::kSlot1; i < AgentManager::kMaxSlot; i++) {
        // draw animation only for leader
        mission_->sfxObjects(4 + i)->setDrawAllFrames(selection_.getLeaderSlot() == i);
    }
}

/*!
 * This method checks among the squad to see if an agent died and deselects him.
 */
void GameplayMenu::updateSelectionForDeadAgents() {
    // TODO change the way this detection is made. It's not clean. use events
    bool b_agentDied = false;
    for (size_t i = AgentManager::kSlot1; i < AgentManager::kMaxSlot; i++) {
        if (mission_->ped(i) && mission_->ped(i)->isOurAgent()) {
            if (g_Session.agents().squadMember(i)->isAlive() && mission_->ped(i)->isDead()) {
                // TODO change this
                g_Session.agents().squadMember(i)->setHealth(0);
                selection_.deselectAgent(i);
                b_agentDied = true;
            }
        }
    }
    // At least one agent died
    if (b_agentDied) {
        // if selection is empty after agent's death
        // selects the first selectable agent
        if (selection_.size() == 0) {
            for (size_t i = AgentManager::kSlot1; i < AgentManager::kMaxSlot; i++) {
                if (selection_.selectAgent(i, false)) {
                    // Agent has been selected -> quit
                    break;
                }
            }
        }

        // anyway updates markers
        updtAgentsMarker();
        updateSelectAll();
    }
}

/*!
 * Updates the minimap.
 */
void GameplayMenu::centerMinimapOnLeader() {
    // Centers the minimap on the selection leader
    if (selection_.size() > 0) {
        PedInstance *pAgent = mission_->ped(selection_.getLeaderSlot());
        mm_renderer_.centerOn(pAgent->tileX(), pAgent->tileY(), pAgent->offX(), pAgent->offY());
    }
}

/*!
 * Updates the minimap.
 */
void GameplayMenu::updateMinimap(int elapsed) {
    centerMinimapOnLeader();
    mm_renderer_.handleTick(elapsed);
}

/*!
 * Update the select all button's state
 */
void GameplayMenu::updateSelectAll() {
    uint8 nbAgentAlive = 0;
    // count the number of remaining agents
    for (int indx = 0; indx < 4; indx++) {
        PedInstance *pAgent = mission_->ped(indx);
        if (pAgent->isAlive()) {
            nbAgentAlive++;
        }
    }

    // if number of agents alive is the same as number of selected agents
    // then button is pressed.
    pressed_btn_select_all_ = ((nbAgentAlive == selection_.size()
        && nbAgentAlive != 0) || nbAgentAlive == 1);
}
