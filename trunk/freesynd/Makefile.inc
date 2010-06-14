CXX = g++

SDL_CFLAGS = `sdl-config --cflags`

CXXFLAGS := -g -Wall -Wshadow -Wimplicit -Wreorder -Wnon-virtual-dtor -Wno-multichar -DSYSTEM_SDL -DUSE_RWOPS -D_DEBUG

CXXFLAGS += $(SDL_CFLAGS)

LIBS = `sdl-config --libs` -lSDL_mixer -lSDL_image -lpng