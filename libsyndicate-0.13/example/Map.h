#ifndef __Map_h__
#define __Map_h__

#include "Syndicate/Data/MapData.h"
#include "Syndicate/Data/MapTile.h"
#include "Syndicate/Data/MapColumn.h"


class Map
{
 public:
  
  Map(const char *data_filename, const char *tile_filename);
  ~Map(void);

  void select(uint8_t *map, unsigned i, unsigned j, unsigned k);

  void tile(unsigned x, unsigned y, unsigned k, unsigned &i, unsigned &j);

  void coord(unsigned i, unsigned j, unsigned k, unsigned &x, unsigned &y);

  unsigned width(void);

  unsigned height(void);

  void get(uint8_t *pixels, unsigned pitch, unsigned i, unsigned j, unsigned k);

  void get(uint8_t *pixels, unsigned pitch);

  Syndicate::Data::MapData *_map_data;
  Syndicate::Data::MapTile *_map_tile;
  Syndicate::Data::MapColumn *_map_column;

};

#endif /* __Map_h__ */
