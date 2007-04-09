#include "common.h"

/*
 *  Main loop is basically an event dispatcher.
 *
 *  Should it return the old fun when registering a new one?
 *
 */

#define FRAMEEVENT (SDL_USEREVENT+1)
#define MUSICENDED (SDL_USEREVENT+2)

static framefunc_t  frame      = NULL;
/*static mode_ends_t  end_cb     = NULL;*/
static const char * input_mode = NULL;
static struct SDL_QuitEvent ue = { SDL_QUIT };

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static int push_quit_event(struct lua_State *L)
{
  SDL_PushEvent((SDL_Event *) &ue);
  return 0;
}

static Uint8 warn=0;

static int set_input_mode(struct lua_State *L)
{
  luaL_argcheck(L, lua_istable(L, 1), 1, "Expected a table");
  lua_setfield(L, LUA_REGISTRYINDEX, "INPUT_MODE");
  warn = 0;
  return 0;
}

static Uint8 get_input_fun(struct lua_State *L) /* Eats key from stack */
{
  lua_getfield(L, LUA_REGISTRYINDEX, "INPUT_MODE");
  if(!lua_istable(L, -1)) {
    if(!warn) printf("no input mode!\n"), warn++;
    lua_pop(L, 2);      /* pop:    key, table ->            */
    return 0;           /* Return, having eaten the key. (and non-table) */
  }

  lua_insert(L, -2);    /* exch:   key, table -> table, key */
  lua_gettable(L, -2);  /* lookup: table, key -> table, val */

  if(lua_isfunction(L, -1)) {
    lua_insert(L, -2);  /* exch:   table, fun -> fun, table */
    lua_pop(L, 1);      /* pop:    fun, table -> fun        */
    return 1;           /* Return, having eaten the key, but FUN ON STACK */
  } else {
    if(!lua_isnil(L, -1)) {
      printf("Something completely different as input fun...\n");
      debug_lua_stack(L, "wtf?");
    }
    lua_pop(L, 2);      /* pop     table, val -> table      */
    return 0;           /* Return, having eaten the key. (and non-function) */
  }
}

static void keycall(const union SDL_Event *e)
{
  int k;

  switch(e->type) {
  case SDL_KEYDOWN:           k =  e->key.keysym.sym; break;
  case SDL_KEYUP:             k = -e->key.keysym.sym; break;
  case SDL_MOUSEBUTTONDOWN:   k =  e->button.button;  break;
  case SDL_MOUSEBUTTONUP:     k = -e->button.button;  break;
  }

  lua_pushnumber(L, k);
  if(!get_input_fun(L)) return;

  lua_pushnumber(L, k);
  if(k > 10) lua_pushnumber(L, e->key.keysym.unicode), lua_pushnil(L);
  else       lua_pushnumber(L, e->button.x), lua_pushnumber(L, e->button.y);

  if(lua_pcall(L, 3, 0, 0) && BUG(L, "keycall"))
    printf("keycall: \"%s\"\n", lua_tostring(L, -1)), lua_pop(L, 1);
}

static void motion(const union SDL_Event *e)
{
  lua_pushstring(L, "motion");
  if(!get_input_fun(L)) return;

  lua_pushnumber(L, e->motion.state);
  lua_pushnumber(L, e->motion.x);
  lua_pushnumber(L, e->motion.y);
  lua_pushnumber(L, e->motion.xrel);
  lua_pushnumber(L, e->motion.yrel);
  if(lua_pcall(L, 5, 0, 0))
    printf("motion: \"%s\"\n", lua_tostring(L, -1)), lua_pop(L, 1);
}

static void music_ended(void)
{
  lua_pushstring(L, "music");
  if(!get_input_fun(L)) return;

  if(lua_pcall(L, 0, 0, 0) && BUG(L, "music"))
    printf("music: \"%s\"\n", lua_tostring(L, -1)), lua_pop(L, 1);
}

static void do_frame(void)
{
  lua_pushstring(L, "frame");
  if(get_input_fun(L))
    if(lua_pcall(L, 0, 0, 0) && BUG(L, "frame"))
      printf("frame: \"%s\"\n", lua_tostring(L, -1)), lua_pop(L, 1);

  if(frame) frame();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void enter_mode(framefunc_t f)
{
  size_t n;

  printf("enter_mode > %d\n", lua_gettop(L));

  lua_getfield(L, LUA_REGISTRYINDEX, "MODE_STACK");  /* >t     */
  n = lua_objlen(L, -1);
  lua_pushlightuserdata(L, frame);                   /* >t,u   */
  lua_rawseti(L, -2, n);                             /* >t     */
  lua_pop(L, 1);                                     /* >      */

  lua_getfield(L, LUA_REGISTRYINDEX, "INPUT_STACK"); /* >t     */
  n = lua_objlen(L, -1);
  lua_getfield(L, LUA_REGISTRYINDEX, "INPUT_MODE");  /* >t,i   */
  lua_rawseti(L, -2, n);                             /* >t     */
  lua_pop(L, 1);                                     /* >      */

  frame  = f;

  printf("enter_mode < %d\n", lua_gettop(L));
}

void exit_mode(void)
{
  size_t n;

  printf("exit_mode > %d\n", lua_gettop(L));         /* >      */

  lua_getfield(L, LUA_REGISTRYINDEX, "MODE_STACK");  /* >t     */
  n = lua_objlen(L, -1);
  lua_rawgeti(L, -1, n);                             /* >t,f   */
  if(lua_isnil(L, -1)) push_quit_event(L);
  frame = lua_touserdata(L, -1);                     /* >t,f   */
  lua_pushnil(L);                                    /* >t,f,n */
  lua_rawseti(L, -3, n);                             /* >t,f   */
  lua_pop(L, 2);                                     /* >      */

  lua_getfield(L, LUA_REGISTRYINDEX, "INPUT_STACK"); /* >t     */
  n = lua_objlen(L, -1);
  lua_rawgeti(L, -1, n);                             /* >t,f   */
  lua_setfield(L, LUA_REGISTRYINDEX, "INPUT_MODE");  /* >t     */
  lua_pushnil(L);                                    /* >t,n   */
  lua_rawseti(L, -2, n);                             /* >t     */
  lua_pop(L, 1);                                     /* >      */

  printf("exit_mode < %d\n", lua_gettop(L));         /* >      */
}

void main_loop(void)
{
  union SDL_Event ev;

  while(SDL_WaitEvent(&ev)) {
    switch(ev.type) {
    case FRAMEEVENT:                  do_frame();                     break;
    case MUSICENDED:                  music_ended();                  break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:           keycall(&ev);                   break;
    case SDL_MOUSEMOTION:             motion(&ev);                    break;
    case SDL_QUIT:                    return;                         break;
    case SDL_ACTIVEEVENT:                                             break;
    case SDL_VIDEORESIZE:
    default:
      ;
    }
  }
}

int init_nyarlathotep(void)
{
  printf("Nyarlathotep event dispatcher module.\n");

  lua_newtable(L);
  lua_setfield(L, LUA_REGISTRYINDEX, "MODE_STACK");
  lua_newtable(L);
  lua_setfield(L, LUA_REGISTRYINDEX, "INPUT_STACK");
  lua_register(L, "input_mode",  set_input_mode);
  lua_register(L,       "quit", push_quit_event);
  return 1;
}
