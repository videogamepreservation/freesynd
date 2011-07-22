# Add this folder to the include directories.
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# Generate the ini header, add it's location to the include directories.
include (tools/embed.cmake)
embed_data (../freesynd.ini embedded_default_ini default_ini.h)
include_directories (${CMAKE_CURRENT_BINARY_DIR})

add_executable (freesynd
	agent.cpp
	agentmanager.cpp
	app.cpp
	core/gamesession.cpp
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
	menus/briefmenu.cpp
	menus/confmenu.cpp
	menus/debriefmenu.cpp
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
	sound/audio.cpp
	sound/musicmanager.cpp
	sound/sdlmixermusic.cpp
	sound/sdlmixersound.cpp
	sound/soundmanager.cpp
	sound/xmidi.cpp
	system_sdl.cpp
	utils/configfile.cpp
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
target_link_libraries (freesynd ${PNG_LIBRARY} ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLMIXER_LIBRARY})

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
target_link_libraries (dump ${PNG_LIBRARY})

if (APPLE)
	# Override certain properties to make the freesynd
	# executable into an application bundle for OS X.
	set_target_properties (freesynd PROPERTIES
		OUTPUT_NAME "FreeSynd"
		MACOSX_BUNDLE true
		MACOSX_BUNDLE_BUNDLE_NAME "FreeSynd"
		MACOSX_BUNDLE_ICON_FILE "sword.icns"
		MACOSX_BUNDLE_GUI_IDENTIFIER "com.freesynd.FreeSynd"
		MACOSX_BUNDLE_SHORT_VERSION_STRING "0.5"
		MACOSX_BUNDLE_COPYRIGHT "© 2011 FreeSynd Team"
	)
	include (tools/osx.cmake)
	copy_resource_file (freesynd "Copying icon" ${CMAKE_SOURCE_DIR}/icon/sword.icns)
	copy_resource_folder (freesynd "Copying data" ${CMAKE_SOURCE_DIR}/data our_data)
	include_libs (freesynd "Including non-standard OS X libraries")
endif()

