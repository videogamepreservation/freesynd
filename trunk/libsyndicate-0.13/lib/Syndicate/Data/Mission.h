#ifndef __Syndicate_Data_Mission_h__
#define __Syndicate_Data_Mission_h__

#include "Syndicate/Data/File.h"
#include "Syndicate/Data/Int.h"

namespace Syndicate
{ 
  namespace Data
  {
    class Mission : public File
    {
    public:
      Mission(void) : File(reinterpret_cast<uint8_t **>(&_data)) {}

      ~Mission(void) {}

    };
  }
}

#endif /* __Syndicate_Data_Mission_h__ */

// Local Variables:
// mode: C++
// End:

