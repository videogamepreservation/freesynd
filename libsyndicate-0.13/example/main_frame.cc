#include <stdio.h>
#include <stdlib.h>

#include <inttypes.h>

#include "SDL.h"

#include "Syndicate/Data/Palette.h"
#include "Animation.h"
#include "Syndicate/Data/Req.h"

SDL_Surface *screen = 0;

Animation *anim = 0;
Syndicate::Data::Palette *palette = 0;
Syndicate::Data::Req *req = 0;

unsigned frame_index = 0;

int main(int argc, char *argv[]) 
{
  while(--argc)
    {
      frame_index = strtol(argv[argc], 0, 0);
    }

  printf("Initializing SDL.\n");
  /* Initialize defaults, Video */
  if((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)==-1)) 
    { 
      printf("Could not initialize SDL: %s.\n", SDL_GetError());
      exit(-1);
    }

  /* Clean up on exit, exit on window close and interrupt */
  atexit(SDL_Quit);

  printf("SDL initialized.\n");

  /* Init anim */
  anim = new Animation("HSTA-0.ANI", "HFRA-0.ANI", "HELE-0.ANI", "HSPR-0.TAB", "HSPR-0.DAT");

#if 0
  req = new Syndicate::Data::Req();
  req->load("HREQ.DAT");

  /* Init palette */
  palette = new Syndicate::Data::Palette();
  palette->load("HPAL02.DAT");


  static SDL_Color colors[256];
  for(unsigned index = 0; index < 256; index++)
    {
      palette->get(index, colors[index].r, colors[index].g, colors[index].b);
    }

  if(frame_index)
    {
      screen = SDL_SetVideoMode(320, 240, 8, SDL_HWSURFACE | SDL_DOUBLEBUF| SDL_RESIZABLE);
      if ( screen == NULL ) 
	{
	  fprintf(stderr, "Couldn't set 320x240 video mode: %s\n", SDL_GetError());
	  exit(1);
	}
      SDL_SetColors(screen, colors, 0, 256);
      SDL_FillRect(screen, 0, 255);

      static const unsigned x_offset = 128;
      static const unsigned y_offset = 128;
      uint8_t *pixels = reinterpret_cast<uint8_t *>(screen->pixels) + y_offset * screen->pitch + x_offset;

      anim->get_frame(pixels, screen->pitch, frame_index);
    }
  else
    {
      screen = SDL_SetVideoMode(1000, 700, 8, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
      if ( screen == NULL ) 
	{
	  fprintf(stderr, "Couldn't set 1024x800x8 video mode: %s\n", SDL_GetError());
	  exit(1);
	}
      SDL_SetColors(screen, colors, 0, 256);
      SDL_FillRect(screen, 0, 255);

      uint8_t *pixels = (uint8_t *)screen->pixels;
      unsigned width_len = 0;
      unsigned height_len = 0;
      unsigned max_height = 0;
      for(int i = 8131; i < anim->_frame->size(); i++)
	{
	  unsigned width = 0;
	  unsigned height = 0;
	  int x_offset = 0;
	  int y_offset = 0;
	  anim->get_frame_info(i, width, height, x_offset, y_offset);
	  if(!width || !height)
	    {
	      continue;
	    }
	  if(width < 33)
	    {
	      width = 33;
	    }
	  height += 12;
	  if(width_len + width > screen->w)
	    {
	      pixels += (screen->w - width_len);
	      pixels += (max_height - 1) * screen->pitch;
	      height_len += max_height;
	      width_len = 0;
	      max_height = 0;
	    }
	  if(height_len + height > screen->h)
	    {
	      printf("Dernier %d\n", i);
	      break;
	    }
	  
	  unsigned m = i / 1000;
	  unsigned c = (i % 1000) / 100;
	  unsigned d = (i % 100) / 10;
	  unsigned u = (i % 10);
	  req->get(16 + m, pixels, screen->pitch);
	  req->get(16 + c, pixels + 8, screen->pitch);
	  req->get(16 + d, pixels + 16, screen->pitch);
	  req->get(16 + u, pixels + 24, screen->pitch);
	  
	  anim->get_frame(pixels - (y_offset - 12) * screen->pitch - x_offset, screen->pitch, i);
	  
	  pixels += width;
	  width_len+= width;
	  if(height > max_height)
	    {
	      max_height = height;
	    }
	}
    }


  SDL_UpdateRect(screen, 0, 0, 0, 0);

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
	    }
	}
    }
#else
  unsigned width_max = 0;
  unsigned height_max = 0;

  for(int i = 0; i < anim->_frame->size(); i++)
    {
      unsigned width = 0;
      unsigned height = 0;
      int x_offset = 0;
      int y_offset = 0;
      anim->get_frame_info(i, width, height, x_offset, y_offset);

      printf("%03d %03d %03d %03d\n", width, height, x_offset, y_offset);
      
      if(width > width_max)
	{
	  width_max = width;
	}
      if(height > height_max)
	{
	  height_max = height;
	}
    }
  printf("%03d %03d\n", width_max, height_max);

#endif

  delete palette;
  delete anim;
  exit(0);
}
