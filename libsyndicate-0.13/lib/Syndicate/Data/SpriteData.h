#ifndef __Syndicate_Data_SpriteData_h__
#define __Syndicate_Data_SpriteData_h__

#include "Syndicate/Data/Block.h"
#include "Syndicate/Data/File.h"

namespace Syndicate
{ 
  namespace Data
  {
    /**
     * Allow to get size and placement information of sprites
     * for frames
     */
    class SpriteData : public File
    {
    public:

      SpriteData(void) : File(reinterpret_cast<uint8_t **>(&_data)) {}

      ~SpriteData(void) {}

      /**
       * Draw the sprite in the given buffer
       * \param[in] offset the *offset* of the data in the SpriteData structure
       * \param[in] width width in pixels of the sprite
       * \param[in] height height in pixels of the sprite
       * \param[in] h_flipped 
       * \param[out] pixels a pointer to a buffer where the method will store the pixels (each pixel is one byte).
       * \param[in] pitch the width of the sprite buffer
       * \param[out] colors a kind of map between a palette index and a color to put in pixels
       */
      template<typename TColors>
      void get(unsigned offset, unsigned width, unsigned height, bool h_flipped, TColors *pixels, unsigned pitch, const TColors *colors = Colors<TColors>::_default) const;

      /**
       * Return the number of sprites
       */
      unsigned nb_sprites(void) const 
      {
	if(_data)
	  {
	    return _data->_nb_sprites;
	  }
	else
	  {
	    return 0;
	  }
      }

      /**
       * Return the number of data. 
       * \note nb_sprites has more sense.
       */
      unsigned size(void) const { return (_data_length - sizeof(_data->_nb_sprites)) / sizeof(Block8); }

    private:

      struct Data
      {
	le_uint16_t _nb_sprites;
	Block8      _blocks[];
      };

      Data * _data;

    };

  }

}

#endif /* __Syndicate_Data_SpriteData_h__ */

// Local Variables:
// mode: C++
// End:
