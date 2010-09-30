#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>

#include "Sdl.h"

Sdl::Sdl(Game *game) :
  _game(game),
  _bpp(32),
  _flags(SDL_HWSURFACE | SDL_DOUBLEBUF| SDL_RESIZABLE),
  _nb_colors(0),
  _palette(0),
  _colors(0),
  _dark_brown(0),
  _dark_red(0),
  _dark_blue(0),
  _white(0),
  _light_brown(0),
  _light_red(0),
  _light_blue(0),
  _alpha_color(0),
  _nb_tiles(0),
  _tiles(0),
  _nb_frames(0),
  _frames(0),
  _display_surface(0),
  _map_surface(0),
  _ctrl_surface(0),
  _I_ini(0),
  _I_fin(0),
  _I_min(0),
  _I_max(0),
  _J_min(0),
  _J_max(0),
  _K_min(0),
  _K_max(0),
  _synchro(0),
  _timer(0),
  _thread(0),
  _stop(true)
{  
}

Sdl::~Sdl(void)
{
}

bool Sdl::init(void)
{
  /****************************************/
  /* init sdl                             */
  /****************************************/
  printf("Initializing SDL.\n");
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1) 
    { 
      fprintf(stderr, "Could not initialize SDL: %s.\n", SDL_GetError());
      return false;
    }
  SDL_WM_SetCaption("libsyndicate demo", "libsyndicate demo");

  /****************************************/
  /* init surface                         */
  /****************************************/
#if 0
  const SDL_VideoInfo *video_info = SDL_GetVideoInfo();
  if(video_info->vfmt->BitsPerPixel == 8 ||
     video_info->vfmt->BitsPerPixel == 32)
    {
      _bpp = video_info->vfmt->BitsPerPixel;
    }
#else
  _bpp = 8;
#endif

  printf("Initializing surface (bpp = %d).\n", _bpp);
  _display_surface = SDL_SetVideoMode(640, 480, _bpp, _flags);
  if(!_display_surface) 
    {
      fprintf(stderr, "Couldn't set 640x480x%d video mode: %s\n", _bpp, SDL_GetError());
      return false;
    }

  SDL_WarpMouse(_display_surface->w / 2, _display_surface->h / 2);

  _map_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, _game->screen_width(), _game->screen_height(), _bpp, 0, 0, 0, 0);
  if(!_map_surface) 
    {
      fprintf(stderr, "Couldn't create %dx%dx%d surface: %s\n", _game->screen_width(), _game->screen_height(), _bpp, SDL_GetError());
      return false;
    }

  _ctrl_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, _ctrl_width, 480, _bpp, 0, 0, 0, 0);
  if(!_ctrl_surface) 
    {
      fprintf(stderr, "Couldn't create %dx%dx%d surface: %s\n", _ctrl_width, 480, _bpp, SDL_GetError());
      return false;
    }

  _map_src_rect.x = _map_surface->w / 2;
  _map_src_rect.y = _map_surface->h / 2 - 400;
  _map_src_rect.w = _display_surface->w - _ctrl_surface->w;
  _map_src_rect.h = _display_surface->h;

  _map_dst_rect.x = _ctrl_surface->w;
  _map_dst_rect.y = 0;
  _map_dst_rect.w = 0;
  _map_dst_rect.h = 0;

  /****************************************/
  /* init palette                         */
  /****************************************/
  printf("Initializing palette.\n");
  _nb_colors = _game->_palette->size();
  if(_bpp == 8)
    {
      _palette = new SDL_Color [_nb_colors];
      memset(_palette, 0, sizeof(*_palette) * _nb_colors);
      for(unsigned index = 0; index < _nb_colors; index++)
	{
	  _game->_palette->get(index, _palette[index].r, _palette[index].g, _palette[index].b);
	}

      _dark_brown = 5;
      _dark_red = 6;
      _dark_blue = 7;
      _white = 12;
      _light_brown = 13;
      _light_red = 14;
      _light_blue = 15;
      _alpha_color = _nb_colors - 1;

      SDL_SetColors(_display_surface, _palette, 0, _nb_colors);
      SDL_SetColors(_map_surface, _palette, 0, _nb_colors);
      SDL_SetColors(_ctrl_surface, _palette, 0, _nb_colors);
    }
  else if(_bpp == 32)
    {
      _colors = new Uint32 [_nb_colors];
      memset(_colors, 0, sizeof(*_colors) * _nb_colors);
      for(unsigned index = 0; index < _nb_colors; index++)
	{
	  Uint8 r = 0;
	  Uint8 g = 0;
	  Uint8 b = 0;
	  _game->_palette->get(index, r, g, b);
	  _colors[index] = SDL_MapRGB(_display_surface->format, r, g, b);	  
	}

      _dark_brown = _colors[5];
      _dark_red = _colors[6];
      _dark_blue = _colors[7];
      _white = _colors[12];
      _light_brown = _colors[13];
      _light_red = _colors[14];
      _light_blue = _colors[15];
      _alpha_color = _colors[_nb_colors - 1];
    }

  SDL_SetColorKey(_display_surface, SDL_SRCCOLORKEY, _alpha_color);
  SDL_SetColorKey(_map_surface, SDL_SRCCOLORKEY, _alpha_color);
  SDL_SetColorKey(_ctrl_surface, SDL_SRCCOLORKEY, _alpha_color);

  /****************************************/
  /* init tiles                           */
  /****************************************/
  printf("Initializing tiles.\n");
  _nb_tiles = _game->_map_tile->_nb_tiles;
  _tiles = new SDL_Surface *[_nb_tiles];
  memset(_tiles, 0, sizeof(*_tiles) * _nb_tiles);

  for(unsigned tile_index = 0; tile_index < _nb_tiles; tile_index++)
    {
      /* allocate surface */
      SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, Syndicate::Data::MapTile::_width, Syndicate::Data::MapTile::_height, _bpp, 0, 0, 0, 0);
      if(!surface) 
	{
	  fprintf(stderr, "Couldn't create %dx%dx%d surface : %s\n", Syndicate::Data::MapTile::_width, Syndicate::Data::MapTile::_height, _bpp, SDL_GetError());
	  return false;
	}

      if(_bpp == 8)
	{
	  SDL_SetColors(surface, _palette, 0, _nb_colors);
	}
      SDL_SetColorKey(surface, SDL_SRCCOLORKEY, _alpha_color);
      SDL_FillRect(surface, 0, _alpha_color);

      /* we get the pixels of the tile */
      if(SDL_MUSTLOCK(surface))
	{
	  if(SDL_LockSurface(surface) < 0)
	    {
	      fprintf(stderr, "Can't lock surface : %s\n", SDL_GetError());
	      return false;            
	    }
	}
      if(_bpp == 8)
	{
	  _game->_map_tile->get(tile_index, (uint8_t *)(surface->pixels), surface->pitch);
	}
      else if(_bpp == 32)
	{
	  _game->_map_tile->get(tile_index, (uint32_t *)(surface->pixels), surface->pitch, _colors);
	}
      if(SDL_MUSTLOCK(surface))
	{
	  SDL_UnlockSurface(surface);
	}
      
      _tiles[tile_index] = surface;
    }

  /****************************************/
  /* init frames (aka sprites)            */
  /****************************************/
  printf("Initializing frames.\n");
  _nb_frames = _game->_spr_frame->size();
  _frames = new Sprite[_nb_frames];
  memset(_frames, 0, sizeof(*_frames) * _nb_frames);

  for(unsigned frame_index = 0; frame_index < _nb_frames; frame_index++)
    {
      unsigned frame_width = 0;
      unsigned frame_height = 0;
      int frame_x_offset = 0;
      int frame_y_offset = 0;

      _game->getFrameInfo(frame_index, frame_width, frame_height, frame_x_offset, frame_y_offset);

      if(frame_width == 0 || frame_height == 0)
	{
	  continue;
	}

      /* allocate surface */
      SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, frame_width, frame_height, _bpp, 0, 0, 0, 0);
      if(!surface) 
	{
	  fprintf(stderr, "Couldn't create %dx%dx%d surface : %s\n", frame_width, frame_height, _bpp, SDL_GetError());
	  return false;
	}

      if(_bpp == 8)
	{
	  SDL_SetColors(surface, _palette, 0, _nb_colors);
	}
      SDL_SetColorKey(surface, SDL_SRCCOLORKEY, _alpha_color);
      SDL_FillRect(surface, 0, _alpha_color);

      /* we get the pixels of the frame */      
      if(SDL_MUSTLOCK(surface))
	{
	  if(SDL_LockSurface(surface) < 0)
	    {
	      fprintf(stderr, "Can't lock surface : %s\n", SDL_GetError());
	      return false;            
	    }
	}
      if(_bpp == 8)
	{
	  uint8_t *pixels = reinterpret_cast<uint8_t*>(surface->pixels) - frame_y_offset * surface->pitch - frame_x_offset;
	  _game->getFrame(frame_index, pixels, surface->pitch);
	}
      else if(_bpp == 32)
	{
	  uint32_t *pixels = reinterpret_cast<uint32_t*>(surface->pixels) - frame_y_offset * surface->pitch / 4 - frame_x_offset;
	  _game->getFrame(frame_index, pixels, surface->pitch, _colors);
	}
      if(SDL_MUSTLOCK(surface))
	{
	  SDL_UnlockSurface(surface);
	}

      _frames[frame_index]._surface = surface;
      _frames[frame_index]._x_offset = frame_x_offset;
      _frames[frame_index]._y_offset = frame_y_offset;
    }

  /****************************************/
  /*                                      */
  /****************************************/
  printf("Initializing system.\n");
  _synchro = SDL_CreateSemaphore(1);
  if(!_synchro) 
    {
      fprintf(stderr, "Couldn't create semaphore : %s\n", SDL_GetError());
      return false;
    }

  _timer = SDL_AddTimer(30, Sdl::timer, this);
  if(!_timer) 
    {
      fprintf(stderr, "Couldn't create timer : %s\n", SDL_GetError());
      return false;
    }

  printf("Init ok.\n");
  return true;
}

void Sdl::start(void)
{
  if(!_thread)
    {
      _stop = false;
  
      _thread = SDL_CreateThread(runWrapper, this);
    }
}

void Sdl::wait(void)
{
  if(_thread)
    {
      int status = 0;
      SDL_WaitThread(_thread, &status);
      _thread = 0;

      _stop = true;
    }
}

void Sdl::stop(void)
{
  if(_thread)
    {
      _stop = true;
  
      int status = 0;
      SDL_WaitThread(_thread, &status);
      _thread = 0;
    }
}

void Sdl::clean()
{
  if(_timer)
    {
      SDL_RemoveTimer(_timer);
    }

  if(_synchro)
    {
      SDL_DestroySemaphore(_synchro);
    }

  if(_frames)
    {
      for(unsigned frame_index = 0; frame_index < _nb_frames; frame_index++)
	{
	  if(_frames[frame_index]._surface)
	    {     
	      SDL_FreeSurface(_frames[frame_index]._surface);
	    }
	}  
      delete [] _frames;
    }

  if(_tiles)
    {
      for(unsigned tile_index = 0; tile_index < _nb_tiles; tile_index++)
	{
	  if(_tiles[tile_index])
	    {
	      SDL_FreeSurface(_tiles[tile_index]);
	    }
	}  
      delete [] _tiles;
    }

  if(_colors)
    {
      delete [] _colors;
    }

  if(_palette)
    {
      delete [] _palette;
    }

  if(_ctrl_surface)
    {
      SDL_FreeSurface(_ctrl_surface);
    }

  if(_map_surface)
    {
      SDL_FreeSurface(_map_surface);
    }

  SDL_Quit();
}

void Sdl::run(void)
{
  bool update_clipping = true;
  bool skip_rendering = false;
  int nb_i = _game->_map_data->nb_i();
  int nb_j = _game->_map_data->nb_j();
  int nb_k = _game->_map_data->nb_k();
  Syndicate::Data::Game::GameItem *selected_item = 0;

  while(!_stop)
    {

      /****************************************/
      /*                                      */
      /****************************************/
      if(SDL_SemWait(_synchro) == -1)
	{
	  break;
	}

      /****************************************/
      /*                                      */
      /****************************************/
      if(!skip_rendering)
	{
	  SDL_BlitSurface(_ctrl_surface, 0, _display_surface, 0);
	  SDL_BlitSurface(_map_surface, &_map_src_rect, _display_surface, &_map_dst_rect);
	  SDL_UpdateRect(_display_surface, 0, 0, 0, 0);
	}

      /****************************************/
      /* handle event                         */
      /****************************************/
      int click_x = 0;
      int click_y = 0;
      SDL_Event event;
      while(SDL_PollEvent(&event) && !_stop) 
	{
	  switch (event.type) 
	    {
	    case SDL_QUIT: 
	      {
		_stop = true;
		skip_rendering = true;
	      }
	      break;
	    case SDL_VIDEORESIZE:
	      {
		if(event.resize.w < 640)
		  {
		    event.resize.w = 640;
		  }
		if(event.resize.h < 480)
		  {
		    event.resize.h = 480;
		  }

		_display_surface = SDL_SetVideoMode(event.resize.w, event.resize.h, _bpp, _flags);

		if(_bpp == 8)
		  {
		    SDL_SetColors(_display_surface, _palette, 0, _nb_colors);
		    SDL_SetColors(_map_surface, _palette, 0, _nb_colors);
		    SDL_SetColors(_ctrl_surface, _palette, 0, _nb_colors);
		  }

		SDL_SetColorKey(_display_surface, SDL_SRCCOLORKEY, _alpha_color);
		SDL_SetColorKey(_map_surface, SDL_SRCCOLORKEY, _alpha_color);
		SDL_SetColorKey(_ctrl_surface, SDL_SRCCOLORKEY, _alpha_color);

		if(_map_src_rect.w != (_display_surface->w - _map_dst_rect.x))
		  {
		    _map_src_rect.x += (_map_src_rect.w - (_display_surface->w - _map_dst_rect.x)) / 2;
		    _map_src_rect.w = _display_surface->w - _map_dst_rect.x;
		    update_clipping = true;
		  }
		if(_map_src_rect.h != (_display_surface->h - _map_dst_rect.y))
		  {
		    _map_src_rect.y += (_map_src_rect.h - (_display_surface->h - _map_dst_rect.y)) / 2;
		    _map_src_rect.h = _display_surface->h - _map_dst_rect.y;
		    update_clipping = true;
		  }
	      }
	      break;
	    case SDL_ACTIVEEVENT:
	      {
		if(SDL_GetAppState() == (SDL_APPMOUSEFOCUS | SDL_APPINPUTFOCUS | SDL_APPACTIVE))
		  {
		    skip_rendering = false;
		  }
		else
		  {
		    skip_rendering = true;
		  }
	      }
	      break;
	    case SDL_MOUSEBUTTONDOWN:
	      {
	      }
	      break;
	    case SDL_MOUSEBUTTONUP:
	      {
		if(_map_dst_rect.x <= event.button.x && event.button.x <= _map_dst_rect.x + _map_dst_rect.w &&
		   _map_dst_rect.y <= event.button.y && event.button.y <= _map_dst_rect.y + _map_dst_rect.h)
		  {
		    click_x = _map_src_rect.x + event.button.x - _map_dst_rect.x; 
		    click_y = _map_src_rect.y + event.button.y - _map_dst_rect.y; 
		  }
		else
		  {
		    
		  }
	      }
	      break;
	    }
	}

      if(skip_rendering)
	{
	  continue;
	}

      /****************************************/
      /* handle scrolling                     */
      /****************************************/
      int x = 0;
      int y = 0;
      SDL_GetMouseState(&x, &y);
      if (_map_dst_rect.x <= x && x <= _map_dst_rect.x + _scroll_limit)
	{
	  _map_src_rect.x -= (_map_dst_rect.x + _scroll_limit - x) * _scroll_speed;
	  update_clipping = true;
	}
      else if((_map_dst_rect.x + _map_dst_rect.w - _scroll_limit) <= x &&
	      x <= (_map_dst_rect.x + _map_dst_rect.w))
	{
	  _map_src_rect.x += (x - (_map_dst_rect.x + _map_dst_rect.w - _scroll_limit)) * _scroll_speed;
	  update_clipping = true;
	}

      if (_map_dst_rect.y <= y && y <= _map_dst_rect.y + _scroll_limit)
	{
	  _map_src_rect.y -= (_map_dst_rect.y + _scroll_limit - y) * _scroll_speed;
	  update_clipping = true;
	}
      else if((_map_dst_rect.y + _map_dst_rect.h - _scroll_limit) <= y &&
	      y <= (_map_dst_rect.y + _map_dst_rect.h))
	{
	  _map_src_rect.y += (y - (_map_dst_rect.y + _map_dst_rect.h - _scroll_limit)) * _scroll_speed;
	  update_clipping = true;
	}

      /****************************************/
      /* update clipping                      */
      /****************************************/
      if(update_clipping)
	{
	  if(_map_src_rect.x < 0)
	    {
	      _map_src_rect.x = 0;
	    }
	  if(_map_src_rect.x + _map_src_rect.w > _map_surface->w)
	    {
	      _map_src_rect.x = _map_surface->w - _map_src_rect.w;
	    }
	  if(_map_src_rect.y < 0)
	    {
	      _map_src_rect.y = 0;
	    }
	  if(_map_src_rect.y + _map_src_rect.h > _map_surface->h)
	    {
	      _map_src_rect.y = _map_surface->h - _map_src_rect.h;
	    }

	  SDL_SetClipRect(_map_surface, &_map_src_rect);

	  int dummy = 0;
	  int margin = -64;
	  int x_min = _map_src_rect.x + margin;
	  int x_max = _map_src_rect.x + _map_src_rect.w - margin;
	  int y_min = _map_src_rect.y + margin;
	  int y_max = _map_src_rect.y + _map_src_rect.h - margin;

	  _K_min = 0;
	  _K_max = nb_k * Syndicate::Data::MapTile::_depth_on_map;

	  _game->screenCoord2mapCoord(x_min, y_min, _K_min, _I_min, dummy);
	  _game->screenCoord2mapCoord(x_max, y_min, _K_min, _I_ini, _J_min);
	  _game->screenCoord2mapCoord(x_min, y_max, _K_max, _I_fin, _J_max);
	  _game->screenCoord2mapCoord(x_max, y_max, _K_max, _I_max, dummy);

	  update_clipping = false;
	}

      /****************************************/
      /* render the map                       */
      /****************************************/
      int i_ini = _I_ini / Syndicate::Data::MapTile::_width_on_map;
      int i_fin = _I_fin / Syndicate::Data::MapTile::_width_on_map;
      int i_min_min = _I_min / Syndicate::Data::MapTile::_width_on_map;
      int i_max_max = _I_max / Syndicate::Data::MapTile::_width_on_map;
      int j_min = _J_min / Syndicate::Data::MapTile::_height_on_map;
      int j_max = _J_max / Syndicate::Data::MapTile::_height_on_map;
      int k_min = _K_min / Syndicate::Data::MapTile::_depth_on_map;
      int k_max = _K_max / Syndicate::Data::MapTile::_depth_on_map;

      for(int k = k_min; k < k_max; k+=1)
	{
	  int K = k * Syndicate::Data::MapTile::_depth_on_map;

	  int i_min = i_ini;
	  int i_max = i_ini;
	  int i_min_delta = -1;
	  int i_max_delta = 1;

	  for(int j = j_min; j <= j_max; j+=1)
	    {
	      int J = j * Syndicate::Data::MapTile::_height_on_map;
	      //#define DEBUG
#ifdef DEBUG
	      printf("k = %4d (%4d %4d) (%4d %4d) (%4d %4d)\n", k, i_ini, i_fin, i_min, i_max, i_min_min, i_max_max);
#else
	      if(0 <= j && j < nb_j)
		{
		  for(int i = i_min; i <= i_max; i+=1)
		    {
		      int I = i * Syndicate::Data::MapTile::_width_on_map;

		      if(0 <= i || i < nb_i)
			{
			  /**********************/
			  /*** render tiles   ***/
			  /**********************/
			  /* we get the id of the tile */
			  unsigned id = 0;
			  _game->_map_data->get(i, j, k, id);

			  if(id >= 5)
			    {
			      /* we get the coordinates of the tile */
			      int x = 0, y = 0;
			      _game->mapCoord2screenCoord(I, J, K, x, y);

			      x -= Syndicate::Data::MapSubTile::_width;
			      y -= Syndicate::Data::MapSubTile::_height;

			      /* we draw the pixels of the tile */
			      SDL_Rect dst_rect;
			      dst_rect.x = x;
			      dst_rect.y = y;
			      SDL_BlitSurface(_tiles[id], 0, _map_surface, &dst_rect);
			    }

			  /**********************/
			  /*** render sprites ***/
			  /**********************/
			  unsigned offset = _game->_game->_data->_offsets[j * nb_i + i];
			  while(offset)
			    {
			      /* we get the item to draw */
			      Syndicate::Data::Game::GameItem *item = _game->_game->getItem(offset);
			      int frame_index = item->_index_current_frame;
			      int frame_I = item->_tile_i;
			      int frame_J = item->_tile_j;
			      int frame_K = item->_tile_k;
			      int frame_k = frame_K / Syndicate::Data::MapTile::_depth_on_map;

			      if(frame_k == k)
				{
				  /* we get the coordinates of the tile */
				  int x = 0, y = 0;
				  _game->mapCoord2screenCoord(frame_I, frame_J, frame_K, x, y);

				  x += _frames[frame_index]._x_offset;
				  y += _frames[frame_index]._y_offset;

				  /* we draw the pixels of the tile */
				  SDL_Rect dst_rect;
				  dst_rect.x = x;
				  dst_rect.y = y;

				  /* we "highlight" the selection */
				  if(selected_item == item)
				    {
				      SDL_FillRect(_map_surface, &dst_rect, _white);
				    }

				  SDL_BlitSurface(_frames[frame_index]._surface, 0, _map_surface, &dst_rect);

				  /* test if selected */
				  if(dst_rect.x <= click_x && click_x <= dst_rect.x + _frames[frame_index]._surface->w &&
				     dst_rect.y <= click_y && click_y <= dst_rect.y + _frames[frame_index]._surface->h)
				    {
				      selected_item = item;
				      click_x = 0;
				      click_y = 0;
				      printf("Item selected : \n");
				      printf("\tframe_index : %d\n", frame_index);
				      printf("\tI : %5d\n", frame_I);
				      printf("\tJ : %5d\n", frame_J);
				      printf("\tK : %5d\n", frame_K);
				      printf("\tx : %4d\n",  x);
				      printf("\ty : %4d\n",  y);
				      printf("\tx_offset : %2d\n",  _frames[frame_index]._x_offset);
				      printf("\ty_offset : %2d\n",  _frames[frame_index]._y_offset);
				      printf("\twidth  : %3d\n",  _frames[frame_index]._surface->w);
				      printf("\theight : %3d\n",  _frames[frame_index]._surface->h);
				    }
				     
				}
			      offset = item->_offset_next;
			    } // while offset

			}
		    }
#endif	      
		}


	      i_min += i_min_delta;
	      if(i_min <= i_min_min)
		{
		  i_min_delta = -i_min_delta;
		}
	      
	      i_max += i_max_delta;
	      if(i_max >= i_max_max)
		{
		  i_max_delta = -i_max_delta;
		}
	    }

	  if(k % 2 == 0)
	    {
	      i_ini++;
	      i_fin++;
	      i_min_min++;
	      i_max_max++;
	      j_min++;
	      j_max++;
	    }

	}


      /****************************************/
      /* render the control pannel            */
      /****************************************/
      SDL_Rect dst_rect;
      
      dst_rect.x = 0;
      dst_rect.y = 0;

      // agent 0
      int ped_id = 0;
      while(ped_id < 4)
	{
	  
	  switch(ped_id)
	    {
	    case 0:
	      SDL_BlitSurface(_frames[1374]._surface, 0, _ctrl_surface, &dst_rect);
	      break;
	    case 1:
	      SDL_BlitSurface(_frames[1385]._surface, 0, _ctrl_surface, &dst_rect);
	      break;
	    case 2:
	      SDL_BlitSurface(_frames[1386]._surface, 0, _ctrl_surface, &dst_rect);
	      break;
	    case 3:
	      SDL_BlitSurface(_frames[1392]._surface, 0, _ctrl_surface, &dst_rect);
	      break;
	    default:
	      break;
	    }

	  if(_game->_game->_data->_pedestrians[ped_id]._type)
	    {
	      SDL_Rect dst_rect_tmp;
	  
	      // sprite
	      int frame_index = _game->_game->_data->_pedestrians[ped_id]._index_current_frame;
	      dst_rect_tmp.x = dst_rect.x + (dst_rect.w / 2 - _frames[frame_index]._surface->w / 2);
	      dst_rect_tmp.y = dst_rect.y + (40 - _frames[frame_index]._surface->h);
	      SDL_BlitSurface(_frames[frame_index]._surface, 0, _ctrl_surface, &dst_rect_tmp);

	      // health
	      int health = _game->_game->_data->_pedestrians[ped_id]._health;
	      if(health)
		{
		  dst_rect_tmp.x = dst_rect.x + 52;
		  dst_rect_tmp.y = dst_rect.y + 42 - health * 36 / 0x10;
		  dst_rect_tmp.w = 6;
		  dst_rect_tmp.h = health * 36 / 0x10;
		  SDL_FillRect(_ctrl_surface, &dst_rect_tmp, _white);
		}

	      // IPA
	      // Adrenalin
	      int adrena_depend = _game->_game->_data->_pedestrians[ped_id]._adrena_dependency;

	      dst_rect_tmp.y = dst_rect.y + 48;
	      dst_rect_tmp.h = 10;

	      int adrena_effect = _game->_game->_data->_pedestrians[ped_id]._adrena_effect;
	      if(adrena_effect < adrena_depend)
		{
		  dst_rect_tmp.x = dst_rect.x + 4 + adrena_effect * 55 / 0xff;
		  dst_rect_tmp.w = adrena_depend * 55 / 0xff - adrena_effect * 55 / 0xff;
		}
	      else
		{
		  dst_rect_tmp.x = dst_rect.x + 4 + adrena_depend * 55 / 0xff;
		  dst_rect_tmp.w = adrena_effect * 55 / 0xff - adrena_depend * 55 / 0xff;
		}
	      SDL_FillRect(_ctrl_surface, &dst_rect_tmp, _light_red);

	      int adrena_amount = _game->_game->_data->_pedestrians[ped_id]._adrena_amount;
	      if(adrena_amount < adrena_depend)
		{
		  dst_rect_tmp.x = dst_rect.x + 4 + adrena_amount * 55 / 0xff;
		  dst_rect_tmp.w = adrena_depend * 55 / 0xff - adrena_amount * 55 / 0xff;
		}
	      else
		{
		  dst_rect_tmp.x = dst_rect.x + 4 + adrena_depend * 55 / 0xff;
		  dst_rect_tmp.w = adrena_amount * 55 / 0xff - adrena_depend * 55 / 0xff;
		}
	      SDL_FillRect(_ctrl_surface, &dst_rect_tmp, _dark_red);

	      dst_rect_tmp.x = dst_rect.x + 4 + adrena_depend * 55 / 0xff;
	      dst_rect_tmp.w = 1;
	      SDL_FillRect(_ctrl_surface, &dst_rect_tmp, _white);
	  

	      // Perception
	      int percep_depend = _game->_game->_data->_pedestrians[ped_id]._percep_dependency * 55 / 0xff;

	      dst_rect_tmp.y = dst_rect.y + 62;
	      dst_rect_tmp.h = 10;

	      int percep_effect = _game->_game->_data->_pedestrians[ped_id]._percep_effect * 55 / 0xff;
	      if(percep_effect < percep_depend)
		{
		  dst_rect_tmp.x = dst_rect.x + 4 + percep_effect;
		  dst_rect_tmp.w = percep_depend - percep_effect;
		}
	      else
		{
		  dst_rect_tmp.x = dst_rect.x + 4 + percep_depend;
		  dst_rect_tmp.w = percep_effect - percep_depend;
		}
	      SDL_FillRect(_ctrl_surface, &dst_rect_tmp, _light_blue);

	      int percep_amount = _game->_game->_data->_pedestrians[ped_id]._percep_amount * 55 / 0xff;
	      if(percep_amount < percep_depend)
		{
		  dst_rect_tmp.x = dst_rect.x + 4 + percep_amount;
		  dst_rect_tmp.w = percep_depend - percep_amount;
		}
	      else
		{
		  dst_rect_tmp.x = dst_rect.x + 4 + percep_depend;
		  dst_rect_tmp.w = percep_amount - percep_depend;
		}
	      SDL_FillRect(_ctrl_surface, &dst_rect_tmp, _dark_blue);

	      dst_rect_tmp.x = dst_rect.x + 4 + percep_depend;
	      dst_rect_tmp.w = 1;
	      SDL_FillRect(_ctrl_surface, &dst_rect_tmp, _white);
	  

	      // Intelligence
	      int inteli_depend = _game->_game->_data->_pedestrians[ped_id]._inteli_dependency * 55 / 0xff;

	      dst_rect_tmp.y = dst_rect.y + 76;
	      dst_rect_tmp.h = 10;

	      int inteli_effect = _game->_game->_data->_pedestrians[ped_id]._inteli_effect * 55 / 0xff;
	      if(inteli_effect < inteli_depend)
		{
		  dst_rect_tmp.x = dst_rect.x + 4 + inteli_effect;
		  dst_rect_tmp.w = inteli_depend - inteli_effect;
		}
	      else
		{
		  dst_rect_tmp.x = dst_rect.x + 4 + inteli_depend;
		  dst_rect_tmp.w = inteli_effect - inteli_depend;
		}
	      SDL_FillRect(_ctrl_surface, &dst_rect_tmp, _light_brown);

	      int inteli_amount = _game->_game->_data->_pedestrians[ped_id]._inteli_amount * 55 / 0xff;
	      if(inteli_amount < inteli_depend)
		{
		  dst_rect_tmp.x = dst_rect.x + 4 + inteli_amount;
		  dst_rect_tmp.w = inteli_depend - inteli_amount;
		}
	      else
		{
		  dst_rect_tmp.x = dst_rect.x + 4 + inteli_depend;
		  dst_rect_tmp.w = inteli_amount - inteli_depend;
		}
	      SDL_FillRect(_ctrl_surface, &dst_rect_tmp, _dark_brown);

	      dst_rect_tmp.x = dst_rect.x + 4 + inteli_depend;
	      dst_rect_tmp.w = 1;
	      SDL_FillRect(_ctrl_surface, &dst_rect_tmp, _white);
	    }

	  dst_rect.x += _frames[1374]._surface->w;
	  ped_id++;
	  if(ped_id == 2)
	    {
	      dst_rect.y += _frames[1385]._surface->h;
	      dst_rect.x = 0;
	  
	      // every agent button
	      SDL_BlitSurface(_frames[8229]._surface, 0, _ctrl_surface, &dst_rect);
	      dst_rect.y += _frames[8229]._surface->h;
	      dst_rect.x = 0;
	    }
	  else if(ped_id == 4)
	    {
	      dst_rect.y += _frames[1392]._surface->h;
	      dst_rect.x = 0;
	  
	      // mission infos
	      SDL_BlitSurface(_frames[1572]._surface, 0, _ctrl_surface, &dst_rect);
	      dst_rect.y += _frames[1572]._surface->h;
	      dst_rect.x = 0;
	    }
	}

      // inventory
      ped_id = 0; // TODO the selected ped
      unsigned offset_eqpt = _game->_game->_data->_pedestrians[ped_id]._offset_equipment;
      unsigned offset_curr = _game->_game->_data->_pedestrians[ped_id]._offset_cur_weapon;
      for(int j = 0; j < 2; j++)
	{
	  for(int i = 0; i < 4; i++)
	    {
	      unsigned frame_index = 1614;
	      int item_health = 0;
	      if(38242 <= offset_eqpt && offset_eqpt < 56674)
		{
		  Syndicate::Data::Game::GameStruct::Weapon *item = reinterpret_cast<Syndicate::Data::Game::GameStruct::Weapon *>(_game->_game->getItem(offset_eqpt));
		  int item_sub_type = item->_sub_type;
		  item_health = item->_nb_amos;
		  if(1 <= item_sub_type && item_sub_type <= 14)
		    {
		      frame_index = 1450 + 6 * (item_sub_type - 1) + ((offset_eqpt == offset_curr)?3:0);
		    }
		  offset_eqpt = item->_offset_next_inventory;
		}
	      SDL_BlitSurface(_frames[frame_index]._surface, 0, _ctrl_surface, &dst_rect);
	      if(item_health)
		{
		  SDL_Rect dst_rect_tmp;
		  
		  dst_rect_tmp.x = dst_rect.x + 4;
		  dst_rect_tmp.y = dst_rect.y + 24;
		  dst_rect_tmp.w = item_health;
		  dst_rect_tmp.h = 4;
		  SDL_FillRect(_ctrl_surface, &dst_rect_tmp, _light_red);		  
		}
	      dst_rect.x += _frames[frame_index]._surface->w;
	    }
	  dst_rect.y += _frames[1614]._surface->h;
	  dst_rect.x = 0;
	}

#ifdef DEBUG
      _stop = true;
#endif
    }
}

Uint32 Sdl::timer(Uint32 interval, void *param)
{
  Sdl *sdl = reinterpret_cast<Sdl *>(param);

  SDL_SemPost(sdl->_synchro);

  return interval;
}
