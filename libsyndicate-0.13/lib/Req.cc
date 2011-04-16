#include "Syndicate/Data/Debug.h"
#include "Syndicate/Data/Req.h"

using namespace Syndicate::Data;

/****************************************************************
 *                                                              *
 ****************************************************************/
template<typename TColors>
void Req::get(unsigned id, TColors *pixels, unsigned pitch, const TColors *colors) const
{
  if(_entries && pixels && id < size() && pitch > _width)
    {
      for(unsigned y = 0; y < _height; y++)
	{
	  TColors *pix = reinterpret_cast<TColors *>(reinterpret_cast<uint8_t *>(pixels) + y * pitch);
	  for(unsigned x = 0; x < _width; x++)
	    {
	      _entries[id]._lines[y].pixel(x, pix + x, colors);
	    }
	}
    }
}

template void Req::get(unsigned id, uint8_t *pixels, unsigned pitch, const uint8_t *colors) const;
template void Req::get(unsigned id, uint32_t *pixels, unsigned pitch, const uint32_t *colors) const;
