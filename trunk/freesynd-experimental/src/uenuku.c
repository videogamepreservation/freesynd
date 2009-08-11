#include "common.h"

static const Uint16 colsize = 3*sizeof(Uint8);
static const Uint16 palsize = 256*3*sizeof(Uint8);

struct Palette { char *n; struct SDL_Color p[256]; };

struct Palette pals[] = {
  {"data/hpal01.dat",   0 },  {"data/hpal02.dat",   0 },
  {"data/hpal03.dat",   0 },  {"data/hpal04.dat",   0 },
  {"data/hpal05.dat",   0 },  {"data/hpalette.dat", 0 },
  {"data/mselect.pal",  0 },  { 0, 0 }
};

struct SDL_Color *get_pal(const char *n)
{
  int i;

  for(i=0; pals[i].n; i++) if(!strcmp(pals[i].n, n)) return pals[i].p;

  return NULL;
}

/* int str_pal(struct lua_State *L) // sensible addition. */

int init_uenuku(void)
{
  struct SDL_RWops *F; int i; Uint8 c = 0;

  for(i=0; pals[i].n; i++) {
    if((F = SDL_RWFromFile(pals[i].n, "rb"))) {
      do SDL_RWread(F, &pals[i].p[c], colsize, 1); while(++c);
      do pals[i].p[c].r<<=2,pals[i].p[c].g<<=2,pals[i].p[c].b<<=2; while(++c);
      SDL_RWclose(F);
    }
  }

  printf("Uenuku palettes\n  +  %d palettes\n", i);

  return 1;
}
