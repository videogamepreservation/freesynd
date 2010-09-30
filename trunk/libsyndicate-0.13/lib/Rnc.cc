#include <cstdio>
#include <cstring>

#include "Syndicate/Data/Int.h"
#include "Syndicate/Data/Debug.h"
#include "Syndicate/Data/HuffmanTable.h"
#include "Syndicate/Data/Bitstream.h"
#include "Syndicate/Data/Rnc.h"

using namespace Syndicate::Data;

uint16_t Rnc::_crctab[256] = {0};

Rnc::Rnc(void)
{
  uint16_t val;

  if(*_crctab == 0)
    {  //computing CRC table
      for (unsigned i = 0; i < 256; i++)
	{
	  val = i;

	  for (unsigned j = 0; j < 8; j++)
	    {
	      if (val & 1)
		{
		  val = (val >> 1) ^ 0xA001;
		}
	      else
		{
		  val = (val >> 1);
		}
	    }
	  _crctab[i] = val;
	}
    }
}

Rnc *Rnc::getInstance(void)
{
  static Rnc _instance;
  return &_instance;
}

uint16_t Rnc::crc (uint8_t const *data, unsigned len) const
{
  uint16_t val = 0;

  if(data)
    {
      uint8_t const *p = data;
      while (len--)
	{
	  val ^= *p++;
	  val = (val >> 8) ^ _crctab[val & 0xFF];
	}
    }

  return val;
}

bool Rnc::isRnc(uint8_t *input) const
{
  if(!input)
    {
      return false;
    }

  // Extract the header
  Header *header = reinterpret_cast<Header *>(input);

  uint32_t signature = header->_signature;
  if(signature == _rnc_signature)
    {
      return true;
    }
  else
    {
      return false;
    }
}

uint8_t *Rnc::unpack(uint8_t *input, unsigned &output_length) const
{
  uint8_t *output = 0;

  if(!input)
    {
      return 0;
    }

  // Extract the header
  Header *header = reinterpret_cast<Header *>(input);

  uint32_t signature = header->_signature;
  if(signature == _rnc_signature)
    {

      uint32_t unpacked_length = header->_unpacked_length;
      uint32_t packed_length   = header->_packed_length;
      uint16_t unpacked_crc    = header->_unpacked_crc;
      uint16_t packed_crc      = header->_packed_crc;
      uint8_t  unknown         = header->_unknown;
      uint8_t  pack_count      = header->_pack_count;

      if (crc(input + 18, packed_length) != packed_crc)
	{
	  return 0;
	}

      Debug("Signature       0x%08x (%d)\n", signature, signature);
      Debug("Unpacked length 0x%08x (%d)\n", unpacked_length, unpacked_length);
      Debug("Packed length   0x%08x (%d)\n", packed_length, packed_length);
      Debug("Unpacked crc    0x%04x (%d)\n", unpacked_crc, unpacked_crc);
      Debug("Packed crc      0x%04x (%d)\n", packed_crc, packed_crc);
      Debug("Unknown         0x%02x (%d)\n", unknown, unknown);
      Debug("Pack count      0x%02x (%d)\n", pack_count, pack_count);
  
      output = new uint8_t [unpacked_length];
      output_length = 0;
      if(output)
	{
	  uint8_t * p_output = output;

	  Bitstream bs(input + 18, packed_length);
	  unsigned strange_bits = bs.get(2);
	  Debug("Strange bits = 0x%02x\n", strange_bits);

	  while(pack_count-- && (output_length < unpacked_length))
	    {
	      //Debug("Uncompressed count = %u\n", output_length);

	      HuffmanTable raw_table(&bs);
	      HuffmanTable dist_table(&bs);
	      HuffmanTable len_table(&bs);

	      unsigned chunk_count = bs.get(16);

	      Debug("Chunk count %d\n", chunk_count);

	      while (chunk_count)
		{
		  /*** Get the len of the raw data to copy ***/
		  unsigned raw_len = raw_table.getValue();
		  Debug("raw = %d\n", raw_len);
		  if (raw_len == -1 /* || raw_len == 0 */)
		    {
		      Error("Rnc decoding error\n");
		      break;
		    }

		  /*** Copy the raw data to the output ***/
		  if (raw_len > 0)
		    {
		      if(unpacked_length < output_length + raw_len)
			{
			  Error("Rnc decoding error\n");
			  break;			  
			}

		      bs.get(raw_len, p_output);
#ifdef DEBUG
		      Debug("    ");
		      for(unsigned i = 0; i < raw_len; i++)
			{
			  Debug("0x%02x ", p_output[i]);
			}
#endif
		      p_output += raw_len;
		      output_length += raw_len;
		      Debug(" (%d)\n", output_length);
		    }

		  /*** decrement chunk ***/
		  if(! --chunk_count)
		    {
		      break;
		    }

		  /*** Get the distance from here where we should copy the data ***/
		  unsigned dist = dist_table.getValue();
		  if (dist == -1)
		    {
		      Error("Rnc decoding error\n");
		      break;
		    }
		  dist += 1;
		  Debug("dist = %d\n", dist);
      
		  /*** Get the number of data to copy here ***/
		  unsigned len = len_table.getValue();
		  if (len == -1)
		    {
		      Error("Rnc decoding error\n");
		      break;
		    }
		  len += 2;
		  Debug("len = %d\n", len);
		  
		  /*** Copy the data from the output to the output ***/
		  if((unpacked_length < output_length + len) || (output_length < dist))
		    {
		      Error("Rnc decoding error\n");
		      break;
		    }
		  while(len--)
		    {
		      *p_output = p_output[-dist];
		      p_output++;
		      output_length ++;
		    }
		  //Debug(" (%d)\n", output_length));
		}
	    }

	  if(output_length != unpacked_length)
	    {
	      Error("Rnc length error.\n");
	    }

	  if (crc(output, output_length) != unpacked_crc)
	    {
	      Error("Rnc CRC error.\n");
	      delete output;
	      output = 0;
	    }

	}
    }
  return output;
}

