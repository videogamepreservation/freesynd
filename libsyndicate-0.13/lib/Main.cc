#include <cstdlib>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Syndicate/Data/Int.h"
#include "Syndicate/Data/Debug.h"

extern "C" 
{
  void __attribute__ ((constructor)) lib_init(void)
  {
#ifdef DEBUG
    Assert( getenv("SYNDICATE_DATA_PATH") );

#ifdef PACKAGE_NAME
    Debug("%s ", PACKAGE_NAME);
#endif

#ifdef PACKAGE_VERSION
    Debug("%s ", PACKAGE_VERSION);
#endif

#if (SYND_BYTE_ORDER == SYND_BIG_ENDIAN)
    Debug("SYND_BYTE_ORDER is SYND_BIG_ENDIAN ");
#elif (SYND_BYTE_ORDER == SYND_LIL_ENDIAN)
    Debug("SYND_BYTE_ORDER is SYND_LIL_ENDIAN ");
#endif
    Debug("\n");

#endif
  }

  /*
    void __attribute__ ((destructor)) lib_fini(void);
  */
}

