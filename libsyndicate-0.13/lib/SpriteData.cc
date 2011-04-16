#include "Syndicate/Data/Debug.h"
#include "Syndicate/Data/SpriteData.h"

using namespace Syndicate::Data;

template<typename TColors>
void SpriteData::get(unsigned offset, unsigned width, unsigned height, bool h_flipped, TColors *pixels, unsigned pitch, const TColors *colors) const
{
  if (_data && pixels && offset < _data_length && width <= pitch)
    {
      uint8_t *data = reinterpret_cast<uint8_t *>(_data) + offset;

      // Encoded with blocks
      if(nb_sprites() == 0)
	{

	  Block8 *block = reinterpret_cast<Block8 *>(data);
	  for(unsigned y = 0, i = 0; y < height; y++)
	    {
	      TColors *pix = reinterpret_cast<TColors *>(reinterpret_cast<uint8_t *>(pixels) + y * pitch);
	      for(unsigned x = 0; x < width; x++, i++)
		{
		  block[i / Block8::_width].pixel(i % Block8::_width, pix + ((h_flipped == true)?(width - 1 - x):(x)), colors);
		}
	    }
	}
      // Encoded with rle
      else
	{
	  for(unsigned y = 0, x = 0; y < height; y++, x = 0)
 	    {
 	      int rl = *reinterpret_cast<int8_t *>(data++);
	      TColors *pix = reinterpret_cast<TColors *>(reinterpret_cast<uint8_t *>(pixels) + y * pitch);	      
	      while(rl != 0)
		{
		  if(rl < 0)
		    {
		      rl *= -1;
		      while(rl)
			{
			  pix[x] = colors[255];
			  rl--;
			  x++;
			}
		    }
		  else if(rl > 0)
		    {
		      while(rl)
			{
			  pix[x] = colors[*data++];
			  rl--;
			  x++;
			}
		    }
		  rl = *reinterpret_cast<int8_t *>(data++);;
		}
	    }
	}
    }
}

template void SpriteData::get(unsigned offset, unsigned width, unsigned height, bool h_flipped, uint8_t *sprite, unsigned pitch, const uint8_t *colors) const;
template void SpriteData::get(unsigned offset, unsigned width, unsigned height, bool h_flipped, uint32_t *sprite, unsigned pitch, const uint32_t *colors) const;
