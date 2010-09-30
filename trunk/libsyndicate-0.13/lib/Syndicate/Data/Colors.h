#ifndef __Syndicate_Data_Colors_h__
#define __Syndicate_Data_Colors_h__

namespace Syndicate
{ 
  namespace Data
  { 
    /**
     * This structure allow to initialise the colors.
     * The template T give the type of colors :
     * - uint8_t 8 bits colors (paletized)
     * - uint32_t 32 bits colors (any kind of rgba)
     */
    template <typename T> 
    struct Colors
    {
      static const T _default[256];
    };

  }
}

#endif /* __Syndicate_Data_Colors_h__ */

// Local Variables:
// mode: C++
// End:
