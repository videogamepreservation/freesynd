#include "common.h"

SDL_Surface * screen_surface = NULL;
#if 0
/*
int           width          = 320;
int           height         = 200;
*/
#else
int           width          = 640;
int           height         = 400;
/* works.
int           width          = 1280;
int           height         = 1024;
*/
#endif
int           depth          = 32;

struct SDL_Color tile_palette[] = {
  0x00,0x00,0x00,0, 0xac,0x5c,0x3c,0, 0x3c,0x48,0x44,0, 0xfc,0xa4,0x60,0,
  0xbc,0x98,0x98,0, 0x70,0x54,0x04,0, 0xa8,0x24,0x00,0, 0x4c,0x58,0x78,0,
  0x34,0x38,0x40,0, 0xc8,0x84,0x50,0, 0x64,0x64,0x64,0, 0xfc,0xbc,0x00,0,
  0xdc,0xfc,0xfc,0, 0x94,0x78,0x18,0, 0xfc,0x3c,0x08,0, 0x00,0x8c,0xb8,0, };

/*****************************************************************************/

SDL_Surface *CreateZSurface(Uint16 w, Uint16 h)
{
  struct SDL_Surface *Z;
  
  if((Z = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCCOLORKEY,w,h,8,0,0,0,0))) {
    SDL_SetColorKey(Z, SDL_SRCCOLORKEY, 255);
    SDL_SetColors(Z, tile_palette, 0, 16);
  }
  return Z;
}

void sdl_version(const char *n,
		 const struct SDL_version *c,
		 const struct SDL_version *d)
{
  printf("  +  %s compiled against %d.%d.%d, dynamically linked to %d.%d.%d\n",
	 n, c->major, c->minor, c->patch, d->major, d->minor, d->patch);
}

int init_narcissus(void)
{
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    error = SDL_GetError();
    return 0;
  }

  printf("Narcissus graphics module:\n");
  
  {
    struct SDL_version compiled;

    SDL_VERSION(&compiled);
    sdl_version("SDL", &compiled, SDL_Linked_Version());
  }

  {
    const SDL_VideoInfo *vi = SDL_GetVideoInfo();
    const char *a[] = {"No", "Yes"};
    const char *m = "-+";

    printf("  %c  Windowmanager available?                    %s\n",
	   m[vi->wm_available], a[vi->wm_available]);
    printf("  %c  Hardware surfaces?                          %s\n",
	   m[vi->hw_available], a[vi->hw_available]);
    printf("  %c  Hardware accelerated blitting?              %s\n",
	   m[vi->blit_hw], a[vi->blit_hw]);
    printf("  %c  Hardware Colorkeyed blits accelerated?      %s\n",
	   m[vi->blit_hw_CC], a[vi->blit_hw_CC]);
    printf("  %c  Hardware Alpha-bits blits accelerated?      %s\n",
	   m[vi->blit_hw_A], a[vi->blit_hw_A]);
    printf("  %c  Soft-to-hard blits accelerated?             %s\n",
	   m[vi->blit_sw], a[vi->blit_sw]);
    printf("  %c  Soft-to-hard Colorkeyed blits accelerated?  %s\n",
	   m[vi->blit_sw_CC], a[vi->blit_sw_CC]);
    printf("  %c  Soft-to-hard Alpha-bits blits accelerated?  %s\n",
	   m[vi->blit_sw_A], a[vi->blit_sw_A]);
    printf("  %c  Are color fill accelerated?                 %s\n",
	   m[vi->blit_fill], a[vi->blit_fill]);
    printf("  o  Accessible hardware video memory?           %d KiB\n",
	   vi->video_mem);
    printf("  o  Current bits per pixel?                     %d\n",
	   vi->vfmt->BitsPerPixel);
  }

  SDL_WM_SetCaption("FreeSynd", NULL);
  screen_surface = SDL_SetVideoMode(width, height, depth,
				    SDL_DOUBLEBUF | SDL_HWSURFACE);

  set_num(L, "width",  width);
  set_num(L, "height", height);

  return 1;
}
