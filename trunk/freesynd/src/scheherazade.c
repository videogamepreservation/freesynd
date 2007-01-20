#include "common.h"
/*
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
*/

#include "fli_format.h"

extern char *error;
extern int run_main_loop;
extern struct lua_State *L;
extern void (*master_event_loop)(void);

static const char *META = "fli_meta";

struct FLI {
  struct SDL_Color    P[256];
  struct SDL_Surface *S;
  struct SDL_RWops   *F;
  const  char        *N;
  int                 R;
  int                 t;
  Uint16 w;
  Uint16 h;
  Uint16 n;
  Uint16 f;
  Uint8  r;
};

static void decode_delta_flc(struct FLI *Z)
{
  Uint16 lines = 0;
  Uint16 l = 0;
  Uint16 k = 0;

  SDL_Rect R = { 1000, 1000, 0, 0 };

  for(lines = SDL_ReadLE16(Z->F); lines--; l++) {
    Uint16 op;

    do {
      op = SDL_ReadLE16(Z->F);

      switch( (op >> 14) & 3 ) {
      case pktcnt: k  =  op;           break;
      case undef:                      break;
      case skip:   l += -(Sint16) op;  break;
      case last: /* Never actually happens. Least not in intro.dat. */
	*( (Uint8 *)(Z->S->pixels) + l * Z->w + Z->h -1) = op & 0xFF;
	break;
      }
    } while( (op >> 14) & 3 != pktcnt );

    Uint16 cl = 0;

    while(k--) {
      Sint8 c;
      Uint16 i;

      cl += SDL_ReadLE8(Z->F);
      c   = SDL_ReadLE8(Z->F);

      R.x = (R.x < cl)? R.x: cl;
      R.y = (R.y <  l)? R.y:  l;
      R.h = (R.h >  l)? R.h:  l;

      if(c > 0) {
	for(i=0; i < c; i++)
	  *(Uint16 *)(Z->S->pixels + l * Z->w + cl + i*2) = SDL_ReadLE16(Z->F);
	cl += c * 2;
      } else if (c < 0) {
	Uint16 x;
	Sint16 z;

	z = -(Sint16) c;
	x = SDL_ReadLE16(Z->F);

	for(i=0; i < z; i++)
	  *(Uint16 *)(Z->S->pixels + l * Z->w + cl + i*2) = x;
	cl += z * 2;
      } else {
	printf("// End of cutscene ? with k=%d\n", k);
	return;
      }
      R.w = (R.w > cl)? R.w: cl;
    }
  }
  if(R.x < 1000 || R.y < 1000 || R.w || R.h) {
    R.w -= R.x-1, R.h -= R.y-1;
    SDL_BlitSurface(Z->S, &R, screen_surface, &R);
  }

}

static void decode_byte_run(struct FLI *Z)
{
  Uint8 *q = Z->S->pixels;

  while( q < (Uint8 *)(Z->S->pixels) + Z->w * Z->h ) {
    Uint8 chunks = SDL_ReadLE8(Z->F);

    while (chunks--) {
      Sint8 count = SDL_ReadLE8(Z->F);
      if(count > 0) {
	Uint8 x = SDL_ReadLE8(Z->F);
	memset(q, x, count);
	q += count;
      } else {
	SDL_RWread(Z->F, q, -(Sint8) count, 1);
	q += -(Sint8) count;
      }
    }
  }
  SDL_BlitSurface(Z->S, NULL, screen_surface, NULL);
}

static void set_palette(struct FLI *Z)
{
  Uint8 n, o = 0;
  Uint8 np = SDL_ReadLE16(Z->F);
  Uint8 i  = SDL_ReadLE16(Z->F) != 0;

  if(i) {
    SDL_RWseek(Z->F, -2, SEEK_CUR);
    while(np--) {
      o += SDL_ReadLE8(Z->F);
      n  = SDL_ReadLE8(Z->F);
      for(i = 0; i < n; i++, o++) {
	Z->P[o].r = SDL_ReadLE8(Z->F) << 2;
	Z->P[o].g = SDL_ReadLE8(Z->F) << 2;
	Z->P[o].b = SDL_ReadLE8(Z->F) << 2;
      }
      SDL_SetColors(Z->S, &Z->P[o-n], o-n, n);
    }
    SDL_ReadLE8(Z->F);
  } else {
    do {
      Z->P[i].r = SDL_ReadLE8(Z->F) << 2;
      Z->P[i].g = SDL_ReadLE8(Z->F) << 2;
      Z->P[i].b = SDL_ReadLE8(Z->F) << 2;
    } while(++i);
    SDL_SetColors(Z->S, Z->P, 0, 256);
  }
}

static void munch_frame(struct FLI *Z)
{
  SDL_RWseek(Z->F, (sizeof(struct FrameTypeChunkHeader) -
		    sizeof(struct ChunkHeader)),
	     SEEK_CUR);
  Z->f++;
  Z->r = Z->r && Z->f < Z->n;
  SDL_Flip(screen_surface);
}

static void decode_frame(struct FLI *Z)
{
  struct FrameTypeChunkHeader   fhead;
  struct ChunkHeader          * chead = (struct ChunkHeader *) &fhead;

  while(Z->f < Z->n) {
    int off = SDL_RWtell(Z->F);
    SDL_RWread(Z->F, chead, sizeof(*chead), 1);
    switch(chead->type) {
    case      4: set_palette(Z);      break;
    case      7: decode_delta_flc(Z); break;
    case     15: decode_byte_run(Z);  break;
    case 0xF1FA: munch_frame(Z);      return;
    default:
                 abend(); break;
    }
    if( off+chead->size != SDL_RWtell(Z->F) ) {
      printf("fli_player: Want: %d, Have: %d, (%+d)!\n",
	     off+chead->size,
	     SDL_RWtell(Z->F),
	     (off+chead->size) - SDL_RWtell(Z->F));
      SDL_RWseek(Z->F, off+chead->size, SEEK_SET);
    }
  }
  Z->r = Z->r && Z->f < Z->n;
}

void fli_player_loop()
{
  union SDL_Event ev;
  struct FLI *Z;

  lua_getfield(L, LUA_REGISTRYINDEX, "CURRENT_FLI_ANIM");

  Z = luaL_checkudata(L, -1, META);
  lua_pop(L, 1);

  if(Z == NULL || !Z->r) {
    set_frame_time(Z->t);
    exit_mode();
    return;
  }

  decode_frame(Z);
}

/* ------------------------------------------------------------------------- */

static int fli_readheader(SDL_RWops *F, FliHeader *h)
{
  h->size      = SDL_ReadLE32(F);
  h->type      = SDL_ReadLE16(F);
  h->numFrames = SDL_ReadLE16(F);
  h->width     = SDL_ReadLE16(F);
  h->height    = SDL_ReadLE16(F);
  return h->type == MAGIC;
}

int fli_new(struct lua_State *L)
{
  struct rwops *rw;
  struct FliHeader h;
  struct FLI *fli;

  luaL_argcheck(L, is_rw(L, 1), 1, "Expected an rw");
  rw = lua_touserdata(L, 1);

  if((fli = lua_newuserdata(L, sizeof(struct FLI)))) {
    if(fli_readheader(rw->F, &h)) {
      if((fli->S = SDL_CreateRGBSurface(SDL_SWSURFACE,
					h.width, h.height, 8,
					0,0,0,0))) {
        lua_pushvalue(L, 1);
	fli->F = rw->F;
	fli->N = rw->N;
	fli->R = luaL_ref(L, LUA_REGISTRYINDEX);
	fli->w = h.width;
	fli->h = h.height;
	fli->n = h.numFrames;
	fli->f = 0;
	printf("%s : (%dx%d) (%d/%d)\n",
	       fli->N,
	       fli->w, fli->h,
	       fli->f, fli->n);
	luaL_getmetatable(L, META);
	lua_setmetatable(L, 2);
	return 1;
      } SDL_RWseek(rw->F, -(int)sizeof(struct FliHeader), RW_SEEK_CUR);
    } lua_pop(L, 1);
  } return 0;
}

int fli_gc(struct lua_State *L)
{
  struct FLI *Z = luaL_checkudata(L, 1, META);
  luaL_argcheck(L, Z, 1, "Expected an FLI animation");
  SDL_FreeSurface(Z->S);
  luaL_unref(L, LUA_REGISTRYINDEX, Z->R);
  return 0;
}

int fli_play(struct lua_State *L)
{
  struct FLI *Z = luaL_checkudata(L, 1, META);
  luaL_argcheck(L, Z, 1, "Expected an FLI animation");
  Z->r = 1;
  lua_setfield(L, LUA_REGISTRYINDEX, "CURRENT_FLI_ANIM");

  enter_mode(fli_player_loop);
  luaL_dostring(L, "input_mode(fliplayer_input); exit_fli = 0;");
  Z->t = set_frame_time(66);

  return 0;
}

int fli_stop(struct lua_State *L)
{
  struct FLI *Z = luaL_checkudata(L, 1, META);
  if(Z) Z->r = 0;
  return 0;
}

int fli_rewind(struct lua_State *L)
{
  struct FLI *Z = luaL_checkudata(L, 1, META);
  luaL_argcheck(L, Z, 1, "Expected an FLI animation");
  SDL_RWseek(Z->F, 0, SEEK_SET);
  return 0;
}

static struct luaL_Reg fli_meta[] = {
  "new",    fli_new,
  "gc",     fli_gc,
  "rewind", fli_rewind,
  "play",   fli_play,
  "stop",   fli_stop,
  0, 0,
};

int init_fliplayer(void)
{
  lua_pushstring(L, META);
  lua_newtable(L);
  lua_pushstring(L, "__index");
  lua_pushvalue(L, -2);
  lua_settable(L, -3);
  luaL_register(L, NULL, fli_meta);
  lua_settable(L, LUA_REGISTRYINDEX);
  lua_register(L, "fli", fli_new);
  return 1;
}
