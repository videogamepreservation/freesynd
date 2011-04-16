#ifndef __Syndicate_Data_Debug_h__
#define __Syndicate_Data_Debug_h__

#include <cstdarg>
#include <cstdlib>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define DEBUG

#ifdef DEBUG
#define Debug(x...)  Syndicate::Data::debug(__FILE__, __PRETTY_FUNCTION__, __LINE__, x);
#define Assert(cond) if(!cond) { Syndicate::Data::debug(__FILE__, __PRETTY_FUNCTION__, __LINE__, "Assertion "#cond" failed !\n"); abort(); }
#else
#define Debug(x...) 
#define Assert(x...)
#endif

#define Error(x...) Syndicate::Data::error(__FILE__, __PRETTY_FUNCTION__, __LINE__, x);

namespace Syndicate
{ 
  namespace Data
  { 
    void error(const char *file, const char *function, const unsigned line, const char *format, ...);

#ifdef DEBUG
    void debug(const char *file, const char *function, const unsigned line, const char *format, ...);
#endif
  }
}

#endif /* __Syndicate_Data_Debug_h__ */

// Local Variables:
// mode: C++
// End:
