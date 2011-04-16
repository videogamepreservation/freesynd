#include <cstdio>
#include <cstring>
#include <assert.h>

#include "Syndicate/Data/Int.h"

int main(int argc, char **argv) 
{
  unsigned ref = 0x01234567;


  // Test little endian
  {
    // Test 'constructor' and 'operator ='
    Syndicate::Data::le_uint32_t foo = ref;
    assert( foo._x[0] == 0x67 );
    assert( foo._x[1] == 0x45 );
    assert( foo._x[2] == 0x23 );
    assert( foo._x[3] == 0x01 );

    // Test 'copy constructor' and 'operator ='
    Syndicate::Data::le_uint32_t bar = foo;
    assert( foo._x[0] == bar._x[0] );
    assert( foo._x[1] == bar._x[1] );
    assert( foo._x[2] == bar._x[2] );
    assert( foo._x[3] == bar._x[3] );

    // Test 'cast'
    unsigned tmp = foo;
    assert( tmp == ref );

#if 0
    printf("foo : 0X%02x 0X%02x 0X%02x 0X%02x\n", foo._x[0], foo._x[1], foo._x[2], foo._x[3]); 
    printf("bar : 0X%02x 0X%02x 0X%02x 0X%02x\n", bar._x[0], bar._x[1], bar._x[2], bar._x[3]); 
    printf("tmp : 0X%08x\n", tmp); 
#endif
  }

  // Test big endian
  {
    // Test 'constructor' and 'operator ='
    Syndicate::Data::be_uint32_t foo = ref;
    assert( foo._x[0] == 0x01 );
    assert( foo._x[1] == 0x23 );
    assert( foo._x[2] == 0x45 );
    assert( foo._x[3] == 0x67 );

    // Test 'copy constructor' and 'operator ='
    Syndicate::Data::be_uint32_t bar = foo;
    assert( foo._x[0] == bar._x[0] );
    assert( foo._x[1] == bar._x[1] );
    assert( foo._x[2] == bar._x[2] );
    assert( foo._x[3] == bar._x[3] );

    // Test 'cast'
    unsigned tmp = foo;
    assert( tmp == ref );

#if 0
    printf("foo : 0X%02x 0X%02x 0X%02x 0X%02x\n", foo._x[0], foo._x[1], foo._x[2], foo._x[3]); 
    printf("bar : 0X%02x 0X%02x 0X%02x 0X%02x\n", bar._x[0], bar._x[1], bar._x[2], bar._x[3]); 
    printf("tmp : 0X%08x\n", tmp); 
#endif
  }

  return 0;  
}


