#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"   /* All SDL App's need this */

#include <inttypes.h>
#include <assert.h>

#include "Syndicate/Data/Palette.h"
#include "Syndicate/Data/MapData.h"

SDL_Surface *screen = 0;

Syndicate::Data::Palette *palette = 0;
Syndicate::Data::MapData *map_data = 0;

unsigned map_index = 0;

int main(int argc, char *argv[]) 
{
  while(--argc)
    {
      map_index = atoi(argv[argc]);
    }

  printf("Initializing SDL.\n");
    
  /* Initialize defaults, Video */
  if((SDL_Init(SDL_INIT_VIDEO)==-1)) 
    { 
      printf("Could not initialize SDL: %s.\n", SDL_GetError());
      exit(-1);
    }

  printf("SDL initialized.\n");

  screen = SDL_SetVideoMode(640, 480, 8, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
  if ( screen == NULL ) 
    {
      fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n", SDL_GetError());
      exit(1);
    }

  /* Clean up on exit, exit on window close and interrupt */
  atexit(SDL_Quit);

  /* Init palette */
  palette = new Syndicate::Data::Palette();
  palette->load("HPAL02.DAT");
  static SDL_Color colors[256];
  for(unsigned index = 0; index < 256; index++)
    {
      palette->get(index, colors[index].r, colors[index].g, colors[index].b);
    }
  SDL_SetColors(screen, colors, 0, 256);

  map_data = new Syndicate::Data::MapData();
  map_data->load("MAP01.DAT");

  for(unsigned k = 0; k < map_data->nb_k(); k++)
    {
      for(unsigned j = 0; j < map_data->nb_j(); j++)
	{
	  for(unsigned i = 0; i < map_data->nb_i(); i++)
	    {
	      unsigned id = 0;
	      map_data->get(i, j, k, id);
	      assert(id < 256);
	      //printf("%u %u %u : %u\n", i, j, k, id);
	    }
	}
    }

  SDL_FillRect(screen, 0, 255);

  SDL_UpdateRect(screen, 0, 0, 0, 0);

  /* Loop waiting for ESC+Mouse_Button */
  SDL_Event event;
  bool stop = false;
  while (!stop) 
    {
      while (SDL_PollEvent(&event)) {
	switch (event.type) 
	  {
	  case SDL_QUIT: 
	    {
	      printf("Quit requested, quitting.\n");
	      stop = true;
	    }
	    break;
	  }
      }
    }

  delete map_data;
  exit(0);
}
