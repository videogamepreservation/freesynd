#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"   /* All SDL App's need this */

#include <inttypes.h>

#include "Syndicate/Data/Palette.h"
#include "Syndicate/Data/Req.h"

SDL_Surface *screen = 0;

Syndicate::Data::Palette *palette = 0;

Syndicate::Data::Req *req = 0;

void draw_req()
{
  if(screen && req)
    {
      SDL_FillRect(screen, 0, 255);

      for(unsigned i = 0; i < 480; i ++)
	{
	  unsigned x_offset = ((Syndicate::Data::Req::_width + 2) * i) % screen->pitch;
	  unsigned y_offset = ((Syndicate::Data::Req::_width + 2) * i) / screen->pitch * Syndicate::Data::Req::_height + 2;
	  uint8_t *pixels = (uint8_t *)screen->pixels + y_offset * screen->pitch + x_offset;
	  req->get(i, pixels, screen->pitch);
	}
      printf("%d\n", screen->pitch / Syndicate::Data::Req::_width);

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

  req = new Syndicate::Data::Req();
  req->load("HREQ.DAT");
  draw_req();

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
		  screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 8, SDL_HWSURFACE | SDL_DOUBLEBUF| SDL_RESIZABLE);
		  SDL_SetColors(screen, colors, 0, 256);
		  draw_req();
		}
	    }
	    break;
	  }
      }
    }

  delete req;
  delete palette;
  exit(0);
}
