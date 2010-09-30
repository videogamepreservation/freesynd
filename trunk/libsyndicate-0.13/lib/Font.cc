#include "Syndicate/Data/Debug.h"
#include "Syndicate/Data/Font.h"

using namespace Syndicate::Data;

#include <cstdio>

/****************************************************************
 *                                                              *
 ****************************************************************/
template<typename TColors>
void Font::get(unsigned id, TColors *pixels, unsigned pitch, const TColors *colors) const
{
  if(_data && pixels && id < _nb_fonts)
    {
      unsigned offset      = _data->_tab[id]._offset;
      unsigned width       = _data->_tab[id]._width;
      unsigned height      = _data->_tab[id]._height;
      unsigned line_offset = _data->_tab[id]._line_offset;

      if((_nb_fonts * 5) < offset && offset < _data_length && width <= pitch)
	{ 
	  uint8_t *data = reinterpret_cast<uint8_t *>(_data);

	  if(width < 8)
	    {	
	      //Debug("%c %u %u %u %u (%u)\n", id, offset, width, height, line_offset, sizeof(Block810));

	      Block810 *block = reinterpret_cast<Block810 *>(data + offset);
	      for(unsigned y = 0; y < height; y++)
		{
		  TColors *pix = reinterpret_cast<TColors *>(reinterpret_cast<uint8_t *>(pixels) + (line_offset + y) * pitch);
		  for(unsigned x = 0; x < width; x++)
		    {
		      block[x / Block810::_width].pixel(x % Block810::_width, pix + x, colors);
		    }
		  block += 1 + ((width - 1) / Block810::_width);	      
		}
	    }
	  else
	    {
	      //Debug("%c %u %u %u %u (%u)\n", id, offset, width, height, line_offset, sizeof(Block1610));

	      Block1610 *block = reinterpret_cast<Block1610 *>(data + offset);
	      for(unsigned y = 0; y < height; y++)
		{
		  TColors *pix = reinterpret_cast<TColors *>(reinterpret_cast<uint8_t *>(pixels) + (line_offset + y) * pitch);
		  for(unsigned x = 0; x < width; x++)
		    {
		      block[x / Block1610::_width].pixel(x % Block1610::_width, pix + x, colors);
		    }
		  block += 1 + ((width - 1) / Block1610::_width);
		}
	    }
	}
    }
}

template void Font::get(unsigned id, uint8_t *pixels, unsigned pitch, const uint8_t *colors) const;
template void Font::get(unsigned id, uint32_t *pixels, unsigned pitch, const uint32_t *colors) const;
