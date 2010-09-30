#ifndef __Syndicate_Data_Rnc_h__
#define __Syndicate_Data_Rnc_h__

#include "Syndicate/Data/Int.h"

namespace Syndicate
{ 
  namespace Data
  { 
    /**
     * This class decompress RNC packed data.
     * The format is deducted from the code source found at
     * http://www.yoda.arachsys.com/dk/rnc.html
     * The original tools to decompress RNC files is at 
     *
     */
    class Rnc
    {
    public:

      /**
       * Get an unique instance of the class.
       */
      static Rnc *getInstance(void);

      /**
       * Unpack the given byte array.
       * \param[in] input is the compressed data array.
       * \param[out] output_length is the uncompressed length
       * \return a pointer to the decompressed data or zero if it fails.
       * \warning The returned pointer must be freed by the user.
       */
      uint8_t *unpack(uint8_t *input, unsigned &output_length) const;

      /**
       * Identify a data array as compressed or not.
       * \param[in] input is a buffer to test.
       * \return true if the buffer is RNC compressed.
       */
      bool isRnc(uint8_t *input) const;

    private:

      Rnc(void);

      /**
       * Compute the CRC of a buffer
       */
      uint16_t crc(uint8_t const *data, unsigned len) const ;
	  
      static uint16_t _crctab[256];

      static const uint32_t _rnc_signature = 0x524E4301; /* "RNC\001" */

      struct Header
      {
	be_uint32_t _signature;
	be_uint32_t _unpacked_length;
	be_uint32_t _packed_length;
	be_uint16_t _unpacked_crc;
	be_uint16_t _packed_crc;
	be_uint8_t  _unknown;
	be_uint8_t  _pack_count;
      };

    };
  }
}

#endif /* __Syndicate_Data_Rnc_h__ */

// Local Variables:
// mode: C++
// End:
