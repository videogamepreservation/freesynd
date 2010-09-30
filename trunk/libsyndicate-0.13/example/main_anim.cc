#include <stdio.h>
#include <stdlib.h>

#include <inttypes.h>

#include "SDL.h"

#include "Syndicate/Data/Palette.h"
#include "Animation.h"

SDL_Surface *screen = 0;

Syndicate::Data::Palette *palette = 0;
Animation *anim = 0;

unsigned anim_index = 69;

Uint32 anim_callback(Uint32 interval, void *param)
{
  static unsigned frame_num = 0;
  static const unsigned x_offset = 128;
  static const unsigned y_offset = 128;
  
  SDL_FillRect(screen, 0, 255);

  uint8_t *pixels = reinterpret_cast<uint8_t *>(screen->pixels) + y_offset * screen->pitch + x_offset;

  anim->get(pixels, screen->pitch, frame_num);

  frame_num++;

  SDL_UpdateRect(screen, 0, 0, 0, 0);

  return interval;
}

int main(int argc, char *argv[]) 
{
  while(--argc)
    {
      anim_index = strtol(argv[argc], 0, 0);
    }

  printf("Initializing SDL.\n");
    
  /* Initialize defaults, Video */
  if((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)==-1)) 
    { 
      printf("Could not initialize SDL: %s.\n", SDL_GetError());
      exit(-1);
    }

  printf("SDL initialized.\n");

  screen = SDL_SetVideoMode(640, 480, 8, SDL_HWSURFACE | SDL_DOUBLEBUF| SDL_RESIZABLE);
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

  /* Init anim */
  anim = new Animation("HSTA-0.ANI", "HFRA-0.ANI", "HELE-0.ANI", "HSPR-0.TAB", "HSPR-0.DAT");
  anim->set(anim_index);

  /* clear screen */
  SDL_FillRect(screen, 0, 255);
  SDL_UpdateRect(screen, 0, 0, 0, 0);

  /* add timer for frame updates */
  SDL_TimerID timer_id = SDL_AddTimer(1000, anim_callback, 0);

  /* Loop waiting for ESC+Mouse_Button */
  SDL_Event event;
  bool stop = false;
  while (!stop) 
    {
      while (SDL_PollEvent(&event)) 
	{
	  switch (event.type) 
	    {
	    case SDL_QUIT: 
	      {
		printf("Quit requested, quitting.\n");
		stop = true;
	      }
	      break;
	    case SDL_VIDEORESIZE:
	      {
		if(!stop)
		  {
		    screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 8, SDL_HWSURFACE | SDL_DOUBLEBUF| SDL_RESIZABLE);
		    SDL_SetColors(screen, colors, 0, 256);
		  }
	      }
	      break;
	    }
	}
    }

  delete palette;
  delete anim;
  exit(0);
}
