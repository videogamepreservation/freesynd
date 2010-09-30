#include <cstdio>
#include <assert.h>

#include "Syndicate/Data/Block.h"
#include "TestBlock.h"

int main(int argc, char **argv) 
{
  // Test Block32
  {
    Syndicate::Data::Block32 *block = reinterpret_cast<Syndicate::Data::Block32 *>(pixels_ref);
    assert( block->_width == 32 );
    //printf("uint8_t block32_pixels_ref[] = {");
    for(unsigned i = 0; i < block->_width; i++)
      {
	uint8_t pixel = 0;
	block->pixel(i, &pixel);
	assert( pixel == block32_pixels_ref[i] );
	//printf("0x%02x, ", pixel);
      }
    //printf("}\n");
  }

  // Test Block8
  {
    Syndicate::Data::Block8 *block = reinterpret_cast<Syndicate::Data::Block8 *>(pixels_ref);
    assert( block->_width == 8 );
    //printf("uint8_t block8_pixels_ref[] = {");
    for(unsigned i = 0; i < block->_width; i++)
      {
	uint8_t pixel = 0;
	block->pixel(i, &pixel);
	assert( pixel == block8_pixels_ref[i] );
	//printf("0x%02x, ", pixel);
      }
    //printf("}\n");
  }

  // Test Block840
  {
    Syndicate::Data::Block840 *block = reinterpret_cast<Syndicate::Data::Block840 *>(pixels_ref);
    assert( block->_width == 8 );
    //printf("uint8_t block840_pixels_ref[] = {");
    for(unsigned i = 0; i < block->_width; i++)
      {
	uint8_t pixel = 0;
	block->pixel(i, &pixel);
	assert( pixel == block840_pixels_ref[i] );
	//printf("0x%02x, ", pixel);
      }
    //printf("}\n");
  }

  // Test Block1610
  {
    Syndicate::Data::Block1610 *block = reinterpret_cast<Syndicate::Data::Block1610 *>(pixels_ref);
    assert( block->_width == 16 );
    //printf("uint8_t block1610_pixels_ref[] = {");
    for(unsigned i = 0; i < block->_width; i++)
      {
	uint8_t pixel = 0;
	block->pixel(i, &pixel);
	assert( pixel == block1610_pixels_ref[i] );
	//printf("0x%02x, ", pixel);
      }
    //printf("}\n");
  }

  // Test Block810
  {
    Syndicate::Data::Block810 *block = reinterpret_cast<Syndicate::Data::Block810 *>(pixels_ref);
    assert( block->_width == 8 );
    //printf("uint8_t block810_pixels_ref[] = {");
    for(unsigned i = 0; i < block->_width; i++)
      {
	uint8_t pixel = 0;
	block->pixel(i, &pixel);
	assert( pixel == block810_pixels_ref[i] );
	//printf("0x%02x, ", pixel);
      }
    //printf("}\n");
  }

  return 0;  
}


