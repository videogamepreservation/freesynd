#include "Syndicate/Data/MapData.h"

using namespace Syndicate::Data;

unsigned MapData::nb_i(void) const
{
  if(_data)
    {
      return _data->_nb_i;
    }
  else
    {
      return 0;
    }
}

unsigned MapData::nb_j(void) const
{
  if(_data)
    {
      return _data->_nb_j;      
    }
  else
    {
      return 0;
    }
}

unsigned MapData::nb_k(void) const
{
  if(_data)
    {
      return _data->_nb_k;
    }
  else
    {
      return 0;
    }
}

void MapData::get(unsigned i, unsigned j, unsigned k, unsigned &id) const
{
  if(_data && i < nb_i() && j < nb_j() && k < nb_k())
    {
      uint32_t offset = _data->_offsets[j * nb_i() + i];
      //id = *(reinterpret_cast<uint8_t *>(_data) + offsetof(Data,_offsets[0]) + offset + k);
      id = *(reinterpret_cast<uint8_t *>(_data) + 12 + offset + k);
    }
}
