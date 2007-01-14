#include <stdio.h>
#include "lua.h"
#include "lauxlib.h"
#include "hugin.h"

int main(int argc, char **argv)
{
  init_hugin();

  debug_lua_stack(L, "main");

  luaL_dostring(L, "return sdlk[3]");

  debug_lua_stack(L, "main");
  printf("TOS: %f\n", lua_tonumber(L, -1));

  printf("hullo\n");

}
