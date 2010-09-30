#ifndef __Syndicate_Data_Block_h__
#define __Syndicate_Data_Block_h__

#include "Syndicate/Data/Int.h"
#include "Syndicate/Data/Colors.h"

namespace Syndicate
{ 
  namespace Data
  { 

    /**
     * It is a block of pixels. There are many way to pack 
     * the data that we can specify with templates parameters.
     * - The type T represent the number of pixels in the 
     *   block. So for a uint8_t there are 8 pixels, for
     *   a uint32_t there are 32 pixels.
     * - The constant depth give the number of bits per 
     *   pixels (without alpha channel).
     * - The constant alpha can be 0 or 1 and tell if there is 
     *   an alpha channel.
     *
     * If there is an alpha channel the data are packed like this :
     *   | alpha | bit0 | bit1 | bit2 | bit3 |
     *
     *
     */
    template <typename T, unsigned depth, unsigned alpha>
    class Block
    {
    private:
      T _bits[alpha + depth];

    public:

      /**
       * The width in pixels
       */
      static const unsigned _width = sizeof(T) * 8;

      /**
       * Fill the pixel byte with the color of the ith pixel of this block.
       * \param[in] i the index of the pixel in the range [0;_width]
       * \param[out] pix the pointer to the byte where the color must be wrote
       */
      template<typename TColors>
      void pixel(unsigned i, TColors *pix, const TColors *colors = Colors<TColors>::_default) const;

    };

    // Used for tiles
    typedef Block<be_uint32_t, 4, 1> Block32; 

    // Used for sprites
    typedef Block<le_uint8_t, 4, 1> Block8; 

    // Used for "fonts" (HREQ.DAT)
    typedef Block<le_uint8_t, 4, 0> Block840; 

    // Used for fonts (HFNT.DAT)
    typedef Block<le_uint16_t, 1, 0> Block1610; 
    // Used for fonts  (HFNT.DAT)
    typedef Block<le_uint8_t,  1, 0> Block810; 

  }
}

#endif /* __Syndicate_Data_Block_h__ */

// Local Variables:
// mode: C++
// End:
