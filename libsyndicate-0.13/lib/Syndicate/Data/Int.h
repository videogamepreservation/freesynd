#ifndef __Syndicate_Data_Int_h__
#define __Syndicate_Data_Int_h__

#include <inttypes.h>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#  ifdef HAVE_ENDIAN_H
#    include <endian.h>
#    define SYND_LIL_ENDIAN __LITTLE_ENDIAN
#    define SYND_BIG_ENDIAN __BIG_ENDIAN
#    define SYND_BYTE_ORDER __BYTE_ORDER
#  else
#    define SYND_LIL_ENDIAN 1234
#    define SYND_BIG_ENDIAN 4321
#    ifdef WORDS_BIGENDIAN
#      define SYND_BYTE_ORDER SYND_BIG_ENDIAN
#    else
#      define SYND_BYTE_ORDER SYND_LIL_ENDIAN
#    endif
#  endif
#else
#  include <endian.h>
#  define SYND_LIL_ENDIAN __LITTLE_ENDIAN
#  define SYND_BIG_ENDIAN __BIG_ENDIAN
#  define SYND_BYTE_ORDER __BYTE_ORDER
#endif

namespace Syndicate
{ 
  namespace Data
  { 
    /**
     * This class is used as a base class for structures.
     * It has two benefits :
     *  - it allow the structures to be packed
     *  - it allow to deal with endianess
     *
     * \attention It is not tested on all architectures.
     * \todo Check that this class is optimized (are all the code 
     *       and branches generated ? it shouldn't)
     */
    template<typename T, unsigned endianess>
    struct Int
    {
      uint8_t _x[sizeof(T)];
      
      Int<T, endianess> (T x_)
      {
	union 
	{ 
	  T _val; 
	  uint8_t _tab[sizeof(T)];
	} x = {x_};

	if(SYND_BYTE_ORDER == endianess)
	  { // could we use memcpy ???
	    if(sizeof(T) == 2)
	      {
		_x[1] = x._tab[1];
		_x[0] = x._tab[0];
	      }
	    else if(sizeof(T) == 4)
	      {
		_x[3] = x._tab[3];
		_x[2] = x._tab[2];
		_x[1] = x._tab[1];
		_x[0] = x._tab[0];
	      }
	  }
	else
	  {
	    if(sizeof(T) == 2)
	      {
		_x[1] = x._tab[0];
		_x[0] = x._tab[1];
	      }
	    else if(sizeof(T) == 4)
	      {
		_x[3] = x._tab[0];
		_x[2] = x._tab[1];
		_x[1] = x._tab[2];
		_x[0] = x._tab[3];
	      }
	  }
      }

      operator T (void) const
      {
	if(endianess == SYND_BIG_ENDIAN)
	  {
	    if(sizeof(T) == 2)
	      {
		return (_x[0] << 8) | _x[1];
	      }
	    else if(sizeof(T) == 4)
	      {
		return (_x[0] << 24) | (_x[1] << 16) | (_x[2] << 8) | _x[3];
	      }
	  }
	else
	  {
	    if(sizeof(T) == 2)
	      {
		return (_x[1] << 8) | _x[0];
	      }
	    else if(sizeof(T) == 4)
	      {
		return (_x[3] << 24) | (_x[2] << 16) | (_x[1] << 8) | _x[0];
	      }
	  }
      }

    };

    typedef int8_t                         le_int8_t;
    typedef int8_t                         be_int8_t;

    typedef uint8_t                        le_uint8_t;
    typedef uint8_t                        be_uint8_t;

    typedef Int<int16_t, SYND_LIL_ENDIAN>  le_int16_t;
    typedef Int<int16_t, SYND_BIG_ENDIAN>  be_int16_t;

    typedef Int<uint16_t, SYND_LIL_ENDIAN> le_uint16_t;
    typedef Int<uint16_t, SYND_BIG_ENDIAN> be_uint16_t;

    typedef Int<int32_t, SYND_LIL_ENDIAN>  le_int32_t;
    typedef Int<int32_t, SYND_BIG_ENDIAN>  be_int32_t;

    typedef Int<uint32_t, SYND_LIL_ENDIAN> le_uint32_t;
    typedef Int<uint32_t, SYND_BIG_ENDIAN> be_uint32_t;

  }
}

#endif /* __Syndicate_Data_Int_h__ */

// Local Variables:
// mode: C++
// End:
