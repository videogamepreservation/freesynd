
FreeSynd 0.5
------------

FreeSynd is a free cross-platform reimplementation of the classic Bullfrog 
game, Syndicate.
For more information see:

    http://freesynd.sourceforge.net/
   
License information
-------------------

FreeSynd is released under the GNU General Public Licence, version 2.
Thanks to Tomasz Lis for some preliminary work on the level files. His code is
used under GNU General Public Licence, version 2 or later with permission from
author.
Some code is based on "libsyndicate" library by Paul Chavent. It is
re-licensed under GNU General Public Licence, version 2. Original license
and code is located in svn source/trunk/libsyndicate/.
See the file "gpl.txt" for informations on this licence.


What's new in this release?
---------------------------

See the file "RELEASE-NOTES.txt".

Runnning under Linux
--------------------

In order to run FreeSynd under Linux, you need to install: 
 - libSDL-1.2
 - libSDL_mixer
 - libSDL_image

On a debian based distribution you can do this as simply as:
   apt-get install libsdl1.2debian
   apt-get install libsdl-mixer1.2

for other distributions, consult the appropriate documentation.

Running under Windows
---------------------

Everything you need to run FreeSynd is included in this release.  Just double
click freesynd.exe and enjoy.

Running options
---------------

When running the game, the following arguments can be used on the command line:
 * -h | --help : displays the list of available options
 * -f | --fullscreen : runs the game in fullscreen mode
 * -p | --path : sets the absolute path to the data directory.
 
Commands
--------
- Mission Selection :
 * 0   : Puts the selector on first mission
 * Left: Puts the selector on the next mission
 * Right : Puts the selector on the previous mission
 
- In game :
 * F1  : Turns music on/off
 * F2  : Turns game sounds on/off
 * 1-4 : Numeric key selects the corresponding agent
 * 0   : Key 0 cycles between current selection and all 4 agents.
 * Ctrl + 0 : Inverts selection.
 * Ctrl + Left Click : sets a waypoint for the selected agents
 * Left,Right,Up, down : scrolls the map

Contact
-------

If you find a bug or would like to help the development of FreeSynd, please 
stop by the sourceforge project bug tracker, drop us an email or visit the
forums.
