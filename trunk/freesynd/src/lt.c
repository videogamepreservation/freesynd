#include <stdio.h>
#include "common.h"

int main(int argc, char **argv)
{
  init_hugin();

  debug_lua_stack(L, "main");

  luaL_dostring(L, "return sdlk[3]");

  debug_lua_stack(L, "main");
  printf("TOS: %f\n", lua_tonumber(L, -1));

  printf("hullo\n");

  lua_pop(L, 1);

  luaL_dofile(L, "scripts/config.lua");

  luaL_dostring(L, "return tostring(sprites[glom(668, 2700, 64)][1].a)");

  printf("TOS: \"%s\"\n", lua_tostring(L, -1));

  return 0;
}
