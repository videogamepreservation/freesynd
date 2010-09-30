#include "Syndicate/Data/Debug.h"
#include "Syndicate/Data/SpriteElement.h"

using namespace Syndicate::Data;

void SpriteElement::get(unsigned id, unsigned &sprite, int &x_offset, int &y_offset, bool &flipped, unsigned &next) const
{
  if(_elements && id < size())
    {
      sprite   = _elements[id]._sprite / 6; // 6 is sizeof(SpriteTab::Entry)
      x_offset = _elements[id]._x_offset;
      y_offset = _elements[id]._y_offset;
      flipped  = _elements[id]._flipped;
      next     = _elements[id]._next;
    }
}
