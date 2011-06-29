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

#ifndef PATHSURFACES_H
#define PATHSURFACES_H

#include "common.h"

    typedef struct {
        // tile walkable data from g_App.walkable_p_[]
        unsigned char twd;
    }surfaceDesc;

    typedef struct {
        int x;
        int y;
        int z;
    }toDefineXYZ;

    typedef struct {
        // 0 - not defined, 1b - base point, 2b - target point,
        // 3b - link (when base point reaches target point or vice versa),
        // 4b - walkable, 5b - constant, 6b - non walkable
        unsigned char t;
        // dirh(z + 1), dirm(z), dirl(z - 1) - directions
        // 0x01 = (x, y + 1, z); 0x02 = (x + 1, y + 1, z);
        // 0x04 = (x + 1, y, z); 0x08 = (x + 1, y - 1, z);
        // 0x10 = (x, y - 1, z); 0x20 = (x - 1, y - 1, z);
        // 0x40 = (x - 1, y, z); 0x80 = (x - 1, y + 1, z)
        // can be combined 0x01 | 0x02; 0x01 | 0x10 | 0x40 etc.
        unsigned char dirh;
        unsigned char dirm;
        unsigned char dirl;

        unsigned short lvl;
    }floodPointDesc;
    #define m_fdNotDefined      0
    #define m_fdBasePoint       1
    #define m_fdTargetPoint     2
    #define m_fdLink            4
    #define m_fdWalkable        8
    #define m_fdConstant        16
    #define m_fdNonWalkable     32
    #define m_fdDefReq          64

#endif

