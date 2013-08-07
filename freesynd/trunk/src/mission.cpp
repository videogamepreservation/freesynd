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

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <string>

#include "mission.h"
#include "gfx/screen.h"
#include "app.h"
#include "model/objectivedesc.h"
#include "utils/log.h"
#include "vehicle.h"
#include "core/squad.h"

Mission::Mission(const LevelData::MapInfos & map_infos)
{
    status_ = RUNNING;
    
    mtsurfaces_ = NULL;
    mdpoints_ = NULL;
    mdpoints_cp_ = NULL;
    i_map_id_ = READ_LE_UINT16(map_infos.map);
    p_map_ = NULL;
    min_x_= READ_LE_UINT16(map_infos.min_x) / 2;
    min_y_ = READ_LE_UINT16(map_infos.min_y) / 2;
    max_x_ = READ_LE_UINT16(map_infos.max_x) / 2;
    max_y_ = READ_LE_UINT16(map_infos.max_y) / 2;
    cur_objective_ = 0;
    p_minimap_ = NULL;
    players_group_id_ = 1;
    p_squad_ = new Squad();
}

Mission::~Mission()
{
    for (unsigned int i = 0; i < vehicles_.size(); i++)
        delete vehicles_[i];
    for (unsigned int i = 0; i < peds_.size(); i++)
        delete peds_[i];
    for (unsigned int i = 0; i < weapons_.size(); i++)
        delete weapons_[i];
    for (unsigned int i = 0; i < sfx_objects_.size(); i++)
        delete sfx_objects_[i];
    for (unsigned int i = 0; i < prj_shots_.size(); i++)
        delete prj_shots_[i];
    for (unsigned int i = 0; i < statics_.size(); i++)
        delete statics_[i];
    for (unsigned int i = 0; i < objectives_.size(); i++)
        delete objectives_[i];
    clrSurfaces();

    if (p_minimap_) {
        delete p_minimap_;
    }

    if (p_squad_) {
        delete p_squad_;
    }
}

/*!
 * Sets the given message with the current objective label.
 */
void Mission::objectiveMsg(std::string& msg) {
    if (cur_objective_ < objectives_.size()) {
        msg = objectives_[cur_objective_]->msg;
    } else {
        msg = "";
    }
}

/*!
 * Sets the given map for the mission.
 * Creates a minimap from it.
 * \param p_map The map to set.
 */
void Mission::set_map(Map *p_map) {
    if (p_map) {
        p_map_ = p_map;
        p_map_->mapDimensions(&mmax_x_, &mmax_y_, &mmax_z_);

        if (p_minimap_) {
            delete p_minimap_;
        }
        p_minimap_ = new MiniMap(p_map_);
    }
}

int Mission::mapWidth()
{
    return p_map_->width();
}

int Mission::mapHeight()
{
    return p_map_->height();
}

int fastKey(int tx, int ty, int tz)
{
    return tx | (ty << 8) | (tz << 16);
}

int fastKey(MapObject * m)
{
    return fastKey(m->tileX(), m->tileY(), m->tileZ());
}

void Mission::createFastKeys(int tilex, int tiley, int maxtilex, int maxtiley) {

    if (tilex < 0)
        tilex = 0;
    if (tiley < 0)
        tiley = 0;
    if (maxtilex >= mmax_x_)
        maxtilex = mmax_x_;
    if (maxtiley >= mmax_y_)
        maxtiley = mmax_y_;

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
        PedInstance *p = p_squad_->member(i);
        if (p != NULL && p->isAlive()) {
            if (p->tileX() >= tilex && p->tileX() < maxtilex
                && p->tileY() >= tiley && p->tileY() < maxtiley)
            {
                //sfx_objects_[i]->setPosition(p->tileX(), p->tileY(), p->tileZ(),
                    //p->offX(), p->offY(), p->offZ() + 320);
                sfx_objects_[i + 4]->setPosition(p->tileX(), p->tileY(),
                    p->tileZ(), p->offX() - 16, p->offY(), p->offZ() + 256);
            }
        } else {
            //sfx_objects_[i]->setMap(-1);
            sfx_objects_[i + 4]->setMap(-1);
        }
    }
        

    // vehicles
    for (unsigned int i = 0; i < vehicles_.size(); i++) {
        VehicleInstance *v = vehicles_[i];
        if (v->tileX() >= tilex && v->tileX() < maxtilex
            && v->tileY() >= tiley && v->tileY() < maxtiley)
        {
            // NOTE: a trick to make vehicles be drawn correctly z+1
            fast_vehicle_cache_.insert(fastKey(v->tileX(),
                v->tileY(), v->tileZ() + 1));
            cache_vehicles_.push_back(v);
        }
    }

    // peds
    for (size_t i = 0; i < AgentManager::kMaxSlot; i++) {
        PedInstance *p = p_squad_->member(i);
        if (p != NULL && p->map() != -1) {
            if (p->tileX() >= tilex && p->tileX() < maxtilex
                && p->tileY() >= tiley && p->tileY() < maxtiley)
            {
                fast_ped_cache_.insert(fastKey(p));
                cache_peds_.push_back(p);
            }
        }
    }
    for (size_t i = p_squad_->size(); i < peds_.size(); i++) {
        PedInstance *p = peds_[i];
        if (p->map() != -1) {
            if (p->tileX() >= tilex && p->tileX() < maxtilex
                && p->tileY() >= tiley && p->tileY() < maxtiley)
            {
                fast_ped_cache_.insert(fastKey(p));
                cache_peds_.push_back(p);
            }
        }
    }

    // weapons
    for (unsigned int i = 0; i < weapons_.size(); i++) {
        WeaponInstance *w = weapons_[i];
        if (w->map() != -1 && w->tileX() >= tilex && w->tileX() < maxtilex
            && w->tileY() >= tiley && w->tileY() < maxtiley)
        {
            fast_weapon_cache_.insert(fastKey(w));
            cache_weapons_.push_back(w);
        }
    }

    // statics
    for (unsigned int i = 0; i < statics_.size(); i++) {
        Static *s = statics_[i];
        if (s->tileX() >= tilex && s->tileX() < maxtilex
            && s->tileY() >= tiley && s->tileY() < maxtiley)
        {
            fast_statics_cache_.insert(fastKey(s));
            cache_statics_.push_back(s);
        }
    }

    // sfx objects
    for (unsigned int i = 0; i < sfx_objects_.size(); i++) {
        SFXObject *so = sfx_objects_[i];
        if (so->map() != -1 && so->tileX() >= tilex && so->tileX() < maxtilex
            && so->tileY() >= tiley && so->tileY() < maxtiley)
        {
            fast_sfx_objects_cache_.insert(fastKey(so));
            cache_sfx_objects_.push_back(so);
        }
    }
}

void Mission::drawMap(int scrollx, int scrolly)
{
    p_map_->draw(scrollx, scrolly, this);
}

void Mission::drawAt(int tilex, int tiley, int tilez, int x, int y)
{
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
            if (cache_weapons_[i]->map() != -1 && cache_weapons_[i]->tileX() == tilex
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

void Mission::start()
{
    LOG(Log::k_FLG_GAME, "Mission", "start()", ("Start mission"));
    // Reset mission statistics
    stats_.agents = p_squad_->size();
    stats_.mission_duration = 0;
    stats_.agentCaptured = 0;
    stats_.enemyKilled = 0;
    stats_.criminalKilled = 0;
    stats_.civilKilled = 0;
    stats_.policeKilled = 0;
    stats_.guardKilled = 0;
    stats_.convinced = 0;
    stats_.nbOfShots = 0;
    stats_.nbOfHits = 0;

    cur_objective_ = 0;

    // creating a list of available weapons
    // TODO: consider weight of weapons when adding?
    std::vector <Weapon *> wpns;
    g_gameCtrl.weapons().getAvailable(MapObject::dmg_Bullet, wpns);
    int indx_best = -1;
    int indx_second = -1;
    for (int i = 0, sz = wpns.size(), rank_best = -1, rank_second = -1;
        i < sz; ++i)
    {
        if (wpns[i]->rank() > rank_best) {
            rank_second = rank_best;
            indx_second = indx_best;
            rank_best = wpns[i]->rank();
            indx_best = i;
        } else if (wpns[i]->rank() > rank_second) {
            rank_second = wpns[i]->rank();
            indx_second = i;
        }
    }
    Weapon *bomb = g_gameCtrl.weapons().getAvailable(Weapon::TimeBomb);

    // TODO: check whether enemy agents weapons are equal to best two
    // if not set them
    for (uint16 i = p_squad_->size(), sz = peds_.size(); i < sz; ++i) {
        PedInstance *p = peds_[i];
        if (p->objGroupDef() == PedInstance::og_dmAgent &&
            p->numWeapons() == 0)
        {
            int index_give = indx_best;
            if (indx_second != -1 && (rand() & 0xFF) > 200)
                index_give = indx_second;
            WeaponInstance *wi = wpns[index_give]->createInstance();
            p->addWeapon(wi);
            wi->setOwner(p);
            wi->setIsIgnored(true);
            weapons_.push_back(wi);
            if (bomb) {
                WeaponInstance *w_bomb = bomb->createInstance();
                p->addWeapon(w_bomb);
                w_bomb->setOwner(p);
                w_bomb->setIsIgnored(true);
                weapons_.push_back(w_bomb);
            }
        }
    }

    for (uint16 i = 0, sz = peds_.size(); i < sz; ++i) {
        PedInstance *p = peds_[i];
        if (p->isAlive())
            p->createDefQueue();
    }
}

/*! 
 * Checks if objectives are completed or failed and updates
 * mission status.
 */
void Mission::checkObjectives() {
    // We only check the current objective
    if (cur_objective_ < objectives_.size()) {
        ObjectiveDesc * pObj = objectives_[cur_objective_];

        // If it's the first time the objective is checked,
        // declares it started
        if (pObj->status == kNotStarted) {
            LOG(Log::k_FLG_GAME, "Mission", "checkObjectives()", ("Start objective : %d", cur_objective_));
            // An objective has just started, warn all listeners
            pObj->start(this);
        }

        // Checks if the objective is completed
        pObj->evaluate(this);

        if (pObj->isTerminated()) {
            if (pObj->status == kFailed) {
                endWithStatus(FAILED);
            } else {
                // Objective is completed -> go to next one
                cur_objective_++;
                if (cur_objective_ >= objectives_.size()) {
                    // the last objective has been completed : mission succeeded
                    endWithStatus(COMPLETED);
                }
            }
        }
    }
}

/*!
 * Ends the mission with the given status.
 * \param status The ending status
 */
void Mission::endWithStatus(Status status) {
    switch (status) {
    case COMPLETED:
        status_ = COMPLETED;
        g_App.gameSounds().play(snd::SPEECH_MISSION_COMPLETED);
        break;
    case FAILED:
        status_ = FAILED;
        g_App.gameSounds().play(snd::SPEECH_MISSION_FAILED);
        break;
    case ABORTED:
        status_ = ABORTED;
        break;
    case RUNNING:
        break;
    }
}

void Mission::addWeaponsFromPedToAgent(PedInstance* p, Agent* pAg)
{
    while (p->numWeapons()) {
        WeaponInstance *wi = p->removeWeapon(0);
        std::vector < WeaponInstance * >::iterator it =
            weapons_.begin();
        while (it != weapons_.end() && *it != wi)
            it++;
        assert(it != weapons_.end());
        weapons_.erase(it);
        wi->deactivate();
        // auto-reload for pistol
        if (wi->getMainType() == Weapon::Pistol)
            wi->setAmmoRemaining(wi->ammo());
        wi->resetWeaponUsedTime();
        pAg->addWeapon(wi);
    }
}


void Mission::end()
{
    LOG(Log::k_FLG_GAME, "Mission", "end()", ("End mission"));
    for (unsigned int i = p_squad_->size(); i < peds_.size(); i++) {
        PedInstance *p = peds_[i];
        // TODO: influence country happiness with number of killed overall
        // civilians+police, more killed less happy
        // TODO: add money per every persuaded non-agent ped
        if (p->isDead()) {
            switch (p->getMainType()) {
                case PedInstance::m_tpAgent:
                    stats_.enemyKilled++;
                    break;
                case PedInstance::m_tpCriminal:
                    stats_.criminalKilled++;
                    break;
                case PedInstance::m_tpPedestrian:
                    stats_.civilKilled++;
                    break;
                case PedInstance::m_tpGuard:
                    stats_.guardKilled++;
                    break;
                case PedInstance::m_tpPolice:
                    stats_.policeKilled++;
                    break;
            }
        } else if (p->isPersuaded()) {
            if (p->objGroupDef() == PedInstance::og_dmAgent) {
                stats_.agentCaptured++;
                if (completed()) {
                    Agent *pAg = g_gameCtrl.agents().createAgent(false);
                    if (pAg) {
                        addWeaponsFromPedToAgent(p, pAg);
                        *((ModOwner *)pAg) = *((ModOwner *)p);
                    }
                }
            } else
                stats_.convinced++;
        }
    }

    // synch ped agents with agent from cryo chamber
    for (size_t i = AgentManager::kSlot1; i < AgentManager::kMaxSlot; i++) {
        PedInstance *p_pedAgent = p_squad_->member(i);
        if (p_pedAgent) {
            Agent *pAg = g_gameCtrl.agents().squadMember(i);
            if (p_pedAgent->isDead()) {
                // an agent died -> remove him from cryo
                g_gameCtrl.agents().destroyAgentSlot(i);
            } else {
                // synch only weapons
                addWeaponsFromPedToAgent(p_pedAgent, pAg);
            }
        }
    }

    // reset squad
    p_squad_->clear();
}

void Mission::addWeapon(WeaponInstance * w)
{
    //w->setMap(p_map_->id());
    for (unsigned int i = 0; i < weapons_.size(); i++)
        if (weapons_[i] == w)
            return;
    weapons_.push_back(w);
}

MapObject * Mission::findAt(int tilex, int tiley, int tilez,
                            MapObject::MajorTypeEnum *majorT, int *searchIndex,
                            bool only)
{
    switch(*majorT) {
        case MapObject::mjt_Ped:
            for (unsigned int i = *searchIndex; i < peds_.size(); i++)
                if ((!peds_[i]->isIgnored()) && peds_[i]->tileX() == tilex
                    && peds_[i]->tileY() == tiley
                    && peds_[i]->tileZ() == tilez)
                {
                    *searchIndex = i + 1;
                    *majorT = MapObject::mjt_Ped;
                    return peds_[i];
                }
            if(only)
                return NULL;
            *searchIndex = 0;
        case MapObject::mjt_Weapon:
            for (unsigned int i = *searchIndex; i < weapons_.size(); i++)
                if ((!weapons_[i]->isIgnored()) && weapons_[i]->tileX() == tilex
                    && weapons_[i]->tileY() == tiley
                    && weapons_[i]->tileZ() == tilez)
                {
                    *searchIndex = i + 1;
                    *majorT = MapObject::mjt_Weapon;
                    return weapons_[i];
                }
            if(only)
                return NULL;
            *searchIndex = 0;
        case MapObject::mjt_Static:
            for (unsigned int i = *searchIndex; i < statics_.size(); i++)
                if (statics_[i]->tileX() == tilex
                    && statics_[i]->tileY() == tiley
                    && statics_[i]->tileZ() == tilez)
                {
                    *searchIndex = i + 1;
                    *majorT = MapObject::mjt_Static;
                    return statics_[i];
                }
            if(only)
                return NULL;
            *searchIndex = 0;
        case MapObject::mjt_Vehicle:
            for (unsigned int i = *searchIndex; i < vehicles_.size(); i++)
                if (vehicles_[i]->tileX() == tilex
                    && vehicles_[i]->tileY() == tiley
                    && vehicles_[i]->tileZ() == tilez)
                {
                    *searchIndex = i + 1;
                    *majorT = MapObject::mjt_Vehicle;
                    return vehicles_[i];
                }
            break;
        default:
            printf("undefined majortype %i", *majorT);
            break;
    }
    return NULL;
}

// Surface walkable
bool Mission::sWalkable(char thisTile, char upperTile) {

    return (
            // checking surface
            (((thisTile >= 0x05 && thisTile <= 0x09) ||
            thisTile == 0x0B || (thisTile >= 0x0D && thisTile <= 0x0F)
            || (thisTile == 0x11 || thisTile == 0x12)))
            // or checking stairs
            || ((thisTile > 0x00 && thisTile < 0x05))
        ) && (upperTile == 0x00 || upperTile == 0x10);
}

bool Mission::isSurface(char thisTile) {
    return (thisTile >= 0x05 && thisTile <= 0x09) ||
        thisTile == 0x0B || (thisTile >= 0x0D && thisTile <= 0x0F)
        || (thisTile == 0x11 || thisTile == 0x12);
}

bool Mission::isStairs(char thisTile) {
    return thisTile > 0x00 && thisTile < 0x05;
}

bool Mission::setSurfaces() {

    // Description: creates map of walkable surfaces, also
    // defines directions where movement is possible

    // NOTE: tiles walkdata type 0x0D are quiet special, and they
    // are not handled correctly, these correction and andjustings
    // can create additional speed drain, as such I didn't
    // implemented them as needed. To make it possible a patch
    // required to walkdata and a lot of changes which I don't
    // want to do.
    // 0x10 appear above walking tile where train stops
    clrSurfaces();
    mmax_m_all = mmax_x_ * mmax_y_ * mmax_z_;
    mtsurfaces_ = (surfaceDesc *)malloc(mmax_m_all * sizeof(surfaceDesc));
    mdpoints_ = (floodPointDesc *)malloc(mmax_m_all * sizeof(floodPointDesc));
    mdpoints_cp_ = (floodPointDesc *)malloc(mmax_m_all * sizeof(floodPointDesc));
    if(mtsurfaces_ == NULL || mdpoints_ == NULL || mdpoints_cp_ == NULL) {
        clrSurfaces();
        printf("ERROR: memory allocation failed in Mission::setSurfaces");
        return false;
    }
    mmax_m_xy = mmax_x_ * mmax_y_;
    memset((void *)mtsurfaces_, 0, mmax_m_all * sizeof(surfaceDesc));
    memset((void *)mdpoints_, 0, mmax_m_all * sizeof(floodPointDesc));
    for (int ix = 0; ix < mmax_x_; ++ix) {
        for (int iy = 0; iy < mmax_y_; ++iy) {
            for (int iz = 0; iz < mmax_z_; ++iz) {
                mtsurfaces_[ix + iy * mmax_x_ + iz * mmax_m_xy].twd =
                    g_App.walkdata_p_[p_map_->tileAt(ix, iy, iz)];
            }
        }
    }

    // to make surfaces where large doors are located walkable
    for (std::vector<Static *>::iterator it = statics_.begin();
        it != statics_.end(); ++it)
    {
        Static *s = *it;
        if (s->getMainType() == Static::smt_LargeDoor) {
            int indx = s->tileX() + s->tileY() * mmax_x_
                + s->tileZ() * mmax_m_xy;
            mtsurfaces_[indx].twd = 0x00;
            if (s->getSubType() == 0) {
                if (indx - 1 >= 0)
                    mtsurfaces_[indx - 1].twd = 0x00;
                if (indx + 1 < mmax_m_all)
                    mtsurfaces_[indx + 1].twd = 0x00;
            } else if (s->getSubType() == 2) {
                if (indx - mmax_x_ >= 0)
                    mtsurfaces_[indx - mmax_x_].twd = 0x00;
                if (indx + mmax_x_ < mmax_m_all)
                    mtsurfaces_[indx + mmax_x_].twd = 0x00;
            }
        }
    }

    //printf("surface data size %i\n", sizeof(surfaceDesc) * mmax_m_all);
    //printf("flood data size %i\n", sizeof(floodPointDesc) * mmax_m_all);

    for (unsigned int i = 0; i < peds_.size(); ++i) {
        PedInstance *p = peds_[i];
        int x = p->tileX();
        int y = p->tileY();
        int z = p->tileZ();
        if (z >= mmax_z_ || z < 0 || p->isDead()) {
            // TODO : check on all maps those peds correct position
            p->setTileZ(mmax_z_ - 1);
            continue;
        }
        if (mdpoints_[x + y * mmax_x_ + z * mmax_m_xy].t == m_fdNotDefined) {
            toDefineXYZ stodef;
            std::vector<toDefineXYZ> vtodefine;
            mdpoints_[x + y * mmax_x_ + z * mmax_m_xy].t = m_fdDefReq;
            stodef.x = x;
            stodef.y = y * mmax_x_;
            stodef.z = z * mmax_m_xy;
            vtodefine.push_back(stodef);
            do {
                stodef = vtodefine.back();
                vtodefine.pop_back();
                x = stodef.x;
                y = stodef.y;
                z = stodef.z;
                //if (x == 50 && y / mmax_x_ == 27 && z / mmax_m_xy == 2)
                    //x = 50;
                uint8 this_s = mtsurfaces_[x + y + z].twd;
                uint8 upper_s = 0;
                floodPointDesc *cfp = &(mdpoints_[x + y + z]);
                int zm = z - mmax_m_xy;
                // if current is 0x00 or 0x10 tile we will use lower tile
                // to define it
                if (this_s == 0x00 || this_s == 0x10) {
                    if (zm < 0) {
                        cfp->t = m_fdNonWalkable;
                        continue;
                    }
                    z = zm;
                    zm -= mmax_m_xy;
                    upper_s = this_s;
                    this_s = mtsurfaces_[x + y + z].twd;
                    if (!sWalkable(this_s, upper_s))
                        continue;
                } else if (this_s == 0x11 || this_s == 0x12) {
                    int zp_tmp = z + mmax_m_xy;
                    if (zp_tmp < mmax_m_all) {
                        // we are defining tile above current
                        cfp = &(mdpoints_[x + y + zp_tmp]);
                    } else
                        cfp->t = m_fdNonWalkable;
                }
                int xm = x - 1;
                int ym = y - mmax_x_;
                int xp = x + 1;
                int yp = y + mmax_x_;
                int zp = z + mmax_m_xy;
                floodPointDesc *nxtfp;
                if (zp < mmax_m_all) {
                    upper_s = mtsurfaces_[x + y + zp].twd;
                    if(!sWalkable(this_s, upper_s)) {
                        cfp->t = m_fdNonWalkable;
                        continue;
                    }
                } else {
                    cfp->t = m_fdNonWalkable;
                    continue;
                }
                unsigned char sdirm = 0x00;
                unsigned char sdirh = 0x00;
                unsigned char sdirl = 0x00;
                unsigned char sdirmr = 0x00;

                switch (this_s) {
                    case 0x00:
                        cfp->t = m_fdNonWalkable;
                        break;
                    case 0x01:
                        cfp->t = m_fdWalkable;
                        cfp->t |= m_fdSafeWalk;
                        if (zm >= 0) {
                            mdpoints_[x + y + zm].t = m_fdNonWalkable;
                            if (yp < mmax_m_xy) {
                                this_s = mtsurfaces_[x + yp + zm].twd;
                                upper_s = mtsurfaces_[x + yp + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    sdirm |= 0x01;
                                    nxtfp = &(mdpoints_[x + yp + z]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = yp;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (this_s == 0x01) {
                                    nxtfp = &(mdpoints_[x + yp + zm]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x01;
                                        nxtfp = &(mdpoints_[x + yp + zm]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = yp;
                                            stodef.z = zm;
                                            vtodefine.push_back(stodef);
                                        }
                                    } else
                                        nxtfp->t = m_fdNonWalkable;
                                }
                            }
                            if (xm >= 0) {
                                this_s = mtsurfaces_[xm + y + zm].twd;
                                upper_s = mtsurfaces_[xm + y + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[xm + y + z]);
                                    sdirm |= 0x40;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xm;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (isStairs(this_s)) {
                                    nxtfp = &(mdpoints_[xm + y + zm]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xm;
                                        stodef.y = y;
                                        stodef.z = zm;
                                        vtodefine.push_back(stodef);
                                    }
                                }
                            }
                            if (xp < mmax_x_) {
                                this_s = mtsurfaces_[xp + y + zm].twd;
                                upper_s = mtsurfaces_[xp + y + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[xp + y + z]);
                                    sdirm |= 0x04;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xp;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (isStairs(this_s)) {
                                    nxtfp = &(mdpoints_[xp + y + zm]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xp;
                                        stodef.y = y;
                                        stodef.z = zm;
                                        vtodefine.push_back(stodef);
                                    }
                                }
                            }
                        }

                        if (ym >= 0) {
                            nxtfp = &(mdpoints_[x + ym + zp]);
                            this_s = mtsurfaces_[x + ym + z].twd;
                            upper_s = mtsurfaces_[x + ym + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                sdirh |= 0x10;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = ym;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if(upper_s == 0x01 && (zp + mmax_m_xy) < mmax_m_all) {
                                if(sWalkable(upper_s, mtsurfaces_[
                                    x + ym + (zp + mmax_m_xy)].twd))
                                {
                                    sdirh |= 0x10;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = ym;
                                        stodef.z = zp;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }

                        if (xm >= 0) {
                            this_s = mtsurfaces_[xm + y + z].twd;
                            upper_s = mtsurfaces_[xm + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                nxtfp = &(mdpoints_[xm + y + zp]);
                                sdirh |= 0x40;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (this_s == 0x01) {
                                nxtfp = &(mdpoints_[xm + y + z]);
                                if (sWalkable(this_s, upper_s)) {
                                    sdirm |= 0x40;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xm;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }

                        if (xp < mmax_x_) {
                            this_s = mtsurfaces_[xp + y + z].twd;
                            upper_s = mtsurfaces_[xp + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                nxtfp = &(mdpoints_[xp + y + zp]);
                                sdirh |= 0x04;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (this_s == 0x01) {
                                nxtfp = &(mdpoints_[xp + y + z]);
                                if (sWalkable(this_s, upper_s)) {
                                    sdirm |= 0x04;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xp;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }
                        cfp->dirm = sdirm;
                        cfp->dirh = sdirh;
                        cfp->dirl = sdirl;

                        break;
                    case 0x02:
                        cfp->t = m_fdWalkable;
                        cfp->t |= m_fdSafeWalk;
                        if (zm >= 0) {
                            mdpoints_[x + y + zm].t = m_fdNonWalkable;
                            if (ym >= 0) {
                                this_s = mtsurfaces_[x + ym + zm].twd;
                                upper_s = mtsurfaces_[x + ym + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[x + ym + z]);
                                    sdirm |= 0x10;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = ym;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (this_s == 0x02) {
                                    nxtfp = &(mdpoints_[x + ym + zm]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x10;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = ym;
                                            stodef.z = zm;
                                            vtodefine.push_back(stodef);
                                        }
                                    } else
                                        nxtfp->t = m_fdNonWalkable;
                                }
                            }
                            if (xm >= 0) {
                                this_s = mtsurfaces_[xm + y + zm].twd;
                                upper_s = mtsurfaces_[xm + y + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[xm + y + z]);
                                    sdirm |= 0x40;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xm;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (isStairs(this_s)) {
                                    nxtfp = &(mdpoints_[xm + y + zm]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xm;
                                        stodef.y = y;
                                        stodef.z = zm;
                                        vtodefine.push_back(stodef);
                                    }
                                }
                            }
                            if (xp < mmax_x_) {
                                this_s = mtsurfaces_[xp + y + zm].twd;
                                upper_s = mtsurfaces_[xp + y + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[xp + y + z]);
                                    sdirm |= 0x04;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xp;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (isStairs(this_s)) {
                                    nxtfp = &(mdpoints_[xp + y + zm]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xp;
                                        stodef.y = y;
                                        stodef.z = zm;
                                        vtodefine.push_back(stodef);
                                    }
                                }
                            }
                        }

                        if (yp < mmax_m_xy) {
                            nxtfp = &(mdpoints_[x + yp + zp]);
                            this_s = mtsurfaces_[x + yp + z].twd;
                            upper_s = mtsurfaces_[x + yp + zp].twd;
                            if(isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                sdirh |= 0x01;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if(upper_s == 0x02 && (zp + mmax_m_xy) < mmax_m_all) {
                                if(sWalkable(upper_s,  mtsurfaces_[
                                    x + yp + (zp + mmax_m_xy)].twd))
                                {
                                    sdirh |= 0x01;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = yp;
                                        stodef.z = zp;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }

                        if (xm >= 0) {
                            this_s = mtsurfaces_[xm + y + z].twd;
                            upper_s = mtsurfaces_[xm + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                nxtfp = &(mdpoints_[xm + y + zp]);
                                sdirh |= 0x40;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (this_s == 0x02) {
                                nxtfp = &(mdpoints_[xm + y + z]);
                                if (sWalkable(this_s, upper_s)) {
                                    sdirm |= 0x40;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xm;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }

                        if (xp < mmax_x_) {
                            this_s = mtsurfaces_[xp + y + z].twd;
                            upper_s = mtsurfaces_[xp + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                nxtfp = &(mdpoints_[xp + y + zp]);
                                sdirh |= 0x04;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (this_s == 0x02) {
                                nxtfp = &(mdpoints_[xp + y + z]);
                                if (sWalkable(this_s, upper_s)) {
                                    sdirm |= 0x04;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xp;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }
                        cfp->dirm = sdirm;
                        cfp->dirh = sdirh;
                        cfp->dirl = sdirl;

                        break;
                    case 0x03:
                        cfp->t = m_fdWalkable;
                        cfp->t |= m_fdSafeWalk;
                        if (zm >= 0) {
                            mdpoints_[x + y + zm].t = m_fdNonWalkable;
                            if (xm >= 0) {
                                this_s = mtsurfaces_[xm + y + zm].twd;
                                upper_s = mtsurfaces_[xm + y + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[xm + y + z]);
                                    sdirm |= 0x40;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xm;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (this_s == 0x03) {
                                    nxtfp = &(mdpoints_[xm + y + zm]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x40;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xm;
                                            stodef.y = y;
                                            stodef.z = zm;
                                            vtodefine.push_back(stodef);
                                        }
                                    } else
                                        nxtfp->t = m_fdNonWalkable;
                                }
                            }
                            if (ym >= 0) {
                                this_s = mtsurfaces_[x + ym + zm].twd;
                                upper_s = mtsurfaces_[x + ym + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[x + ym + z]);
                                    sdirm |= 0x10;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = ym;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (isStairs(this_s)) {
                                    nxtfp = &(mdpoints_[x + ym + zm]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = ym;
                                        stodef.z = zm;
                                        vtodefine.push_back(stodef);
                                    }
                                }
                            }
                            if (yp < mmax_m_xy) {
                                this_s = mtsurfaces_[x + yp + zm].twd;
                                upper_s = mtsurfaces_[x + yp + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[x + yp + z]);
                                    sdirm |= 0x01;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = yp;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (isStairs(this_s)) {
                                    nxtfp = &(mdpoints_[x + yp + zm]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = yp;
                                        stodef.z = zm;
                                        vtodefine.push_back(stodef);
                                    }
                                }
                            }
                        }

                        if (xp < mmax_x_) {
                            nxtfp = &(mdpoints_[xp + y + zp]);
                            this_s = mtsurfaces_[xp + y + z].twd;
                            upper_s = mtsurfaces_[xp + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                sdirh |= 0x04;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if(upper_s == 0x03 && (zp + mmax_m_xy) < mmax_m_all) {
                                if(sWalkable(upper_s,
                                    mtsurfaces_[xp + y + (zp + mmax_m_xy)].twd))
                                {
                                    sdirh |= 0x04;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xp;
                                        stodef.y = y;
                                        stodef.z = zp;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }

                        if (ym >= 0) {
                            this_s = mtsurfaces_[x + ym + z].twd;
                            upper_s = mtsurfaces_[x + ym + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                nxtfp = &(mdpoints_[x + ym + zp]);
                                sdirh |= 0x10;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = ym;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (this_s == 0x03) {
                                nxtfp = &(mdpoints_[x + ym + z]);
                                if (sWalkable(this_s, upper_s)) { 
                                    sdirm |= 0x10;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = ym;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }

                        if (yp < mmax_m_xy) {
                            this_s = mtsurfaces_[x + yp + z].twd;
                            upper_s = mtsurfaces_[x + yp + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                nxtfp = &(mdpoints_[x + yp + zp]);
                                sdirh |= 0x01;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (this_s == 0x03) {
                                nxtfp = &(mdpoints_[x + yp + z]);
                                if (sWalkable(this_s, upper_s)) {
                                    sdirm |= 0x01;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = yp;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }
                        cfp->dirm = sdirm;
                        cfp->dirh = sdirh;
                        cfp->dirl = sdirl;

                        break;
                    case 0x04:
                        cfp->t = m_fdWalkable;
                        cfp->t |= m_fdSafeWalk;
                        if (zm >= 0) {
                            mdpoints_[x + y + zm].t = m_fdNonWalkable;
                            if (xp < mmax_x_) {
                                this_s = mtsurfaces_[xp + y + zm].twd;
                                upper_s = mtsurfaces_[xp + y + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[xp + y + z]);
                                    sdirm |= 0x04;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xp;
                                        stodef.y = y;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (this_s == 0x04) {
                                    nxtfp = &(mdpoints_[xp + y + zm]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x04;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xp;
                                            stodef.y = y;
                                            stodef.z = zm;
                                            vtodefine.push_back(stodef);
                                        }
                                    } else
                                        nxtfp->t = m_fdNonWalkable;
                                }
                            }
                            if (ym >= 0) {
                                this_s = mtsurfaces_[x + ym + zm].twd;
                                upper_s = mtsurfaces_[x + ym + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[x + ym + z]);
                                    sdirm |= 0x10;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = ym;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (isStairs(this_s)) {
                                    nxtfp = &(mdpoints_[x + ym + zm]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = ym;
                                        stodef.z = zm;
                                        vtodefine.push_back(stodef);
                                    }
                                }
                            }
                            if (yp < mmax_m_xy) {
                                this_s = mtsurfaces_[x + yp + zm].twd;
                                upper_s = mtsurfaces_[x + yp + z].twd;
                                if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                    nxtfp = &(mdpoints_[x + yp + z]);
                                    sdirm |= 0x01;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = yp;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else if (isStairs(this_s)) {
                                    nxtfp = &(mdpoints_[x + yp + zm]);
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = yp;
                                        stodef.z = zm;
                                        vtodefine.push_back(stodef);
                                    }
                                }
                            }
                        }

                        if (xm >= 0) {
                            nxtfp = &(mdpoints_[xm + y + zp]);
                            this_s = mtsurfaces_[xm + y + z].twd;
                            upper_s = mtsurfaces_[xm + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                sdirh |= 0x40;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if(upper_s == 0x04 && (zp + mmax_m_xy) < mmax_m_all) {
                                if(sWalkable(upper_s, mtsurfaces_[
                                    xm + y + (zp + mmax_m_xy)].twd))
                                {
                                    sdirh |= 0x40;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = xm;
                                        stodef.y = y;
                                        stodef.z = zp;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }

                        if (ym >= 0) {
                            this_s = mtsurfaces_[x + ym + z].twd;
                            upper_s = mtsurfaces_[x + ym + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                nxtfp = &(mdpoints_[x + ym + zp]);
                                sdirh |= 0x10;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = ym;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (this_s == 0x04) {
                                nxtfp = &(mdpoints_[x + ym + z]);
                                if (sWalkable(this_s, upper_s)) {
                                    sdirm |= 0x10;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = ym;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }

                        if (yp < mmax_m_xy) {
                            this_s = mtsurfaces_[x + yp + z].twd;
                            upper_s = mtsurfaces_[x + yp + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s)) {
                                nxtfp = &(mdpoints_[x + yp + zp]);
                                sdirh |= 0x01;
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (this_s == 0x04) {
                                nxtfp = &(mdpoints_[x + yp + z]);
                                if (sWalkable(this_s, upper_s)) {
                                    sdirm |= 0x01;
                                    if (nxtfp->t == m_fdNotDefined) {
                                        nxtfp->t = m_fdDefReq;
                                        stodef.x = x;
                                        stodef.y = yp;
                                        stodef.z = z;
                                        vtodefine.push_back(stodef);
                                    }
                                } else
                                    nxtfp->t = m_fdNonWalkable;
                            }
                        }
                        cfp->dirm = sdirm;
                        cfp->dirh = sdirh;
                        cfp->dirl = sdirl;

                        break;
                    case 0x05:
                    case 0x06:
                    case 0x07:
                    case 0x08:
                    case 0x09:
                    case 0x0B:
                    case 0x0D:
                    case 0x0E:
                    case 0x0F:
                        cfp->t = m_fdWalkable;
                        if (!((this_s > 0x05 && this_s < 0x0A) || this_s == 0x0B
                            || this_s == 0x0F))
                        {
                            cfp->t |= m_fdSafeWalk;
                        }
                        if (xm >= 0) {
                            this_s = mtsurfaces_[xm + y + z].twd;
                            upper_s = mtsurfaces_[xm + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x20 | 0x40 | 0x80);
                                nxtfp = &(mdpoints_[xm + y + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x20 | 0x80);
                                if (this_s == 0x01 || this_s == 0x02
                                    || this_s == 0x03)
                                {
                                    sdirl |= 0x40;
                                }
                                nxtfp = &(mdpoints_[xm + y + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = y;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x20 | 0x80);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x01 || upper_s == 0x02 || upper_s == 0x04
                                    || upper_s == 0x12)) {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[xm + y + (zp + mmax_m_xy)].twd))
                                    {
                                        if (upper_s == 0x12)
                                            sdirh |= 0x40;
                                        else
                                            sdirm |= 0x40;
                                        nxtfp = &(mdpoints_[xm + y + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xm;
                                            stodef.y = y;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x20 | 0x80);

                        if (xp < mmax_x_) {
                            this_s = mtsurfaces_[xp + y + z].twd;
                            upper_s = mtsurfaces_[xp + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x02 | 0x04 | 0x08);
                                nxtfp = &(mdpoints_[xp + y + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x02 | 0x08);
                                if (this_s == 0x01 || this_s == 0x02
                                    || this_s == 0x04)
                                {
                                    sdirl |= 0x04;
                                }
                                nxtfp = &(mdpoints_[xp + y + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x02 | 0x08);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x01 || upper_s == 0x02
                                    || upper_s == 0x03 || upper_s == 0x11))
                                {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[xp + y + (zp + mmax_m_xy)].twd))
                                    {
                                        if (upper_s == 0x11)
                                            sdirh |= 0x04;
                                        else
                                            sdirm |= 0x04;
                                        nxtfp = &(mdpoints_[xp + y + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xp;
                                            stodef.y = y;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x02 | 0x08);

                        if(ym >= 0) {
                            this_s = mtsurfaces_[x + ym + z].twd;
                            upper_s = mtsurfaces_[x + ym + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x08 | 0x10 | 0x20);
                                nxtfp = &(mdpoints_[x + ym + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = ym;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x08 | 0x20);
                                if (this_s == 0x02 || this_s == 0x03 || this_s == 0x04){
                                    sdirl |= 0x10;
                                }
                                nxtfp = &(mdpoints_[x + ym + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = ym;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x08 | 0x20);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x01 || upper_s == 0x03
                                    || upper_s == 0x04 || upper_s == 0x11))
                                {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[x + ym + (zp + mmax_m_xy)].twd))
                                    {
                                        if (upper_s == 0x11)
                                            sdirh |= 0x10;
                                        else
                                            sdirm |= 0x10;
                                        nxtfp = &(mdpoints_[x + ym + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = ym;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x08 | 0x20);

                        if (yp < mmax_m_xy) {
                            this_s = mtsurfaces_[x + yp + z].twd;
                            upper_s = mtsurfaces_[x + yp + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x80 | 0x01 | 0x02);
                                nxtfp = &(mdpoints_[x + yp + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x80 | 0x02);
                                if (this_s == 0x01 || this_s == 0x03
                                    || this_s == 0x04)
                                {
                                    sdirl |= 0x01;
                                }
                                nxtfp = &(mdpoints_[x + yp + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x80 | 0x02);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x02 || upper_s == 0x03
                                    || upper_s == 0x04 || upper_s == 0x12))
                                {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[x + yp + (zp + mmax_m_xy)].twd))
                                    {
                                        if (upper_s == 0x12)
                                            sdirh |= 0x01;
                                        else
                                            sdirm |= 0x01;
                                        nxtfp = &(mdpoints_[x + yp + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = yp;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x80 | 0x02);
                        sdirm &= (0xFF ^ sdirmr);

                        // edges

                        if (xm >= 0) {
                            if (ym >= 0 && (sdirm & 0x20) != 0) {
                                nxtfp = &(mdpoints_[xm + ym + zp]);
                                this_s = mtsurfaces_[xm + ym + z].twd;
                                upper_s = mtsurfaces_[xm + ym + zp].twd;
                                if (!(isSurface(this_s) && sWalkable(this_s,
                                    upper_s)))
                                {
                                    sdirm &= (0xFF ^ 0x20);
                                } else if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = ym;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            }

                            if (yp < mmax_m_xy && (sdirm & 0x80) != 0) {
                                nxtfp = &(mdpoints_[xm + yp + zp]);
                                this_s = mtsurfaces_[xm + yp + z].twd;
                                upper_s = mtsurfaces_[xm + yp + zp].twd;
                                if (!(isSurface(this_s) && sWalkable(this_s,
                                    upper_s)))
                                {
                                    sdirm &= (0xFF ^ 0x80);
                                } else if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            }
                        }

                        if (xp < mmax_x_) {
                            if (ym >= 0 && (sdirm & 0x08) != 0) {
                                nxtfp = &(mdpoints_[xp + ym + zp]);
                                this_s = mtsurfaces_[xp + ym + z].twd;
                                upper_s = mtsurfaces_[xp + ym + zp].twd;
                                if (!(isSurface(this_s) && sWalkable(this_s,
                                    upper_s)))
                                {
                                    sdirm &= (0xFF ^ 0x08);
                                } else if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = ym;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            }

                            if (yp < mmax_m_xy && (sdirm & 0x02) != 0) {
                                nxtfp = &(mdpoints_[xp + yp + zp]);
                                this_s = mtsurfaces_[xp + yp + z].twd;
                                upper_s = mtsurfaces_[xp + yp + zp].twd;
                                if (!(isSurface(this_s) && sWalkable(this_s,
                                    upper_s)))
                                {
                                    sdirm &= (0xFF ^ 0x02);
                                } else if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            }
                        }
                        cfp->dirm = sdirm;
                        cfp->dirh = sdirh;
                        cfp->dirl = sdirl;

                        break;
                    case 0x0A:
                    case 0x0C:
                    case 0x10:
                        cfp->t = m_fdNonWalkable;
                        break;
                    case 0x11:
                        cfp->t = m_fdWalkable;
                        cfp->t |= m_fdSafeWalk;
                        if (zm >= 0) {
                            mdpoints_[x + y + zm].t = m_fdNonWalkable;
                            if (xm >= 0) {
                                this_s = mtsurfaces_[xm + y + zm].twd;
                                upper_s = mtsurfaces_[xm + y + z].twd;
                                if (isSurface(this_s)) {
                                    nxtfp = &(mdpoints_[xm + y + z]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x40;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xm;
                                            stodef.y = y;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                } else if (isStairs(upper_s) && upper_s != 0x04) {
                                    nxtfp = &(mdpoints_[xm + y + z]);
                                    this_s = upper_s;
                                    upper_s = mtsurfaces_[xm + y + zp].twd;
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x40;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xm;
                                            stodef.y = y;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                            if (ym >= 0) {
                                this_s = mtsurfaces_[x + ym + zm].twd;
                                upper_s = mtsurfaces_[x + ym + z].twd;
                                if (isSurface(this_s)) {
                                    nxtfp = &(mdpoints_[x + ym + z]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x10;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = ym;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                } else if (isStairs(upper_s) && upper_s != 0x01) {
                                    nxtfp = &(mdpoints_[x + ym + z]);
                                    this_s = upper_s;
                                    upper_s = mtsurfaces_[x + ym + zp].twd;
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x10;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = ym;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                            if (yp < mmax_m_xy) {
                                this_s = mtsurfaces_[x + yp + zm].twd;
                                upper_s = mtsurfaces_[x + yp + z].twd;
                                if (isSurface(this_s)) {
                                    nxtfp = &(mdpoints_[x + yp + z]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x01;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = yp;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                } else if (isStairs(upper_s) && upper_s != 0x02) {
                                    nxtfp = &(mdpoints_[x + yp + z]);
                                    this_s = upper_s;
                                    upper_s = mtsurfaces_[x + yp + zp].twd;
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x01;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = yp;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        }

                        if (xp < mmax_x_) {
                            this_s = mtsurfaces_[xp + y + z].twd;
                            upper_s = mtsurfaces_[xp + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x02 | 0x04 | 0x08);
                                nxtfp = &(mdpoints_[xp + y + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x02 | 0x08);
                                if (this_s == 0x01 || this_s == 0x02 || this_s == 0x04){
                                    sdirl |= 0x04;
                                }
                                nxtfp = &(mdpoints_[xp + y + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x02 | 0x08);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x01 || upper_s == 0x02
                                    || upper_s == 0x03))
                                {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[xp + y + (zp + mmax_m_xy)].twd))
                                    {
                                        sdirm |= 0x04;
                                        nxtfp = &(mdpoints_[xp + y + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xp;
                                            stodef.y = y;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x02 | 0x08);

                        if(ym >= 0) {
                            this_s = mtsurfaces_[x + ym + z].twd;
                            upper_s = mtsurfaces_[x + ym + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x08 | 0x10);
                                nxtfp = &(mdpoints_[x + ym + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = ym;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x08 | 0x20);
                                if (this_s == 0x02 || this_s == 0x03 || this_s == 0x04) {
                                    sdirl |= 0x10;
                                }
                                nxtfp = &(mdpoints_[x + ym + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = ym;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x08 | 0x20);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x01 || upper_s == 0x03 || upper_s == 0x04)) {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[x + ym + (zp + mmax_m_xy)].twd))
                                    {
                                        sdirm |= 0x10;
                                        nxtfp = &(mdpoints_[x + ym + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = ym;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x08);

                        if (yp < mmax_m_xy) {
                            this_s = mtsurfaces_[x + yp + z].twd;
                            upper_s = mtsurfaces_[x + yp + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x01 | 0x02);
                                nxtfp = &(mdpoints_[x + yp + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x80 | 0x02);
                                if (this_s == 0x01 || this_s == 0x03 || this_s == 0x04) {
                                    sdirl |= 0x01;
                                }
                                nxtfp = &(mdpoints_[x + yp + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x80 | 0x02);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x02 || upper_s == 0x03
                                    || upper_s == 0x04))
                                {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[x + yp + (zp + mmax_m_xy)].twd))
                                    {
                                        sdirm |= 0x01;
                                        nxtfp = &(mdpoints_[x + yp + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = yp;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x80 | 0x02);
                        sdirm &= (0xFF ^ sdirmr);

                        // edges
                        if (xp < mmax_x_) {
                            if (ym >= 0 && (sdirm & 0x08) != 0) {
                                nxtfp = &(mdpoints_[xp + ym + zp]);
                                this_s = mtsurfaces_[xp + ym + z].twd;
                                upper_s = mtsurfaces_[xp + ym + zp].twd;
                                if (!(isSurface(this_s) && sWalkable(this_s,
                                    upper_s)))
                                {
                                    sdirm &= (0xFF ^ 0x08);
                                } else if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = ym;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            }

                            if (yp < mmax_m_xy && (sdirm & 0x02) != 0) {
                                nxtfp = &(mdpoints_[xp + yp + zp]);
                                this_s = mtsurfaces_[xp + yp + z].twd;
                                upper_s = mtsurfaces_[xp + yp + zp].twd;
                                if (!(isSurface(this_s) && sWalkable(this_s,
                                    upper_s)))
                                {
                                    sdirm &= (0xFF ^ 0x02);
                                } else if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = yp;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            }
                        }
                        cfp->dirm = sdirm;
                        cfp->dirh = sdirh;
                        cfp->dirl = sdirl;

                        break;
                    case 0x12:
                        cfp->t = m_fdWalkable;
                        cfp->t |= m_fdSafeWalk;
                        if (zm >= 0) {
                            mdpoints_[x + y + zm].t = m_fdNonWalkable;
                            if (ym >= 0) {
                                this_s = mtsurfaces_[x + ym + zm].twd;
                                upper_s = mtsurfaces_[x + ym + z].twd;
                                if (isSurface(this_s)) {
                                    nxtfp = &(mdpoints_[x + ym + z]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x10;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = ym;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                } else if (isStairs(upper_s) && upper_s != 0x01) {
                                    nxtfp = &(mdpoints_[x + ym + z]);
                                    this_s = upper_s;
                                    upper_s = mtsurfaces_[x + ym + zp].twd;
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x10;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = ym;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                            if (xm >= 0) {
                                this_s = mtsurfaces_[xm + y + zm].twd;
                                upper_s = mtsurfaces_[xm + y + z].twd;
                                if (isSurface(this_s)) {
                                    nxtfp = &(mdpoints_[xm + y + z]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x40;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xm;
                                            stodef.y = y;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                } else if (isStairs(upper_s) && upper_s != 0x04) {
                                    nxtfp = &(mdpoints_[xm + y + z]);
                                    this_s = upper_s;
                                    upper_s = mtsurfaces_[xm + y + zp].twd;
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x40;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xm;
                                            stodef.y = y;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                            if (xp < mmax_x_) {
                                this_s = mtsurfaces_[xp + y + zm].twd;
                                upper_s = mtsurfaces_[xp + y + z].twd;
                                if (isSurface(this_s)) {
                                    nxtfp = &(mdpoints_[xp + y + z]);
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x04;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xp;
                                            stodef.y = y;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                } else if (isStairs(upper_s) && upper_s != 0x03) {
                                    nxtfp = &(mdpoints_[xp + y + z]);
                                    this_s = upper_s;
                                    upper_s = mtsurfaces_[xp + y + zp].twd;
                                    if (sWalkable(this_s, upper_s)) {
                                        sdirl |= 0x04;
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xp;
                                            stodef.y = y;
                                            stodef.z = z;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        }

                        if (xm >=0) {
                            this_s = mtsurfaces_[xm + y + z].twd;
                            upper_s = mtsurfaces_[xm + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x40 | 0x80);
                                nxtfp = &(mdpoints_[xm + y + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x20 | 0x80);
                                if (this_s == 0x01 || this_s == 0x02 || this_s == 0x03){
                                    sdirl |= 0x40;
                                }
                                nxtfp = &(mdpoints_[xm + y + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = y;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x20 | 0x80);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x01 || upper_s == 0x02
                                    || upper_s == 0x04))
                                {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[xm + y + (zp + mmax_m_xy)].twd))
                                    {
                                        sdirm |= 0x40;
                                        nxtfp = &(mdpoints_[xm + y + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xm;
                                            stodef.y = y;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x20 | 0x80);

                        if (xp < mmax_x_) {
                            this_s = mtsurfaces_[xp + y + z].twd;
                            upper_s = mtsurfaces_[xp + y + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x02 | 0x04);
                                nxtfp = &(mdpoints_[xp + y + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x02 | 0x08);
                                if (this_s == 0x01 || this_s == 0x02
                                    || this_s == 0x04)
                                {
                                    sdirl |= 0x04;
                                }
                                nxtfp = &(mdpoints_[xp + y + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = y;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x02 | 0x08);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x01 || upper_s == 0x02
                                    || upper_s == 0x03))
                                {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[xp + y + (zp + mmax_m_xy)].twd))
                                    {
                                        sdirm |= 0x04;
                                        nxtfp = &(mdpoints_[xp + y + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = xp;
                                            stodef.y = y;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x02 | 0x08);

                        if (yp < mmax_m_xy) {
                            this_s = mtsurfaces_[x + yp + z].twd;
                            upper_s = mtsurfaces_[x + yp + zp].twd;
                            if (isSurface(this_s) && sWalkable(this_s, upper_s))
                            {
                                sdirm |= (0x80 | 0x01 | 0x02);
                                nxtfp = &(mdpoints_[x + yp + zp]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            } else if (isStairs(this_s) && sWalkable(this_s,
                                upper_s))
                            {
                                sdirmr |= (0x80 | 0x02);
                                if (this_s == 0x01 || this_s == 0x03 || this_s == 0x04) {
                                    sdirl |= 0x01;
                                }
                                nxtfp = &(mdpoints_[x + yp + z]);
                                if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = x;
                                    stodef.y = yp;
                                    stodef.z = z;
                                    vtodefine.push_back(stodef);
                                }
                            } else {
                                sdirmr |= (0x80 | 0x02);
                                if ((zp + mmax_m_xy) < mmax_m_all
                                    && (upper_s == 0x02 || upper_s == 0x03
                                    || upper_s == 0x04))
                                {
                                    if (sWalkable(upper_s,
                                        mtsurfaces_[x + yp + (zp + mmax_m_xy)].twd))
                                    {
                                        sdirm |= 0x01;
                                        nxtfp = &(mdpoints_[x + yp + zp]);
                                        if (nxtfp->t == m_fdNotDefined) {
                                            nxtfp->t = m_fdDefReq;
                                            stodef.x = x;
                                            stodef.y = yp;
                                            stodef.z = zp;
                                            vtodefine.push_back(stodef);
                                        }
                                    }
                                }
                            }
                        } else
                            sdirmr |= (0x80 | 0x02);
                        sdirm &= (0xFF ^ sdirmr);

                        // edges
                        if (yp < mmax_m_xy) {
                            if (xm >= 0 && (sdirm & 0x80) != 0) {
                                nxtfp = &(mdpoints_[xm + yp + zp]);
                                this_s = mtsurfaces_[xm + yp + z].twd;
                                upper_s = mtsurfaces_[xm + yp + zp].twd;
                                if (!(isSurface(this_s) && sWalkable(this_s,
                                    upper_s)))
                                {
                                    sdirm &= (0xFF ^ 0x80);
                                } else if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xm;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            }
                            if (xp < mmax_x_ && (sdirm & 0x02) != 0) {
                                nxtfp = &(mdpoints_[xp + yp + zp]);
                                this_s = mtsurfaces_[xp + yp + z].twd;
                                upper_s = mtsurfaces_[xp + yp + zp].twd;
                                if (!(isSurface(this_s) && sWalkable(this_s,
                                    upper_s)))
                                {
                                    sdirm &= (0xFF ^ 0x02);
                                } else if (nxtfp->t == m_fdNotDefined) {
                                    nxtfp->t = m_fdDefReq;
                                    stodef.x = xp;
                                    stodef.y = yp;
                                    stodef.z = zp;
                                    vtodefine.push_back(stodef);
                                }
                            }
                        }

                        cfp->dirm = sdirm;
                        cfp->dirh = sdirh;
                        cfp->dirl = sdirl;

                        break;
                }
            } while (vtodefine.size());
        }
    }
#if 0
    unsigned int cw = 0;
    for (int iz = 0; iz < mmax_z_; iz++) {
        for (int iy = 0; iy < mmax_y_; iy++) {
            for (int ix = 0; ix < mmax_x_; ix++) {
                floodPointDesc *cfpp = &(mdpoints_[ix + iy * mmax_x_ + iz * mmax_m_xy]);

                if ((cfpp->t & m_fdWalkable) == m_fdWalkable)
                    cw++;
            }
        }
    }

    printf("flood walkables %i\n", cw);
#endif
    return true;
}

void Mission::clrSurfaces() {

    if(mtsurfaces_ != NULL) {
        free(mtsurfaces_);
        mtsurfaces_ = NULL;
    }
    if(mdpoints_ != NULL) {
        free(mdpoints_);
        mdpoints_ = NULL;
    }
    if(mdpoints_cp_ != NULL) {
        free(mdpoints_cp_);
        mdpoints_cp_ = NULL;
    }
}

/*!
 * Uses isometric coordinates transformation to find walkable tile,
 * starting from top.
 * \param mtp
 * \return True if walkable tile found
 */
bool Mission::getWalkable(MapTilePoint &mtp) {
    bool gotit = false;
    int bx, by, box, boy;
    int bz = mmax_z_;
    unsigned int cindx;
    unsigned char twd;
    do {
        bz--;
        // using lowered Z, at start postion is at top of tile not at bottom
        bx = mtp.tx * 256 + mtp.ox + 128 * (bz - 1);
        box = bx % 256;
        bx = bx / 256;
        by = mtp.ty * 256 + mtp.oy + 128 * (bz - 1);
        boy = by % 256;
        by = by / 256;
        if (bz >= mmax_z_ || bx >= mmax_x_ || by >= mmax_y_)
            continue;
        if (bz < 0 || bx < 0 || by < 0)
            break;
        cindx = bx + by * mmax_x_ + bz * mmax_m_xy;
        if ((mdpoints_[cindx].t & m_fdWalkable) == m_fdWalkable) {
            twd = mtsurfaces_[cindx].twd;
            int dx = 0;
            int dy = 0;
            switch (twd) {
                case 0x01:
                    dy = ((boy + 128) * 2) / 3;
                    dx = box + 128 - dy / 2;
                    if (dx < 256) {
                        gotit = true;
                        box = dx;
                        boy = dy;
                    } else {
                        if ((bx + 1) < mmax_x_) {
                            cindx = (bx + 1) + by * mmax_x_ + bz * mmax_m_xy;
                            if ((mdpoints_[cindx].t & m_fdWalkable) == m_fdWalkable
                                && mtsurfaces_[cindx].twd == 0x01)
                            {
                                gotit = true;
                                bx++;
                                box = dx - 256;
                                boy = dy;
                            }
                        }
                    }
                    break;
                case 0x02:
                    if (boy < 128) {
                        dy = boy * 2;
                        dx = box + boy;
                        if (dx < 256) {
                            gotit = true;
                            box = dx;
                            boy = dy;
                        } else {
                            if ((bx + 1) < mmax_x_) {
                                cindx = (bx + 1) + by * mmax_x_ + bz * mmax_m_xy;
                                if ((mdpoints_[cindx].t & m_fdWalkable) == m_fdWalkable
                                    && mtsurfaces_[cindx].twd == 0x02)
                                {
                                    gotit = true;
                                    bx++;
                                    box = dx - 256;
                                    boy = dy;
                                }
                            }
                        }
                    } else {
                        // TODO : add check 0x01?
                    }
                    break;
                case 0x03:
                    if (box < 128) {
                        dx = box * 2;
                        dy = box + boy;
                        if (dy < 256) {
                            gotit = true;
                            box = dx;
                            boy = dy;
                        } else {
                            if ((by + 1) < mmax_y_) {
                                cindx = bx + (by + 1) * mmax_x_ + bz * mmax_m_xy;
                                if ((mdpoints_[cindx].t & m_fdWalkable) == m_fdWalkable
                                    && mtsurfaces_[cindx].twd == 0x03)
                                {
                                    gotit = true;
                                    by++;
                                    box = dx;
                                    boy = dy - 256;
                                }
                            }
                        }
                    } else {
                        // TODO : 0x04?
                    }
                    break;
                case 0x04:
                    dx = ((box + 128) * 2) / 3;
                    dy = boy + 128 - dx / 2;
                    if (dy < 256) {
                        gotit = true;
                        box = dx;
                        boy = dy;
                    } else {
                        if ((by + 1) < mmax_y_) {
                            cindx = bx + (by + 1) * mmax_x_ + bz * mmax_m_xy;
                            if ((mdpoints_[cindx].t & m_fdWalkable) == m_fdWalkable
                                && mtsurfaces_[cindx].twd == 0x04)
                            {
                                gotit = true;
                                by++;
                                box = dx;
                                boy = dy - 256;
                            }
                        }
                    }
                    break;
                default:
                    gotit = true;
                    // TODO: 0x01 0x02 0x03 0x04?
                break;
            }
        } else if (bz - 1 >= 0) {
            cindx = bx + by * mmax_x_ + (bz - 1) * mmax_m_xy;
            if ((mdpoints_[cindx].t & m_fdWalkable) == m_fdWalkable) {
                twd = mtsurfaces_[cindx].twd;
                int dx = 0;
                int dy = 0;
                switch (twd) {
                    case 0x01:
                        dy = (boy * 2) / 3;
                        dx = box - dy / 2;
                        if (dx >= 0) {
                            gotit = true;
                            box = dx;
                            boy = dy;
                            bz--;
                        }
                        break;
                    case 0x02:
                        dy = (boy - 128) * 2;
                        dx = (box + dy / 2) - 128;
                        if (dy >= 0 && dx >= 0 && dx < 256) {
                            gotit = true;
                            box = dx;
                            boy = dy;
                            bz--;
                        }
                        break;
                    case 0x03:
                        dx = (box - 128) * 2;
                        dy = (boy + dx / 2) - 128;
                        if (dx >= 0 && dy >= 0 && dy < 256) {
                            gotit = true;
                            box = dx;
                            boy = dy;
                            bz--;
                        }
                        break;
                    case 0x04:
                        dx = (box * 2) / 3;
                        dy = boy - dx / 2;
                        if (dy >= 0) {
                            gotit = true;
                            box = dx;
                            boy = dy;
                            bz--;
                        }
                        break;
                    default:
                        break;
                }
                
            }
        }
    } while (bz != 0 && !gotit);
    if (gotit) {
        mtp.tx = bx;
        mtp.ty = by;
        mtp.tz = bz;
        mtp.ox = box;
        mtp.oy = boy;
    }
    return gotit;
}

bool Mission::getWalkableClosestByZ(MapTilePoint &mtp) {
    // NOTE: using z from mtp as start to find closest z
    int inc_z = mtp.tz;
    int dec_z = mtp.tz;
    bool found = false;

    do {
        if (inc_z < mmax_z_) {
            if ((mdpoints_[mtp.tx + mtp.ty * mmax_x_
                + inc_z * mmax_m_xy].t & m_fdWalkable) == m_fdWalkable)
            {
                mtp.tz = inc_z;
                found = true;
                break;
            }
            inc_z++;
        }
        if (dec_z >= 0) {
            if ((mdpoints_[mtp.tx + mtp.ty * mmax_x_
                + dec_z * mmax_m_xy].t & m_fdWalkable) == m_fdWalkable)
            {
                mtp.tz = dec_z;
                found = true;
                break;
            }
            dec_z--;
        }
    } while (inc_z < mmax_z_ || dec_z >= 0);

    return found;
}

void Mission::adjXYZ(int &x, int &y, int &z) {
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (z < 0 || z >= mmax_z_)
        z = 0;
    if (x >= mmax_x_)
        x = mmax_x_ - 1;
    if (y >= mmax_y_)
        y = mmax_y_ - 1;
}

/*!
* This function looks for blockers - statics, vehicles, peds, weapons
*/
void Mission::blockerExists(toDefineXYZ * startXYZ, toDefineXYZ * endXYZ,
                            double *dist, MapObject** blockerObj)
{
    // TODO: calculating closest blocker first? (start point can be closer though)
    double inc_xyz[3];
    inc_xyz[0] = (endXYZ->x - startXYZ->x) / (*dist);
    inc_xyz[1] = (endXYZ->y - startXYZ->y) / (*dist);
    inc_xyz[2] = (endXYZ->z - startXYZ->z) / (*dist);
    toDefineXYZ copyStartXYZ = *startXYZ;
    toDefineXYZ copyEndXYZ = *endXYZ;
    toDefineXYZ blockStartXYZ;
    toDefineXYZ blockEndXYZ;
    double closest = *dist;

    for (unsigned int i = 0; i < statics_.size(); ++i) {
        MapObject * s_blocker = statics_[i];
        if (s_blocker->isIgnored())
            continue;
        if (s_blocker->isBlocker(&copyStartXYZ, &copyEndXYZ, inc_xyz)) {
            int cx = startXYZ->x - copyStartXYZ.x;
            int cy = startXYZ->y - copyStartXYZ.y;
            int cz = startXYZ->z - copyStartXYZ.z;
            double dist_blocker = sqrt((double) (cx * cx + cy * cy + cz * cz));
            if (closest == -1 || dist_blocker < closest) {
                closest = dist_blocker;
                *blockerObj = s_blocker;
                blockStartXYZ = copyStartXYZ;
                blockEndXYZ = copyEndXYZ;
            }
            copyStartXYZ = *startXYZ;
            copyEndXYZ = *endXYZ;
        }
    }
    for (unsigned int i = 0; i < vehicles_.size(); ++i) {
        MapObject * v_blocker = vehicles_[i];
        if (v_blocker->isIgnored())
            continue;
        if (v_blocker->isBlocker(&copyStartXYZ, &copyEndXYZ, inc_xyz)) {
            int cx = startXYZ->x - copyStartXYZ.x;
            int cy = startXYZ->y - copyStartXYZ.y;
            int cz = startXYZ->z - copyStartXYZ.z;
            double dist_blocker = sqrt((double) (cx * cx + cy * cy + cz * cz));
            if (closest == -1 || dist_blocker < closest) {
                closest = dist_blocker;
                *blockerObj = v_blocker;
                blockStartXYZ = copyStartXYZ;
                blockEndXYZ = copyEndXYZ;
            }
            copyStartXYZ = *startXYZ;
            copyEndXYZ = *endXYZ;
        }
    }
    for (unsigned int i = 0; i < peds_.size(); ++i) {
        MapObject * p_blocker = peds_[i];
        if (p_blocker->isIgnored())
            continue;
        if (p_blocker->isBlocker(&copyStartXYZ, &copyEndXYZ, inc_xyz)) {
            int cx = startXYZ->x - copyStartXYZ.x;
            int cy = startXYZ->y - copyStartXYZ.y;
            int cz = startXYZ->z - copyStartXYZ.z;
            double dist_blocker = sqrt((double) (cx * cx + cy * cy + cz * cz));
            if (closest == -1 || dist_blocker < closest) {
                closest = dist_blocker;
                *blockerObj = p_blocker;
                blockStartXYZ = copyStartXYZ;
                blockEndXYZ = copyEndXYZ;
            }
            copyStartXYZ = *startXYZ;
            copyEndXYZ = *endXYZ;
        }
    }
    for (unsigned int i = 0; i < weapons_.size(); ++i) {
        MapObject * w_blocker = weapons_[i];
        if (w_blocker->isIgnored())
            continue;
        if (w_blocker->isBlocker(&copyStartXYZ, &copyEndXYZ, inc_xyz)) {
            int cx = startXYZ->x - copyStartXYZ.x;
            int cy = startXYZ->y - copyStartXYZ.y;
            int cz = startXYZ->z - copyStartXYZ.z;
            double dist_blocker = sqrt((double) (cx * cx + cy * cy + cz * cz));
            if (closest == -1 || dist_blocker < closest) {
                closest = dist_blocker;
                *blockerObj = w_blocker;
                blockStartXYZ = copyStartXYZ;
                blockEndXYZ = copyEndXYZ;
            }
            copyStartXYZ = *startXYZ;
            copyEndXYZ = *endXYZ;
        }
    }
    if (*blockerObj != NULL) {
        *startXYZ = blockStartXYZ;
        *endXYZ = blockEndXYZ;
        *dist = closest;
    }
}

/*!
* \return mask where bits are:
* 0b - target in range(1); 1b - blocker is object, "t" is set(2)
* 2b - blocker object, "pn" is set(4), 3b - reachable point set,
* 4b - blocker tile, "pn" is set(16), 5b - out of visible reach(32)
* NOTE: only if "pn" or "t" are not null, variables are set
*/
uint8 Mission::inRangeCPos(toDefineXYZ * cp, ShootableMapObject ** t,
    PathNode * pn, bool setBlocker, bool checkTileOnly, double maxr,
    double * distTo)
{
    // TODO: some objects mid point is higher then map z
    assert(maxr >= 0);

    int cx = (*cp).x;
    int cy = (*cp).y;
    int cz = (*cp).z;
    if (cz > (mmax_z_ - 1) * 128)
        return 32;
    int tx = 0;
    int ty = 0;
    int tz = 0;
    if (t && *t) {
        tx = (*t)->tileX() * 256 + (*t)->offX();
        ty = (*t)->tileY() * 256 + (*t)->offY();
        tz = (*t)->tileZ() * 128 + (*t)->offZ() + ((*t)->sizeZ() >> 1);
    } else {
        tx = pn->tileX() * 256 + pn->offX();
        ty = pn->tileY() * 256 + pn->offY();
        tz = pn->tileZ() * 128 + pn->offZ();
    }
    if (tz > (mmax_z_ - 1) * 128)
        return 32;

    double d = 0;
    d = sqrt((double)((tx - cx) * (tx - cx) + (ty - cy) * (ty - cy)
        + (tz - cz) * (tz - cz)));
    uint8 block_mask = 1;

    if (distTo)
        *distTo = d;
    if (d == 0)
        return block_mask;

    double sx = (double) cx;
    double sy = (double) cy;
    double sz = (double) cz;

    if (d >= maxr) {
        block_mask = 0;
        if (pn == NULL)
            return block_mask;
        if (t && *t) {
            tz -= 128;
            *t = NULL;
        }
        double dist_k = (double)maxr / d;
        tx = cx + (int)((tx - cx) * dist_k);
        ty = cy + (int)((ty - cy) * dist_k);
        tz = cz + (int)((tz - cz) * dist_k);
        block_mask = 8;
        if (setBlocker) {
            pn->setTileXYZ(tx / 256, ty / 256, tz / 128);
            pn->setOffXYZ(tx % 256, ty % 256, tz % 128);
        }
        d = maxr;
    }

    // NOTE: these values are less then 1, if they are incremented time
    // required to check range will be shorter less precise check, if
    // decremented longer more precise. Increment is (n * 8)
    double inc_x = ((tx - cx) * 8) / d;
    double inc_y = ((ty - cy) * 8) / d;
    double inc_z = ((tz - cz) * 8) / d;

    int oldx = cx / 256;
    int oldy = cy / 256;
    int oldz = cz / 128;
    double dist_close = d;
    // look note before, should be same increment
    double dist_dec = 1.0 * 8;

    while (dist_close > dist_dec) {
        int nx = (int)sx / 256;
        int ny = (int)sy / 256;
        int nz = (int)sz / 128;
        unsigned char twd = mtsurfaces_[nx + ny * mmax_x_
            + nz * mmax_m_xy].twd;
        if (oldx != nx || oldy != ny || oldz != nz
            || (twd >= 0x01 && twd <= 0x04))
        {
            if (!(twd == 0x00 || twd == 0x0C || twd == 0x10)) {
                bool is_blocked = false;
                int offz = (int)sz % 128;
                switch (twd) {
                    case 0x01:
                        if (offz <= (127 - (((int)sy % 256) >> 1)))
                            is_blocked = true;
                        break;
                    case 0x02:
                        if (offz <= (((int)sy % 256) >> 1))
                            is_blocked = true;
                        break;
                    case 0x03:
                        if (offz <= (((int)sx % 256) >> 1))
                            is_blocked = true;
                        break;
                    case 0x04:
                        if (offz <= (127 - (((int)sx % 256) >> 1)))
                            is_blocked = true;
                        break;
                    default:
                        is_blocked = true;
                }
                if (is_blocked) {
                    sx -= inc_x;
                    sy -= inc_y;
                    sz -= inc_z;
                    double dsx = sx - (double)cx;
                    double dsy = sy - (double)cy;
                    double dsz = sz - (double)cz;
                    tx = (int)sx;
                    ty = (int)sy;
                    tz = (int)sz;
                    dist_close = sqrt(dsx * dsx + dsy * dsy + dsz * dsz);
                    if (block_mask == 1)
                        block_mask = 16;
                    else
                        block_mask |= 16;
                    if (setBlocker) {
                        if (pn) {
                            pn->setTileXYZ((int)sx / 256, (int)sy / 256,
                                (int)sz / 128);
                            pn->setOffXYZ((int)sx % 256, (int)sy % 256,
                                (int)sz % 128);
                        }
                        break;
                    }
                    break;
                }
            }
            oldx = nx;
            oldy = ny;
            oldz = nz;
        }
        sx += inc_x;
        sy += inc_y;
        sz += inc_z;
        dist_close -= dist_dec;
    }
    if (checkTileOnly)
        return block_mask;

    toDefineXYZ startXYZ = {cx, cy, cz};
    toDefineXYZ endXYZ = {tx, ty, tz};
    MapObject *blockerObj = NULL;

    // if blocker will exist it will be always closer then tile
    // that might block
    double dist_blocker = (block_mask & 16) != 0 ? dist_close : d;
    blockerExists(&startXYZ, &endXYZ, &dist_blocker, &blockerObj);

    if (blockerObj) {
        if (block_mask == 1)
            block_mask = 0;

        if (setBlocker) {
            if (pn) {
                pn->setTileXYZ(startXYZ.x / 256, startXYZ.y / 256,
                    startXYZ.z / 128);
                pn->setOffXYZ(startXYZ.x % 256, startXYZ.y % 256,
                    startXYZ.z % 128);
                block_mask |= 4;
            }
            if (t) {
                *t = (ShootableMapObject *)blockerObj;
                block_mask |= 2;
            }
        } else {
            if (t && *t) {
                if (*t != blockerObj)
                    block_mask |= 6;
                else
                    block_mask = 1;
            } else
                block_mask |= 6;
        }
    } else {
        if (setBlocker) {
            if (block_mask != 1 && t)
                *t = NULL;
        }
    }

    return block_mask;
}

/*!
 * Returns all objects of given type that are in the range of the given point.
 * Methods searches through those objects:
 * - Ped : must be not ignored and alive
 * - Static : must be not ignored
 * - Vehicle : must be not ignored
 * - Weapon : must be not ignored
 * \param cp The central point
 * \param targets The list of found objects
 * \param mask A mask to select the type of objects
 * \param checkTileOnly
 * \param maxr The range
 */
void Mission::getInRangeAll(toDefineXYZ * cp,
   std::vector<ShootableMapObject *> & targets, uint8 mask,
   bool checkTileOnly, double maxr)
{
    if (mask & MapObject::mjt_Ped) {
        for (int i = 0; i < numPeds(); ++i) {
            ShootableMapObject *p = ped(i);
            if (!p->isIgnored() && p->isAlive())
                if (inRangeCPos(cp, &p, NULL, false, checkTileOnly,
                    maxr) == 1)
                {
                    targets.push_back(p);
                }
        }
    }
    if (mask & MapObject::mjt_Static) {
        for (int i = 0; i < numStatics(); ++i) {
            ShootableMapObject *st = statics(i);
            if (!st->isIgnored())
                if (inRangeCPos(cp, &st, NULL, false, checkTileOnly,
                    maxr) == 1)
                {
                    targets.push_back(st);
                }
        }
    }
    if (mask & MapObject::mjt_Vehicle) {
        for (int i = 0; i < numVehicles(); ++i) {
            ShootableMapObject *v = vehicle(i);
            if (!v->isIgnored())
                if (inRangeCPos(cp, &v, NULL, false, checkTileOnly,
                    maxr) == 1)
                {
                    targets.push_back(v);
                }
        }
    }
    if (mask & MapObject::mjt_Weapon) {
        for (int i = 0; i < numWeapons(); ++i) {
            ShootableMapObject *w = weapon(i);
            if (!w->isIgnored())
                if (inRangeCPos(cp, &w, NULL, false, checkTileOnly,
                    maxr) == 1)
                {
                    targets.push_back(w);
                }
        }
    }
}

bool Mission::getShootableTile(int &x, int &y, int &z, int &ox, int &oy,
    int &oz)
{
    // TODO: review later
    bool gotit = false;
    int bx, by, box, boy;
    int bz = mmax_z_;
    unsigned char twd;
    unsigned int cindx;
    do {
        --bz;
        int bzm = bz - 1;
        bx = x * 256 + ox + 128 * bzm;
        box = bx % 256;
        bx = bx / 256;
        by = y * 256 + oy + 128 * bzm;
        boy = by % 256;
        by = by / 256;
        if (bz >= mmax_z_ || bx >= mmax_x_ || by >= mmax_y_)
            continue;
        if (bz < 0 || bx < 0 || by < 0)
            break;
        twd = mtsurfaces_[bx + by * mmax_x_ + bzm * mmax_m_xy].twd;
        int dx = 0;
        int dy = 0;
        switch (twd) {
            case 0x01:
                dy = (boy * 2) / 3;
                dx = box - dy / 2;
                if (dx >= 0) {
                    gotit = true;
                    box = dx;
                    boy = dy;
                } else {
                    if ((bx - 1) >= 0) {
                        cindx = (bx - 1) + by * mmax_x_ + bzm * mmax_m_xy;
                        if (mtsurfaces_[cindx].twd == 0x01) {
                            gotit = true;
                            --bx;
                            box = dx + 256;
                            boy = dy;
                        }
                    }
                }
                break;
            case 0x02:
                dy = (boy - 128) * 2;
                dx = (box + dy / 2) - 128;
                if (dy >= 0) {
                    if (dx >= 0) {
                        if (dx < 256) {
                            gotit = true;
                            box = dx;
                            boy = dy;
                        } else {
                            if ((bx + 1) < mmax_x_) {
                                cindx = (bx + 1) + by * mmax_x_ + bzm * mmax_m_xy;
                                if (mtsurfaces_[cindx].twd == 0x02) {
                                    gotit = true;
                                    ++bx;
                                    box = dx - 256;
                                    boy = dy;
                                }
                            }
                        }
                    } else {
                        if ((bx - 1) >= 0) {
                            cindx = (bx - 1) + by * mmax_x_ + bzm * mmax_m_xy;
                            if (mtsurfaces_[cindx].twd == 0x02) {
                                gotit = true;
                                --bx;
                                box = dx + 256;
                                boy = dy;
                            }
                        }
                    }
                }
                break;
            case 0x03:
                dx = (box - 128) * 2;
                dy = (boy + dx / 2) - 128;
                if (dx >= 0) {
                    if (dy >= 0) {
                        if (dy < 256) {
                            gotit = true;
                            box = dx;
                            boy = dy;
                        } else {
                            if ((by + 1) < mmax_y_) {
                                cindx = bx + (by + 1) * mmax_x_ + bzm * mmax_m_xy;
                                if (mtsurfaces_[cindx].twd == 0x03) {
                                    gotit = true;
                                    by++;
                                    box = dx;
                                    boy = dy - 256;
                                }
                            }
                        }
                    } else {
                        if ((by - 1) >= 0) {
                            cindx = bx + (by - 1) * mmax_x_ + bzm * mmax_m_xy;
                            if (mtsurfaces_[cindx].twd == 0x03) {
                                gotit = true;
                                --by;
                                box = dx;
                                boy = dy + 256;
                            }
                        }
                    }
                }
                break;
            case 0x04:
                dx = (box * 2) / 3;
                dy = boy - dx / 2;
                if (dy >= 0) {
                    gotit = true;
                    box = dx;
                    boy = dy;
                } else {
                    if ((by - 1) >= 0) {
                        cindx = bx + (by - 1) * mmax_x_ + bzm * mmax_m_xy;
                        if (mtsurfaces_[cindx].twd == 0x04) {
                            gotit = true;
                            --by;
                            box = dx;
                            boy = dy + 256;
                        }
                    }
                }
                break;
            default:
                if (!(twd == 0x00 || twd == 0x0C || twd == 0x10))
                    gotit = true;
            break;
        }
       if (!gotit) {
            if (box < 128 && (bx - 1) >= 0) {
                cindx = (bx - 1) + by * mmax_x_ + bzm * mmax_m_xy;
                twd = mtsurfaces_[cindx].twd;
                if (twd == 0x01) {
                    dy = (boy * 2) / 3;
                    dx = (box + 256) - dy / 2;
                    if (dx < 256) {
                        --bx;
                        gotit = true;
                        box = dx;
                        boy = dy;
                    }
                } else if (twd == 0x04) {
                    dx = ((box + 256) * 2) / 3;
                    dy = boy - dx / 2;
                    if (dy >= 0) {
                        --bx;
                        gotit = true;
                        box = dx;
                        boy = dy;
                    }
                }
            }
            if (!gotit && boy < 128 && (by - 1) >= 0) {
                cindx = bx + (by - 1) * mmax_x_ + bzm * mmax_m_xy;
                twd = mtsurfaces_[cindx].twd;
                if (twd == 0x01) {
                    dy = ((boy + 256) * 2) / 3;
                    dx = box - dy / 2;
                    if (dx >= 0) {
                        --by;
                        gotit = true;
                        box = dx;
                        boy = dy;
                    }
                } else if (twd == 0x04) {
                    dx = (box * 2) / 3;
                    dy = (boy + 256) - dx / 2;
                    if (dy < 256) {
                        --by;
                        gotit = true;
                        box = dx;
                        boy = dy;
                    }
                }
                if(!gotit && box < 128 && (bx - 1) >= 0) {
                    --cindx;
                    int dx2 = 0;
                    int dy2 = 0;
                    twd = mtsurfaces_[cindx].twd;
                    if (twd == 0x01) {
                        dy2 = ((boy + 256) * 2) / 3;
                        dx2 = (box + 256) - dy2 / 2;
                        if (dx2 < 256 && dy2 < 256) {
                            --bx;
                            --by;
                            gotit = true;
                            box = dx2;
                            boy = dy2;
                        }
                    } else if (twd == 0x04) {
                        dx2 = ((box + 256) * 2) / 3;
                        dy2 = (boy + 256) - dx2 / 2;
                        if (dx2 < 256 && dy2 < 256) {
                            --bx;
                            --by;
                            gotit = true;
                            box = dx2;
                            boy = dy2;
                        }
                    }
                }
            }
       }
    } while (bz != 0 && !gotit);
    if (gotit) {
        twd = mtsurfaces_[bx + by * mmax_x_ + (bz - 1) * mmax_m_xy].twd;
        switch (twd) {
            case 0x01:
                oz = 127 - (boy >> 1);
                --bz;
                break;
            case 0x02:
                oz = boy >> 1;
                --bz;
                break;
            case 0x03:
                oz = box >> 1;
                --bz;
                break;
            case 0x04:
                oz = 127 - (box >> 1);
                --bz;
                break;
            default:
                twd = mtsurfaces_[bx + by * mmax_x_ + bz * mmax_m_xy].twd;
                if (!(twd == 0x00 || twd == 0x0C || twd == 0x10)) {
                    // recalculating point of collision
                    if (box > 192 || boy > 192) {
                        if (box >= boy)
                            oz = (256 - box) << 1;
                        else if (boy > box)
                            oz = (256 - boy) << 1;
                    } else
                        oz = 128;

                    bx = x * 256 + ox + 128 * (bz - 1) + oz;
                    box = bx % 256;
                    bx = bx / 256;
                    by = y * 256 + oy + 128 * (bz - 1) + oz;
                    boy = by % 256;
                    by = by / 256;
                    bz += oz / 128;
                    oz %= 128;
                } else
                    oz = 0;
        }
        x = bx;
        y = by;
        z = bz;
        ox = box;
        oy = boy;
        assert(bz >= 0);
    }
    return gotit;
}

bool Mission::isTileSolid(int x, int y, int z, int ox, int oy, int oz) {
    bool solid = true;
    uint8 twd = mtsurfaces_[x + y * mmax_x_ + z * mmax_m_xy].twd;
    switch (twd) {
        case 0x00:
        case 0x0C:
        case 0x10:
            solid = false;
            break;
        case 0x01:
            if (oz > (127 - (oy >> 1)))
                solid = false;
            break;
        case 0x02:
            if (oz > (oy >> 1))
                solid = false;
            break;
        case 0x03:
            if (oz > (ox >> 1))
                solid = false;
            break;
        case 0x04:
            if (oz > (127 - (ox >> 1)))
                solid = false;
            break;
        default:
            break;
    }

    return solid;
}
