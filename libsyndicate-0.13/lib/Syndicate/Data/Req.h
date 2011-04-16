#ifndef __Syndicate_Data_Req_h__
#define __Syndicate_Data_Req_h__

#include "Syndicate/Data/File.h"
#include "Syndicate/Data/Block.h"

namespace Syndicate
{ 
  namespace Data
  { 
    class Req : public File
    {
    public:

      Req(void) : File(reinterpret_cast<uint8_t **>(&_entries)) {}

      ~Req(void) {};

      template<typename TColors>
      void get(unsigned id, TColors *pixels, unsigned pitch = _width, const TColors *colors = Colors<TColors>::_default) const;

      unsigned size(void) const { return _data_length / sizeof(Entry); }

      static const unsigned _width  = Block840::_width;
      static const unsigned _height = 12; 

    private:

      struct Entry
      {
	Block840   _lines[_height];
	le_uint8_t _spares[16];
      };

      Entry   *_entries;
    };
  }
}

#endif /* __Syndicate_Data_Req_h__ */

// Local Variables:
// mode: C++
// End:
