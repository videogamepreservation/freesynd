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
#ifndef RESEARCH_H
#define RESEARCH_H

#include <string>

/*!
 * Represents a research on a specific field.
 */
class Research {
public:
    Research(std::string name, int min);
    ~Research() {}

    int getId() { return id_; }
    std::string getName() { return name_; }
    int getMinFunding() { return minFunding_; }
    int getMaxFunding() { return minFunding_ * 100; }
    int getCurrFunding();
    bool incrFunding();
    bool decrFunding();
    int isStarted() { return isStarted_; }
    void start() { isStarted_ = true; }
protected:
    /*! A counter to have unique IDs.*/
    static int researchCnt;
    /*! Id of the research.*/
    int id_;
    /*! Name of the research.*/
    std::string name_;
    /*! Minimum amount of funding.*/
    int minFunding_;
    /*! Current funding.*/
    int currFunding_;
    /*! True is search has started.*/
    bool isStarted_;
    /*! Index of the multiplicator coeff.*/
    int coeffInd_;
};

#endif //RESEARCH_H