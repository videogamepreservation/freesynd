#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"   /* All SDL App's need this */

#include <inttypes.h>

#include "Syndicate/Data/Palette.h"
#include "Syndicate/Data/Font.h"

SDL_Surface *screen = 0;

Syndicate::Data::Palette *palette = 0;

Syndicate::Data::Font *font = 0;

void draw_font()
{
  if(screen && font)
    {
      SDL_FillRect(screen, 0, 255);

      for(unsigned i = 0; i < Syndicate::Data::Font::_nb_fonts; i ++)
	{
	  static const unsigned width = 16;
	  static const unsigned height = 16;

	  unsigned x_offset = (width * i) % screen->pitch;
	  unsigned y_offset = (width * i) / screen->pitch * height;
	  uint8_t *pixels = (uint8_t *)screen->pixels + y_offset * screen->pitch + x_offset;
	  font->get(i, pixels, screen->pitch);
	}

      SDL_UpdateRect(screen, 0, 0, 0, 0);
    }
}

int main(int argc, char *argv[]) 
{
  printf("Initializing SDL.\n");
    
  /* Initialize defaults, Video */
  if((SDL_Init(SDL_INIT_VIDEO)==-1)) 
    { 
      printf("Could not initialize SDL: %s.\n", SDL_GetError());
      exit(-1);
    }

  printf("SDL initialized.\n");

  screen = SDL_SetVideoMode(110, 310, 8, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
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
  for(unsigned i = 0; i < 256; i++)
    {
      palette->get(i, colors[i].r, colors[i].g, colors[i].b);
    }
  SDL_SetColors(screen, colors, 0, 256);

  font = new Syndicate::Data::Font();
  font->load("HFNT01.DAT");
  draw_font();

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
	      printf("Quit fontuested, quitting.\n");
	      stop = true;
	    }
	  case SDL_VIDEORESIZE:
	    {
	      if(!stop)
		{
		  screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 8, SDL_HWSURFACE | SDL_DOUBLEBUF| SDL_RESIZABLE);
		  SDL_SetColors(screen, colors, 0, 256);
		  draw_font();
		}
	    }
	    break;
	  }
      }
    }

  delete font;
  delete palette;
  exit(0);
}
