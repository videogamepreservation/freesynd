# FreeSynd makefile
#
# $Id$

SDL_CFLAGS = `sdl-config --cflags`
SDL_LIBS = `sdl-config --libs`

CXX = g++
CXXFLAGS := -O2 -Wall -Wuninitialized -Wshadow -Wimplicit -Wreorder -Wnon-virtual-dtor -Wno-multichar -DSYSTEM_SDL
CXXFLAGS += $(SDL_CFLAGS) $(DEFINES)

TARGET = freesynd
SRC = ./src/freesynd.cpp

OBJS = $(SRC:.cpp=.o)
DEPS = $(SRC:.cpp=.d)

$(TARGET): $(OBJS)
	-mkdir ./dist
	$(CXX) $(LDFLAGS) -o ./dist/$@ $(OBJS) $(SDL_LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $*.o

clean:
	rm -rf ./dist ./src/*.o ./src/*.d

-include $(DEPS)
