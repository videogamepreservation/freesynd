#ifndef __Syndicate_Engine_Sdl_h__
#define __Syndicate_Engine_Sdl_h__

#include <SDL.h>
#include <SDL_thread.h>

#include "Game.h"

class Sdl
{
public:

  Sdl(Game *game);
  
  ~Sdl(void);

  bool init(void);

  void start(void);

  void wait(void);

  void stop(void);

  void clean(void);

private:

  static Uint32 timer(Uint32 interval, void *param);

  static int runWrapper(void *param)
  {   
    Sdl *instance = reinterpret_cast<Sdl *>(param);
    instance->run();
    return 0;
  }

  void run(void);

  Game *_game;

  int _bpp;

  Uint32 _flags;

  unsigned _nb_colors;
  SDL_Color *_palette;
  Uint32    *_colors;

  Uint32 _dark_brown;
  Uint32 _dark_red;
  Uint32 _dark_blue;
  Uint32 _white;
  Uint32 _light_brown;
  Uint32 _light_red;
  Uint32 _light_blue;
  Uint32 _alpha_color;

  unsigned _nb_tiles;
  SDL_Surface **_tiles;

  unsigned _nb_frames;
  struct Sprite
  {
    SDL_Surface *_surface;
    int _x_offset;
    int _y_offset;
  } *_frames;

  SDL_Surface *_display_surface;

  SDL_Surface *_map_surface;

  SDL_Rect _map_src_rect;

  SDL_Rect _map_dst_rect;

  SDL_Surface *_ctrl_surface;

  int _I_ini;
  int _I_fin;
  int _I_min;
  int _I_max;
  int _J_min;
  int _J_max;
  int _K_min;
  int _K_max;

  SDL_sem *_synchro;

  SDL_TimerID _timer;

  SDL_Thread *_thread;

  bool _stop;

  static const int _map_tile_side = 256;
  static const int _scroll_limit = 10;
  static const int _scroll_speed = 2;
  static const int _ctrl_width = 128;

};

#endif /* __Syndicate_Engine_Sdl_h__ */

// Local Variables:
// mode: C++
// End:
