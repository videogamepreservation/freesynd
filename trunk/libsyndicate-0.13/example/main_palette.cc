#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include <inttypes.h>

#include "SDL.h"

#include "Syndicate/Data/Palette.h"

SDL_Surface *screen = 0;

Syndicate::Data::Palette *palette = 0;

void draw_palette()
{
  if(screen)
    {
      SDL_FillRect(screen, 0, 255);

      for(unsigned i = 0; i < 256; i ++)
	{
	  unsigned size = 32;
	  SDL_Rect rect;
	  rect.x = (i % (screen->w / size)) * size;
	  rect.y = (i / (screen->w / size)) * size;
	  rect.w = size;
	  rect.h = size;
	  SDL_FillRect(screen, &rect, i);
	}

      SDL_UpdateRect(screen, 0, 0, 0, 0);
    }
}

int main(int argc, char *argv[]) 
{
  unsigned palette_index = 1;
  while(--argc)
    {
      palette_index = strtol(argv[argc], 0, 0);
    }

  printf("Initializing SDL.\n");
  if((SDL_Init(SDL_INIT_VIDEO)==-1)) 
    { 
      printf("Could not initialize SDL: %s.\n", SDL_GetError());
      exit(-1);
    }
  printf("SDL initialized.\n");

  screen = SDL_SetVideoMode(640, 480, 8, SDL_HWPALETTE | SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
  if ( screen == NULL ) 
    {
      fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n", SDL_GetError());
      exit(1);
    }

  /* Clean up on exit, exit on window close and interrupt */
  atexit(SDL_Quit);

  /* Init palette */
  palette = new Syndicate::Data::Palette();
  char palette_name[PATH_MAX] = {0};
  snprintf(palette_name, sizeof(palette_name), "HPAL%02d.DAT", palette_index);
  palette->load(palette_name);

  static SDL_Color colors[256];
  for(unsigned index = 0; index < 256; index++)
    {
      palette->get(index, colors[index].r, colors[index].g, colors[index].b);
    }
  SDL_SetColors(screen, colors, 0, 256);
  draw_palette();

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
	  case SDL_VIDEORESIZE:
	    {
	      if(!stop)
		{
		  screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 8, SDL_HWPALETTE | SDL_HWSURFACE | SDL_DOUBLEBUF| SDL_RESIZABLE);
		  SDL_SetColors(screen, colors, 0, 256);
		  draw_palette();
		}
	    }
	    break;
	  }
      }
    }

  delete palette;
  exit(0);
}
