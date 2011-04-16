#include <cstdio>
#include <cstring>
#include <assert.h>

#include "Syndicate/Data/MapTile.h"
#include "TestMapTile.h"

int main(int argc, char **argv) 
{
  Syndicate::Data::MapTile *map_tile = new Syndicate::Data::MapTile();

  map_tile->load(ref_tile_filename);

  uint8_t tile_data[Syndicate::Data::MapTile::_width * Syndicate::Data::MapTile::_height];
  memset(tile_data, 0xFF, sizeof(tile_data));

  map_tile->get(ref_tile_index, tile_data);

#if 0
  printf("{");
  for(unsigned y = 0; y < Syndicate::Data::MapTile::_height; y++)
    {
      for(unsigned x = 0; x < Syndicate::Data::MapTile::_width; x++)
	{
	  printf("0x%02x, ", tile_data[y * Syndicate::Data::MapTile::_width + x]);
	}
    }
  printf("}\n");
#endif

  for(unsigned i = 0; i < sizeof(tile_data); i++)
    {
      assert( tile_data[i] == ref_tile_data[i] );
    }

  delete map_tile;

  return 0;  
}

