#include "Syndicate/Data/Debug.h"
#include "Syndicate/Data/MapTile.h"

using namespace Syndicate::Data;

template<typename T>
void MapTile::get(uint8_t id, T *tile, unsigned pitch, const T *colors) const
{
  if(_data && tile && pitch >= _width)
    {
      // Pointer to this tile subtile's offset.
      //uint32_t *offsets = _data->_offsets[id];

      // For each row of subtiles
      for(unsigned j = 0; j < _sub_tiles_per_tile_y; j++)
	{
	  // For each column of subtiles
	  for(unsigned i = 0; i < _sub_tiles_per_tile_x; i++)
	    {
	      uint32_t offset = _data->_offsets[id][j + i * _sub_tiles_per_tile_y];

	      if (_min_offset <= offset && offset < _data_length)
		{
		  MapSubTile *subtile = reinterpret_cast<MapSubTile *>(reinterpret_cast<uint8_t *>(_data) + offset);
		  subtile->get(tile + i * MapSubTile::_width, pitch, colors);
		}
	    }

	  tile = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(tile) + pitch * MapSubTile::_height);
	}
    }
}
 
template void MapTile::get(uint8_t id, uint8_t *tile, unsigned pitch, const uint8_t *colors) const;
template void MapTile::get(uint8_t id, uint32_t *tile, unsigned pitch, const uint32_t *colors) const;
