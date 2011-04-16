#ifndef __TestMapTile_h__
#define __TestMapTile_h__

#include <inttypes.h>

static const char *ref_sprite_tab_filename = "HSPR-0.TAB";
static const unsigned ref_sprite_tab_index = 12;
static const struct 
{
  uint32_t _offset;
  uint8_t  _width;
  uint8_t  _height;
} ref_sprite_tab = {0x000002fa, 0x10, 0x07};

#endif // __TestMapTile_h__
