/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
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

#include "common.h"
#include "mission.h"
#include "ped.h"
#include "pathsurfaces.h"

#if 0
#include "SDL.h"
#define EXECUTION_SPEED_TIME
#endif

void PedInstance::setDestinationP(Mission *m, int x, int y, int z,
                                    int ox, int oy)
{
    // NOTE: this is a "flood" algorithm, it expands until it reaches other's
    // flood point, then it removes unrelated points
#ifdef EXECUTION_SPEED_TIME
    printf("---------------------------");
    printf("start time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif
    m->adjXYZ(x, y, z);
    dest_path_.clear();

    if (map_ == -1 || health_ <= 0)
        return;

    floodPointDesc *targetd = &(m->mdpoints_[x + y * m->mmax_x_ + z * m->mmax_m_xy]);

    floodPointDesc *based = &(m->mdpoints_[tile_x_
        + tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);

#if 0
#if _DEBUG
    printf("target t %x, dirm %x ; base t %x, dirm %x\n", targetd->t,
        targetd->dirm, based->t, based->dirm);
    printf("target dirh %x, dirl %x ; base dirh %x, dirl %x\n", targetd->dirh,
        targetd->dirl, based->dirh, based->dirl);
    printf("ttwd %X \n",m->mtsurfaces_[x + y * m->mmax_x_
        + z * m->mmax_m_xy].twd);
    printf("target pos: x %i; y %i; z %i, ox %i, oy %i\n",
        x, y, z, ox, oy);
    //printf("tileAt %x\n", g_App.maps().map(map())->tileAt(x,y,z));
    printf("base pos: x %i; y %i; z %i, ox %i, oy %i, oz %i\n",
        tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
    printf("zmax %x\n", m->mmax_z_);
    if ((z - 1)>= 0) {
        printf("lower twd %i\n", m->mtsurfaces_[x + y * m->mmax_x_
            + (z - 1) * m->mmax_m_xy].twd);
    }
#endif
#endif

    //return;
    if(targetd->t == m_fdNonWalkable || map_ == -1 || health_ <= 0) {
        printf("==== unwalk target: x %i; y %i; z %i, ox %i, oy %i\n",
            x, y, z, ox, oy);
        printf("Movement to nonwalkable postion\n");
        return;
    }

    if(based->t == m_fdNonWalkable) {
        printf("==== unwalk pos: x %i; y %i; z %i, ox %i, oy %i, oz %i\n",
            tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
        printf("Movement from nonwalkable postion\n");
        return;
    }

    if (tile_x_ == x && tile_y_ == y && tile_z_ == z) {
        dest_path_.push_back(PathNode(x, y, z, ox, oy));
        return;
    }
#ifdef EXECUTION_SPEED_TIME
    printf("directions-map copy start %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif
    floodPointDesc *mdpmirror = m->mdpoints_cp_;
    memcpy((void *)mdpmirror, (void *)m->mdpoints_,
        m->mmax_m_all * sizeof(floodPointDesc));

#ifdef EXECUTION_SPEED_TIME
    printf("directions-map copy complete %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif

    unsigned char lt;
    unsigned short blvl = 0, tlvl = 0;
    // these are all tiles that belong to base and target
    std::vector <toSetDesc> bv;
    std::vector <toSetDesc> tv;
    bv.reserve(8192);
    tv.reserve(8192);
    // these are used for setting values through algorithm
    toSetDesc sadd;
    floodPointDesc *pfdp;
    // setup
    pfdp = &(mdpmirror[tile_x_ + tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
    pfdp->t |= m_fdBasePoint;
    sadd.coords.x = tile_x_;
    sadd.coords.y = tile_y_;
    sadd.coords.z = tile_z_;
    sadd.p = pfdp;
    bv.push_back(sadd);
    pfdp = &(mdpmirror[x + y * m->mmax_x_ + z * m->mmax_m_xy]);
    pfdp->t |= (m_fdTargetPoint | m_fdConstant);
    sadd.coords.x = x;
    sadd.coords.y = y;
    sadd.coords.z = z;
    sadd.p = pfdp;
    tv.push_back(sadd);
    // for setting lvls data
    lvlNodesDesc ladd;
    ladd.indxs = 0;
    ladd.n = 1;
    // these are number of nodes per lvl and index start for "bv" and "tv"
    std::vector <lvlNodesDesc> bn;
    std::vector <lvlNodesDesc> tn;
    bn.reserve(512);
    tn.reserve(512);
    bn.push_back(ladd);
    tn.push_back(ladd);
    bool nodeset, lnknr = true;
#ifdef EXECUTION_SPEED_TIME
    printf("data allocation/setup complete %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif

#ifdef FIND_DEFINED_TILE
    bool assertion_bool = true;
    int x_check = 48, y_check = 23, z_check = 6;
#endif
    do {
        unsigned short mindx = bn[blvl].indxs + bn[blvl].n;
        unsigned short nlvl = blvl + 1;
        unsigned int cindx = 0;
        for (unsigned short i = bn[blvl].indxs; i < mindx; i++) {
            toSetDesc bref = bv[i];
            cindx = bref.coords.x + bref.coords.y * m->mmax_x_
                + bref.coords.z * m->mmax_m_xy;
            if (bref.p->dirh != 0) {
                if ((bref.p->dirh & 0x01) == 0x01) {
                    sadd.p = &(mdpmirror[cindx + m->mmax_x_ + m->mmax_m_xy]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z + 1;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirh & 0x04) == 0x04) {
                    sadd.p = &(mdpmirror[cindx + 1 + m->mmax_m_xy]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z + 1;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirh & 0x10) == 0x10) {
                    sadd.p = &(mdpmirror[cindx - m->mmax_x_ + m->mmax_m_xy]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z + 1;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirh & 0x40) == 0x40) {
                    sadd.p = &(mdpmirror[cindx - 1 + m->mmax_m_xy]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z + 1;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lnknr = false;
                    }
                }
            }
            if (bref.p->dirl != 0) {
                if ((bref.p->dirl & 0x01) == 0x01) {
                    sadd.p = &(mdpmirror[cindx + m->mmax_x_ - m->mmax_m_xy]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z - 1;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirl & 0x04) == 0x04) {
                    sadd.p = &(mdpmirror[cindx + 1 - m->mmax_m_xy]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z - 1;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirl & 0x10) == 0x10) {
                    sadd.p = &(mdpmirror[cindx - m->mmax_x_ - m->mmax_m_xy]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z - 1;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirl & 0x40) == 0x40) {
                    sadd.p = &(mdpmirror[cindx - 1 - m->mmax_m_xy]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z - 1;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lnknr = false;
                    }
                }
            }
            if (bref.p->dirm != 0) {
                if ((bref.p->dirm & 0x01) == 0x01) {
                    sadd.p = &(mdpmirror[cindx + m->mmax_x_]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirm & 0x02) == 0x02) {
                    sadd.p = &(mdpmirror[cindx + 1 + m->mmax_x_]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirm & 0x04) == 0x04) {
                    sadd.p = &(mdpmirror[cindx + 1]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirm & 0x08) == 0x08) {
                    sadd.p = &(mdpmirror[cindx + 1 - m->mmax_x_]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirm & 0x10) == 0x10) {
                    sadd.p = &(mdpmirror[cindx - m->mmax_x_]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirm & 0x20) == 0x20) {
                    sadd.p = &(mdpmirror[cindx - 1 - m->mmax_x_]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirm & 0x40) == 0x40) {
                    sadd.p = &(mdpmirror[cindx - 1]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirm & 0x80) == 0x80) {
                    sadd.p = &(mdpmirror[cindx - 1 + m->mmax_x_]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdBasePoint;
                        bv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdTargetPoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdBasePoint;
                        lnknr = false;
                    }
                }
            }
        }
        ladd.indxs = mindx;
        ladd.n = bv.size() - mindx;
        if (ladd.n > 0) {
            nodeset = true;
            bn.push_back(ladd);
            blvl++;
        } else {
            nodeset = false;
            break;
        }
        if (!lnknr)
            break;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
        mindx = tn[tlvl].indxs + tn[tlvl].n;
        nlvl = tlvl + 1;
        for (unsigned short i = tn[tlvl].indxs; i < mindx; i++) {
            toSetDesc bref = tv[i];
            cindx = bref.coords.x + bref.coords.y * m->mmax_x_
                + bref.coords.z * m->mmax_m_xy;
            if (bref.p->dirh != 0) {
                if ((bref.p->dirh & 0x01) == 0x01) {
                    sadd.p = &(mdpmirror[cindx + m->mmax_x_ + m->mmax_m_xy]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z + 1;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirh & 0x04) == 0x04) {
                    sadd.p = &(mdpmirror[cindx + 1 + m->mmax_m_xy]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z + 1;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirh & 0x10) == 0x10) {
                    sadd.p = &(mdpmirror[cindx - m->mmax_x_ + m->mmax_m_xy]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z + 1;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirh & 0x40) == 0x40) {
                    sadd.p = &(mdpmirror[cindx - 1 + m->mmax_m_xy]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z + 1;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lnknr = false;
                    }
                }
            }
            if (bref.p->dirl != 0) {
                if ((bref.p->dirl & 0x01) == 0x01) {
                    sadd.p = &(mdpmirror[cindx + m->mmax_x_ - m->mmax_m_xy]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z - 1;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirl & 0x04) == 0x04) {
                    sadd.p = &(mdpmirror[cindx + 1 - m->mmax_m_xy]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z - 1;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirl & 0x10) == 0x10) {
                    sadd.p = &(mdpmirror[cindx - m->mmax_x_ - m->mmax_m_xy]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z - 1;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirl & 0x40) == 0x40) {
                    sadd.p = &(mdpmirror[cindx - 1 - m->mmax_m_xy]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z - 1;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lnknr = false;
                    }
                }
            }
            if (bref.p->dirm != 0) {
                if ((bref.p->dirm & 0x01) == 0x01) {
                    sadd.p = &(mdpmirror[cindx + m->mmax_x_]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirm & 0x02) == 0x02) {
                    sadd.p = &(mdpmirror[cindx + 1 + m->mmax_x_]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirm & 0x04) == 0x04) {
                    sadd.p = &(mdpmirror[cindx + 1]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirm & 0x08) == 0x08) {
                    sadd.p = &(mdpmirror[cindx + 1 - m->mmax_x_]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirm & 0x10) == 0x10) {
                    sadd.p = &(mdpmirror[cindx - m->mmax_x_]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0) {
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirm & 0x20) == 0x20) {
                    sadd.p = &(mdpmirror[cindx - 1 - m->mmax_x_]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirm & 0x40) == 0x40) {
                    sadd.p = &(mdpmirror[cindx - 1]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lnknr = false;
                    }
                }
                if ((bref.p->dirm & 0x80) == 0x80) {
                    sadd.p = &(mdpmirror[cindx - 1 + m->mmax_x_]);
                    if ((sadd.p->t & (m_fdWalkable | m_fdBasePoint | m_fdTargetPoint)) == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z;
#ifdef FIND_DEFINED_TILE
                        if (sadd.coords.x == x_check && sadd.coords.y == y_check
                            && sadd.coords.z == z_check)
                            assert(assertion_bool);
#endif
                        sadd.p->lvl = nlvl;
                        sadd.p->t |= m_fdTargetPoint;
                        tv.push_back(sadd);
                    } else if ((sadd.p->t & m_fdBasePoint) != 0){
                        bref.p->t |= m_fdLink;
                        sadd.p->t |= m_fdLink;
                        lt = m_fdTargetPoint;
                        lnknr = false;
                    }
                }
            }
        }
        ladd.indxs = mindx;
        ladd.n = tv.size() - mindx;
        if (ladd.n > 0) {
            nodeset = true;
            tn.push_back(ladd);
            tlvl++;
        } else {
            nodeset = false;
            break;
        }
    } while (lnknr);
    //printf("bv %i, tv %i\n", bv.size(), tv.size());
#ifdef EXECUTION_SPEED_TIME
    printf("blvl %i, tlvl %i\n",tlvl, blvl);
    printf("target reached in %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif
    if (!nodeset && lnknr) {
        return;
    }
    if (blvl == bn.size())
        blvl--;
    if (tlvl == tn.size())
        tlvl--;
    // when link is set data of nlvl is useless, that is why it is removed
    if (nodeset) {
        if (lt == m_fdBasePoint) {
            unsigned short n = bn[blvl].n;
            std::vector <toSetDesc>::iterator it = bv.begin() + bn[blvl].indxs;
            for (unsigned short i = 0; i < n; i++) {
                it->p->t ^= m_fdBasePoint;
                it->p->lvl = 0;
                //bv.erase(it);
                it++;
            }
            //bn.pop_back();
            blvl--;
        } else {
            unsigned short n = tn[tlvl].n;
            std::vector <toSetDesc>::iterator it = tv.begin() + tn[tlvl].indxs;
            for (unsigned short i = 0; i < n; i++) {
                it->p->t ^= m_fdTargetPoint;
                it->p->lvl = 0;
                //tv.erase(it);
                it++;
            }
            //tn.pop_back();
            tlvl--;
        }
    }

    // level which created link have also non-link tiles they are useless
    if (blvl != 0) {
        unsigned short n = bn[blvl].n;
        unsigned short nr = 0;
        std::vector <toSetDesc>::iterator it = bv.begin() + bn[blvl].indxs;
        for (unsigned short i = 0; i < n; i++) {
            if ((it->p->t & m_fdLink) == 0) {
                it->p->t ^= m_fdBasePoint;
                it->p->lvl = 0;
                //bv.erase(it);
                nr++;
            }
            it++;
        }
        bn[blvl].n -= nr;
    }

    if (tlvl != 0) {
        unsigned short n = tn[tlvl].n;
        unsigned short nr = 0;
        std::vector <toSetDesc>::iterator it = tv.begin() + tn[tlvl].indxs;
        for (unsigned short i = 0; i < n; i++) {
            if ((it->p->t & m_fdLink) == 0) {
                it->p->t ^= m_fdTargetPoint;
                it->p->lvl = 0;
                //tv.erase(it);
                nr++;
            }
            it++;
        }
        tn[tlvl].n -= nr;
    }
    //printf("bv %i, tv %i\n", bv.size(), tv.size());
#ifdef EXECUTION_SPEED_TIME
    printf("tops removed time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif

    // tiles that have no childs are removed
    if (blvl > 1) {
        blvl--;
        unsigned short indx = bn[blvl].indxs + bn[blvl].n;
        indx--;
        do {
            toSetDesc &bref = bv[indx];
            uint16 lvl_child = (bref.p->lvl + 1);
            bool remv = true;
            if (bref.p->dirh != 0) {
                if ((bref.p->dirh & 0x01) == 0x01) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y + 1) * m->mmax_x_
                        + (bref.coords.z + 1) * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirh & 0x04) == 0x04) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + bref.coords.y * m->mmax_x_
                        + (bref.coords.z + 1) * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirh & 0x10) == 0x10) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y - 1) * m->mmax_x_
                        + (bref.coords.z + 1) * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirh & 0x40) == 0x40) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + bref.coords.y * m->mmax_x_
                        + (bref.coords.z + 1) * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
            }
            if (bref.p->dirl != 0) {
                if ((bref.p->dirl & 0x01) == 0x01) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y + 1) * m->mmax_x_
                        + (bref.coords.z - 1) * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirl & 0x04) == 0x04) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + bref.coords.y * m->mmax_x_
                        + (bref.coords.z - 1) * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirl & 0x10) == 0x10) {
                    pfdp = &(mdpmirror[(bref.coords.x)
                        + (bref.coords.y - 1) * m->mmax_x_
                        + (bref.coords.z - 1) * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirl & 0x40) == 0x40) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + bref.coords.y * m->mmax_x_
                        + (bref.coords.z - 1) * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
            }
            if (bref.p->dirm != 0) {
                if ((bref.p->dirm & 0x01) == 0x01) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y + 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x02) == 0x02) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + (bref.coords.y + 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x04) == 0x04) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + bref.coords.y * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x08) == 0x08) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + (bref.coords.y - 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x10) == 0x10) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y - 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x20) == 0x20) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + (bref.coords.y - 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x40) == 0x40) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + bref.coords.y * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x80) == 0x80) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + (bref.coords.y + 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
            }
            if (remv) {
                bref.p->t ^= m_fdBasePoint;
                bref.p->lvl = 0;
                //bv.erase(it);
            }
            indx--;
        } while(indx != 0);
    }
    if (tlvl > 1) {
        tlvl--;
        unsigned short indx = tn[tlvl].indxs + tn[tlvl].n;
        indx--;
        do {
            toSetDesc &bref = tv[indx];
            uint16 lvl_child = (bref.p->lvl + 1);
            bool remv = true;
            if (bref.p->dirh != 0) {
                if ((bref.p->dirh & 0x01) == 0x01) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y + 1) * m->mmax_x_
                        + (bref.coords.z + 1) * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirh & 0x04) == 0x04) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + bref.coords.y * m->mmax_x_
                        + (bref.coords.z + 1) * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirh & 0x10) == 0x10) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y - 1) * m->mmax_x_
                        + (bref.coords.z + 1) * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirh & 0x40) == 0x40) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + bref.coords.y * m->mmax_x_
                        + (bref.coords.z + 1) * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
            }
            if (bref.p->dirl != 0) {
                if ((bref.p->dirl & 0x01) == 0x01) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y + 1) * m->mmax_x_
                        + (bref.coords.z - 1) * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirl & 0x04) == 0x04) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + bref.coords.y * m->mmax_x_
                        + (bref.coords.z - 1) * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirl & 0x10) == 0x10) {
                    pfdp = &(mdpmirror[(bref.coords.x)
                        + (bref.coords.y - 1) * m->mmax_x_
                        + (bref.coords.z - 1) * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirl & 0x40) == 0x40) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + bref.coords.y * m->mmax_x_
                        + (bref.coords.z - 1) * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
            }
            if (bref.p->dirm != 0) {
                if ((bref.p->dirm & 0x01) == 0x01) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y + 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x02) == 0x02) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + (bref.coords.y + 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x04) == 0x04) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + bref.coords.y * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x08) == 0x08) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + (bref.coords.y - 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x10) == 0x10) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y - 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x20) == 0x20) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + (bref.coords.y - 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x40) == 0x40) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + bref.coords.y * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x80) == 0x80) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + (bref.coords.y + 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if (lvl_child == pfdp->lvl)
                        remv = false;
                }
            }
            if (remv) {
                bref.p->t ^= m_fdTargetPoint;
                bref.p->lvl = 0;
                //tv.erase(it);
            }
            indx--;
        } while(indx != 0);
    }
    //printf("bv %i, tv %i\n", bv.size(), tv.size());
#ifdef EXECUTION_SPEED_TIME
    printf("non-related removed time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif
#if 0
    bn.clear();
    tn.clear();
    bv.clear();
    tv.clear();
#endif

    // path is created here
    toDefineXYZ ctile;
    ctile.x = tile_x_;
    ctile.y = tile_y_;
    ctile.z = tile_z_;
    unsigned char ct = m_fdBasePoint;
    bool tnr = true, np = true;
    std::vector<PathNode> cdestpath;
    cdestpath.reserve(256);
    do {
        unsigned char nt = ct;
        toDefineXYZ toadd;
        char dist = 5;
        pfdp = &(mdpmirror[ctile.x + ctile.y * m->mmax_x_
                    + ctile.z * m->mmax_m_xy]);
        uint16 lvl_child = ct == m_fdBasePoint ? pfdp->lvl + 1
            : pfdp->lvl - 1;
        if (pfdp->dirh != 0) {
            if ((pfdp->dirh & 0x01) == 0x01) {
                sadd.coords.x = ctile.x;
                sadd.coords.y = ctile.y + 1;
                sadd.coords.z = ctile.z + 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if (lvl_child == sadd.p->lvl) {
                        if (3 < dist) {
                            toadd = sadd.coords;
                            dist = 3;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    if (0 < dist) {
                        nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                        dist = 0;
                        toadd = sadd.coords;
                    }
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirh & 0x04) == 0x04) {
                sadd.coords.x = ctile.x + 1;
                sadd.coords.y = ctile.y;
                sadd.coords.z = ctile.z + 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if (lvl_child == sadd.p->lvl) {
                        if (3 < dist) {
                            toadd = sadd.coords;
                            dist = 3;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    if (0 < dist) {
                        nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                        dist = 0;
                        toadd = sadd.coords;
                    }
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirh & 0x10) == 0x10) {
                sadd.coords.x = ctile.x;
                sadd.coords.y = ctile.y - 1;
                sadd.coords.z = ctile.z + 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if (lvl_child == sadd.p->lvl) {
                        if (3 < dist) {
                            toadd = sadd.coords;
                            dist = 3;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    if (0 < dist) {
                        nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                        dist = 0;
                        toadd = sadd.coords;
                    }
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirh & 0x40) == 0x40) {
                sadd.coords.x = ctile.x - 1;
                sadd.coords.y = ctile.y;
                sadd.coords.z = ctile.z + 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if (lvl_child == sadd.p->lvl) {
                        if (3 < dist) {
                            toadd = sadd.coords;
                            dist = 3;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    if (0 < dist) {
                        nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                        dist = 0;
                        toadd = sadd.coords;
                    }
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
        }
        if (pfdp->dirl != 0) {
            if ((pfdp->dirl & 0x01) == 0x01) {
                sadd.coords.x = ctile.x;
                sadd.coords.y = ctile.y + 1;
                sadd.coords.z = ctile.z - 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if (lvl_child == sadd.p->lvl) {
                        if (3 < dist) {
                            toadd = sadd.coords;
                            dist = 3;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    if (0 < dist) {
                        nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                        dist = 0;
                        toadd = sadd.coords;
                    }
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirl & 0x04) == 0x04) {
                sadd.coords.x = ctile.x + 1;
                sadd.coords.y = ctile.y;
                sadd.coords.z = ctile.z - 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if (lvl_child == sadd.p->lvl) {
                        if (3 < dist) {
                            toadd = sadd.coords;
                            dist = 3;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    if (0 < dist) {
                        nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                        dist = 0;
                        toadd = sadd.coords;
                    }
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirl & 0x10) == 0x10) {
                sadd.coords.x = ctile.x;
                sadd.coords.y = ctile.y - 1;
                sadd.coords.z = ctile.z - 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if (lvl_child == sadd.p->lvl) {
                        if (3 < dist) {
                            toadd = sadd.coords;
                            dist = 3;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    if (0 < dist) {
                        nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                        dist = 0;
                        toadd = sadd.coords;
                    }
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirl & 0x40) == 0x40) {
                sadd.coords.x = ctile.x - 1;
                sadd.coords.y = ctile.y;
                sadd.coords.z = ctile.z - 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if (lvl_child == sadd.p->lvl) {
                        if (3 < dist) {
                            toadd = sadd.coords;
                            dist = 3;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    if (0 < dist) {
                        nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                        dist = 0;
                        toadd = sadd.coords;
                    }
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
        }
        if (pfdp->dirm != 0) {
            if ((pfdp->dirm & 0x01) == 0x01) {
                sadd.coords.x = ctile.x;
                sadd.coords.y = ctile.y + 1;
                sadd.coords.z = ctile.z;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if (lvl_child == sadd.p->lvl) {
                        unsigned char twd = m->mtsurfaces_[sadd.coords.x
                            + sadd.coords.y * m->mmax_x_
                            + sadd.coords.z * m->mmax_m_xy].twd;
                        if (twd > 0x00 && twd < 0x05) {
                            if (3 < dist) {
                                toadd = sadd.coords;
                                dist = 3;
                            }
                        } else {
                            if (1 < dist) {
                                toadd = sadd.coords;
                                dist = 1;
                            }
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    unsigned char twd = m->mtsurfaces_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy].twd;
                    if (twd > 0x00 && twd < 0x05) {
                        if (-1 < dist) {
                            nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                            dist = -1;
                            toadd = sadd.coords;
                        }
                    } else {
                        if (-2 < dist) {
                            nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                            dist = -2;
                            toadd = sadd.coords;
                        }
                    }
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirm & 0x02) == 0x02) {
                sadd.coords.x = ctile.x + 1;
                sadd.coords.y = ctile.y + 1;
                sadd.coords.z = ctile.z;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if (lvl_child == sadd.p->lvl) {
                        if (2 < dist) {
                            toadd = sadd.coords;
                            dist = 2;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    if (-1 < dist) {
                        nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                        dist = -1;
                        toadd = sadd.coords;
                    }
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirm & 0x04) == 0x04) {
                sadd.coords.x = ctile.x + 1;
                sadd.coords.y = ctile.y;
                sadd.coords.z = ctile.z;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if (lvl_child== sadd.p->lvl) {
                        unsigned char twd = m->mtsurfaces_[sadd.coords.x
                            + sadd.coords.y * m->mmax_x_
                            + sadd.coords.z * m->mmax_m_xy].twd;
                        if (twd > 0x00 && twd < 0x05) {
                            if (3 < dist) {
                                toadd = sadd.coords;
                                dist = 3;
                            }
                        } else {
                            if (1 < dist) {
                                toadd = sadd.coords;
                                dist = 1;
                            }
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    unsigned char twd = m->mtsurfaces_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy].twd;
                    if (twd > 0x00 && twd < 0x05) {
                        if (-1 < dist) {
                            nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                            dist = -1;
                            toadd = sadd.coords;
                        }
                    } else {
                        if (-2 < dist) {
                            nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                            dist = -2;
                            toadd = sadd.coords;
                        }
                    }
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirm & 0x08) == 0x08) {
                sadd.coords.x = ctile.x + 1;
                sadd.coords.y = ctile.y - 1;
                sadd.coords.z = ctile.z;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if (lvl_child == sadd.p->lvl) {
                        if (2 < dist) {
                            toadd = sadd.coords;
                            dist = 2;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    if (-1 < dist) {
                        nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                        dist = -1;
                        toadd = sadd.coords;
                    }
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirm & 0x10) == 0x10) {
                sadd.coords.x = ctile.x;
                sadd.coords.y = ctile.y - 1;
                sadd.coords.z = ctile.z;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if (lvl_child == sadd.p->lvl) {
                        unsigned char twd = m->mtsurfaces_[sadd.coords.x
                            + sadd.coords.y * m->mmax_x_
                            + sadd.coords.z * m->mmax_m_xy].twd;
                        if (twd > 0x00 && twd < 0x05) {
                            if (3 < dist) {
                                toadd = sadd.coords;
                                dist = 3;
                            }
                        } else {
                            if (1 < dist) {
                                toadd = sadd.coords;
                                dist = 1;
                            }
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    unsigned char twd = m->mtsurfaces_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy].twd;
                    if (twd > 0x00 && twd < 0x05) {
                        if (-1 < dist) {
                            nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                            dist = -1;
                            toadd = sadd.coords;
                        }
                    } else {
                        if (-2 < dist) {
                            nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                            dist = -2;
                            toadd = sadd.coords;
                        }
                    }
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirm & 0x20) == 0x20) {
                sadd.coords.x = ctile.x - 1;
                sadd.coords.y = ctile.y - 1;
                sadd.coords.z = ctile.z;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if (lvl_child == sadd.p->lvl) {
                        if (2 < dist) {
                            toadd = sadd.coords;
                            dist = 2;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    if (-1 < dist) {
                        nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                        dist = -1;
                        toadd = sadd.coords;
                    }
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirm & 0x40) == 0x40) {
                sadd.coords.x = ctile.x - 1;
                sadd.coords.y = ctile.y;
                sadd.coords.z = ctile.z;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if (lvl_child == sadd.p->lvl) {
                        unsigned char twd = m->mtsurfaces_[sadd.coords.x
                            + sadd.coords.y * m->mmax_x_
                            + sadd.coords.z * m->mmax_m_xy].twd;
                        if (twd > 0x00 && twd < 0x05) {
                            if (3 < dist) {
                                toadd = sadd.coords;
                                dist = 3;
                            }
                        } else {
                            if (1 < dist) {
                                toadd = sadd.coords;
                                dist = 1;
                            }
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    unsigned char twd = m->mtsurfaces_[sadd.coords.x
                        + sadd.coords.y * m->mmax_x_
                        + sadd.coords.z * m->mmax_m_xy].twd;
                    if (twd > 0x00 && twd < 0x05) {
                        if (-1 < dist) {
                            nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                            dist = -1;
                            toadd = sadd.coords;
                        }
                    } else {
                        if (-2 < dist) {
                            nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                            dist = -2;
                            toadd = sadd.coords;
                        }
                    }
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
            if ((pfdp->dirm & 0x80) == 0x80) {
                sadd.coords.x = ctile.x - 1;
                sadd.coords.y = ctile.y + 1;
                sadd.coords.z = ctile.z;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if (lvl_child == sadd.p->lvl) {
                        if (2 < dist) {
                            toadd = sadd.coords;
                            dist = 2;
                        }
                    }
                } else if(np && (sadd.p->t & (m_fdBasePoint | m_fdTargetPoint)) != 0) {
                    if (-1 < dist) {
                        nt = sadd.p->t & (m_fdBasePoint | m_fdTargetPoint);
                        dist = -1;
                        toadd = sadd.coords;
                    }
                }
                if ((sadd.p->t & m_fdConstant) != 0)
                    tnr = false;
            }
        }
        if (dist < 1) {
            np = false;
            ct = nt;
        }
        cdestpath.push_back(PathNode(toadd.x, toadd.y, toadd.z));
        // this assert might save from memory fill up,
        assert(ctile.x != toadd.x || ctile.y != toadd.y || ctile.z != toadd.z);
        //if(toadd.x == 49 && toadd.y == 86 && toadd.z == 1)
            //toadd.x = 49;
        //if(ctile.x == toadd.x && ctile.y == toadd.y && ctile.z == toadd.z)
            //ctile = toadd;
        ctile = toadd;
    } while (tnr);
#ifdef EXECUTION_SPEED_TIME
    printf("path creation time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif

    // stairs to surface, surface to stairs correction
    if (cdestpath.size() != 0) {
        PathNode prvpn = PathNode(tile_x_, tile_y_, tile_z_, off_x_, off_y_);
        // TODO: use these to smoother path. later
        //std::vector<PathNode> vsingleway;
        //unsigned char mdir = 0xFF;
        //unsigned char hl = 0xFF;
        for (std::vector <PathNode>::iterator it = cdestpath.begin();
            it != cdestpath.end(); it++) {
            std::vector <PathNode>::iterator fit = it + 1;
            bool modified = false;
            unsigned char twd = m->mtsurfaces_[prvpn.tileX()
                + prvpn.tileY() * m->mmax_x_
                + prvpn.tileZ() * m->mmax_m_xy].twd;
            unsigned char twdn = m->mtsurfaces_[it->tileX()
                + it->tileY() * m->mmax_x_
                + it->tileZ() * m->mmax_m_xy].twd;
            char xf = prvpn.tileX() - it->tileX();
            char yf = prvpn.tileY() - it->tileY();
            char zf = prvpn.tileZ() - it->tileZ();
            // TODO: check offsets
            if (twd > 0x0 && twd < 0x05) {
                if (twdn > 0x0 && twdn < 0x05) {
                    dest_path_.push_back(*it);
                } else {
                    switch (twd) {
                        case 0x01:
                            if (zf == -1) {
                                if (xf == 0) {
                                    dest_path_.push_back(*it);
                                    break;
                                }
                                if (xf == 1) {
                                    prvpn.setOffXY(0,0);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(255,0);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                                if (xf == -1) {
                                    prvpn.setOffXY(255,0);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,0);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                            }
                            if (zf == 0) {
                                if (xf == 0) {
                                    dest_path_.push_back(*it);
                                    break;
                                }
                                if (xf == 1) {
                                    prvpn.setOffXY(0,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(255,255);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                                if (xf == -1) {
                                    prvpn.setOffXY(255,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,255);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                            }
                            if (zf == 1)
                                dest_path_.push_back(*it);
                            break;
                        case 0x02:
                            if (zf == -1) {
                                if (xf == 0) {
                                    dest_path_.push_back(*it);
                                    break;
                                }
                                if (xf == 1) {
                                    prvpn.setOffXY(0,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(255,255);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                                if (xf == -1) {
                                    prvpn.setOffXY(255,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,255);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                            }
                            if (zf == 0) {
                                if (xf == 0) {
                                    dest_path_.push_back(*it);
                                    break;
                                }
                                if (xf == 1) {
                                    prvpn.setOffXY(0,0);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(255,0);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                                if (xf == -1) {
                                    prvpn.setOffXY(255,0);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,0);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                            }
                            if (zf == 1)
                                dest_path_.push_back(*it);
                            break;
                        case 0x03:
                            if (zf == -1) {
                                if (yf == 0) {
                                    dest_path_.push_back(*it);
                                    break;
                                }
                                if (yf == 1) {
                                    prvpn.setOffXY(0,0);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(255,0);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                                if (yf == -1) {
                                    prvpn.setOffXY(255,0);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,0);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                            }
                            if (zf == 0) {
                                if (yf == 0) {
                                    dest_path_.push_back(*it);
                                    break;
                                }
                                if (yf == 1) {
                                    prvpn.setOffXY(0,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(255,255);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                                if (yf == -1) {
                                    prvpn.setOffXY(255,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,255);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                            }
                            if (zf == 1)
                                dest_path_.push_back(*it);
                            break;
                        case 0x04:
                            if (zf == -1) {
                                if (yf == 0) {
                                    dest_path_.push_back(*it);
                                    break;
                                }
                                if (yf == 1) {
                                    prvpn.setOffXY(0,0);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,255);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                                if (yf == -1) {
                                    prvpn.setOffXY(0,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,0);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                            }
                            if (zf == 0) {
                                if (yf == 0) {
                                    dest_path_.push_back(*it);
                                    break;
                                }
                                if (yf == 1) {
                                    prvpn.setOffXY(255,0);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(255,255);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                                if (yf == -1) {
                                    prvpn.setOffXY(255,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(255,0);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                            }
                            if (zf == 1)
                                dest_path_.push_back(*it);
                            break;
                    }
                }
            } else {
                if (twdn > 0x0 && twdn < 0x05) {
                    switch (twdn) {
                        case 0x01:
                            if (zf == -1) {
                                if (xf == 0) {
                                    dest_path_.push_back(*it);
                                    break;
                                }
                                if (xf == -1) {
                                    prvpn.setOffXY(255,0);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,0);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                                if (xf == 1) {
                                    prvpn.setOffXY(0,0);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(255,0);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                            }
                            if (zf == 0) {
                                if (xf == 0) {
                                    dest_path_.push_back(*it);
                                    break;
                                }
                                if (xf == -1) {
                                    prvpn.setOffXY(255,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,255);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                                if (xf == 1) {
                                    prvpn.setOffXY(0,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(255,255);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                            }
                            if (zf == 1)
                                dest_path_.push_back(*it);
                            break;
                        case 0x02:
                            if (zf == -1) {
                                if (xf == 0) {
                                    dest_path_.push_back(*it);
                                    break;
                                }
                                if (xf == -1) {
                                    prvpn.setOffXY(255,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,255);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                                if (xf == 1) {
                                    prvpn.setOffXY(255,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,255);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                            }
                            if (zf == 0) {
                                if (xf == 0) {
                                    dest_path_.push_back(*it);
                                    break;
                                }
                                if (xf == -1) {
                                    prvpn.setOffXY(255,0);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,0);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                                if (xf == 1) {
                                    prvpn.setOffXY(0,0);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(255,0);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                            }
                            if (zf == 1)
                                dest_path_.push_back(*it);
                            break;
                        case 0x03:
                            if (zf == -1) {
                                if (yf == 0) {
                                    dest_path_.push_back(*it);
                                    break;
                                }
                                if (yf == -1) {
                                    prvpn.setOffXY(0,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,0);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                                if (yf == 1) {
                                    prvpn.setOffXY(0,0);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(255,0);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                            }
                            if (zf == 0) {
                                if (yf == 0) {
                                    dest_path_.push_back(*it);
                                    break;
                                }
                                if (yf == -1) {
                                    prvpn.setOffXY(255,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,255);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                                if (yf == 1) {
                                    prvpn.setOffXY(0,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(255,255);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                            }
                            if (zf == 1)
                                dest_path_.push_back(*it);
                            break;
                        case 0x04:
                            if (zf == -1) {
                                if (yf == 0) {
                                    dest_path_.push_back(*it);
                                    break;
                                }
                                if (yf == -1) {
                                    prvpn.setOffXY(0,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,0);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                                if (yf == 1) {
                                    prvpn.setOffXY(0,0);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(0,255);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                            }
                            if (zf == 0) {
                                if (yf == 0) {
                                    dest_path_.push_back(*it);
                                    break;
                                }
                                if (yf == -1) {
                                    prvpn.setOffXY(255,255);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(255,0);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                                if (yf == 1) {
                                    prvpn.setOffXY(255,0);
                                    dest_path_.push_back(prvpn);
                                    it->setOffXY(255,255);
                                    dest_path_.push_back(*it);
                                    modified = true;
                                    break;
                                }
                            }
                            if (zf == 1)
                                dest_path_.push_back(*it);
                            break;
                    }
                } else {
                    dest_path_.push_back(*it);
                }
            }
            prvpn = *it;
            if (fit == cdestpath.end()) {
                if (modified) {
                    dest_path_.push_back(PathNode(x,y,z,ox,oy));
                } else {
                    // untill correct smoothing implemented this
                    // will prevent walking on non-walkable tile
                    if (xf == -1 && yf == -1) {
                        dest_path_.back().setOffXY(0,0);
                        dest_path_.push_back(prvpn);
                    }
                    if (xf == 1 && yf == -1) {
                        dest_path_.back().setOffXY(255,0);
                        dest_path_.push_back(prvpn);
                    }
                    if (xf == 1 && yf == 1) {
                        dest_path_.back().setOffXY(255,255);
                        dest_path_.push_back(prvpn);
                    }
                    if (xf == -1 && yf == 1) {
                        dest_path_.back().setOffXY(0,255);
                        dest_path_.push_back(prvpn);
                    }
                    dest_path_.back().setOffXY(ox,oy);
                }
            }
        }
    }
#ifdef EXECUTION_SPEED_TIME
    printf("smoothing time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif

#if 0
    for (std::list <PathNode>::iterator it = dest_path_.begin();
        it != dest_path_.end(); it++) {
        printf("x %i, y %i, z %i\n", it->tileX(),it->tileY(),it->tileZ());
    }
#endif
#ifdef EXECUTION_SPEED_TIME
    dest_path_.clear();
    printf("+++++++++++++++++++++++++++");
    printf("end time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif
}

bool PedInstance::movementP(Mission *m, int elapsed)
{
    bool updated = false;
    int used_time = elapsed;

    while (dest_path_.size() && used_time != 0) {
        int nxtTileX = dest_path_.front().tileX();
        int nxtTileY = dest_path_.front().tileY();
        int nxtTileZ = dest_path_.front().tileZ();
        if (hold_on_.wayFree != 0 && hold_on_.pathBlocker->isPathBlocker()) {
            if (hold_on_.xadj || hold_on_.yadj) {
                if(abs(hold_on_.tilex - nxtTileX) <= hold_on_.xadj
                    && abs(hold_on_.tiley - nxtTileY) <= hold_on_.yadj
                    && hold_on_.tilez == nxtTileZ)
                {
                    if (hold_on_.wayFree == 1)
                        return updated;
                    // hold_on_.wayFree == 2
                    dest_path_.clear();
                    speed_ = 0;
                    return updated;
                }
            } else {
                if (hold_on_.tilex == nxtTileX && hold_on_.tiley == nxtTileY
                    && hold_on_.tilez == nxtTileZ)
                {
                    if (hold_on_.wayFree == 1)
                        return updated;
                    // hold_on_.wayFree == 2
                    dest_path_.clear();
                    speed_ = 0;
                    return updated;
                }
            }
        } else
            hold_on_.wayFree = 0;
        // TODO: not ignore Z, if tile is stairs diffz is wrong
        int adx =
             nxtTileX * 256 + dest_path_.front().offX();
        int ady =
             nxtTileY * 256 + dest_path_.front().offY();
        int atx = tile_x_ * 256 + off_x_;
        int aty = tile_y_ * 256 + off_y_;
        int diffx = adx - atx, diffy = ady - aty;

        if (abs(diffx) < 16 && abs(diffy) < 16) {
            // TODO: maybe something better? then using diffx/diffy?
            // for this check
            off_y_ = dest_path_.front().offY();
            off_x_ = dest_path_.front().offX();
            tile_z_ = nxtTileZ;
            tile_y_ = nxtTileY;
            tile_x_ = nxtTileX;
            dest_path_.pop_front();
            if (dest_path_.empty())
                speed_ = 0;
            updated = true;
        } else {
            setDirection(diffx, diffy, &dir_);

            int dx = 0, dy = 0;
            double d = sqrt((double)(diffx * diffx + diffy * diffy));
            // object will not move over a distance he can actually move
            double avail_time_use = (d / (double)speed_) * 1000.0;
            // correcting time availiable for this distance to time
            // we can use
            if (avail_time_use > used_time)
                avail_time_use = used_time;

            if (abs(diffx) > 0)
                // dx = diffx * (speed_ * used_time / 1000) / d;
                dx = (int)((diffx * (speed_ * avail_time_use) / d) / 1000);
            if (abs(diffy) > 0)
                // dy = diffy * (speed_ * used_time / 1000) / d;
                dy = (int)((diffy * (speed_ * avail_time_use) / d) / 1000);

            if (dx || dy) {
                int prv_time = used_time;
                if (dx) {
                    used_time -= (int)(((double) dx * 1000.0 * d)
                        / (double)(diffx * speed_));
                } else if (dy) {
                    used_time -= (int)(((double) dy * 1000.0 * d)
                        / (double)(diffy * speed_));
                } else
                    used_time = 0;
                if (used_time < 0 || prv_time == used_time)
                    used_time = 0;
            } else
                used_time = 0;

            updatePlacement(off_x_ + dx, off_y_ + dy);
            // TODO : what obstacles? cars? doors are already
            // setting stop signal, reuse it?
#if 0
            if (updatePlacement(off_x_ + dx, off_y_ + dy)) {
                ;
            } else {
                // TODO: avoid obstacles.
                speed_ = 0;
            }
#endif
            if(nxtTileX == tile_x_ && nxtTileY == tile_y_)
                tile_z_ = nxtTileZ;

            if(nxtTileX == tile_x_ && nxtTileY == tile_y_
                && nxtTileZ == tile_z_ 
                && dest_path_.front().offX() == off_x_
                && dest_path_.front().offY() == off_y_)
                dest_path_.pop_front();
            if (dest_path_.size() == 0)
                speed_ = 0;

            updated = true;
        }

            if ((state_ & pa_smFollowing) != 0) {
                toDefineXYZ xyz;
                // TODO: calculate length for dest_path_?
                // TODO: too big elapsed makes ped move to close to target
                dest_path_.back().convertPosToXYZ(&xyz);
                double dist_cur = distanceToPosXYZ(&xyz);
                if (dist_cur < (double)dist_to_pos_) {
                    dest_path_.clear();
                    speed_ = 0;
                }
            }

        offzOnStairs(m->mtsurfaces_[tile_x_ + tile_y_ * m->mmax_x_
            + tile_z_ * m->mmax_m_xy].twd);
    }
#ifdef _DEBUG
    if (dest_path_.empty() && speed_) {
        printf("Was running at speed %i, destination unknown\n", speed_);
        speed_ = 0;
    }
#endif

    return updated;
}

// TODO: add responses, possible failure, bounced etc.
uint8 PedInstance::moveToDir(Mission* m, int elapsed, dirMoveType &dir_move,
    int dir, int t_posx, int t_posy, int dist)
{
    floodPointDesc *based = &(m->mdpoints_[tile_x_
        + tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);
    if(based->t == m_fdNonWalkable) {
        printf("==== unwalk pos: x %i; y %i; z %i, ox %i, oy %i, oz %i\n",
            tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
        printf("Movement from nonwalkable postion\n");
        return false;
    }

    // TODO: set safewalk need, somewhere
    bool check_safe_walk = true;

    // TODO: find safewalk tile and use normal pathfinding
    // to get there
    if ((based->t & m_fdSafeWalk) == 0)
        check_safe_walk = false;
    bool move_to_pos = false;
    if (dir == -1) {
        if (t_posx != -1 && t_posy != -1) {
            setDirection(t_posx - tile_x_ * 256 - off_x_,
                t_posy - tile_y_ * 256 - off_y_, &dir);
            if (dir == -1)
                return false;
            move_to_pos = true;
            if (dir_move.dir_modifier == 0)
                dir_move.dir_orig = dir;
        } else {
            dir = dir_;
            dir_move.dir_orig = dir;
        }
    }
    double dist_curr = (elapsed * speed_) / 1000.0;
    if (dist == 0) {
         if (dist_to_pos_ > 0 && (int)dist_curr > dist_to_pos_)
             dist_curr = (double) dist_to_pos_;
    } else if ((int) dist_curr > dist)
        dist_curr = (double)dist;
    bool bounced = false;
    bool should_bounce = dir_move.bounce;

    // TODO: better way for recovering direction, forward 7 tiles check
    // for current dir?
    if (dir_move.dir_modifier != 0) {
        dir = dir_move.dir_last;
    }

    while ((int)dist_curr > 0) {
        bool need_bounce = false;
        double diffx = 0, diffy = 0;
        if (dir == 0) {
            diffy = 1.0;
        } else if (dir == 64) {
            diffx = 1.0;
        } else if (dir == 128) {
            diffy = -1.0;
        } else if (dir == 192) {
            diffx = -1.0;
        } else if (dir < 64) {
            diffx = sin((dir / 128.0) * PI);
            diffy = cos((dir / 128.0) * PI);
        } else if (dir < 128) {
            int dirn = dir % 64;
            diffy = -sin((dirn / 128.0) * PI);
            diffx = cos((dirn / 128.0) * PI);
        } else if (dir < 192) {
            int dirn = dir % 64;
            diffx = -sin((dirn / 128.0) * PI);
            diffy = -cos((dirn / 128.0) * PI);
        } else if (dir < 256) {
            int dirn = dir % 64;
            diffy = sin((dirn / 128.0) * PI);
            diffx = -cos((dirn / 128.0) * PI);
        }

        double posx = (double)(tile_x_ * 256 + off_x_);
        double posy = (double)(tile_y_ * 256 + off_y_);
        floodPointDesc *fpd = &(m->mdpoints_[tile_x_ + tile_y_ * m->mmax_x_ +
            tile_z_ * m->mmax_m_xy]);
        double dist_passsed = 0;
        double dist_inc = sqrt(diffx * diffx + diffy * diffy);

        do {
            double px = posx + diffx;
            double py = posy + diffy;
            int tilenx = diffx >= 0 ? ((int)ceil(px)) / 256
                : ((int)floor(px)) / 256;
            int tileny = diffy >= 0 ? ((int)ceil(py)) / 256
                : ((int)floor(py)) / 256;
            if (tilenx < 0 ||  tileny < 0 || tilenx >= m->mmax_x_
                || tileny >= m->mmax_y_)
            {
                need_bounce = true;
                break;
            }
            if (tile_x_ != tilenx || tile_y_ != tileny) {
                // TODO: check for stairs and offset should be correct,
                // to avoid jumping on top of stairs
                int8 dec_z = 0;
                if (tilenx - tile_x_ == 0) {
                    if (tileny - tile_y_ > 0) {
                        if ((fpd->dirh & 0x01) == 0x01) {
                            tile_z_++;
                            dec_z = -1;
                        } else if ((fpd->dirl & 0x01) == 0x01) {
                            tile_z_--;
                            dec_z = 1;
                        } else if ((fpd->dirm & 0x01) != 0x01) {
                            need_bounce = true;
                            break;
                        }
                    } else {
                        if ((fpd->dirh & 0x10) == 0x10) {
                            tile_z_++;
                            dec_z = -1;
                        } else if ((fpd->dirl & 0x10) == 0x10) {
                            tile_z_--;
                            dec_z = 1;
                        } else if ((fpd->dirm & 0x10) != 0x10) {
                            need_bounce = true;
                            break;
                        }
                    }
                } else if (tileny - tile_y_ == 0) {
                    if (tilenx - tile_x_ > 0) {
                        if ((fpd->dirh & 0x04) == 0x04) {
                            tile_z_++;
                            dec_z = -1;
                        } else if ((fpd->dirl & 0x04) == 0x04) {
                            tile_z_--;
                            dec_z = 1;
                        } else if ((fpd->dirm & 0x04) != 0x04) {
                            need_bounce = true;
                            break;
                        }
                    } else {
                        if ((fpd->dirh & 0x40) == 0x40) {
                            tile_z_++;
                            dec_z = -1;
                        } else if ((fpd->dirl & 0x40) == 0x40) {
                            tile_z_--;
                            dec_z = 1;
                        } else if ((fpd->dirm & 0x40) != 0x40) {
                            need_bounce = true;
                            break;
                        }
                    }
                } else if (tileny - tile_y_ > 0) {
                    if (tilenx - tile_x_ > 0) {
                        if ((fpd->dirm & 0x02) != 0x02) {
                            need_bounce = true;
                            break;
                        }
                    } else {if ((fpd->dirm & 0x80) != 0x80) {
                            need_bounce = true;
                            break;
                        }
                    }
                } else {
                    // (tileny - tile_y_ < 0)
                    if (tilenx - tile_x_ > 0) {
                        if ((fpd->dirm & 0x08) != 0x08) {
                            need_bounce = true;
                            break;
                        }
                    } else {if ((fpd->dirm & 0x20) != 0x20) {
                            need_bounce = true;
                            break;
                        }
                    }
                }
                fpd = &(m->mdpoints_[tilenx + tileny * m->mmax_x_
                    + tile_z_ * m->mmax_m_xy]);
                if (check_safe_walk && (fpd->t & m_fdSafeWalk) == 0) {
                    tile_z_ += dec_z;
                    need_bounce = true;
                    break;
                }
                tile_x_ = tilenx;
                tile_y_ = tileny;
                if (dir_move.dir_modifier != 0) {
                    dist_passsed += dist_inc;
                    posx = px;
                    posy = py;
                    if (move_to_pos) {
                        dir_move.on_new_tile = true;
                        if (dir_move.dir_modifier < 3) {
                            dir_move.dir_modifier = 0;
                            dir_move.dir_last = -1;
                            dir = dir_move.dir_orig;
                        } else {
                            dir_move.dir_modifier -= 2;
                            // & 0x00C0 = ((% 256) / 64) * 64
                            dir = (256 + dir - dir_move.modifier_value) & 0x00C0;
                            dir_move.dir_last = dir;
                        }
                    } else if (rand() % 256 < 64) {
                        dir_move.dir_last = -1;
                        dir_move.dir_modifier = 0;
                    }
                    break;
                }
            }
            dist_passsed += dist_inc;
            posx = px;
            posy = py;
        } while (dist_passsed < dist_curr);
        if (diffx >= 0)
            off_x_ = ((int)ceil(posx)) % 256;
        else
            off_x_ = ((int)floor(posx)) % 256;
        if (diffy >= 0)
            off_y_ = ((int)ceil(posy)) % 256;
        else
            off_y_ = ((int)floor(posy)) % 256;
        dist_curr -= dist_passsed;
        if (need_bounce && should_bounce) {
            if (move_to_pos) {
                if (dir_move.dir_modifier == 0) {
                    dir_move.modifier_value = 64;
                    dir_move.modifier_value *= getClosestDirs(dir_move.dir_orig,
                        dir_move.dir_closest, dir_move.dir_closer);;
                    dir_move.dir_modifier = 1;
                    dir = dir_move.dir_closest;
                    dir_move.dir_last = dir_move.dir_closest;
                    dir_move.on_new_tile = false;
                } else if (dir_move.dir_modifier == 1) {
                    if (dir_move.on_new_tile) {
                        // TODO: +2?
                        dir_move.dir_modifier = 3;
                        // & 0x00C0 = ((% 256) / 64) * 64
                        dir = (256 + dir_move.dir_closest + dir_move.modifier_value) & 0x00C0;
                    } else {
                        dir_move.modifier_value *= -1;
                        dir_move.dir_modifier = 4;
                        dir = dir_move.dir_closer;
                        dir_move.dir_last = dir_move.dir_closer;
                    }
                    dir_move.dir_last = dir;
                } else if (dir_move.dir_modifier == 3) {
                    dir = (256 + dir_move.dir_closest + dir_move.modifier_value * 2) & 0x00C0;
                    dir_move.dir_last = dir;
                    dir_move.dir_modifier = 5;
                } else if (dir_move.dir_modifier == 5) {
                    dir = (256 + dir_move.dir_closest + dir_move.modifier_value * 3) & 0x00C0;
                    dir_move.dir_last = dir;
                    dir_move.dir_modifier = 7;
                } else if (dir_move.dir_modifier == 2) {
                    dir = dir_move.dir_closer;
                    dir_move.dir_last = dir_move.dir_closer;
                    dir_move.dir_modifier = 4;
                } else if (dir_move.dir_modifier == 4) {
                    dir = (256 + dir_move.dir_closer + dir_move.modifier_value) & 0x00C0;
                    dir_move.dir_last = dir;
                    dir_move.dir_modifier = 6;
                } else if (dir_move.dir_modifier == 6) {
                    dir = (256 + dir_move.dir_closer + dir_move.modifier_value * 2) & 0x00C0;
                    dir_move.dir_last = dir;
                    dir_move.dir_modifier = 8;
                } else if (dir_move.dir_modifier == 8) {
                    dir = (256 + dir_move.dir_closer + dir_move.modifier_value * 3) & 0x00C0;
                    dir_move.dir_last = dir;
                    dir_move.dir_modifier = 10;
                } else {
                    dir_move.dir_modifier = 0;
                    dir_move.dir_last = -1;
                    dir = dir_move.dir_orig;
                    break;
                }
            } else if (dir_move.dir_modifier) {
                if (dir_move.dir_modifier == 2)
                    dir_move.dir_last += 64;
                if (dir_move.dir_modifier == 3) {
                    dir_move.dir_last += (256 - 64);
                }
                dir_move.dir_last %= 256;
                dir = dir_move.dir_last;
                dir_move.dir_modifier--;
            } else {
                dir_move.dir_last = (dir_move.dir_last + 64) % 256;
                dir = dir_move.dir_last;
                dir_move.dir_modifier = 1;
            }
            setDirection(dir);
        } else if (!move_to_pos && dir_move.dir_modifier != 0) {
            setDirection(dir);
            if (dir_move.dir_modifier == 1) {
                dir_move.dir_modifier++;
                dir_move.dir_last += (256 - 64);
            }
            if (dir_move.dir_modifier == 2) {
                dir_move.dir_modifier++;
                dir_move.dir_last += 128;
            }
            if (dir_move.dir_modifier == 3) {
                dir_move.dir_modifier++;
                dir_move.dir_last += (256 - 64);
            }
            if (dir_move.dir_modifier == 3) {
                dir_move.dir_modifier = 0;
                dir_move.dir_last += (256 - 64);
            }
            // &0x00FF = % 256
            dir_move.dir_last &= 0x00FF;
            dir = dir_move.dir_last;
        } else {
            setDirection(dir);
            // TODO: fix this
            // To avoid looping
            if (need_bounce)
                break;
        }
        bounced = bounced || need_bounce;
    }
    offzOnStairs(m->mtsurfaces_[tile_x_ + tile_y_ * m->mmax_x_
        + tile_z_ * m->mmax_m_xy].twd);

    // TODO: handle bounce answer everywhere
    return bounced;
}

int PedInstance::getClosestDirs(int dir, int& closest, int& closer) {
    // & 0x003F = % 64
    int mod = dir & 0x003F;
    if (mod == 0) {
        // & 0x00C0 = ((% 256) / 64) * 64
        closest = (dir + 64) & 0x00C0;
        closer = (256 + dir - 64) & 0x00C0;
        return 1;
    } else {
        if (mod < 32) {
            closest = dir & 0x00C0;
            closer = (dir + 64) & 0x00C0;
            return -1;
        } else {
            closest = (dir + 64) & 0x00C0;
            closer = dir & 0x00C0;
        }
    }
    return 1;
}
