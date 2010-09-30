#include "Syndicate/Data/MapSubTile.h"

using namespace Syndicate::Data;

template<typename T>
void MapSubTile::get(T *subtile, unsigned pitch, const T *colors) const
{
  if(subtile && pitch >= _width)
    {
      /* for each row */
      for(unsigned j = 0; j < _height; j ++)
	{
	  /* for each column */
	  for(unsigned i = 0; i < _width; i ++)
	    {
	      _lines[j].pixel(i, subtile + i, colors);
	    }
	  subtile = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(subtile) + pitch);
	}
    }
}

template void MapSubTile::get(uint8_t *subtile, unsigned pitch, const uint8_t *colors) const;
template void MapSubTile::get(uint32_t *subtile, unsigned pitch, const uint32_t *colors) const;
 
