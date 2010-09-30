#include <unistd.h>

#include "Sdl.h"

int main(int argc, char **argv)
{
  unsigned game_index = 1;

  while(--argc)
    {
      game_index = strtol(argv[argc], 0, 0);
    }

  Game *game = new Game();

  game->init();

  game->load(game_index);

  Sdl *sdl = new Sdl(game);

  sdl->init();

  sdl->start();

  sdl->wait();

  sdl->clean();

  delete sdl;

  delete game;

  return 0;
}
