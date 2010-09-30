#ifndef __Syndicate_Data_SpriteFrame_h__
#define __Syndicate_Data_SpriteFrame_h__

#include "Syndicate/Data/File.h"
#include "Syndicate/Data/Int.h"

namespace Syndicate
{ 
  namespace Data
  {
    /**
     * Allow to get size information and the sprite compostion 
     * for a frame
     */
    class SpriteFrame : public File
    {
    public:
      SpriteFrame(void) : File(reinterpret_cast<uint8_t **>(&_frames)) {}

      ~SpriteFrame(void) {}

      /**
       * Given the index of the frame, returns the index of
       * the first element of the frame and other informations.
       * \param[in] id the *index* of the frame
       * \param[out] first the *index* of the first element that make this frame
       * \param[out] width the width of the frame
       * \param[out] height the height of the frame
       * \param[out] flags some information on the frame
       * \param[out] next the *index* of the next frame
       */
      void get(unsigned id, unsigned &first, unsigned &width, unsigned &height, unsigned &flags, unsigned &next) const;

      /**
       * Return the number of frames
       */
      unsigned size(void) const { return _data_length / sizeof(Frame); }

    private:

      struct Frame
      {
	le_uint16_t _first;
	le_uint8_t  _width;
	le_uint8_t  _height;
	le_uint16_t _flags;
	le_uint16_t _next;
      };

      Frame   *_frames;
    };

  }

}

#endif /* __Syndicate_Data_SpriteFrame_h__ */

// Local Variables:
// mode: C++
// End:
