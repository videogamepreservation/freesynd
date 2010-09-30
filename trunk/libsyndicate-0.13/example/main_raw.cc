#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"   /* All SDL App's need this */

#include "Syndicate/Data/Palette.h"
#include "Syndicate/Data/Raw.h"

SDL_Surface *screen = 0;

Syndicate::Data::Palette *palette = 0;
Syndicate::Data::Raw *raw = 0;

char *file = 0;

int main(int argc, char *argv[]) 
{
  if(argc < 2)
    {
      printf("usage %s filename\n", argv[0]);
      return 1;
    }
  file = argv[1];

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

  raw = new Syndicate::Data::Raw();
  raw->load(file);

  SDL_FillRect(screen, 0, 255);

  unsigned id = 0, width = 64, height = 44;

  raw->get(id, width, height, (uint8_t *)screen->pixels, screen->pitch);
  
  SDL_UpdateRect(screen, 0, 0, 0, 0);

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

  delete raw;
  exit(0);
}
