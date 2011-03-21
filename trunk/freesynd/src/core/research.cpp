/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2011  Benoit Blancard <benblan@users.sourceforge.net>*
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

#include "research.h"

int Research::researchCnt = 0;

const short g_Coeffs[] = {1, 2, 4, 6, 10, 15, 30, 50, 75 , 100};

Research::Research(std::string name, int min) {
    id_ = researchCnt++;
    name_ = name;
    minFunding_ = min;
    isStarted_ = false;
    currFunding_ = minFunding_;
    coeffInd_ = 0;
}

int Research::getCurrFunding() { 
    if (coeffInd_ == -1) {
        return 0;
    } else {
        return g_Coeffs[coeffInd_] * minFunding_;
    }
}

/*!
 * \return True if funding has changed.
 */
bool Research::incrFunding() {
    if (coeffInd_ < 9) {
        coeffInd_++;
        return true;
    }

    return false;
}

/*!
 * \return True if funding has changed.
 */
bool Research::decrFunding() {
    if (coeffInd_ >= 0) {
        coeffInd_--;
        return true;
    }

    return false;
}