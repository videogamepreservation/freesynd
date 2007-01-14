#include "SDL.h"
#include "lua.h"

struct rwops {
  struct SDL_RWops *F;
  const  char       N[1];
};

int isrwops    (struct lua_State *L, int i);

int rwops_new  (struct lua_State *L);
int rwops_gc   (struct lua_State *L);
int rwops_rew  (struct lua_State *L);

int init_munin ();

void *inhale_file(const char *name, Uint32 blk, Uint32 off);

Uint16 SDL_ReadLE8(SDL_RWops *src);
Uint16 SDL_ReadBE8(SDL_RWops *src);
