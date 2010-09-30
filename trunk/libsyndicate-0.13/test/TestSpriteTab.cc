#include <cstdio>
#include <cstring>
#include <assert.h>

#include "Syndicate/Data/SpriteTab.h"
#include "TestSpriteTab.h"

int main(int argc, char **argv) 
{
  Syndicate::Data::SpriteTab *sprite_tab = new Syndicate::Data::SpriteTab();

  sprite_tab->load(ref_sprite_tab_filename);

  unsigned offset, width, height;
  sprite_tab->get(ref_sprite_tab_index, offset, width, height);

#if 0
  printf("sizeof(Syndicate::Data::SpriteTab::Entry) = %d\n", sizeof(Syndicate::Data::SpriteTab::Entry));
  printf("%d : 0x%08x 0x%02x 0x%02x\n", ref_sprite_tab_index, offset, width, height); 
#endif

  assert( offset == ref_sprite_tab._offset );
  assert( width  == ref_sprite_tab._width );
  assert( height == ref_sprite_tab._height );

  delete sprite_tab;

  return 0;
}
