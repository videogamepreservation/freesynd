#include "Syndicate/Data/Debug.h"
#include "Syndicate/Data/Block.h"

namespace Syndicate
{ 
  namespace Data
  { 
    template <typename T, unsigned depth, unsigned alpha> template <typename TColors>
    void Block<T, depth, alpha>::pixel(unsigned i, TColors *pix, const TColors *colors) const
    {
      i &= (_width - 1);

      T mask = (T)1 << (_width - 1 - i);

      if (!alpha || !(_bits[0] & mask))
	{
	  uint8_t col_idx = 0;
	  for(i = 0; i < depth; i++)
	    {
	      col_idx += (_bits[alpha + i] & mask)?(1 << i):(0);
	    }
	  *pix = colors[col_idx];
	}
    }

    template <> template <typename TColors>
    void Block<be_uint32_t, 4, 1>::pixel(unsigned i, TColors *pix, const TColors *colors) const
    {
      i &= (_width - 1);

      uint32_t mask = 1 << (_width - 1 - i);

      if (!(_bits[0] & mask))
	{
	  uint8_t col_idx = 
	    ((_bits[1] & mask)?1:0) +
	    ((_bits[2] & mask)?2:0) +
	    ((_bits[3] & mask)?4:0) +
	    ((_bits[4] & mask)?8:0);
	  *pix = colors[col_idx];
	}
    }

    template <> template <typename TColors>
    void Block<be_uint8_t, 4, 1>::pixel(unsigned i, TColors *pix, const TColors *colors) const
    {
      i &= (_width - 1);

      uint8_t mask = 1 << (_width - 1 - i);

      if (!(_bits[0] & mask))
	{
	  uint8_t col_idx = 
	    ((_bits[1] & mask)?1:0) +
	    ((_bits[2] & mask)?2:0) +
	    ((_bits[3] & mask)?4:0) +
	    ((_bits[4] & mask)?8:0);
	  *pix = colors[col_idx];
	}
    }

    template <> template <typename TColors>
    void Block<le_uint8_t, 4, 0>::pixel(unsigned i, TColors *pix, const TColors *colors) const
    {
      i &= (_width - 1);

      uint8_t mask = 1 << (_width - 1 - i);

      uint8_t col_idx = 
	((_bits[0] & mask)?1:0) +
	((_bits[1] & mask)?2:0) +
	((_bits[2] & mask)?4:0) +
	((_bits[3] & mask)?8:0);

      *pix = colors[col_idx];
      // TODO is any color considered as an alpha ?
    }

    template <> template <typename TColors>
    void Block<le_uint8_t, 1, 0>::pixel(unsigned i, TColors *pix, const TColors *colors) const
    {
      i &= (_width - 1);

      uint8_t mask = 1 << (_width - 1 - i);

      if(_bits[0] & mask)
	{
	  // The text color is black ?
	  *pix = colors[0];
	}
    }

    template <> template <typename TColors>
    void Block<le_uint16_t, 1, 0>::pixel(unsigned i, TColors *pix, const TColors *colors) const
    {
      i &= (_width - 1);

      uint16_t mask = 1 << (_width - 1 - i);

      if(_bits[0] & mask)
	{
	  // The text color is black ?
	  *pix = 0;
	}
    }

  }

}

using namespace Syndicate::Data;

/** Explicit instanciation of templates for Block of 32 pixels */
template class Block<be_uint32_t, 4, 1>;
template void Block<be_uint32_t, 4, 1>::pixel(unsigned i, uint8_t *pix, const uint8_t *colors) const;
template void Block<be_uint32_t, 4, 1>::pixel(unsigned i, uint32_t *pix, const uint32_t *colors) const;

/** Explicit instanciation of templates for Block of 8 pixels */
template class Block<le_uint8_t,  4, 1>;
template void Block<le_uint8_t, 4, 1>::pixel(unsigned i, uint8_t *pix, const uint8_t *colors) const;
template void Block<le_uint8_t, 4, 1>::pixel(unsigned i, uint32_t *pix, const uint32_t *colors) const;

template class Block<le_uint8_t,  4, 0>;
template void Block<le_uint8_t, 4, 0>::pixel(unsigned i, uint8_t *pix, const uint8_t *colors) const;
template void Block<le_uint8_t, 4, 0>::pixel(unsigned i, uint32_t *pix, const uint32_t *colors) const;

template class Block<le_uint16_t, 1, 0>;
template void Block<le_uint16_t, 1, 0>::pixel(unsigned i, uint8_t *pix, const uint8_t *colors) const;
template void Block<le_uint16_t, 1, 0>::pixel(unsigned i, uint32_t *pix, const uint32_t *colors) const;

template class Block<le_uint8_t,  1, 0>;
template void Block<le_uint8_t, 1, 0>::pixel(unsigned i, uint8_t *pix, const uint8_t *colors) const;
template void Block<le_uint8_t, 1, 0>::pixel(unsigned i, uint32_t *pix, const uint32_t *colors) const;
    
