#ifndef __Syndicate_Data_SpriteElement_h__
#define __Syndicate_Data_SpriteElement_h__

#include "Syndicate/Data/File.h"
#include "Syndicate/Data/Int.h"

namespace Syndicate
{ 
  namespace Data
  {
    /**
     * Allow to get placement information and composition of sprites
     * for a frame
     */
    class SpriteElement : public File
    {
    public:
      SpriteElement(void) : File(reinterpret_cast<uint8_t **>(&_elements)) {}

      ~SpriteElement(void) {}

      /**
       * Given the index of the element, returns the index of
       * the first sprite of the element and other informations.
       * \param[in] id the *index* of the element
       * \param[out] sprite the *index* of the first sprite that make this element
       * \param[out] x_offset the horizontal offset of this sprite
       * \param[out] y_offset the vertical offset of this sprite
       * \param[out] flipped say if the sprite is flipped horizontaly
       * \param[out] next the *index* of the next element
       */
      void get(unsigned id, unsigned &sprite, int &x_offset, int &y_offset, bool &flipped, unsigned &next) const ;

      /**
       * Return the number of elements
       */
      unsigned size(void) const { return _data_length / sizeof(Element); }

    private:

      struct Element
      {
	le_uint16_t _sprite;
	le_int16_t  _x_offset;
	le_int16_t  _y_offset;
	le_uint16_t _flipped;
	le_uint16_t _next;
      };

      Element *_elements;
    };

  }

}

#endif /* __Syndicate_Data_SpriteElement_h__ */

// Local Variables:
// mode: C++
// End:
