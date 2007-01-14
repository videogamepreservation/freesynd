#include "SDL.h"
#include "narcissus.h"
#include "uenuku.h"
#include "munin.h" /* Inhale file */

struct SpriteAnim {
  Uint16 b; /* begin    */
  Uint8  w;
  Uint8  h;
  Uint16 f; /* flags    */
  Uint16 n; /* next     */
} __attribute__((packed));

struct SpriteFrag {
  Uint16 s; /* sprite   */
  Sint16 x;
  Sint16 y;
  Uint16 m; /* mirror   */
  Uint16 n; /* next     */
} __attribute__((packed));

struct SprTab {
  Uint32 o; /* file off */
  Uint8  w; /* width    */
  Uint8  h; /* height   */
} __attribute__((packed));

struct SpriteFile       { Uint32 n; struct SprTab s[1]; };

typedef struct SDL_Surface *surfptr_t;

struct SpriteAnimIndex  { Uint32 n; Uint16 I[1]; };
struct SpriteAnimVector { Uint32 n; struct SpriteAnim a[1]; };
struct SpriteFragVector { Uint32 n; struct SpriteFrag f[1]; };
struct SpriteVector     { Uint32 n;        surfptr_t  (s[1]); };

static struct SpriteAnimIndex  *SI; /* index_  */
static struct SpriteAnimVector *SA; /* GSF     */
static struct SpriteFragVector *SF; /* GSFE    */
static struct SpriteVector     *SN;  /* normal sprites */
static struct SpriteVector     *SM;  /* mirror sprites */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void blit_sprite(const Uint16 x, const Uint16 y, Sint16 i)
{
  struct SDL_Rect      R = { 0, 0, 0, 0 };
  struct SpriteVector *S = (i>0)? SN: SM;

  if((i = (i<0)? -i: i) < S->n && S->s[i] != NULL) {
    R.x = x, R.y = y, R.w = S->s[i]->w, R.h = S->s[i]->h;
    SDL_BlitSurface(S->s[i], NULL, screen_surface, &R);
  }
}

Uint16 blit_frame(Uint16 x, Uint16 y, Uint16 a, Uint16 f)
{
  Uint16 c;

  if(a >= SI->n) return -1;
  
  for(c = f, f = SI->I[a]; c; c--) f = SA->a[f].n;

  for(c = SA->a[f].b; c; c = SF->f[c].n)
    if(!SF->f[c].m) blit_sprite(x + SF->f[c].x, SF->f[c].y+y,  SF->f[c].s);
    else            blit_sprite(x + SF->f[c].x, SF->f[c].y+y, -SF->f[c].s);

  return SA->a[f].n == SI->I[a];
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static void unblk1(Uint8 *z, const Uint8 *p)
{
  Uint8 i=8; /* p->[  t p0 p1 p2 p3 ] => z->[ px px px px    ] */
  while(i--) z[7-i] = ( !!(p[0] & 1 << i) * 0xFF |
			!!(p[1] & 1 << i) << 0   |
			!!(p[2] & 1 << i) << 1   |
			!!(p[3] & 1 << i) << 2   |
			!!(p[4] & 1 << i) << 3   );
}

int load_sprite(struct SpriteVector * SN, struct SpriteVector * SM,
		struct SpriteFile   *  Q, const int i, const void *p)
{
  struct SDL_Surface *Z, *Y;
  struct SDL_Rect R = { 0, 0, 0, 0 };
  Uint8 *z, x, y;

  R.w = CEIL8(Q->s[i].w), R.h = Q->s[i].h;

  if(!R.w || !R.h || !(Z = CreateZSurface(R.w, R.h))) return 0;

  z = Z->pixels;
  memset(z, 255, R.w * R.h);
  p += Q->s[i].o;

  for(y = 0; y < R.h; y++)
    for(x = 0; x < R.w; x += 8, p += 5)
      unblk1( &z[ R.w * y + x ], p);

  SN->s[i] = Z;

  if((Y = CreateZSurface(R.w, R.h)) == NULL)
    printf("Error when creating mirrored sprite: %s", SDL_GetError());
  else { 
    Uint8 *q; /* Mirror */

    z = Z->pixels;
    q = Y->pixels;
    for(y = 0; y < R.h; y++)
      for(x = 0; x < R.w; x++)
	q[y*R.w+x] = z[(y+1)*R.w-x-1];

    SM->s[i] = Y;
  }

  return 1;
}

load_rle_sprite(struct SpriteVector * SN, struct SpriteVector * SM,
		struct SpriteFile   *  Q, const int i, const void *p)
{
  struct SDL_Surface *Z, *Y;
  struct SDL_Rect R = { 0, 0, 0, 0 };
  Uint8 *z, x, y;
  Sint16 r;

  R.w = CEIL8(Q->s[i].w), R.h = Q->s[i].h;

  if(!R.w || !R.h || !(Z = CreateZSurface(R.w, R.h))) {
    if(!R.w) printf("  -  sprite %d has width  zero.\n", i);
    if(!R.h) printf("  -  sprite %d has height zero.\n", i);
    if(!Z)   printf("  -  couldn't createZsurface for sprite %d.\n", i);
    return 0;
  }

  z = Z->pixels;
  memset(z, 255, R.w * R.h);
  p += Q->s[i].o;

  for(x = y = 0, r = 1; y < R.h; y++, x=0) {
    r = *((Sint8 *) p++);
    while(r && x < Q->s[i].w) {
      if( r > 0 ) memcpy( &z[ R.w * y + x ],   p, r = +r), p += r;
      if( r < 0 ) memset( &z[ R.w * y + x ], 255, r = -r);
      x += r;
      r = *((Sint8 *) p++);
    }
  }

  SDL_SetColors(Z, get_pal("data/mselect.pal"), 0, 256);
  SN->s[i] = Z;

  if((Y = CreateZSurface(R.w, R.h)) == NULL)
    printf("Error when creating mirrored sprite: %s", SDL_GetError());
  else { 
    Uint8 *q; /* Mirror */

    z = Z->pixels;
    q = Y->pixels;
    for(y = 0; y < R.h; y++)
      for(x = 0; x < R.w; x++)
	q[y*R.w+x] = z[(y+1)*R.w-x-1];

    SDL_SetColors(Y, get_pal("data/mselect.pal"), 0, 256);
    SM->s[i] = Y;
  }

  return 1;
}


/* TODO: Since the sprites can only be of certain widths,
 *       make a stripe for each width.
 */
int init_avatar(void)
{
  int i, x;

  printf("Avatar sprite module:\n");

  { /* load sprite images */
    struct { Uint32 n; Uint8 p[1]; } *p;
    struct SpriteFile *Q;

    /* ko: mspr mfnt-0, ok: hspr-0  hpointer  */
    Q = inhale_file("data/hspr-0.tab", sizeof(struct SprTab), 0);
    p = inhale_file("data/hspr-0.dat", 1, 0);
    /*
    Q = inhale_file("data/mspr-0.tab", sizeof(struct SprTab), 0);
    p = inhale_file("data/mspr-0.dat", 1, 0);
    */

    if(((SN = calloc(sizeof(struct SpriteVector) +
		     sizeof(struct SDL_Surface *) * (Q->n - 1), 1)) == NULL) ||
       ((SM = calloc(sizeof(struct SpriteVector) +
		     sizeof(struct SDL_Surface *) * (Q->n - 1), 1)) == NULL))
      abend(0);

    /*if(load_rle_sprite(SN, SM, Q, i, p->p))*/
    for(x = i = 0; i <  Q->n; i++)
      if(load_sprite(SN, SM, Q, i, p->p))
	SM->n = SN->n = i+1, x++;

    printf("  +  %d of %d sprite images loaded\n", x, Q->n);
    free(Q);
    free(p);
  }

  { /* load sprite anim frags. */
    Uint16 *w;

    SF = inhale_file("data/hele-0.ani", sizeof(struct SpriteFrag), 0);

    for(i = 0; i < SF->n; i++)
      if(SF->f[i].s % 6) {
	printf("eh?! SF %d starts at %d  (%% %d)\n",
	       i, SF->f[i].s, SF->f[i].s % 6);
	printf("  s:%d x:%d y:%d m:%d n:%d\n",
	       SF->f[i].s, SF->f[i].x, SF->f[i].y, SF->f[i].m, SF->f[i].n);
      }
      else     SF->f[i].s /= 6;

    printf("  +  %d spritefrags loaded\n", SF->n);
  }
  { /* load sprite anim */
    SA = inhale_file("data/hfra-0.ani", sizeof(struct SpriteAnim), 0);
    for(i = 0; i < SA->n; i++)
      if(SA->a[i].n > SA->n)
	abend("Animation continues beyond array bounds!");

    printf("  +  %d spriteanims loaded\n", SA->n);
  }
  { /* load index (?) */
    SI = inhale_file("data/hsta-0.ani", sizeof(Uint16), 0);
    for(i = 0; i < SI->n; i++)
      if(SI->I[i] > SA->n)
	abend("AnimIndex refers beyond array bounds!");

    printf("  +  %d elements in animation index loaded\n", SI->n);
  }
}
