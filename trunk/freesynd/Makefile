
SDL_CFLAGS = `sdl-config --cflags`
SDL_LIBS = `sdl-config --libs`
SDL_MIXER_LIBS = -lSDL_mixer

CXX = g++
CXXFLAGS := -g -Wall -Wshadow -Wimplicit -Wreorder -Wnon-virtual-dtor -Wno-multichar -DSYSTEM_SDL -DUSE_RWOPS
#CXXFLAGS := -g -O2 -Wall -Wuninitialized -Wshadow -Wimplicit -Wreorder -Wnon-virtual-dtor -Wno-multichar -DSYSTEM_SDL -DUSE_RWOPS
CXXFLAGS += $(SDL_CFLAGS) $(DEFINES)

TARGET = freesynd
SRC = agent.cpp agentmanager.cpp app.cpp audio.cpp briefmenu.cpp confmenu.cpp \
	debriefmenu.cpp dernc.cpp file.cpp fliplayer.cpp font.cpp \
	fontmanager.cpp freesynd.cpp gameplaymenu.cpp loadingmenu.cpp \
	loadsavemenu.cpp logoutmenu.cpp mainmenu.cpp map.cpp mapobject.cpp \
	mapmanager.cpp mapmenu.cpp menu.cpp menumanager.cpp mission.cpp \
	missionmanager.cpp misslosemenu.cpp misswinmenu.cpp \
	mod.cpp modmanager.cpp music.cpp musicmanager.cpp \
	ped.cpp pedmanager.cpp researchmenu.cpp \
	screen.cpp selectmenu.cpp sound.cpp soundmanager.cpp \
	sprite.cpp spritemanager.cpp system_sdl.cpp \
	tile.cpp tilemanager.cpp vehicle.cpp vehiclemanager.cpp weapon.cpp \
	weaponmanager.cpp xmidi.cpp

DUMP_TARGET = dump
DUMP_OBJS = dump.o spritemanager.o sprite.o file.o dernc.o

DUMPMAPS_TARGET = dumpmaps
DUMPMAPS_OBJS = dumpmaps.o mapmanager.o map.o tilemanager.o tile.o file.o dernc.o

OBJS = $(SRC:.cpp=.o)
DEPS = $(SRC:.cpp=.d)

all: $(TARGET) $(DUMP_TARGET) $(DUMPMAPS_TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(SDL_LIBS) $(SDL_MIXER_LIBS) -lpng

$(DUMP_TARGET): $(DUMP_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(DUMP_OBJS) -lpng

$(DUMPMAPS_TARGET): $(DUMPMAPS_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(DUMPMAPS_OBJS) -lpng

.cpp.o:
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $*.o

clean:
	rm -f $(TARGET) *.o *.d

-include $(DEPS)
