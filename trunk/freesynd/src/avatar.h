

/* should these be made static?
*/
void   blit_sprite(Uint16 x, Uint16 y, Uint32 i);
Uint16 blit_frame(Uint16 x, Uint16 y, Uint16 a, Uint16 f);

Uint8  do_sprites(Uint32 k, Uint16 sx, Uint16 sy); /* called from gaia */

int    init_avatar(void);
