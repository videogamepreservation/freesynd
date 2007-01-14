
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

#define REV8(x) ({                             \
  int n=x;                                     \
  n = ((n >> 1) & 0x55) | ((n << 1) & 0xaa);   \
  n = ((n >> 2) & 0x33) | ((n << 2) & 0xcc);   \
  n = ((n >> 4) & 0x0f) | ((n << 4) & 0xf0); })

/* wtf? */
#define SWABd(x) ((SWABw(x) << 16) | ((SWABw(x >> 16) & 0xFFFF)))

#define SWABw(x) ((x << 8)|((x >> 8) & 0xFF))

#define CEIL8(x) ((x & 7)? (x & ~7) + 8: x)

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

int init_narcissus(void);
