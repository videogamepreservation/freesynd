/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2010 Bohdan Stelmakh <chamel@users.sourceforge.net>  *
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

#ifndef PATHSURFACES_H
#define PATHSURFACES_H

#include "common.h"

    typedef struct {
        // tyle type
        // 0x0 - not defined, 1b - surface, 2b - stairs,
        // 3b - junction, 4b - definition required, 5b - non
        // walkable/reachable
        unsigned char t;
        // direction
        // tile possible directions max 0x76543210, min 0x0
        unsigned int dir;
        unsigned int id;
        // id of junction surface lower
        // for surfaces this represents direction where stairs are
        unsigned int idjl;
        // id of junction surface higher
        // for surfaces this represents direction where stairs are
        unsigned int idjh;
        // tile walkable data from g_App.walkable_[]
        unsigned char twd;
        // index of junction inside its vector
        unsigned int indx;
    }surfaceDesc;

    #define m_sdNotdefined      0
    #define m_sdSurface         1
    #define m_sdStairs          2
    #define m_sdJunction        4
    #define m_sdDefreq          8
    #define m_sdNonwalkable     16

    typedef struct {
        int x;
        int y;
        int z;
    }toDefineXYZ;

    typedef struct {
        surfaceDesc *pj;
        int x;
        int y;
        int z;
        unsigned int fastxyz;
    }junctionDesc;

#endif
