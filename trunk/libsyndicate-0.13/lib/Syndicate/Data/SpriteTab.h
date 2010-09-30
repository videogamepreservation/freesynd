#ifndef __Syndicate_Data_SpriteTab_h__
#define __Syndicate_Data_SpriteTab_h__

#include "Syndicate/Data/File.h"
#include "Syndicate/Data/Int.h"

namespace Syndicate
{ 
  namespace Data
  {
    /**
     * Allow to get size information and the offset of the data
     * for sprites.
     */
    class SpriteTab : public File
    {
    public:
      SpriteTab(void) : File(reinterpret_cast<uint8_t **>(&_entries)) {}

      ~SpriteTab(void) {}

      /**
       * Given the index of the sprite, returns the *offset* of the data
       * in the SpriteData structure and other informations.
       * \param[in] id the *index* of the sprite
       * \param[out] offset the *offset* of the data in the SpriteData structure
       * \param[out] width width in pixels of the sprite
       * \param[out] height height in pixels of the sprite
       */
      void get(unsigned id, unsigned &offset, unsigned &width, unsigned &height) const;

      /**
       * Return the number of entries
       */
      unsigned size(void) const { return _data_length / sizeof(Entry); }

    private:

      struct Entry
      {
	le_uint32_t _offset;
	le_uint8_t  _width;
	le_uint8_t  _height;
      };

      Entry   *_entries;
    };
  }
}

#endif /* __Syndicate_Data_SpriteTab_h__ */

// Local Variables:
// mode: C++
// End:
