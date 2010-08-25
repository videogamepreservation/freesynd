include Makefile.inc

TARGET	= freesynd
SRC	= agent.cpp agentmanager.cpp app.cpp audio.cpp briefmenu.cpp confmenu.cpp \
	debriefmenu.cpp dernc.cpp file.cpp fliplayer.cpp font.cpp \
	fontmanager.cpp freesynd.cpp gameplaymenu.cpp loadingmenu.cpp \
	loadsavemenu.cpp logoutmenu.cpp mainmenu.cpp map.cpp mapobject.cpp \
	mapmanager.cpp mapmenu.cpp menu.cpp menumanager.cpp mission.cpp \
	missionmanager.cpp misslosemenu.cpp misswinmenu.cpp \
	mod.cpp modmanager.cpp musicmanager.cpp \
	ped.cpp pedmanager.cpp researchmenu.cpp \
	screen.cpp selectmenu.cpp soundmanager.cpp \
	sprite.cpp spritemanager.cpp system_sdl.cpp \
	tile.cpp tilemanager.cpp vehicle.cpp vehiclemanager.cpp weapon.cpp \
	weaponmanager.cpp xmidi.cpp

DUMP_TARGET = dump
DUMP_OBJS = dump.o spritemanager.o sprite.o file.o dernc.o

DUMPMAPS_TARGET = dumpmaps
DUMPMAPS_OBJS = dumpmaps.o mapmanager.o map.o tilemanager.o tile.o file.o dernc.o

DIRS	= utils sound core
OBJS = $(SRC:.cpp=.o)
SND_OBJ	= sound/sdlmixermusic.o sound/sdlmixersound.o
UTL_OBJ	= utils/log.o
CORE_OBJ= core/gamesession.o
DEPS = $(SRC:.cpp=.d)

all: $(TARGET) $(DUMP_TARGET) $(DUMPMAPS_TARGET)

$(TARGET): $(CORE_OBJ) $(UTL_OBJ) $(SND_OBJ) $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(SND_OBJ) $(UTL_OBJ) $(CORE_OBJ) $(LIBS)

$(DUMP_TARGET): $(DUMP_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(DUMP_OBJS) -lpng

$(DUMPMAPS_TARGET): $(DUMPMAPS_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(DUMPMAPS_OBJS) -lpng

$(CORE_OBJ): force_look
	cd core;  $(MAKE) $(MFLAGS)

$(SND_OBJ): force_look
	cd utils; $(MAKE) $(MFLAGS)

$(UTL_OBJ): force_look
	cd sound; $(MAKE) $(MFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $*.o

clean:
	rm -f $(TARGET) $(DUMP_TARGET) $(DUMPMAPS_TARGET) *.o *.d
	-for d in $(DIRS); do (cd $$d; $(MAKE) clean ); done

force_look:
	true

-include $(DEPS)
