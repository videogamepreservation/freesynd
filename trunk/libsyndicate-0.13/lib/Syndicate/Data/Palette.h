#ifndef __Syndicate_Data_Palette_h__
#define __Syndicate_Data_Palette_h__

#include "Syndicate/Data/File.h"
#include "Syndicate/Data/Int.h"

namespace Syndicate
{ 
namespace Data
{ 
  class Palette : public File
    {
    public:

      Palette(void) : File(reinterpret_cast<uint8_t **>(&_colors)) {}
      
      ~Palette(void) {}

      void get(uint8_t index, uint8_t &r, uint8_t &g, uint8_t &b) const;

      void get(uint8_t index, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a) const;

      /**
       * Return the number of colors
       */
      unsigned size(void) const { return _data_length / sizeof(Color); }

    private:

      struct Color
      {
	le_uint8_t _r;
	le_uint8_t _g;
	le_uint8_t _b;
      };

      Color *_colors;

    public:
      enum HPAL02
	{
	  HPAL02_DARK_BLACK,
	  HPAL02_DARK_FOO,
	  HPAL02_DARK_GREY,
	  HPAL02_GREEN,
	  HPAL02_DARK_WHITE,
	  HPAL02_DARK_BROWN,
	  HPAL02_RED,
	  HPAL02_DARK_BLUE,
	  HPAL02_BLACK,
	  HPAL02_FOO,
	  HPAL02_GREY,
	  HPAL02_YELLOW,
	  HPAL02_WHITE,
	  HPAL02_BROWN,
	  HPAL02_ORANGE,
	  HPAL02_BLUE,
	  HPAL02_WINE,
	};

    };

}

}

#endif /* __Syndicate_Data_Palette_h__ */

// Local Variables:
// mode: C++
// End:
