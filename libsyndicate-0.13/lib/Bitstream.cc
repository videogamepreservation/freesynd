#include <cstdio>
#include <cstring>

#include "Syndicate/Data/Debug.h"
#include "Syndicate/Data/Bitstream.h"

using namespace Syndicate::Data;

uint16_t const Bitstream::_masks[16] =
  {
    0x0001,
    0x0003,
    0x0007,
    0x000f,
    0x001f,
    0x003f,
    0x007f,
    0x00ff,
    0x01ff,
    0x03ff,
    0x07ff,
    0x0fff,
    0x1fff,
    0x3fff,
    0x7fff,
    0xffff
  };

Bitstream::Bitstream(uint8_t const *data, unsigned data_size)
{
  _data = data;
  _data_size = data_size;
  _buffer = 0;
  _count = 0;
  _data_offset = 0;

  reload(16);
}

unsigned Bitstream::get(unsigned nb_bits)
{
  unsigned result = pop(nb_bits);
  reload();
  return result;
}

void Bitstream::get(unsigned nb_bytes, uint8_t *output)
{
  Assert( ((_data_offset + nb_bytes - 2) <= _data_size) );  

  // we save the bits until we are aligned.
  unsigned nb_bits = _count % 16;
  unsigned buffer = pop(nb_bits);

  // first we get the bytes from the buffer
  while(_count && nb_bytes)
    {
      *output++ = pop(8);
      nb_bytes--;
    }
  // then we get the bytes from the data stream
  if(nb_bytes)
    {
      memcpy(output, _data + _data_offset, nb_bytes);
      _data_offset += nb_bytes;
    }
    
  // we reload the data stream in the buffer to keep aligned
  reload(16 - _count);

  // we restore the saved bits in the stream
  push(nb_bits, buffer);
}


unsigned Bitstream::pop(unsigned nb)
{
  Assert( ((nb <= 16) && (nb <= _count)) );

  unsigned result = 0;
  if(nb <= 16 && nb <= _count)
     {
      // we save the result
      result = _buffer &  _masks[nb - 1]; 
      // we shift the buffer
      _buffer >>= nb;
      // we decount the bit we remove 
      _count -= nb;
    }
  return result;
}

void Bitstream::push(unsigned nb, unsigned buffer)
{
  Assert( ((nb <= 16) && (nb + _count <= 32)) );

  if((nb <= 16) && (nb + _count <= 32))
    {
      // we decount the bit we add 
      _count += nb;
      // we shift the buffer
      _buffer <<= nb;
      // we store the result
      _buffer |= (buffer & _masks[nb - 1]);
    }
}

void Bitstream::reload(unsigned nb)
{
  // If the the lsb of the buffer has been invalidated 
  if((nb <= 16) && (_count < 16))
    {
      while(nb)
	{
	  if(_data_offset + 1 <= _data_size)
	    {
	      _buffer |= *(_data + _data_offset) << _count;
	      _data_offset += 1;
	    }
	  _count += 8;
	  nb -= 8;
	}
    }
}

