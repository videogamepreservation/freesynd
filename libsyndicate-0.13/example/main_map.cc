#include <stdio.h>
#include <stdlib.h>

#include <inttypes.h>

#include "SDL.h"

#include "Syndicate/Data/Palette.h"
#include "Map.h"

SDL_Surface *screen = 0;

Syndicate::Data::Palette *palette = 0;

Map *map = 0;

static const unsigned margin = 10;

int map_index = -1;
int pal_index = -1;

int main(int argc, char *argv[]) 
{
  for(int i = 0; i < (argc - 1); i ++)
    {
      if(map_index == -1)
	{
	  map_index = atoi(argv[1 + i]);
	}
      else if(pal_index == -1)
	{
	  pal_index = atoi(argv[1 + i]);
	}      
    }

  if(map_index == -1)
    {
      map_index = 1;
    }
  if(pal_index == -1)
    {
      pal_index = 2;
    }

  printf("Initializing SDL.\n");
    
  /* Initialize defaults, Video */
  if((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)==-1)) 
    { 
      printf("Could not initialize SDL: %s.\n", SDL_GetError());
      exit(-1);
    }

  printf("SDL initialized.\n");

  screen = SDL_SetVideoMode(640, 480, 8, SDL_HWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF| SDL_RESIZABLE);
  if ( screen == NULL ) 
    {
      fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n", SDL_GetError());
      exit(1);
    }

  /* Clean up on exit, exit on window close and interrupt */
  atexit(SDL_Quit);

  /* Init palette */
  char pal_name[64] = {0};
  snprintf(pal_name, sizeof(pal_name), "HPAL%02d.DAT", pal_index);
  palette = new Syndicate::Data::Palette();
  palette->load(pal_name);
  static SDL_Color colors[256];
  for(unsigned index = 0; index < 256; index++)
    {
      palette->get(index, colors[index].r, colors[index].g, colors[index].b);
    }
  SDL_SetColors(screen, colors, 0, 256);

  /* Init map */
  char map_name[64] = {0};
  snprintf(map_name, sizeof(map_name), "MAP%02d.DAT", map_index);
  map = new Map(map_name, "HBLK01.DAT");
  static SDL_Surface *map_surface = 0;
  map_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, map->width(), map->height(), 8, 0, 0, 0, 0);

  printf("Open %s %u %u\n", map_name, map_surface->w, map_surface->h);

  map->get((uint8_t *)map_surface->pixels, map_surface->pitch);
  SDL_SetColors(map_surface, colors, 0, 256);

  /* Init viewport */
  static SDL_Rect viewport;
  viewport.x = map_surface->w / 2;
  viewport.y = map_surface->h / 2;
  viewport.w = screen->w;
  viewport.h = screen->h;


  /* First display */
  SDL_BlitSurface(map_surface, &viewport, screen, 0);
  SDL_UpdateRect(screen, 0, 0, 0, 0);

  
  /* Loop waiting for ESC+Mouse_Button */
  SDL_Event event;
  bool stop = false;
  bool refresh = false;
  bool scroll = false;
  while (!stop) 
    {
      while (SDL_PollEvent(&event) && !stop) 
	{
	  switch (event.type) 
	    {
	    case SDL_QUIT: 
	      {
		printf("Quit requested, quitting.\n");
		stop = true;
	      }
	      break;
	    case SDL_MOUSEBUTTONDOWN:
	      {
		unsigned x = event.button.x;
		unsigned y = event.button.y;
		unsigned i = 0, j = 0, k = 3;
		map->tile(viewport.x + x, viewport.y + y, k, i, j);
		printf("Click (%u %u) (%u %u %u).\n", x, y, i, j, k);

		map->select((uint8_t *)map_surface->pixels, i, j, k);

		refresh = true;
	      }
	      break;
	    case SDL_VIDEORESIZE:
	      {
		screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 8, SDL_HWSURFACE | SDL_DOUBLEBUF| SDL_RESIZABLE);
		SDL_SetColors(screen, colors, 0, 256);
		SDL_SetColors(map_surface, colors, 0, 256);

		viewport.w = screen->w;
		viewport.h = screen->h;

		refresh = true;
	      }
	      break;
	    case SDL_MOUSEMOTION:
	      {
		unsigned x = event.motion.x;
		unsigned y = event.motion.y;

		if(x <= margin || (screen->w - margin) <= x || y <= margin || (screen->h - margin) <= y)
		  {
		    scroll = true;
		  }
		else
		  {
		    scroll = false;
		  }
	      }
	      break;
	    case SDL_ACTIVEEVENT:
	      {
#if 0
		if(event.active.gain)
		  {
		    scroll = true;
		  }
		else
		  {
		    scroll = false;
		  }
#endif 
		scroll = false;
	      }
	      break;

	    }
	}

      if((refresh || scroll) && !stop)
	{
	  if(refresh)
	    {
	      //printf("r\n");
	      refresh = false;
	    }
	  if(scroll)
	    {
	      unsigned x = 0, y = 0;
	      {
		int tmp_x = 0;
		int tmp_y = 0;
		SDL_GetMouseState(&tmp_x, &tmp_y);
		x = tmp_x;
		y = tmp_y;
	      }
	      if (x <= margin)
		{
		  viewport.x -= 10;
		  if(viewport.x < 0)
		    {
		      viewport.x = 0;
		    }
		}
	      if((screen->w - margin) <= x)
		{
		  viewport.x += 10;
		  if(viewport.x > map_surface->w - viewport.w)
		    {
		      viewport.x = map_surface->w - viewport.w;
		    }
		}
	      if (y <= margin)
		{
		  viewport.y -= 10;
		  if(viewport.y < 0)
		    {
		      viewport.y = 0;
		    }
		}
	      if((screen->h - margin) <= y)
		{
		  viewport.y += 10;
		  if(viewport.y > map_surface->h - viewport.h)
		    {
		      viewport.y = map_surface->h - viewport.h;
		    }
		}

	      //printf("s\n");
	    }

	  SDL_BlitSurface(map_surface, &viewport, screen, 0);

	  SDL_UpdateRect(screen, 0, 0, 0, 0);

	}
    }

  delete palette;
  delete map;
  exit(0);
}
