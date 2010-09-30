#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"   /* All SDL App's need this */

#include <inttypes.h>

#include "Syndicate/Data/Palette.h"
#include "Syndicate/Data/MapTile.h"
#include "Syndicate/Data/MapColumn.h"
#include "Syndicate/Data/Req.h"

SDL_Surface *screen = 0;

Syndicate::Data::Palette *palette = 0;
Syndicate::Data::MapTile *map_tile = 0;
Syndicate::Data::MapColumn *map_column = 0;
Syndicate::Data::Req *req = 0;

unsigned type_index = 1;

int main(int argc, char *argv[]) 
{
  while(--argc)
    {
      type_index = atoi(argv[argc]);
    }

  /* Initialize defaults, Video */
  printf("Initializing SDL.\n");
  if((SDL_Init(SDL_INIT_VIDEO)==-1)) 
    { 
      printf("Could not initialize SDL: %s.\n", SDL_GetError());
      exit(-1);
    }
  atexit(SDL_Quit);
  printf("SDL initialized.\n");

  /* init data */
  map_tile = new Syndicate::Data::MapTile();
  map_tile->load("HBLK01.DAT");

  map_column = new Syndicate::Data::MapColumn();
  map_column->load("COL01.DAT");

  req = new Syndicate::Data::Req();
  req->load("HREQ.DAT");

  unsigned width = 640;
  unsigned height = 1740;

  /* init screen */
  screen = SDL_SetVideoMode(width, height, 8, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
  if ( screen == NULL ) 
    {
      fprintf(stderr, "Couldn't set %dx%dx8 video mode: %s\n", width, height, SDL_GetError());
      exit(1);
    }
  SDL_FillRect(screen, 0, 255);
  uint8_t *pixels = (uint8_t *)screen->pixels;

  /* init palette */
  palette = new Syndicate::Data::Palette();
  palette->load("HPAL02.DAT");
  static SDL_Color colors[256];
  for(unsigned index = 0; index < 256; index++)
    {
      palette->get(index, colors[index].r, colors[index].g, colors[index].b);
    }
  SDL_SetColors(screen, colors, 0, 256);

  for(type_index = 0; type_index < 17; type_index++)
    {
      unsigned j = 0;
      /* draw tiles */
      for(unsigned i = 0; i < 255; i++)
	{
	  uint8_t type = 0;
	  map_column->get(i, type);
	  if(type == type_index)
	    {
	      map_tile->get(i, pixels, screen->pitch);
	      unsigned c = i / 100;
	      unsigned d = (i % 100) / 10;
	      unsigned u = (i % 10);

	      req->get(16 + c, pixels, screen->pitch);
	      req->get(16 + d, pixels + 8, screen->pitch);
	      req->get(16 + u, pixels + 16, screen->pitch);
	      pixels += 64;
	      j ++;
	      if(j == 10)
		{
		  pixels += (48 * screen->pitch);
		  j = 0;
		}
	    }
	}

      unsigned d = type_index / 10;
      unsigned u = (type_index % 10);

      req->get(176 + d, pixels, screen->pitch);
      req->get(176 + u, pixels + 8, screen->pitch);
	
      pixels += ((10 - j) * 64 + 48 * screen->pitch);
      for(unsigned x = 0; x < 640; x++)
	{
	  pixels[x] = 3;
	}
    }
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

  delete map_tile;
  delete map_column;
  exit(0);
}
