#include "Syndicate/Data/Debug.h"
#include "Syndicate/Data/Palette.h"
#include "Syndicate/Data/Colors.h"

using namespace Syndicate::Data;

void Palette::get(uint8_t index, uint8_t &r, uint8_t &g, uint8_t &b) const
{
  if(_colors && index < size())
    {
      r = (_colors[index]._r % 0x40) << 2;
      g = (_colors[index]._g % 0x40) << 2;
      b = (_colors[index]._b % 0x40) << 2;
    }
}
