#include "SDL.h"
#include "SDL_mixer.h"
#include "lua.h"
#include "lauxlib.h"

#include "hugin.h"
#include "munin.h"
#include "narcissus.h"

extern char * error;

int    frequency = MIX_DEFAULT_FREQUENCY;
Uint16 format    = MIX_DEFAULT_FORMAT;
int    channels  = MIX_DEFAULT_CHANNELS;
int    chunksize = 1024;
/* ("http://osdl.sourceforge.net/OSDL/OSDL-0.3/src/doc/web/main/docume"
    "ntation/rendering/SDL-audio.html") */

static const char *SND_META = "snd_meta";
static const char *MUS_META = "mus_meta";

struct Sound {
  Mix_Chunk *mc;
  int ch;
};

int snd_new(struct lua_State *L)
{
  struct rwops *rw;
  struct Sound *S;

  luaL_argcheck(L, is_rw(L, 1), 1, "Expected an rw");
  rw = lua_touserdata(L, 1);

  if((S = lua_newuserdata(L, sizeof(struct Sound)))) {
    if((S->mc = Mix_LoadWAV_RW(rw->F, 0))) {
      S->ch = -1;
      luaL_getmetatable(L, SND_META);
      lua_setmetatable(L, 2);
      return 1;
    } else
      printf("snd_new: %s\n", SDL_GetError());
  }
  return 0;
}

int snd_gc(struct lua_State *L)
{
  struct Sound *S = luaL_checkudata(L, 1, SND_META);
  luaL_argcheck(L, S, 1, "Expected a sound");

  if(S->ch >= 0) Mix_HaltChannel(S->ch);
  Mix_FreeChunk(S->mc);
  return 0;
}

int snd_play(struct lua_State *L)
{
  struct Sound *S = luaL_checkudata(L, 1, SND_META);
  luaL_argcheck(L, S, 1, "Expected a sound");

  if(S->ch != -1) Mix_HaltChannel(S->ch);
  S->ch = Mix_PlayChannel(-1, S->mc, 0);
  return 0;
}

int snd_stop(struct lua_State *L)
{
  struct Sound *S = luaL_checkudata(L, 1, SND_META);
  luaL_argcheck(L, S, 1, "Expected a sound");

  if(S->ch != -1) Mix_HaltChannel(S->ch);
  return 0;
}

int snd_pause(struct lua_State *L)
{
  struct Sound *S = luaL_checkudata(L, 1, SND_META);
  luaL_argcheck(L, S, 1, "Expected a sound");

  if(S->ch >= 0) Mix_Pause(S->ch);
  return 0;
}

int snd_resume(struct lua_State *L)
{
  struct Sound *S = luaL_checkudata(L, 1, SND_META);
  luaL_argcheck(L, S, 1, "Expected a sound");

  if(S->ch >= 0) Mix_Resume(S->ch);
  return 0;
}

int snd_volume(struct lua_State *L)
{
  struct Sound *S = luaL_checkudata(L, 1, SND_META);
  luaL_argcheck(L, S, 1, "Expected a sound");

  int v;

  v = (lua_isnumber(L, 2))? lua_tonumber(L, 2): -1;
  v = (S->ch >= 0)? Mix_Volume(S->ch, v): -1;
  lua_pushnumber(L, v);
  return 1;
}

static struct luaL_Reg snd_meta[] = {
  "new",    snd_new,
  "gc",     snd_gc,
  "play",   snd_play,
  "stop",   snd_stop,
  "pause",  snd_pause,
  "resume", snd_resume,
  "volume", snd_volume,
  0, 0,
};

int siren_resume(struct lua_State *L) { Mix_Resume(-1); return 0; }
int siren_pause (struct lua_State *L) { Mix_Pause(-1);  return 0; }

/*****************************************************************************/

int mus_new(struct lua_State *L)
{
  const char *name;
  Mix_Music **M;

  name = lua_tostring(L, 1);

  if((M = lua_newuserdata(L, sizeof(Mix_Music *)))) {
    if((*M = Mix_LoadMUS(name))) {
      luaL_getmetatable(L, MUS_META);
      lua_setmetatable(L, 2);
      return 1;
    }
  }
  printf("music_new: %s\n", SDL_GetError());
  return 0;
}

int mus_gc(struct lua_State *L)
{
  Mix_Music **M = luaL_checkudata(L, 1, MUS_META);
  luaL_argcheck(L, *M, 1, "Expected a music object");

  printf("music_gc\n");
  Mix_HaltMusic();
  Mix_FreeMusic(*M);
  return 0;
}

int mus_play(struct lua_State *L)
{
  Mix_Music **M = luaL_checkudata(L, 1, MUS_META);
  luaL_argcheck(L, *M, 1, "Expected a music object");

  int l;

  l = (lua_isnumber(L, 2))? lua_tonumber(L, 2)-1: 0;
  l = Mix_PlayMusic(*M, l);
  return 0;
}

int mus_stop(struct lua_State *L)
{
  Mix_Music **M = luaL_checkudata(L, 1, MUS_META);
  luaL_argcheck(L, *M, 1, "Expected a music object");

  Mix_HaltMusic();
  return 0;
}

int mus_pause(struct lua_State *L)
{
  Mix_Music **M = luaL_checkudata(L, 1, MUS_META);
  luaL_argcheck(L, *M, 1, "Expected a music object");

  Mix_PauseMusic();
  return 0;
}

int mus_resume(struct lua_State *L)
{
  Mix_Music **M = luaL_checkudata(L, 1, MUS_META);
  luaL_argcheck(L, *M, 1, "Expected a music object");

  if(Mix_PlayingMusic()) Mix_ResumeMusic();
  else                   Mix_PlayMusic(*M, -1);
  return 0;
}

int mus_volume(struct lua_State *L)
{
  Mix_Music **M = luaL_checkudata(L, 1, MUS_META);
  luaL_argcheck(L, *M, 1, "Expected a music object");

  int v;
  v = (lua_isnumber(L, 2))? lua_tonumber(L, 2)-1: 0;

  v = Mix_VolumeMusic(v);

  lua_pushnumber(L, v);
  return 0;
}

int mus_seek(struct lua_State *L)
{
  Mix_Music **M = luaL_checkudata(L, 1, MUS_META);
  luaL_argcheck(L, *M, 1, "Expected a music object");

  int p;

  p = (lua_isnumber(L, 2))? lua_tonumber(L, 2): 0;
  if(!p) {
    Mix_RewindMusic();
    return 0;
  }

  switch(Mix_GetMusicType(*M)) {
  case MUS_MP3: Mix_RewindMusic();    /* fall-through */
  case MUS_MOD:                       /* fall-through */
  case MUS_OGG: Mix_SetMusicPosition(p); break;
  }
  
  return 0;
}

static struct luaL_Reg mus_meta[] = {
  "new",    mus_new,
  "gc",     mus_gc,
  "play",   mus_play,
  "stop",   mus_stop,
  "pause",  mus_pause,
  "resume", mus_resume,
  "volume", mus_volume,
  0, 0,
};



/*****************************************************************************/

int init_siren(void)
{
  printf("Siren sound and music module:\n");

  {
    struct SDL_version compiled;
    SDL_MIXER_VERSION(&compiled);
    sdl_version("SDL_mixer", &compiled, Mix_Linked_Version());
  }

  Mix_QuerySpec(0, 0, 0);
  if(Mix_OpenAudio(frequency, format, channels, chunksize) < 0) {
    printf("  -  Mix_OpenAudio: %s\n", SDL_GetError());
    return 0;
  }

  {
    lua_pushstring(L, SND_META);
    lua_newtable(L);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_register(L, NULL, snd_meta);
    lua_settable(L, LUA_REGISTRYINDEX);
    lua_register(L, "snd", snd_new);

    lua_pushstring(L, MUS_META);
    lua_newtable(L);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_register(L, NULL, mus_meta);
    lua_settable(L, LUA_REGISTRYINDEX);
    lua_register(L, "mus", mus_new);
  }

  return 1;
}
