#include "common.h"

static const char *META = "rw_meta";

struct rw {
  struct SDL_RWops *F;
  char              N[1];
};

int is_rw(struct lua_State *L, int i)
{
  return luaL_checkudata(L, i, META) != NULL;
}

int rw_new(struct lua_State *L)
{
  struct SDL_RWops *F;
  struct rw *rw;
  const char *s;

  s = lua_tostring(L, 1);

  if(s && strlen(s)) {
    if((F = SDL_RWFromFile(s, "r")) != NULL) {
      if((rw = lua_newuserdata(L, sizeof(struct SDL_RWops *) + 
			       strlen(s)+8)) != NULL) {
	luaL_getmetatable(L, META);
	lua_setmetatable(L, 2);
	rw->F = F;
	strcpy(rw->N, s);
	printf("rw ok: '%s'\n", s);
	return 1;
      }
      lua_pop(L, 1);
    }
    ;/* s is NULL or length is zero */
  }
  printf("rw ko: '%s'\n", s);
  return 0;
}

int rw_gc(struct lua_State *L)
{
  struct SDL_RWops *rw = luaL_checkudata(L, 1, META);

  luaL_argcheck(L, rw, 1, "Expected an rw");
  SDL_RWclose(rw);
  return 0;
}

int rw_rew(struct lua_State *L)
{
  struct SDL_RWops *rw = luaL_checkudata(L, 1, META);

  luaL_argcheck(L, rw, 1, "Expected an rw");
  SDL_RWseek(rw, 0, SEEK_SET);
  return 0;
}

static struct luaL_Reg rw_meta[] = {
  "new",    rw_new,
  "gc",     rw_gc,
  "rewind", rw_rew,
  0, 0,
};

int init_munin()
{
  printf("Munin script-accessible storage module.\n");

  lua_pushstring(L, META);
  lua_newtable(L);
  lua_pushstring(L, "__index");
  lua_pushvalue(L, -2);
  lua_settable(L, -3);
  luaL_register(L, NULL, rw_meta);
  lua_settable(L, LUA_REGISTRYINDEX);
  lua_register(L, "rw", rw_new);
  return 1;
}

/*** UTILS ***/

void *inhale_file(const char *name, Uint32 blk, Uint32 off)
{
  struct SDL_RWops *F = NULL;
  Uint32 x, *n;
  void *p;

  if(!(F = SDL_RWFromFile(name,"r"))) abend(0);

  /* determine size */
  x = SDL_RWseek(F, 0, SEEK_END), SDL_RWseek(F, 0, SEEK_SET);
  off += sizeof(Uint32);

  if((n = p = malloc(off+x)) == NULL) abend(0);

  *n = SDL_RWread(F, p+off, blk, x/blk);
  SDL_RWclose(F);

  return p;
}

Uint16 SDL_ReadLE8(SDL_RWops *src)
{
  Uint16 x;

  SDL_RWread(src, &x, 1, 1);
  return x;
}

/* Uint16 (*SDL_ReadBE8)(SDL_RWops *src) = SDL_ReadLE8; */

void file_debug(struct SDL_RWops *F)
{
  int i, o = SDL_RWtell(F);

  SDL_RWseek(F, -16, SEEK_CUR);

  printf("%6d:", o-16);
  for(i=0; i<16; i++)
    printf(" %02x", SDL_ReadLE8(F));
  printf("\n");
  printf("%6d:", o);
  for(i=0; i<16; i++)
    printf(" %02x", SDL_ReadLE8(F));
  printf("\n");

  SDL_RWseek(F, o, SEEK_SET);
}

