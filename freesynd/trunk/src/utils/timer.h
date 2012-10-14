/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2012  Benoit Blancard <benblan@users.sourceforge.net>*
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

#ifndef UTILS_TIMER_H_
#define UTILS_TIMER_H_

#include "common.h"
namespace fs_utils {

/*!
 * A timer is used to .
 */
class Timer {
 public:
     /*!
      * Contructor to set the max time.
      */
     Timer(uint32 i_max) {
         i_counter_ = 0;
         i_max_ = i_max;
     }

     /*!
      * Reset the counter.
      */
     void reset() {
         i_counter_ = 0;
     }

     /*!
      * Adds time to time and returns true if max is reached.
      * If max is reached, counter is reset.
      * \return True if timer has reached max.
      */
     bool update(uint32 elapsed) {
         i_counter_ += elapsed;
         if (i_counter_ > i_max_) {
             i_counter_ = 0;
             return true;
         }
         return false;
     }
 private:
    uint32 i_counter_;
    uint32 i_max_;
};
};

#endif  // UTILS_TIMER_H_
