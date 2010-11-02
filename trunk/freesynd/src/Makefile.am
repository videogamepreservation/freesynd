bin_PROGRAMS = freesynd dump

AM_CXXFLAGS = -Wall -Wshadow -Wimplicit -Wreorder -Wnon-virtual-dtor -Wno-multichar -MMD ${SDL_CFLAGS}

AM_CPPFLAGS = -DSYSTEM_SDL -DUSE_RWOPS

freesynd_LDADD = ${SDL_LIBS} -lpng -lSDL_mixer -lSDL_image

freesynd_SOURCES = common.h config.h keys.h singleton.h \
	path.h pathsurfaces.h \
	utils/file.cpp utils/file.h \
	utils/dernc.cpp utils/dernc.h\
	utils/log.cpp utils/log.h \
	core/gamesession.cpp core/gamesession.h \
	sound/audio.cpp sound/audio.h \
	sound/soundmanager.cpp sound/soundmanager.h sound/sound.h \
	sound/musicmanager.cpp sound/musicmanager.h sound/music.h \
	sound/sdlmixermusic.cpp sound/sdlmixermusic.h \
	sound/sdlmixersound.cpp sound/sdlmixersound.h \
	sound/xmidi.cpp sound/xmidi.h
	agent.cpp agent.h \
	agentmanager.cpp agentmanager.h \
	app.cpp app.h \
	briefmenu.cpp briefmenu.h \
	confmenu.cpp confmenu.h \
	debriefmenu.cpp debriefmenu.h \
	fliplayer.cpp fliplayer.h \
	font.cpp font.h \
	fontmanager.cpp fontmanager.h \
	freesynd.cpp \
	gameplaymenu.cpp gameplaymenu.h \
	loadingmenu.cpp loadingmenu.h \
	loadsavemenu.cpp loadsavemenu.h \
	logoutmenu.cpp logoutmenu.h \
	mainmenu.cpp mainmenu.h \
	map.cpp map.h \
	mapobject.cpp mapobject.h \
	mapmanager.cpp mapmanager.h \
	mapmenu.cpp mapmenu.h \
	menu.cpp menu.h \
	menumanager.cpp menumanager.h \
	mission.cpp mission.h \
	missionmanager.cpp missionmanager.h \
	misslosemenu.cpp misslosemenu.h \
	misswinmenu.cpp misswinmenu.h \
	mod.cpp mod.h \
	modmanager.cpp modmanager.h \
	ped.cpp ped.h \
	pedmanager.cpp pedmanager.h \
	researchmenu.cpp researchmenu.h \
	screen.cpp screen.h \
	selectmenu.cpp selectmenu.h \
	sprite.cpp sprite.h \
	spritemanager.cpp spritemanager.h \
	system_sdl.cpp system_sdl.h system.h \
	tile.cpp tile.h \
	tilemanager.cpp tilemanager.h \
	vehicle.cpp vehicle.h \
	vehiclemanager.cpp vehiclemanager.h \
	weapon.cpp weapon.h weaponholder.h \
	weaponmanager.cpp weaponmanager.h

dump_LDADD = ${SDL_LIBS} -lpng

dump_SOURCES = \
	dump.cpp \
	sprite.cpp \
	spritemanager.cpp \
	utils/file.cpp \
	utils/dernc.cpp