#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"   /* All SDL App's need this */

#include <inttypes.h>

#include "Syndicate/Data/Palette.h"
#include "Syndicate/Data/MapTile.h"
#include "Syndicate/Data/Req.h"

SDL_Surface *screen = 0;

Syndicate::Data::Palette *palette = 0;
Syndicate::Data::MapTile *map_tile = 0;
Syndicate::Data::Req *req = 0;

unsigned tile_index = 0;

int main(int argc, char *argv[]) 
{
  while(--argc)
    {
      tile_index = atoi(argv[argc]);
    }

  printf("Initializing SDL.\n");
    
  /* Initialize defaults, Video */
  if((SDL_Init(SDL_INIT_VIDEO)==-1)) 
    { 
      printf("Could not initialize SDL: %s.\n", SDL_GetError());
      exit(-1);
    }
  atexit(SDL_Quit);
  printf("SDL initialized.\n");

  /* Init palette */
  palette = new Syndicate::Data::Palette();
  palette->load("HPAL02.DAT");
  static SDL_Color colors[256];
  for(unsigned index = 0; index < 256; index++)
    {
      palette->get(index, colors[index].r, colors[index].g, colors[index].b);
    }

  map_tile = new Syndicate::Data::MapTile();
  map_tile->load("HBLK01.DAT");

  req = new Syndicate::Data::Req();
  req->load("HREQ.DAT");

  if(tile_index)
    {
      screen = SDL_SetVideoMode(64, 48, 8, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
      if ( screen == NULL ) 
	{
	  fprintf(stderr, "Couldn't set 64x48x8 video mode: %s\n", SDL_GetError());
	  exit(1);
	}
      SDL_SetColors(screen, colors, 0, 256);
      SDL_FillRect(screen, 0, 255);

      map_tile->get(tile_index, (uint8_t *)screen->pixels, screen->pitch);
#if 0
      for(unsigned y = 0; y < Syndicate::Data::MapTile::_height; y++)
	{
	  for(unsigned x = 0; x < Syndicate::Data::MapTile::_width; x++)
	    {
	      printf("%02x ", *((uint8_t *)screen->pixels + y * screen->pitch + x));
	    }
	  printf("\n");
	}
#endif
    }
  else
    {
      screen = SDL_SetVideoMode(1024, 800, 8, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
      if ( screen == NULL ) 
	{
	  fprintf(stderr, "Couldn't set 1024x800x8 video mode: %s\n", SDL_GetError());
	  exit(1);
	}
      SDL_SetColors(screen, colors, 0, 256);
      SDL_FillRect(screen, 0, 255);

      uint8_t *pixels = (uint8_t *)screen->pixels;
      unsigned j = 0;
      for(unsigned i = 0; i < 256; i++)
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
	  if(j == 16)
	    {
	      pixels += (48 * screen->pitch);
	      j = 0;
	    }
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

  delete palette;
  delete map_tile;
  exit(0);
}
