#include "Syndicate/Data/Debug.h"
#include "Syndicate/Data/SpriteTab.h"

using namespace Syndicate::Data;

void SpriteTab::get(unsigned id, unsigned &offset, unsigned &width, unsigned &height) const
{
  if(_entries && id < size())
    {
      offset = _entries[id]._offset;
      width  = _entries[id]._width;
      height = _entries[id]._height;
    }
}


