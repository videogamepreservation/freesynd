#ifndef __Syndicate_Data_MapSubTile_h__
#define __Syndicate_Data_MapSubTile_h__

#include "Syndicate/Data/Block.h"

namespace Syndicate
{
  namespace Data
  {
    class MapSubTile
    {
    public:

      /**
       * This method extract pixels of the subtile in the subtile array of 
       * uint8_t (palettized) or uint32_t (4bpp).
       * \param[out] subtile array of pixels where the data should be stored
       * \param[in] pitch length of a scanline of tile in bytes
       * \param[in] colors lut for mapping palette indexes on the format of pixels
       */
      template<typename TColors>
      void get(TColors *subtile, unsigned pitch = _width, const TColors *colors = Colors<TColors>::_default) const ;

      static const unsigned _width  = Block32::_width;
      static const unsigned _height = 16;

    private:

      Block32 _lines[_height];

    };

  }

}

#endif /* __Syndicate_Data_MapSubTile_h__ */

// Local Variables:
// mode: C++
// End:
