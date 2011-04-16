#ifndef __Syndicate_Data_MapTile_h__
#define __Syndicate_Data_MapTile_h__

#include <cstddef>

#include "Syndicate/Data/File.h"
#include "Syndicate/Data/Int.h"
#include "Syndicate/Data/MapSubTile.h"

namespace Syndicate
{
  namespace Data
  {
    /**
     * This class allow to access the hblk01.dat file.
     */
    class MapTile : public File
    {
    public:

      MapTile(void) : File(reinterpret_cast<uint8_t **>(&_data)) {}

      ~MapTile(void) {}

      /**
       * This method extract pixels of the tile in the tile array of 
       * uint8_t (palettized) or uint32_t (4bpp).
       * \param[in] id 
       * \param[out] tile array of pixels where the data should be stored
       * \param[in] pitch length of a scanline of tile in bytes
       * \param[in] colors lut for mapping palette indexes on the format of pixels
       */
      template<typename TColors>
      void get(uint8_t id, TColors *tile, unsigned pitch = _width, const TColors *colors = Colors<TColors>::_default) const;

      static const unsigned _nb_tiles = 256;

      static const unsigned _sub_tiles_per_tile_x = 2;
      static const unsigned _sub_tiles_per_tile_y = 3;
      static const unsigned _sub_tiles_per_tile   = _sub_tiles_per_tile_x * _sub_tiles_per_tile_y; // = 6

      static const unsigned _width  = _sub_tiles_per_tile_x * MapSubTile::_width; // = 64
      static const unsigned _height = _sub_tiles_per_tile_y * MapSubTile::_height; // = 48

      static const unsigned _width_on_map  = 256;
      static const unsigned _height_on_map = 256;
      static const unsigned _depth_on_map = 128;

      static const unsigned _min_offset = _nb_tiles * _sub_tiles_per_tile * sizeof(le_uint32_t); // = 6144

    private:

      struct Data 
      {
	le_uint32_t _offsets[_nb_tiles][_sub_tiles_per_tile_x * _sub_tiles_per_tile_y];
	MapSubTile  _subtiles[];
      };
       
      Data *_data;

    };
  }
}

#endif /* __Syndicate_Data_MapTile_h__ */

// Local Variables:
// mode: C++
// End:

