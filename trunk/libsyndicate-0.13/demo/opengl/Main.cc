#include <unistd.h>
#include <cstring>
#include <cstdio>

#include <SDL.h>

#include "Data.h"
#include "Sight.h"

uint8_t tile_id = 5;

int main(int argc, char **argv)
{
  unsigned game_index = 1;

  while(--argc)
    {
      game_index = strtol(argv[argc], 0, 0);
    }

  printf("Initializing data.\n");
  Data *data = new Data();

  data->init();

  data->load(game_index);

  printf("Initializing sdl.\n");
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1) 
    { 
      fprintf(stderr, "Could not initialize SDL: %s.\n", SDL_GetError());
      return 1;
    }

  SDL_WM_SetCaption("libsyndicate demo", "libsyndicate demo");

  printf("Initializing surface.\n");

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  int width = 640;
  int height = 480;
  int bpp = 32;
  Uint32 flags = SDL_RESIZABLE | SDL_OPENGL;
  bpp = SDL_VideoModeOK(width, height, bpp, flags);
  SDL_Surface *display_surface = SDL_SetVideoMode(width, height, bpp, flags);
  if(!display_surface) 
    {
      fprintf(stderr, "Couldn't set %ux%ux%u video mode: %s\n", width, height, bpp, SDL_GetError());
    }

  printf("Initializing sight.\n");
  Sight *sight = new Sight(data, width, height);

  sight->init();

  SDL_EnableUNICODE(1);
  
  printf("Go.\n");
  bool stop = false;
  while(!stop)
    {
      SDL_Event event;
      while(SDL_PollEvent(&event) && !stop) 
	{
	  switch (event.type) 
	    {
	    case SDL_QUIT: 
	      {
		stop = true;
	      }
	      break;
	    case SDL_VIDEORESIZE:
	      {
		width = event.resize.w;
		height = event.resize.h;
		display_surface = SDL_SetVideoMode(width, height, bpp, flags);
		if(!display_surface) 
		  {
		    fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n", width, height, bpp, SDL_GetError());
		    return false;
		  }
		// Here is a bug because we should reinitialise Opengl...
		sight->resize(width, height);
	      }
	      break;
	    case SDL_ACTIVEEVENT:
	      {
	      }
	      break;
	    case SDL_MOUSEBUTTONDOWN:
	      {
	      }
	      break;
	    case SDL_MOUSEBUTTONUP:
	      {
	      }
	      break;
	    case SDL_KEYDOWN:
	      {
		//printf("hit %d\n", event.key.keysym.unicode);
		switch(event.key.keysym.unicode)
		  {
		  case 'a':
		    sight->panZoom(1, 0, 0);
		    break;
		  case 'z':
		    sight->panZoom(-1, 0, 0);
		    break;
		  case 'q':
		    sight->panZoom(0, 1, 0);
		    break;
		  case 's':
		    sight->panZoom(0, -1, 0);
		    break;
		  case 'w':
		    sight->panZoom(0, 0, 1);
		    break;
		  case 'x':
		    sight->panZoom(0, 0, -1);
		    break;
		  case 'o':
		    sight->perspective(false);
		    break;
		  case 'p':
		    sight->perspective(true);
		    break;
		  case 'l':
		    sight->smooth(false);
		    break;
		  case 'm':
		    sight->smooth(true);
		    break;
		  case 'b':
		    tile_id --;
		    printf("%d\n", tile_id);
		    break;
		  case 'n':
		    tile_id++;
		    printf("%d\n", tile_id);
		    break;
		  }
	      }
	      break;
	    case SDL_KEYUP:
	      {
	      }
	      break;
	    }
	}

      sight->render();
      
      sight->draw();

      SDL_GL_SwapBuffers();	

    }


  delete sight;

  delete data;

  return 0;
}
