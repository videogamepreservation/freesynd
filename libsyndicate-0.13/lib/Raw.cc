#include "Syndicate/Data/Debug.h"
#include "Syndicate/Data/Raw.h"

using namespace Syndicate::Data;

/****************************************************************
 *                                                              *
 ****************************************************************/
template<typename TColors>
void Raw::get(unsigned id, unsigned width, unsigned height, TColors *pixels, unsigned pitch, const TColors *colors) const
{
  // FIXME : improve
  for(unsigned y = 0; y < height; y++)
    {
      TColors *pix = reinterpret_cast<TColors *>(reinterpret_cast<uint8_t *>(pixels) + y * pitch);
      for(unsigned x = 0; x < width; x++)
	{
	  pix[x] = colors[_data[y * width + x]];
	}
    }
}

template void Raw::get(unsigned id, unsigned width, unsigned height, uint8_t *pixels, unsigned pitch, const uint8_t *colors) const;
template void Raw::get(unsigned id, unsigned width, unsigned height, uint32_t *pixels, unsigned pitch, const uint32_t *colors) const;
