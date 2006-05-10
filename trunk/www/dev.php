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

$title = 'Development'; require_once 'include/common.php'; ?>

<h3>Getting the code</h3>

<p>The FreeSynd CVS repository is hosted by SourceForge. To checkout a module
from the repository simply type the following commands in your shell (press
enter if the login command asks for a password):</p>

<pre>cvs -d:pserver:anonymous@cvs.sourceforge.net:/cvsroot/freesynd login
cvs -z3 -d:pserver:anonymous@cvs.sourceforge.net:/cvsroot/freesynd co &lt;module&gt;</pre>

<br />

<p>The available modules, and what they contain, are as follows:</p>

<pre>freesynd           FreeSynd source code
www                FreeSynd website code</pre>

<br />

<p>Web-based CVS access is also available through SourceForge's <a
href="http://cvs.sourceforge.net/viewcvs.py/freesynd">ViewCVS interface</a>. You
might also want to take a look at the <a
href="http://sourceforge.net/projects/freesynd/">project page</a> on
SourceForge.</p>

<hr />

<h3>CVS commit notifications</h3>
<p>Developers may be interested in the following mailing list; summaries of
all changes made to the CVS repository are posted here.</p>
FreeSynd CVS mailing list<br />
<small>
(<a href="http://lists.sourceforge.net/lists/listinfo/freesynd-cvs">Subscribe/Manage</a> |
<a href="http://sourceforge.net/mailarchive/forum.php?forum=freesynd-cvs">Archive</a>)
</small>

<?php printFooter(); ?>
