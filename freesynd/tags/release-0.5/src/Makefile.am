bin_PROGRAMS = freesynd dump

AM_CXXFLAGS = -Wall -Wshadow -Wimplicit -Wreorder -Wnon-virtual-dtor -Wno-multichar -MMD ${SDL_CFLAGS}

AM_CPPFLAGS = -DSYSTEM_SDL -DUSE_RWOPS

freesynd_LDADD = ${SDL_LIBS} -lpng -lSDL_mixer -lSDL_image

freesynd_SOURCES = common.h config.h keys.h utils/singleton.h  core/gameevent.h\
	path.h pathsurfaces.h \
	utils/file.cpp utils/file.h \
	utils/dernc.cpp utils/dernc.h\
	utils/log.cpp utils/log.h \
	utils/configfile.cpp utils/configfile.h \
	utils/seqmodel.cpp utils/seqmodel.h \
	core/gamesession.cpp core/gamesession.h \
	core/research.cpp core/research.h \
	core/researchmanager.cpp core/researchmanager.h \
	sound/audio.cpp sound/audio.h \
	sound/soundmanager.cpp sound/soundmanager.h sound/sound.h \
	sound/musicmanager.cpp sound/musicmanager.h sound/music.h \
	sound/sdlmixermusic.cpp sound/sdlmixermusic.h \
	sound/sdlmixersound.cpp sound/sdlmixersound.h \
	sound/xmidi.cpp sound/xmidi.h \
	gfx/dirtylist.cpp gfx/dirtylist.h \
	gfx/fliplayer.cpp gfx/fliplayer.h \
	gfx/font.cpp gfx/font.h \
	gfx/fontmanager.cpp gfx/fontmanager.h \
	gfx/screen.cpp gfx/screen.h \
	gfx/sprite.cpp gfx/sprite.h \
	gfx/spritemanager.cpp gfx/spritemanager.h \
	gfx/tile.cpp gfx/tile.h \
	gfx/tilemanager.cpp gfx/tilemanager.h \
	menus/widget.cpp menus/widget.h \
	agent.cpp agent.h \
	agentmanager.cpp agentmanager.h \
	app.cpp app.h \
	briefmenu.cpp briefmenu.h \
	confmenu.cpp confmenu.h \
	debriefmenu.cpp debriefmenu.h \
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
	mod.cpp mod.h \
	modmanager.cpp modmanager.h \
	ped.cpp ped.h \
	pedmanager.cpp pedmanager.h \
	researchmenu.cpp researchmenu.h \
	selectmenu.cpp selectmenu.h \
	system_sdl.cpp system_sdl.h system.h \
	vehicle.cpp vehicle.h \
	vehiclemanager.cpp vehiclemanager.h \
	weapon.cpp weapon.h weaponholder.h \
	weaponmanager.cpp weaponmanager.h

dump_LDADD = ${SDL_LIBS} -lpng

dump_SOURCES = \
	dump.cpp \
	gfx/sprite.cpp \
	gfx/spritemanager.cpp \
	map.cpp \
	mapmanager.cpp \
	gfx/tile.cpp \
	gfx/tilemanager.cpp \
	utils/file.cpp \
	utils/dernc.cpp
