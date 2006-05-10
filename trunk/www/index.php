<?php

/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005,2006                                            *
 *      Stuart Bingë  <skbinge@gmail.com>                               *
 *      Joost Peters  <joostp@users.sourceforge.net>                    *
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

// $Id$

$title = 'News'; require_once 'include/common.php';

// The newsItem() signature is newsItem($title, $content, $submitter, $date);

newsItem(
    'New development push',
"We're currently gearing up for a renewed development effort in order to get
this project on track and hopefully get an initial release out the door. This
updated website is a first step - all existing content has been revised, the
interface has been streamlined, and the file format documentation has been
significantly updated. Expect more soon!

Currently we've documented the map tile format for Syndicate and the sprite
format for both Syndicate and Syndicate Wars. We'll be constantly updating this
with additional formats - there are still several that we know the format of
that need documenting.",
    'Stuart',
    '2005-01-31'
);

printFooter();
