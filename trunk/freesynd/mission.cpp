/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
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

#include "screen.h"
#include "app.h"

Mission::Mission():map_(0), min_x_(0), min_y_(0), max_x_(0), max_y_(0), objective_(0),
objective_ped_(-1), objective_vehicle_(-1)
{
    memset(&level_data_, 0, sizeof(level_data_));
}

Mission::~Mission()
{
    for (unsigned int i = 0; i < vehicles_.size(); i++)
        delete vehicles_[i];
    for (unsigned int i = 0; i < peds_.size(); i++)
        delete peds_[i];
    for (unsigned int i = 0; i < weapons_.size(); i++)
        delete weapons_[i];
}

#define copydata(x, y) memcpy(&level_data_.x, levelData + y, sizeof(level_data_.x))

bool Mission::loadLevel(uint8 * levelData)
{
    copydata(u01, 0);
    copydata(map, 8);
    copydata(people, 32776);
    copydata(cars, 56328);
    copydata(statics, 59016);
    copydata(u06, 71016);
    copydata(u07, 97116);
    copydata(u08, 97128);
    copydata(u09, 113512);
    copydata(u10, 113949);
    copydata(u11, 114075);

#if 0
    uint8 *data = (uint8 *) & level_data_;
    for (int i = 6; i < 0x8000; i++)
        if (data[i] != 0) {
            printf("%04X: ", i);
            for (; data[i]; i++)
                printf("%02X ", data[i]);
            printf("\n");
        }
#endif

    //printf("%08x\n", (char*)&level_data_.u10[0].unkn10[11] - (char*)&level_data_);
    map_ = level_data_.u10.map[0] | (level_data_.u10.map[1] << 8);
    min_x_ =
        (level_data_.u10.minx[0] | (level_data_.u10.minx[1] << 8)) / 2;
    min_y_ =
        (level_data_.u10.miny[0] | (level_data_.u10.miny[1] << 8)) / 2;
    max_x_ =
        (level_data_.u10.maxx[0] | (level_data_.u10.maxx[1] << 8)) / 2 + 4;
    max_y_ =
        (level_data_.u10.maxy[0] | (level_data_.u10.maxy[1] << 8)) / 2 + 4;
    objective_ =
        level_data_.u10.objective[0] | (level_data_.u10.objective[1] << 8);
    int objective_data =
        level_data_.u10.objective_data[0] | (level_data_.u10.
                                             objective_data[1] << 8);

    /*
       mission 20    1 922 39a
       mission 5     1 1106 452    or 39a + 5c * 2
       mission 38    1 1106 452
       mission 27    1 1474 5c2    or 39a + 5c * 6
       mission 41    1 1474 5c2
       mission 12    1 1566 61e    or 39a + 5c * 7
       mission 16    1 2118 846
       mission 21    1 2946 b82
       mission 45    1 2946 b82
       mission 29    1 3682 e62
       mission 3     1 4786 12b2
       mission 39    1 14446 386e
       mission 49    1 16378 3ffa  or 39a + 5c * a8

       mission 1     2 738 2e2   same
       mission 10    2 738 2e2
       mission 14    2 830 33e
       mission 32    2 922 39a
       mission 17    2 1198 4ae
       mission 19    2 1106 452
       mission 31    2 1106 452
       mission 7     2 1474 5c2
       mission 36    2 1474 5c2
       mission 24    2 2210 8a2
       mission 33    2 2946 b82
       mission 4     2 5614 15ee
       mission 34    2 9570 2562
       mission 35    2 14538 38ca
       mission 6     2 14998 3a96

       mission 25    3 3498 daa
       mission 30    3 3682 e62
       mission 40    3 3682 e62

       mission 8     5 40258 9d42        so I guess this would correspond to some
       mission 11    5 38242 9562        weapon table, that I don't even know exists
       mission 13    5 38242 9562
       mission 26    5 38314 95aa

       mission 43    10 0 0

       mission 2     11 0 0
       mission 9     11 0 0
       mission 15    11 0 0
       mission 18    11 0 0
       mission 22    11 0 0
       mission 28    11 0 0
       mission 37    11 0 0
       mission 42    11 0 0
       mission 48    11 0 0
       mission 50    11 0 0

       mission 23    14 23680 5c80  3 * 2a = 7e + 2 = 80
       mission 46    14 23596 5c2c  the vehicle struct size is 2a, +2 = 2c
       mission 47    14 23596 5c2c  so what's the 5c?  surely it can't be related to the size of the ped struct..

       mission 44    15 23596 5c2c
     */

    // 1  = persuade
    // 2  = assassinate
    // 3  = protect
    // 5  = equipment aquisition
    // 10 = combat sweep (police)
    // 11 = combat sweep
    // 14 = raid and rescue
    // 15 = use vehicle
    //printf("%i %i %x\n", objective_, objective_data, objective_data);

    if (objective_ == 1 || objective_ == 2 || objective_ == 3)
        objective_ped_ = (objective_data - 2) / 0x5c;

    if (objective_ == 14 || objective_ == 15)
        objective_vehicle_ = ((objective_data & 0xff) - 2) / 0x2a;

    vehicles_.clear();          // TODO: free existing?

    for (int i = 0; i < 64; i++) {
        LEVELDATA_CARS & car = level_data_.cars[i];
        // lots of things can make a car not exist
        if (car.unkn3 != 4 || car.unkn6 != 6
            || (car.unkn12[0] == 122 && car.unkn12[1] == 122))
            continue;
        VehicleInstance *v =
            g_App.vehicles().loadInstance((uint8 *) & car, map_);
        if (v)
            vehicles_.push_back(v);
    }

    for (int i = 0; i < 256; i++) {
        LEVELDATA_PEOPLE & pedref = level_data_.people[i];
        if (pedref.unkn3 != 4)
            continue;
        PedInstance *p =
            g_App.peds().loadInstance((uint8 *) & pedref, map_);
        peds_.push_back(p);
        if (p) {
            if (p->isHostile()) {
                Weapon *w = g_App.weapons().findWeapon(Ped::Pistol);
                if (w) {
                    WeaponInstance *wi = w->createInstance();
                    weapons_.push_back(wi);
                    p->addWeapon(wi);
                }
            }
        }
    }

    for (unsigned int i = 0; i < 400; i++) {
        LEVELDATA_STATICS & sref = level_data_.statics[i];
        Static *s = Static::loadInstance((uint8 *) & sref, map_);
        if (s)
            statics_.push_back(s);
    }

#if 0
    std::map < int, int >markers;
    for (unsigned int j = 0; j < 128; j++) {
        for (unsigned int i = 0; i < 128; i++) {
            uint8 a = level_data_.map.objs[j][i][0];
            uint8 b = level_data_.map.objs[j][i][1];
            int t = (a << 8) | b;

            if (t == 0)
                printf(".");
            else {
                if (markers.find(t) == markers.end())
                    markers[t] = markers.size();
                printf("%c", markers[t] + 'a');
            }
        }
        printf("\n");
    }

    for (std::map < int, int >::iterator it = markers.begin();
         it != markers.end(); it++)
        printf("%c -> %04x\n", it->second + 'a', it->first);
#endif

    return true;
}

bool Mission::loadMission(uint8 * missData, int size)
{
    char *miss = (char *) missData;
    miss[size - 1] = 0;

    info_costs_[0] = info_costs_[1] = info_costs_[2] = 0;
    enhance_costs_[0] = enhance_costs_[1] = enhance_costs_[2] = 0;

    int i = 0;

    while (*miss != '|') {
        info_costs_[i++] = atoi(miss);
        miss = strchr(miss, '\n') + 1;
    }

    miss += 2;
    i = 0;

    while (*miss != '|') {
        enhance_costs_[i++] = atoi(miss);
        miss = strchr(miss, '\n') + 1;
    }

    miss += 2;

    briefing_ = "";
    if (miss) {
        briefing_ = miss;
    }
    return true;
}

bool Mission::loadMap()
{
    return g_App.maps().loadMap(map_);
}

int Mission::mapWidth()
{
    return g_App.maps().mapWidth(map_);
}

int Mission::mapHeight()
{
    return g_App.maps().mapHeight(map_);
}

int Mission::startX()
{
    int x =
        g_App.maps().tileToScreenX(map_, peds_[0]->tileX(),
                                   peds_[0]->tileY(), peds_[0]->tileZ(), 0,
                                   0);
    x -= (GAME_SCREEN_WIDTH - 129) / 2;
    if (x < 0)
        x = 0;
    return x;
}

int Mission::startY()
{
    int y =
        g_App.maps().tileToScreenY(map_, peds_[0]->tileX(),
                                   peds_[0]->tileY(), peds_[0]->tileZ(), 0,
                                   0);
    y -= GAME_SCREEN_HEIGHT / 2;
    if (y < 0)
        y = 0;
    return y;
}

int Mission::minScreenX()
{
    return g_App.maps().tileToScreenX(map_, min_x_, min_y_, 0, 0, 0);
}

int Mission::minScreenY()
{
    return g_App.maps().tileToScreenY(map_, min_x_, min_y_, 0, 0, 0);
}

int Mission::maxScreenX()
{
    return g_App.maps().tileToScreenX(map_, max_x_, max_y_, 0, 0, 0);
}

int Mission::maxScreenY()
{
    return g_App.maps().tileToScreenY(map_, max_x_, max_y_, 0, 0, 0);
}

int fastKey(int tx, int ty, int tz)
{
    return tx | (ty << 8) | (tz << 16);
}

int fastKey(MapObject * m)
{
    return fastKey(m->tileX(), m->tileY(), m->tileZ());
}

void Mission::drawMap(int scrollx, int scrolly)
{
    fast_vehicle_cache_.clear();
    for (unsigned int i = 0; i < vehicles_.size(); i++)
        fast_vehicle_cache_.insert(fastKey(vehicles_[i]));

    fast_ped_cache_.clear();
    for (unsigned int i = 0; i < 4; i++)
        if (g_App.teamMember(i)->isActive())
            fast_ped_cache_.insert(fastKey(peds_[i]));
    for (unsigned int i = 8; i < peds_.size(); i++)
        fast_ped_cache_.insert(fastKey(peds_[i]));

    fast_weapon_cache_.clear();
    for (unsigned int i = 0; i < weapons_.size(); i++)
        if (weapons_[i]->map() != -1)
            fast_weapon_cache_.insert(fastKey(weapons_[i]));

    fast_statics_cache_.clear();
    for (unsigned int i = 0; i < statics_.size(); i++)
        fast_statics_cache_.insert(fastKey(statics_[i]));

    g_App.maps().drawMap(map_, scrollx, scrolly, this);
}

void Mission::drawAt(int tilex, int tiley, int tilez, int x, int y,
                     int scrollX, int scrollY)
{
    int key = fastKey(tilex, tiley, tilez);

    if (fast_vehicle_cache_.find(key) != fast_vehicle_cache_.end()) {
        // draw vehicles
        for (unsigned int i = 0; i < vehicles_.size(); i++)
            if (vehicles_[i]->tileX() == tilex
                && vehicles_[i]->tileY() == tiley
                && vehicles_[i]->tileZ() == tilez)
                vehicles_[i]->draw(x, y);
    }

    if (fast_ped_cache_.find(key) != fast_ped_cache_.end()) {
        // draw agents
        for (unsigned int i = 0; i < 4; i++)
            if (g_App.teamMember(i)->isActive())
                if (peds_[i]->tileX() == tilex
                    && peds_[i]->tileY() == tiley
                    && peds_[i]->tileZ() == tilez) {
                    peds_[i]->draw(x, y, scrollX, scrollY);
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
        // draw peds
        for (unsigned int i = 8; i < peds_.size(); i++)
            if (peds_[i]->tileX() == tilex && peds_[i]->tileY() == tiley
                && peds_[i]->tileZ() == tilez)
                peds_[i]->draw(x, y, scrollX, scrollY);
    }

    if (fast_weapon_cache_.find(key) != fast_weapon_cache_.end()) {
        // draw weapons
        for (unsigned int i = 0; i < weapons_.size(); i++)
            if (weapons_[i]->map() != -1 && weapons_[i]->tileX() == tilex
                && weapons_[i]->tileY() == tiley
                && weapons_[i]->tileZ() == tilez) {
                weapons_[i]->draw(x, y);
            }
    }

    if (fast_statics_cache_.find(key) != fast_statics_cache_.end()) {
        // draw statics
        for (unsigned int i = 0; i < statics_.size(); i++)
            if (statics_[i]->tileX() == tilex
                && statics_[i]->tileY() == tiley
                && statics_[i]->tileZ() == tilez) {
                statics_[i]->draw(x, y);
            }
    }
}

void Mission::start()
{
    for (int i = 0; i < 4; i++)
        if (g_App.teamMember(i)) {
            peds_[i]->setHealth(g_App.teamMember(i)->health() *
                                peds_[i]->health() / 255);
            while (g_App.teamMember(i)->numWeapons()) {
                WeaponInstance *wi = g_App.teamMember(i)->removeWeapon(0);
                weapons_.push_back(wi);
                peds_[i]->addWeapon(wi);
            }
        } else {
            delete peds_[i];
            peds_[i] = 0;
        }
}

bool Mission::failed()
{
    // TODO: other ways to fail
    if (objective_ == 1 && peds_[objective_ped_]->health() <= 0)
        return true;
    return false;
}

bool Mission::completed()
{
    // TODO: other ways to complete
    if (objective_ == 2 && peds_[objective_ped_]->health() <= 0)
        return true;
    return false;
}

void Mission::end()
{
    for (int i = 0; i < 4; i++)
        if (g_App.teamMember(i)) {
            // TODO: kill agents if associated ped is dead
            while (peds_[i]->numWeapons()) {
                WeaponInstance *wi = peds_[i]->removeWeapon(0);
                std::vector < WeaponInstance * >::iterator it =
                    weapons_.begin();
                while (it != weapons_.end() && *it != wi)
                    it++;
                assert(it != weapons_.end());
                weapons_.erase(it);
                g_App.teamMember(i)->addWeapon(wi);
            }
        }
}

void Mission::addWeapon(WeaponInstance * w)
{
    w->setMap(map_);
    for (unsigned int i = 0; i < weapons_.size(); i++)
        if (weapons_[i] == w)
            return;
    weapons_.push_back(w);
}
