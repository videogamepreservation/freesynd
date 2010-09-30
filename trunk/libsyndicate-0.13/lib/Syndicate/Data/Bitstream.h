#ifndef __Syndicate_Data_Bitstream_h__
#define __Syndicate_Data_Bitstream_h__

#include <inttypes.h>

namespace Syndicate
{ 
  namespace Data
  { 
    /**
     * Process a buffer of bytes as a bitstream.
     * It is specific to the RNC file format.
     */
    class Bitstream
    {
    public:

      /**
       * Instanciate a bitstream on the given data.
       * \param[in] data the data
       * \param[in] data_size the number of bytes in the data.
       */
      Bitstream(uint8_t const *data, unsigned data_size);

      /**
       * Extract somes bits from the stream.
       * \param[in] nb_bits numbers of bits to extract [1;16]
       * \return the value extracted.
       */
      unsigned get(unsigned nb_bits);

      /**
       * Extract somes bytes from the stream. This is different of
       * extracting bits.
       * \param[in] nb_bytes numbers of bytes to extract [1;n]
       * \param[out] output a pointer to a nb_bytes length buffer to stores the result
       */
      void get(unsigned nb_bytes, uint8_t *output);

    private:

      /** 
       * Pop nb bytes from the stream.
       * \param[in] nb_bits numbers of bits to extract [1;16]
       */
      unsigned pop(unsigned nb_bits);

      /** 
       * Push nb bytes into the stream.
       * \param[in] nb_bits numbers of bits to insert [1;16]
       * \param[in] buffer add the comment FIXME
       */
      void push(unsigned nb_bits, unsigned buffer);

      /** 
       * Reload
       * Push nb bytes into the stream.
       * \param[in] nb_bits numbers of bits to insert [1;16]
       */
      void reload(unsigned nb_bits = 16);

      /** Precomputed table of mask */
      static uint16_t const _masks [16];
      /** A pointer to the data array */
      uint8_t const * _data;
      /** The data array size */
      unsigned _data_size;
      /** The offset of the read pointer in the data array */
      unsigned _data_offset;
      /** 
       * The buffer currently processed. 
       * The data are processed by batch of 16 bits 
       */
      uint32_t _buffer;
      /** The number of fresh bits in the buffer */ 
      unsigned _count;
    };
  }
}

#endif /* __Syndicate_Data_Bitstream_h__ */

// Local Variables:
// mode: C++
// End:
