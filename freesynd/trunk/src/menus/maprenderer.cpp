/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2010  Bohdan Stelmakh <chamel@users.sourceforge.net> *
 *   Copyright (C) 2013  Benoit Blancard <benblan@users.sourceforge.net>*
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

#include "menus/maprenderer.h"
#include "common.h"
#include "mission.h"
#include "vehicle.h"
#include "agentmanager.h"
#include "mapobject.h"
#include "core/squad.h"
#include "gfx/screen.h"
#include "gfx/tile.h"
#include "system.h"
#include "menus/squadselection.h"

void MapRenderer::init(Mission *pMission) {
    pMission_ = pMission;
    pMap_ = pMission->get_map();
}

/**
 * Draw tiles and map objects.
 */
void MapRenderer::render(int worldX, int worldY, SquadSelection *pSelection) {
    // TODO: after a lot of attempts to fix this, map drawing remains buggy
    MapTilePoint mtp = pMap_->screenToTilePoint(worldX, worldY);
    int sw = mtp.tx;
    int chk = g_Screen.gameScreenWidth() / (TILE_WIDTH / 2) + 2
        + g_Screen.gameScreenHeight() / (TILE_HEIGHT / 3) + pMap_->maxZ() * 2;
    int swm = sw + chk;
    int sh = mtp.ty - 8;

    int shm = sh + chk;

#ifdef EXECUTION_SPEED_TIME
    printf("---------------------------");
    int measure_ticks = SDL_GetTicks();
    printf("start time %i.%i\n", measure_ticks/1000, measure_ticks%1000);
#endif

    createFastKeys(sw, sh, swm, shm);
    int cmw = worldX + g_Screen.gameScreenWidth() -
                g_Screen.gameScreenLeftMargin() + 128;
    int cmh = worldY + g_Screen.gameScreenHeight() + 128;
    int cmx = worldX - g_Screen.gameScreenLeftMargin();
     //  z = 0 - is minimap data and mapdata
    int chky = sh < 0 ? 0 : sh;
    int zr = shm + pMap_->maxZ() + 1;
    for (int inc = 0; inc < zr; ++inc) {
        int ye = sh + inc;
        int ys = ye - pMap_->maxZ() - 2;
        int tile_z = pMap_->maxZ() + 1;  // the Z coord of the next tile to draw
        for (int yb = ys; yb < ye; ++yb) {
            if (yb < 0 || yb < sh || yb >= shm) {
                --tile_z;
                continue;
            }
            int tile_y = yb;  // The Y coord of the tile to draw
            for (int tile_x = sw; tile_y >= chky && tile_x < pMap_->maxX(); ++tile_x) {
                if (tile_x < 0 || tile_y >= pMap_->maxY()) {
                    --tile_y;
                    continue;
                }
                int screen_w = (pMap_->maxX() + (tile_x - tile_y)) * (TILE_WIDTH / 2);
                // int screen_h = (max_z_ + w + h) * (TILE_HEIGHT / 3);
                int coord_h = ((pMap_->maxZ() + tile_x + tile_y) - (tile_z - 1)) * (TILE_HEIGHT / 3);
                if (screen_w >= worldX - TILE_WIDTH * 2
                    && screen_w + TILE_WIDTH * 2 < cmw
                    && coord_h >= worldY - TILE_HEIGHT * 2
                    && coord_h + TILE_HEIGHT * 2 < cmh) {
#if 0
                    if (z > 2)
                        continue;
#endif
                    // draw a tile
                    if (tile_z < pMap_->maxZ()) {
                        Tile *p_tile = pMap_->getTileAt(tile_x, tile_y, tile_z);
                        if (p_tile->notTransparent()) {
                            int dx = 0, dy = 0;
                            if (screen_w - worldX < 0)
                                dx = -(screen_w - worldX);
                            if (coord_h - worldY < 0)
                                dy = -(coord_h - worldY);
                            if (dx < TILE_WIDTH && dy < TILE_HEIGHT) {
                                p_tile->drawToScreen(screen_w - cmx, coord_h - worldY);
                            }
                        }
                    }

                    // draw everything that's on the tile
                    if (tile_z - 1 >= 0) {
                        drawAt(tile_x, tile_y, tile_z - 1,
                            screen_w - cmx + TILE_WIDTH / 2,
                            coord_h - worldY + TILE_HEIGHT / 3 * 2);
                    }
                }
                --tile_y;
            }
            --tile_z;
        }
    }

#ifdef _DEBUG
    if (g_System.getKeyModState() & KMD_LALT) {
        for (SquadSelection::Iterator it = pSelection->begin();
            it != pSelection->end(); ++it) {
            (*it)->showPath(worldX, worldY);
        }
    }
#endif

#ifdef EXECUTION_SPEED_TIME
    printf("+++++++++++++++++++++++++++");
    int measure_ticks_end = SDL_GetTicks();
    printf("end time %i.%i,%i\n", measure_ticks_end/1000, measure_ticks_end%1000,
        measure_ticks_end - measure_ticks);
#endif
}

int MapRenderer::fastKey(MapObject * m) {
    return fastKey(m->tileX(), m->tileY(), m->tileZ());
}

void MapRenderer::createFastKeys(int tilex, int tiley, int maxtilex, int maxtiley) {
    if (tilex < 0)
        tilex = 0;
    if (tiley < 0)
        tiley = 0;
    if (maxtilex >= pMap_->maxX())
        maxtilex = pMap_->maxX();
    if (maxtiley >= pMap_->maxY())
        maxtiley = pMap_->maxY();

    cache_vehicles_.clear();
    cache_peds_.clear();
    cache_weapons_.clear();
    cache_statics_.clear();
    cache_sfx_objects_.clear();

    fast_vehicle_cache_.clear();
    fast_ped_cache_.clear();
    fast_weapon_cache_.clear();
    fast_statics_cache_.clear();
    fast_sfx_objects_cache_.clear();

    // updating position for visual markers
    for (size_t i = 0; i < AgentManager::kMaxSlot; i++) {
        PedInstance *p = pMission_->getSquad()->member(i);
        if (p != NULL && p->isAlive()) {
            if (p->tileX() >= tilex && p->tileX() < maxtilex
                && p->tileY() >= tiley && p->tileY() < maxtiley) {
                // sfx_objects_[i]->setPosition(p->tileX(), p->tileY(), p->tileZ(),
                    // p->offX(), p->offY(), p->offZ() + 320);
                pMission_->sfxObjects(i + 4)->setPosition(p->tileX(), p->tileY(),
                    p->tileZ(), p->offX() - 16, p->offY(), p->offZ() + 256);
            }
        } else {
             // sfx_objects_[i]->setMap(-1);
             pMission_->sfxObjects(i + 4)->setMap(-1);
        }
    }

    // vehicles
    for (unsigned int i = 0; i < pMission_->numVehicles(); i++) {
        Vehicle *v = pMission_->vehicle(i);
        if (v->tileX() >= tilex && v->tileX() < maxtilex
            && v->tileY() >= tiley && v->tileY() < maxtiley) {
            // NOTE: a trick to make vehicles be drawn correctly z+1
            fast_vehicle_cache_.insert(fastKey(v->tileX(),
                v->tileY(), v->tileZ() + 1));
            cache_vehicles_.push_back(v);
        }
    }

    // peds
    for (size_t i = 0; i < AgentManager::kMaxSlot; i++) {
        PedInstance *p = pMission_->getSquad()->member(i);
        if (p != NULL && p->map() != -1) {
            if (p->tileX() >= tilex && p->tileX() < maxtilex
                && p->tileY() >= tiley && p->tileY() < maxtiley) {
                fast_ped_cache_.insert(fastKey(p));
                cache_peds_.push_back(p);
            }
        }
    }
    for (size_t i = pMission_->getSquad()->size(); i < pMission_->numPeds(); i++) {
        PedInstance *p = pMission_->ped(i);
        if (p->map() != -1) {
            if (p->tileX() >= tilex && p->tileX() < maxtilex
                && p->tileY() >= tiley && p->tileY() < maxtiley) {
                fast_ped_cache_.insert(fastKey(p));
                cache_peds_.push_back(p);
            }
        }
    }

    // weapons
    for (unsigned int i = 0; i < pMission_->numWeapons(); i++) {
        WeaponInstance *w = pMission_->weapon(i);
        if (w->map() != -1 && w->tileX() >= tilex && w->tileX() < maxtilex
            && w->tileY() >= tiley && w->tileY() < maxtiley) {
            fast_weapon_cache_.insert(fastKey(w));
            cache_weapons_.push_back(w);
        }
    }

    // statics
    for (unsigned int i = 0; i < pMission_->numStatics(); i++) {
        Static *s = pMission_->statics(i);
        if (s->tileX() >= tilex && s->tileX() < maxtilex
            && s->tileY() >= tiley && s->tileY() < maxtiley) {
            fast_statics_cache_.insert(fastKey(s));
            cache_statics_.push_back(s);
        }
    }

    // sfx objects
    for (unsigned int i = 0; i < pMission_->numSfxObjects(); i++) {
        SFXObject *so = pMission_->sfxObjects(i);
        if (so->map() != -1 && so->tileX() >= tilex && so->tileX() < maxtilex
            && so->tileY() >= tiley && so->tileY() < maxtiley) {
            fast_sfx_objects_cache_.insert(fastKey(so));
            cache_sfx_objects_.push_back(so);
        }
    }
}

void MapRenderer::drawAt(int tilex, int tiley, int tilez, int x, int y) {
    int key = fastKey(tilex, tiley, tilez);

    if (fast_vehicle_cache_.find(key) != fast_vehicle_cache_.end()) {
        // draw vehicles
        for (unsigned int i = 0; i < cache_vehicles_.size(); i++)
            if (cache_vehicles_[i]->tileX() == tilex
                && cache_vehicles_[i]->tileY() == tiley
                // NOTE: a trick to make vehicles be drawn correctly z+1
                && (cache_vehicles_[i]->tileZ() + 1) == tilez)
                cache_vehicles_[i]->draw(x, y);
    }

    if (fast_ped_cache_.find(key) != fast_ped_cache_.end()) {
        // draw peds
        for (unsigned int i = 0; i < cache_peds_.size(); i++)
            if (cache_peds_[i]->tileX() == tilex
                && cache_peds_[i]->tileY() == tiley
                && cache_peds_[i]->tileZ() == tilez) {
                cache_peds_[i]->draw(x, y);
#if 0
                g_Screen.drawLine(x - TILE_WIDTH / 2, y,
                                  x + TILE_WIDTH / 2, y, 11);
                g_Screen.drawLine(x + TILE_WIDTH / 2, y,
                                  x + TILE_WIDTH / 2, y + TILE_HEIGHT,
                                  11);
                g_Screen.drawLine(x + TILE_WIDTH / 2, y + TILE_HEIGHT,
                                  x - TILE_WIDTH / 2, y + TILE_HEIGHT,
                                  11);
                g_Screen.drawLine(x - TILE_WIDTH / 2, y + TILE_HEIGHT,
                                  x - TILE_WIDTH / 2, y, 11);
#endif
            }
    }

    if (fast_weapon_cache_.find(key) != fast_weapon_cache_.end()) {
        // draw weapons
        for (unsigned int i = 0; i < cache_weapons_.size(); i++)
            if (cache_weapons_[i]->map() != -1
                && cache_weapons_[i]->tileX() == tilex
                && cache_weapons_[i]->tileY() == tiley
                && cache_weapons_[i]->tileZ() == tilez) {
                cache_weapons_[i]->draw(x, y);
            }
    }

    if (fast_statics_cache_.find(key) != fast_statics_cache_.end()) {
        // draw statics
        for (unsigned int i = 0; i < cache_statics_.size(); i++)
            if (cache_statics_[i]->tileX() == tilex
                && cache_statics_[i]->tileY() == tiley
                && cache_statics_[i]->tileZ() == tilez) {
                cache_statics_[i]->draw(x, y);
            }
    }

    if (fast_sfx_objects_cache_.find(key) != fast_sfx_objects_cache_.end()) {
        // draw sfx objects
        for (unsigned int i = 0; i < cache_sfx_objects_.size(); i++)
            if (cache_sfx_objects_[i]->tileX() == tilex
                && cache_sfx_objects_[i]->tileY() == tiley
                && cache_sfx_objects_[i]->tileZ() == tilez) {
                cache_sfx_objects_[i]->draw(x, y);
            }
    }
}
