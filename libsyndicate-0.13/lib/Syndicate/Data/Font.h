#ifndef __Syndicate_Data_Font_h__
#define __Syndicate_Data_Font_h__

#include "Syndicate/Data/File.h"
#include "Syndicate/Data/Block.h"
#include "Syndicate/Data/Int.h"

namespace Syndicate
{
  namespace Data
  {
    /**
     * This class allow to access the hfnt01.dat file.
     */
    class Font : public File
    {
    public:

      Font(void) : File(reinterpret_cast<uint8_t **>(&_data)) {}

      ~Font(void) {}

      template<typename TColors>
      void get(unsigned id, TColors *pixels, unsigned pitch, const TColors *colors = Colors<TColors>::_default) const;

      static const unsigned _nb_fonts = 128;

    private:

      /**
       * The data array is 
       */
      struct Data 
      {
	struct
	{
	  le_uint16_t _offset;
	  le_uint8_t  _width;
	  le_uint8_t  _height;
	  le_uint8_t  _line_offset;
	} _tab[_nb_fonts];
	le_uint8_t _data[];
      };

      Data *_data;

    };
  }
}

#endif /* __Syndicate_Data_Font_h__ */

// Local Variables:
// mode: C++
// End:

