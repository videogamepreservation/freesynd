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
#ifndef RESEARCHMANAGER_H
#define RESEARCHMANAGER_H

#include <list>

class Research;

/*!
 * This class manages all research functionnality.
 */
class ResearchManager {
public:
    //! Default constructor
    ResearchManager();
    ~ResearchManager();

    /*!
     * Returns a list of all available search on mods.
     * \return List can be empty but not null.
     */
    std::list<Research *> * getAvailableModsSearch() { return &availableModsSearch_; }
    /*!
     * Returns a list of all available search on equips.
     * \return List can be empty but not null.
     */
    std::list<Research *> * getAvailableEquipsSearch() { return &availableEquipsSearch_; }

    //! Returns a Research on equips with given id
    Research * getEquipsSearch(int id);
    //! Returns a Research on mods with given id
    Research * getModsSearch(int id);
protected:
    /*! List of all currently available research on mods.*/
    std::list<Research *> availableModsSearch_;
    /*! List of all currently available research on equips.*/
    std::list<Research *> availableEquipsSearch_;
};

#endif //RESEARCHMANAGER_H