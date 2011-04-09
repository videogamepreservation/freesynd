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

/*! This is a list of multiplicator to compute current funding 
 * based on the minimun funding.
 */
const short g_CoeffsFund[] = {1, 2, 4, 6, 10, 15, 30, 50, 75 , 100};

/*!
 * This is a list of coeff to compute the end projection.
 */
const float g_CoeffsProg[] = {0.416f, 0.83f, 1.25f, 1.666f, 2.083f, 2.5f, 2.941f, 3.333f, 3.846f, 4.166f};

Research::Research(Weapon::WeaponType wType, std::string name, int min, Weapon::WeaponType nextWeapon) {
    init(name, min);
    weapon_ = wType;
    type_ = EQUIPS;
    nextWeapon_ = nextWeapon;
}

Research::Research(Mod::EModType mType, Mod::EModVersion ver, std::string name, int min) {
    init(name, min);
    type_ = MODS;
    modType_ = mType;
    modVersion_ = ver;
}

void Research::init(std::string name, int min) {
    id_ = researchCnt++;
    weapon_ = Weapon::Unknown;
    modType_ = Mod::Unknown;
    modVersion_ = Mod::MOD_V1;
    name_ = name;
    minFunding_ = min;
    status_ = NOT_STARTED;
    currFunding_ = minFunding_;
    coeffInd_ = 0;
    // with a coeff of 1, research lasts 10 days (so 240 hours)
    projectedHour_ = 240;

    // insert a first point in the progression list.
    ProgressPoint p;
    p.hours = 0;
    p.percentage = 0.0f;
    p.coeffId = 0;
    progressList_.push_back(p);
}

int Research::getCurrFunding() { 
    if (coeffInd_ == -1) {
        return 0;
    } else {
        return g_CoeffsFund[coeffInd_] * minFunding_;
    }
}

/*!
 * Computes the ending hour based on the current progression point and coefficient.
 */
void Research::updateProjection() {
    if (coeffInd_ != -1) {
        float x = (100.0f - progressList_.back().percentage + g_CoeffsProg[coeffInd_] * progressList_.back().hours) / g_CoeffsProg[coeffInd_];

        projectedHour_ = (int) x;
    }
}

/*!
 * \return True if funding has changed.
 */
bool Research::incrFunding() {
    if (coeffInd_ < 9) {
        coeffInd_++;
        updateProjection();
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
        updateProjection();
        return true;
    }

    return false;
}

int Research::updateProgression(short hourElapsed, int budget) {
    int cost = 0;
    if(coeffInd_ != -1 && status_==STARTED) {

        ProgressPoint lastPt = progressList_.back();
        ProgressPoint newPt;
        newPt.coeffId = coeffInd_;

        // hours elapsed cannot exceed projected hours
        if (lastPt.hours + hourElapsed > projectedHour_) {
            hourElapsed = projectedHour_ - lastPt.hours;
        }

        // check if we can afford progression
        int fundPerHour = currFunding_ / 24;
        int maxCost = fundPerHour * hourElapsed;
        if (maxCost > budget) {
            hourElapsed = budget / fundPerHour;
        }

        if (lastPt.hours + hourElapsed >= projectedHour_) {
            // Nb of hour elapsed overcome the projectedHour
            // so research is finished
            newPt.percentage = 100.0f;
            newPt.hours = projectedHour_;
            status_ = FINISHED;
            cost = (projectedHour_ - lastPt.hours) * fundPerHour;
        } else {
            newPt.hours = lastPt.hours + hourElapsed;
            newPt.percentage = g_CoeffsProg[coeffInd_] * (newPt.hours - lastPt.hours) + lastPt.percentage;
            cost = hourElapsed * fundPerHour;
        }

        if (coeffInd_ == lastPt.coeffId) {
            // Progression rate is the same, update last point
            // by removing the last one and adding a new one
            progressList_.pop_back();
        }
        progressList_.push_back(newPt);
    }

    return cost;
}