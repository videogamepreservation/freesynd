#include <stdlib.h>
#include <stdio.h>
#include "SDL.h"
#include "lua.h"
#include "lauxlib.h"

#include "hugin.h"
#include "munin.h"
#include "narcissus.h"
//#include "uenuku.h"
#include "chronos.h"
#include "siren.h"
#include "scheherazade.h"
#include "gaia.h"
#include "avatar.h"
#include "nyarlathotep.h"

#define Q 0x7FFFFFFFu /* Cheap old-school random. Thank you, Big Blue */
#define IBM (I=(16807*(I%127773))-(I/127773)*2836,I+=(I<Q+1)? Q:0)
static int I=32;


int init_uenuku(void);


/* App
   + major mode, input mapping
   + Lua state
 */

char * error = NULL;


/* Snow show * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static struct SDL_Surface *snow = NULL;

void the_snow_show(void)
{
  int x, y, i;

  I = frame_number;
  for(i=0; i < 64000/2; i++) ((Uint16 *) snow->pixels)[i] = IBM;
  SDL_BlitSurface(snow, NULL, screen_surface, NULL);
  SDL_Flip(screen_surface);
}

int init_snow(void)
{
  SDL_Color c;
  int i;

  snow = CreateZSurface(320, 200);
  for(i=0; i < 255; i++) {
    c.r = c.g = c.b =  i;
    SDL_SetColors(snow, &c, i, 1);
  }

  luaL_dostring(L,
		"snow = 1;"
		"snow_input = { "
		"  [1]        = function() snow=0 end, "
		"  [sdlk.F12] = function() v=v+1; M:vol(v) end, "
		"  [sdlk.F11] = function() v=v-1; M:vol(v) end, "
		"}; ");
  return 1;
}
/* Snow show * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Flake show  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static int i=0, j=0, k=0, f=0, a=9;//1601; //general

void the_flake_show(void)
{
  int x, y;

  SDL_FillRect(screen_surface, NULL, 0x00000000);

  int i, j;

  i = get_num(L, "i");
  j = get_num(L, "j");

  k = get_num(L, "k");

  blit_map(i, j);

  if(blit_frame(160, 160, k, f)) f = 0; else f++;
  //blit_sprite(160, 160, k);

  SDL_Flip(screen_surface);

  if(!get_num(L, "flake"))
    exit_mode();
}
/* Flake show  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void abend(const char *s)
{
  printf("ABEND: %s\n", (s)? s: (error)? error: SDL_GetError() );
  exit(5);
}

int init(void)
{
  init_hugin(); /* Init this first, since others may want to register here. */
  init_narcissus();
  init_uenuku();
  init_chronos();
  init_siren();
  init_munin();
  init_fliplayer();
  init_gaia();
  init_avatar();
  init_nyarlathotep();

  if(luaL_dofile(L, "scripts/config.lua"))
    printf("config: %s\n", lua_tostring(L, -1)), lua_pop(L, 1);

  init_snow();

  SDL_EnableUNICODE(1);
  printf("---------------------------------------"
	 "---------------------------------------\n"
	 "INITIALISATION COMPLETE\n\n");

  return 1;
}

int main(int argc, char **argv)
{
  if(!init()) abend(0);

  enter_mode(the_flake_show);
  if(luaL_dostring(L, "input_mode(flake_input);"))
    printf("str:  %d\n", lua_tostring(L, -1)), lua_pop(L, 1);

  set_frame_time(75);
  main_loop();

  printf("\nOK\n");

  return 0;
}
