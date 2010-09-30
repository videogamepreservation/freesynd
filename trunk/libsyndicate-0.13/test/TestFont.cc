#include <cstdio>
#include <cstring>
#include <assert.h>

#include "Syndicate/Data/Font.h"
#include "TestFont.h"

int main(int argc, char **argv) 
{
  Syndicate::Data::Font *font = new Syndicate::Data::Font();

  font->load(ref_font_filename);

  uint8_t pixels[sizeof(ref_font_pixels)];
  memset(pixels, 0xff, sizeof(pixels));

  font->get(ref_font_id, pixels, ref_font_pitch);

  for(unsigned i = 0; i < sizeof(pixels); i++)
    {
      assert( ref_font_pixels[i] == pixels[i] );
    }

#if 0
  printf("{\n");
  for(unsigned j = 0; j < 12; j++)
    {
      for(unsigned i = 0; i < 12; i++)
	{
	  printf("0x%02x, ", pixels[j * 12 + i]);
	}
    }
  printf("}\n");
#endif

  delete font;

  return 0;  
}

