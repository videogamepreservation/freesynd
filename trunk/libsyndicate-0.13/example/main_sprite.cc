#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"   /* All SDL App's need this */

#include <inttypes.h>

#include "Syndicate/Data/Palette.h"
#include "Syndicate/Data/SpriteTab.h"
#include "Syndicate/Data/SpriteData.h"

SDL_Surface *screen = 0;

Syndicate::Data::Palette *palette = 0;
Syndicate::Data::SpriteTab *tab = 0;
Syndicate::Data::SpriteData *data = 0;

unsigned sprite_index = 0;

int main(int argc, char *argv[]) 
{
  while(--argc)
    {
      sprite_index = atoi(argv[argc]);
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
  palette->load("MSELECT.PAL");
  static SDL_Color colors[256];
  for(unsigned index = 0; index < 256; index++)
    {
      palette->get(index, colors[index].r, colors[index].g, colors[index].b);
    }
  SDL_SetColors(screen, colors, 0, 256);

  tab = new Syndicate::Data::SpriteTab();
  data = new Syndicate::Data::SpriteData();
#if 0
  tab->load("HSPR-0.TAB");
  data->load("HSPR-0.DAT");
#else
  tab->load("MSPR-0.TAB");
  data->load("MSPR-0.DAT");
#endif

  SDL_FillRect(screen, 0, 255);

  unsigned offset = 0, width = 0, height = 0;

  tab->get(sprite_index, offset, width, height);
  printf("offset = %u widh = %d height = %d\n", offset, width, height);

  data->get(offset, width, height, false, (uint8_t *)screen->pixels, screen->pitch);
  
  SDL_UpdateRect(screen, 0, 0, 0, 0);

  SDL_Event event;
  while (1) 
    {
      while (SDL_PollEvent(&event)) {
	switch (event.type) 
	  {
	  case SDL_QUIT: 
	    {
	      printf("Quit requested, quitting.\n");
	      delete tab;
	      delete data;
	      exit(0);
	    }
	    break;
	  }
      }
    }

  delete tab;
  delete data;
  exit(0);
}
