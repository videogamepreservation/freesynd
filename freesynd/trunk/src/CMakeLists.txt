# Add this folder to the include directories.
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# Generate the ini header, add it's location to the include directories.
include (tools/embed.cmake)
embed_data (../freesynd.ini embedded_default_ini default_ini.h)
include_directories (${CMAKE_CURRENT_BINARY_DIR})

set (SOURCES
	agent.cpp
	agentmanager.cpp
	app.cpp
	core/gamesession.cpp
	core/missionbriefing.cpp
	core/research.cpp
	core/researchmanager.cpp
	default_ini.h
	freesynd.cpp
	gfx/dirtylist.cpp
	gfx/fliplayer.cpp
	gfx/font.cpp
	gfx/fontmanager.cpp
	gfx/screen.cpp
	gfx/sprite.cpp
	gfx/spritemanager.cpp
	gfx/tile.cpp
	gfx/tilemanager.cpp
	map.cpp
	mapobject.cpp
	mapmanager.cpp
	menus/minimaprenderer.cpp
	menus/briefmenu.cpp
	menus/confmenu.cpp
	menus/debriefmenu.cpp
	menus/flimenu.cpp
	menus/gameplaymenu.cpp
	menus/loadingmenu.cpp
	menus/loadsavemenu.cpp
	menus/logoutmenu.cpp
	menus/mainmenu.cpp
	menus/mapmenu.cpp
	menus/menu.cpp
	menus/menumanager.cpp
	menus/researchmenu.cpp
	menus/selectmenu.cpp
	menus/widget.cpp
	mission.cpp
	missionmanager.cpp
	mod.cpp
	modmanager.cpp
	ped.cpp
	pedmanager.cpp
	pedpathfinding.cpp
	sound/audio.cpp
	sound/musicmanager.cpp
	sound/sdlmixermusic.cpp
	sound/sdlmixersound.cpp
	sound/soundmanager.cpp
	sound/xmidi.cpp
	system_sdl.cpp
	utils/configfile.cpp
	utils/ccrc32.cpp
	utils/dernc.cpp
	utils/file.cpp
	utils/log.cpp
	utils/portablefile.cpp
	utils/seqmodel.cpp
	vehicle.cpp
	vehiclemanager.cpp
	weapon.cpp
	weaponmanager.cpp
)

if (APPLE AND ${SDL_INCLUDE_DIR} MATCHES ".framework")
	# When using the official SDL 1.2 framework for Mac,
	# you must link in an implementation of SDLmain.
	set (SOURCES ${SOURCES} SDLMain.m)
endif()

set(HEADERS
	agent.h
	agentmanager.h
	app.h
	common.h
	config.h
	cp437.h
	keys.h
	map.h
	mapmanager.h
	mapobject.h
	mission.h
	missionmanager.h
	mod.h
	modmanager.h
	path.h
	pathsurfaces.h
	ped.h
	pedmanager.h
	resources.h
	system.h
	system_sdl.h
	vehicle.h
	vehiclemanager.h
	version.h
	weapon.h
	weaponholder.h
	weaponmanager.h
	core/gameevent.h
	core/gamesession.h
	core/missionbriefing.h
	core/research.h
	core/researchmanager.h
	gfx/dirtylist.h
	gfx/fliplayer.h
	gfx/font.h
	gfx/fontmanager.h
	gfx/screen.h
	gfx/sprite.h
	gfx/spritemanager.h
	gfx/tile.h
	gfx/tilemanager.h
	menus/minimaprenderer.h
	menus/briefmenu.h
	menus/confmenu.h
	menus/debriefmenu.h
	menus/flimenu.h
	menus/gameplaymenu.h
	menus/loadingmenu.h
	menus/loadsavemenu.h
	menus/logoutmenu.h
	menus/mainmenu.h
	menus/mapmenu.h
	menus/menu.h
	menus/menumanager.h
	menus/researchmenu.h
	menus/selectmenu.h
	menus/widget.h
	sound/audio.h
	sound/music.h
	sound/musicmanager.h
	sound/sdlmixermusic.h
	sound/sdlmixersound.h
	sound/sound.h
	sound/soundmanager.h
	sound/xmidi.h
	utils/configfile.h
	utils/ccrc32.h
	utils/dernc.h
	utils/file.h
	utils/log.h
	utils/portablefile.h
	utils/seqmodel.h
	utils/singleton.h
	utils/utf8.h
	utils/utf8/checked.h
	utils/utf8/core.h
	utils/utf8/unchecked.h)

source_group(Sources REGULAR_EXPRESSION .*.cpp)
source_group(Headers REGULAR_EXPRESSION .*.h)
source_group(menus REGULAR_EXPRESSION .*menu.cpp FILES widget.cpp)
source_group(menus REGULAR_EXPRESSION .*menu.h FILES widget.cpp)
source_group(utils FILES log.cpp log.h)

add_executable (freesynd ${SOURCES} ${HEADERS})

target_link_libraries (freesynd ${PNG_LIBRARIES} ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLMIXER_LIBRARY})

if (BUILD_DEV_TOOLS)
	add_executable (dump
		dump.cpp
		gfx/sprite.cpp
		gfx/spritemanager.cpp
		gfx/tile.cpp
		gfx/tilemanager.cpp
		map.cpp
		mapmanager.cpp
		utils/dernc.cpp
		utils/file.cpp
		utils/log.cpp
		utils/portablefile.cpp
	)
	target_link_libraries (dump ${PNG_LIBRARIES})
else ()
	# We only define an install target if we're doing a release build.
	if (APPLE)
		install (TARGETS freesynd DESTINATION /Applications)
	else ()
		install (TARGETS freesynd DESTINATION bin)
	endif ()
endif ()

if (APPLE)
	# Override certain properties to make the freesynd
	# executable into an application bundle for OS X.
	set_target_properties (freesynd PROPERTIES
		OUTPUT_NAME "FreeSynd"
		MACOSX_BUNDLE true
		MACOSX_BUNDLE_BUNDLE_NAME "FreeSynd"
		MACOSX_BUNDLE_ICON_FILE "sword.icns"
		MACOSX_BUNDLE_GUI_IDENTIFIER "com.freesynd.FreeSynd"
		MACOSX_BUNDLE_SHORT_VERSION_STRING "${FREESYND_VERSION}"
		MACOSX_BUNDLE_COPYRIGHT "© 2011 FreeSynd Team"
	)
	include (tools/osx.cmake)
	copy_resource_file (freesynd "Copying icon" ${CMAKE_SOURCE_DIR}/icon/sword.icns)
	copy_resource_folder (freesynd "Copying data" ${CMAKE_SOURCE_DIR}/data)
	include_libs (freesynd "Including non-standard OS X libraries")
endif()

