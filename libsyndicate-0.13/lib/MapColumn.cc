#include "Syndicate/Data/MapColumn.h"

using namespace Syndicate::Data;

void MapColumn::get(uint8_t tile, uint8_t &type) const
{
  if(tile < _data_length)
    {
      type = _types[tile];
    }
}
 
