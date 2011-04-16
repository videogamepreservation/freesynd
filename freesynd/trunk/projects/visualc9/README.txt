Building FreeSynd using Microsoft Visual C++ 2008

Copyright (C) 2010 Benoit Blancard.

List of librairies used to build freesynd:
------------------------------------------
* sdl       : 1.2.14
* sdl_mixer : 1.2.11
* sdl_image : 1.2.10
* libpng    : 1.4.1
* zlib      : 1.2.3

Assumptions:
------------
* The FreeSynd source files are in ..\..
* The external libraries are in ..\..\..\libs

List of required DLL to run freesynd (see dll in the libs directory)
-------------------------------------
	- libogg-0.dll
	- libpng14.dll
	- libvorbis-0.dll
	- libvorbisfile-3.dll
	- SDL.dll
	- SDL_image.dll
	- SDL_mixer.dll
	- zlib1.dll

	
How to build:
------------
1) On the main menu, select "File | Open Solution".
   Open "projects\visualc9\freesynd.sln".
   
2) Set the appropriate macros :
* dynamic macros : those macros are settable in the project properties
	_WIN32 / WIN32 : used to specify it's a WinNt platform (mandatory)
        DEBUG : used in the Debug configuration (for example for logging)
        WIN_SECURE : used to switch between standard lib functions and
                     secure ones (only in Windows with VS)
	SYSTEM_SDL : used to specify the implementation of the System interface.
                 For now, this is the only implementation
	GP2X : used to build for the GP2X platform
	CHEAP_LEAK_DETECTION : set to debug memory leaks
	
* static macros : you need to modify the config.h file to modify those macros
	HAVE_SDL_MIXER : tells whether the sdl_mixer librairy is used or not.
	USE_INTRO_MP3 : tells whether to play the intro music from custom file
	                or original file
	USE_ASSASSINATE_MP3 : tells whether to play the game music from custom file
	                or original file
	USE_OGG_INSTEAD_OF_MP3 : tells whether to play mp3 or ogg files

2) Select "Build | Configuration Manager...".
   Choose the configuration you wish to build.

3) Select "Build | Clean Solution".

4) Select "Build | Build Solution".


How to run the game
---------------
1) Copy the required DLL to the Debug/Release directory

2) In the project properties, set the working directory to the directory 
   containing the data directory
   
3) Select "Debug | Run".


