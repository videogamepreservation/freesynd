/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2013  Bohdan Stelmakh <chamel@users.sourceforge.net> *
 *   Copyright (C) 2013  Benoit Blancard <benblan@users.sourceforge.net>*
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

#ifndef CORE_ACTIONS_H_
#define CORE_ACTIONS_H_

namespace fs_actions {

    /*!
     * List the different process by which action can be created.
     */
    enum CreatOrigin {
        //! Origin unknown
        kOrigUnknow = 0,
        //! By script
        kOrigScript = 1,
        //! From default action list
        kOrigDefault = 2,
        //! By another action group
        kOrigAction = 3,
        //! From user input
        kOrigUser = 4,
        //! Something happened that generated this action
        kOrigEvent = 5
    };
}

#endif // CORE_ACTIONS_H_
