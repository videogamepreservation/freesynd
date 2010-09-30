#include <cstdio>

#include "Syndicate/Data/Debug.h"
#include "Syndicate/Data/SpriteFrame.h"

using namespace Syndicate::Data;

void SpriteFrame::get(unsigned id, unsigned &first, unsigned &width, unsigned &height, unsigned &flags, unsigned &next) const
{
  if(_frames && id < size())
    {
      first  = _frames[id]._first;
      width  = _frames[id]._width;
      height = _frames[id]._height;
      flags  = _frames[id]._flags;
      next   = _frames[id]._next;
    }
}

