#include "Syndicate/Data/Debug.h"
#include "Syndicate/Data/SpriteAnim.h"

using namespace Syndicate::Data;

void SpriteAnim::get(unsigned id, unsigned &index) const
{
  if(_anims && id < size())
    {
      index = _anims[id]._index;
    }
}
