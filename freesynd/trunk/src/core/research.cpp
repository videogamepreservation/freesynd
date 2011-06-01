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

int Research::researchCnt = 1;

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
        currFunding_ = minFunding_ * g_CoeffsFund[coeffInd_];
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
        currFunding_ = minFunding_ * g_CoeffsFund[coeffInd_];
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

/*!
 * Improving a research consists in adding 25% of progression
 * to the research. Plus, the name of the searched weapon is
 * now displayed instead of the field search.
 */
void Research::improve(Weapon *pWeapon) {
    if (type_ == EQUIPS) {
        name_.assign(pWeapon->getName());

        ProgressPoint lastPt = progressList_.back();
        ProgressPoint newPt;
        newPt.coeffId = lastPt.coeffId;
        newPt.hours = lastPt.hours;
        // Adds a bonus to research
        newPt.percentage = lastPt.percentage + 25.0f;

        // Add new point to the list
        progressList_.push_back(newPt);

        if (newPt.percentage < 100.0f) {
            // research is not finished so update projected end
            updateProjection();
        } else {
            status_ = FINISHED;
        }
    }
}

/*!
 * Saves Research structure to the given file.
 */
bool Research::saveToFile(std::ofstream &file) {
    // id
    file.write(reinterpret_cast<const char*>(&id_), sizeof(int));

    // Research name : 15 caracters max
    char buf[15];
    memset(buf, '\0', 15);
    fs_strcpy(buf, 15, name_.c_str()); 
    file.write(buf, 15);
    
    if (type_ == EQUIPS) {
        // Weapons specific infos
        int ival = weapon_;
        file.write(reinterpret_cast<const char*>(&ival), sizeof(int));
    } else {
        // Mods specific infos
        int ival = modType_;
        file.write(reinterpret_cast<const char*>(&ival), sizeof(int));
        ival = modVersion_;
        file.write(reinterpret_cast<const char*>(&ival), sizeof(int));
    }
            
    // Current funding
    file.write(reinterpret_cast<const char*>(&currFunding_), sizeof(int));
    // Current status
    int status = status_;
    file.write(reinterpret_cast<const char*>(&status), sizeof(int));
    // Coeff index
    file.write(reinterpret_cast<const char*>(&coeffInd_), sizeof(short));
        
    // Progression points
    unsigned int ival = progressList_.size();
    file.write(reinterpret_cast<const char*>(&ival), sizeof(unsigned int));
            
    for (std::list < ProgressPoint >::iterator it = progressList_.begin(); 
            it != progressList_.end(); it++) {
        ProgressPoint pt = *it;
        float pct = pt.percentage;
        file.write(reinterpret_cast<const char*>(&pct), sizeof(float));
        short hours = pt.hours;
        file.write(reinterpret_cast<const char*>(&pct), sizeof(short));
        short coef = pt.coeffId;
        file.write(reinterpret_cast<const char*>(&coef), sizeof(short));
    }

    return true;
}

bool Research::loadFromFile(std::ifstream &infile, EResType type) {
    return false;
}