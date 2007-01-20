#include "SDL.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define REV32(x) ({                                        \
  int n=x;                                                 \
  n = ((n >>  1) & 0x55555555) | ((n <<  1) & 0xaaaaaaaa); \
  n = ((n >>  2) & 0x33333333) | ((n <<  2) & 0xcccccccc); \
  n = ((n >>  4) & 0x0f0f0f0f) | ((n <<  4) & 0xf0f0f0f0); \
  n = ((n >>  8) & 0x00ff00ff) | ((n <<  8) & 0xff00ff00); \
  n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000); \
})

#define REV16(x) ({                                \
  int n=x;                                         \
  n = ((n >>  1) & 0x5555) | ((n <<  1) & 0xaaaa); \
  n = ((n >>  2) & 0x3333) | ((n <<  2) & 0xcccc); \
  n = ((n >>  4) & 0x0f0f) | ((n <<  4) & 0xf0f0); \
  n = ((n >>  8) & 0x00ff) | ((n <<  8) & 0xffff); \
})

#define REV8(x) ({ \
  int n=x;\
  n = ((n >> 1) & 0x55) | ((n << 1) & 0xaa); \
  n = ((n >> 2) & 0x33) | ((n << 2) & 0xcc); \
  n = ((n >> 4) & 0x0f) | ((n << 4) & 0xf0); })


#define CEIL8(x) ((x & 7)? x & ~7 + 8: x)

#define SWABd(x) ((SWABw(x) << 16) | ((SWABw(x >> 16) & 0xFFFF)))

#define SWABw(x) ((x << 8)|((x >> 8) & 0xFF))

extern char *error;

/* narcissus */
static const Uint32 RR=0xFF000000;
static const Uint32 GG=0x00FF0000;
static const Uint32 BB=0x0000FF00;
static const Uint32 AA=0x000000FF;

extern struct SDL_Surface * screen_surface;
extern        int           width;
extern        int           height;
extern        int           depth;

struct SDL_Surface *CreateZSurface(Uint16 w, Uint16 h);

void sdl_version(const char *n,
                 const struct SDL_version *c,
                 const struct SDL_version *d);

/* scheherazade */
int fli_new(struct lua_State *L);
int fli_gc(struct lua_State *L);
int fli_play(struct lua_State *L);
int fli_rewind(struct lua_State *L);

/* gaia */
#define SCALE 32 /* units per tile in map-space. */
#define W 64     /* tile size */
#define H 32

#define MS_x(x,y,z) ( ( 2*(x) - 2*(y) + 0*(z) ) / 2)
#define MS_y(x,y,z) ( ( 1*(x) + 1*(y) - 1*(z) ) / 2)
#define SM_x(x,y)   ( ( 1*(x) + 2*(y) ) / 2 )
#define SM_y(x,y)   ( (-1*(x) + 2*(y) ) / 2 )
#define TILE(x) ((x) & ~(SCALE-1))
#define SUB(x)  ((x) % SCALE)

#define GLOM(x,y,z) ((z << 16) + (y << 8) + (x << 0))

/* avatar */
void   blit_sprite(const Uint16 x, const Uint16 y, Sint16 i);
Uint16 blit_frame(Uint16 x, Uint16 y, Uint16 a, Uint16 f);

Uint8  do_sprites(Uint32 k, Uint16 sx, Uint16 sy); /* called from gaia */

/* chronos */
#define FRAMEEVENT (SDL_USEREVENT+1)
#define MUSICENDED (SDL_USEREVENT+2)

extern int frame_time;
extern int frame_number;

Uint16 set_frame_time(const Uint16 t);

/* hugin */
extern struct lua_State *L;

int          get_int(struct lua_State *L, int ix, const char *n,const int alt);
const char * get_str(struct lua_State *L, const char *n);
double       get_num(struct lua_State *L, const char *n);
void         set_num(struct lua_State *L, const char *n, const double d);
void         set_str(struct lua_State *L, const char *n, const char *s);

void debug_lua_table(struct lua_State *L, int ind);
void debug_lua_stack(struct lua_State *L, const char *n);

#define foreach(L, ix) for(lua_pushnil(L); lua_next(L, ix-1); lua_pop(L, 1))

/* munin */

struct rwops {
  struct SDL_RWops *F;
  const  char       N[1];
};

int isrwops    (struct lua_State *L, int i);

int rwops_new  (struct lua_State *L);
int rwops_gc   (struct lua_State *L);
int rwops_rew  (struct lua_State *L);

void *inhale_file(const char *name, Uint32 blk, Uint32 off);

/*
Uint16 SDL_ReadLE8(SDL_RWops *src);
Uint16 SDL_ReadBE8(SDL_RWops *src);
*/

/* nyarlathotep */
typedef void(*framefunc_t)(void);
typedef void(*mode_ends_t)(void);

void enter_mode(framefunc_t f);
void exit_mode();

void main_loop(void);

/* siren */
/* uenuku */
struct SDL_Color *get_pal(const char *n);

/* inits */
int init_narcissus(void);
int init_scheherazade(void);
int init_gaia(void);
int init_avatar(void);
int init_chronos(void);
int init_hugin(void);
int init_munin(void);
int init_nyarlathotep(void);
int init_siren(void);
int init_uenuku(void);
