#ifndef __Syndicate_Data_SpriteAnim_h__
#define __Syndicate_Data_SpriteAnim_h__

#include "Syndicate/Data/File.h"
#include "Syndicate/Data/Int.h"

namespace Syndicate
{ 
  namespace Data
  {
    /**
     * Allow to get the first frame for an animation
     */
    class SpriteAnim : public File
    {
    public:

      SpriteAnim(void) : File(reinterpret_cast<uint8_t **>(&_anims)) {}

      ~SpriteAnim(void) {}

      /**
       * Given the index of the animation, returns the index of
       * the first frame.
       * \param[in] id the *index* of the animation
       * \param[out] index the *index* of the first frame that make this animation
       */
      void get(unsigned id, unsigned &index) const;

      /**
       * Return the number of anims
       */
      unsigned size(void) const { return _data_length / sizeof(Anim); }

    private:

      struct Anim
      {
	le_uint16_t _index;
      };

      Anim    *_anims;
    };

  }

}

#endif /* __Syndicate_Data_SpriteAnim_h__ */

// Local Variables:
// mode: C++
// End:
