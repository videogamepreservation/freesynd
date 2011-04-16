#ifndef __Syndicate_Data_Raw_h__
#define __Syndicate_Data_Raw_h__

#include "Syndicate/Data/File.h"
#include "Syndicate/Data/Block.h"

namespace Syndicate
{ 
  namespace Data
  { 
    class Raw : public File
    {
    public:

      Raw(void) : File(reinterpret_cast<uint8_t **>(&_data)) {}

      ~Raw(void) {};

      template<typename TColors>
      void get(unsigned id, unsigned width, unsigned height, TColors *pixels, unsigned pitch, const TColors *colors = Colors<TColors>::_default) const;

    private:

      uint8_t *_data;
    };
  }
}

#endif /* __Syndicate_Data_Raw_h__ */

// Local Variables:
// mode: C++
// End:
