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

// Our list of screenshots
$screenshots = array(
    '2005-01-31' => array(
        'caption' => 'Rendering a couple of FLI videos',
        'shots' => array(
            'ss4.png' => 'Intro video #1',
            'ss5.png' => 'Intro video #2',
            'ss6.png' => 'Intro video #3',
            'ss7.png' => 'Lose game video',
            'ss8.png' => 'Win game video',
            'ss9.png' => 'Credits video',
        ),
    ),

    '2004-06-21' => array(
        'caption' => 'An early demo of the map rendering',
        'shots' => array(
            'ss1.png' => 'This should be familiar :-)',
            'ss2.png' => 'Just some random map',
            'ss3.png' => 'Rendered with a different palette',
        ),
    ),
);

// See if we need to show a screenshot
$ss_date = isset($_GET['ss_date']) ? $_GET['ss_date'] : '';
$ss_file = isset($_GET['ss_file']) ? $_GET['ss_file'] : '';

$invalid = (empty($ss_date) || empty($ss_file) || empty($screenshots[$ss_date]) ||
    empty($screenshots[$ss_date]['shots']) || !isset($screenshots[$ss_date]['shots'][$ss_file]));

$title = ($invalid ? 'Screenshots' : "Screenshot ($ss_file)");

require_once 'include/common.php';

print "<div class=\"cent_algn\">\n";

if ($invalid) {
    // Show the list of all screenshots

    foreach ($screenshots as $date => $shotlist) {
        if (empty($shotlist['shots'])) {
            next;
        }

        $caption = !empty($shotlist['caption']) ? ' &raquo; ' . $shotlist['caption'] : '';
        print "<div class=\"caption\">$date$caption</div>\n";
        print "<table class=\"screenshots\"><tr>\n";

        $rowcount = 0;
        $endrow = false;
        foreach ($shotlist['shots'] as $file => $caption) {
            $thumb = "thumb.$file";
            $thumb_img = "screenshots/$thumb";
            $thumbnail = "images/$thumb_img";

            $file_img = "screenshots/$file";
            $filename = "images/$file_img";

            if (!file_exists($filename) || !file_exists($thumbnail)) {
                next;
            }

            if ($endrow) {
                print "</tr><tr>\n";
                $endrow = false;
            }

            // We need the GD extension installed for this to work
            $thumb_size = getimagesize($thumbnail);
            $file_size = getFileSize($filename);
            $img = img($thumb_img, $thumb, $thumb_size[0], $thumb_size[1]);

            print "<td><a href=\"ss.php?ss_date=$date&amp;ss_file=$file\">\n";
            print "$img<br />\n";
            print "$file ($file_size)</a><br />\n";
            print "$caption</td>\n\n";

            $rowcount++;
            if ($rowcount == 3) {
                $rowcount = 0;
                $endrow = true;
            }
        }

        print "</tr></table><br />\n";
    }

} else {
    // View a single screenshot

    $shots = $screenshots[$ss_date]['shots'];

    $caption = !empty($shots[$ss_file]) ? ' &raquo; ' . $shots[$ss_file] : '';

    print "<div class=\"caption\">$ss_date$caption</div><br />\n";

    $ss_url = "screenshots/$ss_file";
    // We need the GD extension installed for this to work
    $ss_size = getimagesize("images/$ss_url");
    echo '<a href="ss.php">', img($ss_url, $ss_file, $ss_size[0], $ss_size[1]), "</a>\n<br /><br />\n";

    print "<div class=\"caption\">Click on the screenshot to return</div>\n";
}

print "</div>\n";

printFooter();
