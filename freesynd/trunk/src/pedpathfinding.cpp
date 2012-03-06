/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
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

//#define NEW_ANIMATE_HANDLING
#include "common.h"
#include "app.h"
#include "pathsurfaces.h"

#if 0
#define EXECUTION_SPEED_TIME
#endif

void PedInstance::setDestinationP(Mission *m, int x, int y, int z,
                                     int ox, int oy, int new_speed)
{
    // NOTE: this is a "flood" algorithm, it expands until it reaches other's
    // flood point, then it removes unrelated points
#ifdef EXECUTION_SPEED_TIME
    printf("---------------------------");
    printf("start time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif
    m->adjXYZ(x, y, z);
    dest_path_.clear();
    speed_ = 0;

    if (map_ == -1 || health_ <= 0)
        return;

    floodPointDesc *targetd = &(m->mdpoints_[x + y * m->mmax_x_ + z * m->mmax_m_xy]);

    floodPointDesc *based = &(m->mdpoints_[tile_x_
        + tile_y_ * m->mmax_x_ + tile_z_ * m->mmax_m_xy]);

#if 0
    printf("target t %x, dirm %x ; base t %x, dirm %x\n", targetd->t,
        targetd->dirm, based->t, based->dirm);
    printf("target dirh %x, dirl %x ; base dirh %x, dirl %x\n", targetd->dirh,
        targetd->dirl, based->dirh, based->dirl);
    printf("ttwd %X \n",m->mtsurfaces_[x + y * m->mmax_x_ 
        + z * m->mmax_m_xy].twd);
    printf("target pos: x %x; y %x; z %x, ox %i, oy %i\n",
        x, y, z, ox, oy);
    //printf("tileAt %x\n", g_App.maps().map(map())->tileAt(x,y,z));
    printf("base pos: x %i; y %i; z %i, ox %i, oy %i, oz %i\n",
        tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
    printf("zmax %x\n", m->mmax_z_);
    if ( (z + 1) < m->mmax_z_) {
        unsigned char utwd = 
        printf("upper twd %i\n", m->mtsurfaces_[x + y * m->mmax_x_ 
        + (z + 1) * m->mmax_m_xy].twd);
    }
#endif

    if(targetd->t == m_fdNonWalkable || map_ == -1 || health_ <= 0) {
        return;
    }

    if(based->t == m_fdNonWalkable) {
        printf("unwalk pos: x %i; y %i; z %i, ox %i, oy %i, oz %i\n",
            tile_x_, tile_y_, tile_z_, off_x_, off_y_, off_z_);
        printf("Movement from nonwalkable postion\n");
        return;
    }

#ifndef NEW_ANIMATE_HANDLING
    if (in_vehicle_) {
        if(in_vehicle_->tileX() != x
            || in_vehicle_->tileY() != y
            || in_vehicle_->tileZ() != z
            || in_vehicle_->offX() != ox
            || in_vehicle_->offY() != oy)
        in_vehicle_ = NULL;
    }
    if (pickup_weapon_) {
        if(pickup_weapon_->tileX() != x
            || pickup_weapon_->tileY() != y
            || pickup_weapon_->tileZ() != z
            || pickup_weapon_->offX() != ox
            || pickup_weapon_->offY() != oy)
        pickup_weapon_ = 0;
    }
#endif

    if (tile_x_ == x && tile_y_ == y && tile_z_ == z) {
        dest_path_.push_back(PathNode(x, y, z, ox, oy));
        speed_ = new_speed;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z + 1;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z + 1;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z + 1;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z + 1;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z - 1;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z - 1;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z - 1;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z - 1;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z + 1;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z + 1;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z + 1;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z + 1;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z - 1;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z - 1;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z - 1;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z - 1;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x + 1;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y - 1;
                        sadd.coords.z = bref.coords.z;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y;
                        sadd.coords.z = bref.coords.z;
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
                    if (sadd.p->t == m_fdWalkable) {
                        sadd.coords.x = bref.coords.x - 1;
                        sadd.coords.y = bref.coords.y + 1;
                        sadd.coords.z = bref.coords.z;
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
    if (lt == m_fdBasePoint) {
        if (nodeset) {
            unsigned short n = bn[blvl].n;
            std::vector <toSetDesc>::iterator it = bv.begin() + bn[blvl].indxs;
            for (unsigned short i = 0; i < n; i++) {
                it->p->t ^= m_fdBasePoint;
                it->p->lvl = 0;
                //bv.erase(it);
                it++;
            }
            bn.pop_back();
        }
        blvl--;
    } else {
        if (nodeset) {
            unsigned short n = tn[tlvl].n;
            std::vector <toSetDesc>::iterator it = tv.begin() + tn[tlvl].indxs;
            for (unsigned short i = 0; i < n; i++) {
                it->p->t ^= m_fdTargetPoint;
                it->p->lvl = 0;
                //tv.erase(it);
                it++;
            }
            tn.pop_back();
        }
        tlvl--;
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
            bool remv = true;
            if (bref.p->dirh != 0) {
                if ((bref.p->dirh & 0x01) == 0x01) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y + 1) * m->mmax_x_
                        + (bref.coords.z + 1) * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirh & 0x04) == 0x04) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + bref.coords.y * m->mmax_x_
                        + (bref.coords.z + 1) * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirh & 0x10) == 0x10) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y - 1) * m->mmax_x_
                        + (bref.coords.z + 1) * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirh & 0x40) == 0x40) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + bref.coords.y * m->mmax_x_
                        + (bref.coords.z + 1) * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
            }
            if (bref.p->dirl != 0) {
                if ((bref.p->dirl & 0x01) == 0x01) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y + 1) * m->mmax_x_
                        + (bref.coords.z - 1) * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirl & 0x04) == 0x04) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + bref.coords.y * m->mmax_x_
                        + (bref.coords.z - 1) * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirl & 0x10) == 0x10) {
                    pfdp = &(mdpmirror[(bref.coords.x)
                        + (bref.coords.y - 1) * m->mmax_x_
                        + (bref.coords.z - 1) * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirl & 0x40) == 0x40) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + bref.coords.y * m->mmax_x_
                        + (bref.coords.z - 1) * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
            }
            if (bref.p->dirm != 0) {
                if ((bref.p->dirm & 0x01) == 0x01) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y + 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x02) == 0x02) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + (bref.coords.y + 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x04) == 0x04) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + bref.coords.y * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x08) == 0x08) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + (bref.coords.y - 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x10) == 0x10) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y - 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x20) == 0x20) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + (bref.coords.y - 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x40) == 0x40) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + bref.coords.y * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x80) == 0x80) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + (bref.coords.y + 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
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
            bool remv = true;
            if (bref.p->dirh != 0) {
                if ((bref.p->dirh & 0x01) == 0x01) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y + 1) * m->mmax_x_
                        + (bref.coords.z + 1) * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirh & 0x04) == 0x04) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + bref.coords.y * m->mmax_x_
                        + (bref.coords.z + 1) * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirh & 0x10) == 0x10) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y - 1) * m->mmax_x_
                        + (bref.coords.z + 1) * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirh & 0x40) == 0x40) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + bref.coords.y * m->mmax_x_
                        + (bref.coords.z + 1) * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
            }
            if (bref.p->dirl != 0) {
                if ((bref.p->dirl & 0x01) == 0x01) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y + 1) * m->mmax_x_
                        + (bref.coords.z - 1) * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirl & 0x04) == 0x04) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + bref.coords.y * m->mmax_x_
                        + (bref.coords.z - 1) * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirl & 0x10) == 0x10) {
                    pfdp = &(mdpmirror[(bref.coords.x)
                        + (bref.coords.y - 1) * m->mmax_x_
                        + (bref.coords.z - 1) * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirl & 0x40) == 0x40) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + bref.coords.y * m->mmax_x_
                        + (bref.coords.z - 1) * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
            }
            if (bref.p->dirm != 0) {
                if ((bref.p->dirm & 0x01) == 0x01) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y + 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x02) == 0x02) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + (bref.coords.y + 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x04) == 0x04) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + bref.coords.y * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x08) == 0x08) {
                    pfdp = &(mdpmirror[(bref.coords.x + 1)
                        + (bref.coords.y - 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x10) == 0x10) {
                    pfdp = &(mdpmirror[bref.coords.x
                        + (bref.coords.y - 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x20) == 0x20) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + (bref.coords.y - 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x40) == 0x40) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + bref.coords.y * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
                        remv = false;
                }
                if ((bref.p->dirm & 0x80) == 0x80) {
                    pfdp = &(mdpmirror[(bref.coords.x - 1)
                        + (bref.coords.y + 1) * m->mmax_x_
                        + bref.coords.z * m->mmax_m_xy]);
                    if ((bref.p->lvl + 1) == pfdp->lvl)
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
    bn.clear();
    tn.clear();
    bv.clear();
    tv.clear();

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
        if (pfdp->dirh != 0) {
            if ((pfdp->dirh & 0x01) == 0x01) {
                sadd.coords.x = ctile.x;
                sadd.coords.y = ctile.y + 1;
                sadd.coords.z = ctile.z + 1;
                sadd.p = &(mdpmirror[sadd.coords.x
                    + sadd.coords.y * m->mmax_x_
                    + sadd.coords.z * m->mmax_m_xy]);
                if ((sadd.p->t & ct) != 0) {
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
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
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
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
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
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
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
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
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
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
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
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
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
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
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
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
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
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
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
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
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
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
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
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
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
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
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
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
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
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
                    if ((ct == m_fdBasePoint && (pfdp->lvl + 1) == sadd.p->lvl)
                        || (ct == m_fdTargetPoint && (pfdp->lvl - 1) == sadd.p->lvl)) {
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
            if (twd > 0x0 && twd < 0x05) {
                if (twdn > 0x0 && twdn < 0x05) {
                    dest_path_.push_back(*it);
                } else {
                    switch (twd) {
                        case 0x01:
                            if (zf == 1) {
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
                            if (zf == -1)
                                dest_path_.push_back(*it);
                            break;
                        case 0x02:
                            if (zf == 1) {
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
                            if (zf == -1)
                                dest_path_.push_back(*it);
                            break;
                        case 0x03:
                            if (zf == 1) {
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
                            if (zf == -1)
                                dest_path_.push_back(*it);
                            break;
                        case 0x04:
                            if (zf == 1) {
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
                            if (zf == 0) {
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
                            if (zf == -1)
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
                        case 0x02:
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
                        case 0x04:
                            if (zf == -1) {
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
                            if (zf == 0) {
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
#endif
    if(dest_path_.size() != 0)
        speed_ = new_speed;
#ifdef EXECUTION_SPEED_TIME
    printf("+++++++++++++++++++++++++++");
    printf("end time %i.%i\n", SDL_GetTicks()/1000, SDL_GetTicks()%1000);
#endif
}

