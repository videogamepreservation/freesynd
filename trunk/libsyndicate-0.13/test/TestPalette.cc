#include <cstdio>
#include <cstring>
#include <assert.h>

#include "Syndicate/Data/Palette.h"
#include "TestPalette.h"

int main(int argc, char **argv) 
{
  Syndicate::Data::Palette *palette = new Syndicate::Data::Palette();

  palette->load(ref_palette_filename);

  uint8_t colors[256 * 3];
  memset(colors, 0xFF, sizeof(colors));

  for(unsigned i = 0; i < 256; i++)
    {
      palette->get(i, colors[i * 3 + 0], colors[i * 3 + 1], colors[i * 3 + 2]);
    }

#if 0
  printf("{");
  for(unsigned i = 0; i < 256; i++)
    {
      printf("0x%02x, 0x%02x, 0x%02x, \n", colors[i * 3 + 0], colors[i * 3 + 1], colors[i * 3 + 2]);
    }
  printf("}\n");
#endif

  for(unsigned i = 0; i < 256; i++)
    {
      assert( colors[i * 3 + 0] == ref_colors[i * 3 + 0] );
      assert( colors[i * 3 + 1] == ref_colors[i * 3 + 1] );
      assert( colors[i * 3 + 2] == ref_colors[i * 3 + 2] );
    }

  delete palette;

  return 0;  
}

