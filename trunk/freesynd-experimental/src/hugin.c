#include <stdlib.h>
#include "common.h"

lua_State *L = NULL;

/* FIXME:
   Make get_* and set_* accept names like "catdog\0cthulhu\0milkshake\0\0".
   (for catdog.cthulhu.milkhake)

   Consider dropping *L, since we use the same damn one all the time,
   everywhere...
*/

double get_num(struct lua_State *L, const char *n)
{
  double d;

  lua_getfield(L, LUA_GLOBALSINDEX, n);
  d = (lua_isnumber(L, -1))? lua_tonumber(L, -1): strtod("NAN", NULL);
  lua_pop(L, 1);

  return d;
}

int get_int(struct lua_State *L, const int ix, const char *n, const int alt)
{
  int i;

  /*lua_getfield(L, LUA_GLOBALSINDEX, n);*/
  lua_getfield(L, ix, n);
  i = (lua_isnumber(L, -1))? lua_tointeger(L, -1): alt;
  lua_pop(L, 1);

  return i;
}

const char *get_str(struct lua_State *L, const char *n)
{
  const char *s;

  lua_getfield(L, LUA_GLOBALSINDEX, n);
  s = (lua_isstring(L, -1))? lua_tostring(L, -1): NULL;
  lua_pop(L, 1);

  return s;
}

void set_num(struct lua_State *L, const char *n, const double d)
{
  lua_pushnumber(L, d); lua_setfield(L, LUA_GLOBALSINDEX, n);
}

void set_str(struct lua_State *L, const char *n, const char *s)
{
  lua_pushstring(L, s); lua_setfield(L, LUA_GLOBALSINDEX, n);
}

void debug_lua_table(struct lua_State *L, int ind)
{
  if(ind > 4) return;
  printf("%*s{\n", ind, "");
  ind += 2;

  foreach(L, -1)
    printf("%*s%s - %s\n", ind, "", bug_str(L, -2), bug_str(L, -1));

  /* recursive too dangerous. */
  /*if(lua_type(L, -1) == LUA_TTABLE) debug_lua_table(L, ind);*/

  printf("%*s}\n", ind -= 2, "");
}

void debug_lua_stack(struct lua_State *L, const char *s)
{
  int i, t;

  t = lua_gettop(L);

  printf("%10s:%2d>", s, t);

  for(i=1; i <= t; i++) {
    switch(lua_type(L, i)) {
    case LUA_TNONE:          printf("_,"); break;
    case LUA_TNIL:           printf("N,"); break;
    case LUA_TBOOLEAN:       printf("b,"); break;
    case LUA_TLIGHTUSERDATA: printf("u,"); break;
    case LUA_TNUMBER:        printf("#,"); break;
    case LUA_TSTRING:        printf("s,"); break;
    case LUA_TTABLE:         printf("t,"); break;
    case LUA_TFUNCTION:      printf("f,"); break;
    case LUA_TUSERDATA:      printf("U,"); break;
    case LUA_TTHREAD:        printf("T,"); break;
    }
  }
  printf("\n");
}

int init_hugin(void)
{
  if(!(L = luaL_newstate())) {
    error = "Could not allocate new Lua state";
    return 0;
  }
  printf("Hugin script module:\n");
  printf("  +  %s\n", LUA_RELEASE);

  luaL_openlibs(L);
  lua_pushvalue(L, LUA_REGISTRYINDEX);
  lua_setfield(L, LUA_GLOBALSINDEX, "restricted");

  if(luaL_dofile(L, "scripts/gaoler.lua"))
     printf("gaoler:  %d\n", lua_tostring(L, -1)), lua_pop(L, 1);

  if(luaL_dofile(L, "scripts/sdlkeys.lua"))
     printf("sdlk:    %d\n", lua_tostring(L, -1)), lua_pop(L, 1);

  if(luaL_dofile(L, "scripts/sprites.lua"))
     printf("sprites: %d\n", lua_tostring(L, -1)), lua_pop(L, 1);

  /*
  lua_pushvalue(L, LUA_REGISTRYINDEX);
  debug_lua_table(L, 2);
  lua_pop(L, 1);
  */

  return 1;
}
