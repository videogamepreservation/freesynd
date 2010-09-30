#include <cstdio>

#include "Map.h"
#include "Syndicate/Data/MapSubTile.h"
#include "Syndicate/Data/Palette.h"

Map::Map(const char *data_filename, const char *tile_filename)
{
  _map_data = new Syndicate::Data::MapData();
  _map_data->load(data_filename);
  _map_tile = new Syndicate::Data::MapTile();
  _map_tile->load(tile_filename);
  _map_column = new Syndicate::Data::MapColumn();
  _map_column->load("COL01.DAT");
}

Map::~Map(void)
{
  delete _map_tile;
  delete _map_data;
  delete _map_column;
}

void Map::select(uint8_t *map, unsigned i, unsigned j, unsigned k)
{
  k = 0;
  if (i < _map_data->nb_i() && j < _map_data->nb_j() && k < _map_data->nb_k())
    {
      unsigned x, y;
      // Highlight point 0
      coord(0, 0, 0, x, y);
      uint8_t *the_map = map + y * width() + x;
      *the_map = 255;

      coord(i, j, k, x, y);
      the_map = map + y * width() + x;

      for (unsigned xx = 0; xx < Syndicate::Data::MapSubTile::_width; xx++)
	{
	  the_map[(xx / 2) * width() + Syndicate::Data::MapSubTile::_width + xx] = Syndicate::Data::Palette::HPAL02_YELLOW;
	  the_map[(xx / 2) * width() + Syndicate::Data::MapSubTile::_width - 1 - xx] = Syndicate::Data::Palette::HPAL02_YELLOW;

	  the_map[(2 * Syndicate::Data::MapSubTile::_height - xx / 2) * width() + Syndicate::Data::MapSubTile::_width + xx] = Syndicate::Data::Palette::HPAL02_YELLOW;
	  the_map[(2 * Syndicate::Data::MapSubTile::_height - xx / 2) * width() + Syndicate::Data::MapSubTile::_width - 1 - xx] = Syndicate::Data::Palette::HPAL02_YELLOW;
	}

#if 0
      for (unsigned zz = 0; zz < (_map_data->nb_k() - 1) * Syndicate::Data::MapSubTile::_height; zz++)
	{
	  the_map[(Syndicate::Data::MapSubTile::_height - zz) * width() + 0] = Syndicate::Data::Palette::Red;
	  the_map[(Syndicate::Data::MapSubTile::_height - zz) * width() + Tile::_width - 1] = Palette::Red;

	}
      
      coord(i, j, _map_data->nb_k() - 1, x, y);
      the_map = map + y * width() + x;

      for (unsigned xx = 0; xx < Syndicate::Data::MapSubTile::_width; xx++)
	{
	  the_map[(xx / 2) * width() + Syndicate::Data::MapSubTile::_width + xx] = Syndicate::Data::Palette::Red;
	  the_map[(xx / 2) * width() + Syndicate::Data::MapSubTile::_width - 1 - xx] = Syndicate::Data::Palette::Red;

	  the_map[(2 * Syndicate::Data::MapSubTile::_height - xx / 2) * width() + Syndicate::Data::MapSubTile::_width + xx] = Syndicate::Data::Palette::Red;
	  the_map[(2 * Syndicate::Data::MapSubTile::_height - xx / 2) * width() + Syndicate::Data::MapSubTile::_width - 1 - xx] = Syndicate::Data::Palette::Red;
	}
#endif
    }
}

void Map::tile(unsigned x, unsigned y, unsigned k, unsigned &i, unsigned &j)
{
  if (x < width() && y < height() && k < _map_data->nb_k())
    {
      unsigned X0 = 0, Y0 = 0;
      int X = 0, Y = 0;

      coord(0, 0, k, X0, Y0);

      X = x - (X0 + Syndicate::Data::MapSubTile::_width);
      Y = y - (Y0);

      printf("X = %d Y = %d X0 = %u Y0 = %u x = %u y = %u\n", X, Y, X0, Y0, x, y);

      i = (Y + X / 2) / (2 * Syndicate::Data::MapSubTile::_height);
      j = (Y - X / 2) / (2 * Syndicate::Data::MapSubTile::_height);

      for (unsigned K = 0; K < _map_data->nb_k(); K++)
	{
	  unsigned tiles_id = 0;
	  _map_data->get(i, j, K, tiles_id);
	  printf("%u ", tiles_id);
	}
      printf("\n");
      for (unsigned K = 0; K < _map_data->nb_k(); K++)
	{
	  unsigned tiles_id = 0;
	  _map_data->get(i, j, K, tiles_id);
	  uint8_t type = 0;
	  _map_column->get(tiles_id, type);
	  printf("%u ", type);
	}
      printf("\n");
    }
}

void Map::coord(unsigned i, unsigned j, unsigned k, unsigned &x, unsigned &y)
{
  if (i < _map_data->nb_i() && j < _map_data->nb_j() && k < _map_data->nb_k())
    {
      x  = 0;
      x += (_map_data->nb_j() - 1);
      x -= j;
      x += i;
      x *= Syndicate::Data::MapSubTile::_width;

      y  = 0;
      y += (_map_data->nb_k() - 1 - k);
      y += j;
      y += i;
      y *= Syndicate::Data::MapSubTile::_height;
    }
}

unsigned Map::width(void)
{
  return Syndicate::Data::MapTile::_width + Syndicate::Data::MapSubTile::_width * (_map_data->nb_i() - 1 + _map_data->nb_j() - 1) ;
}

unsigned Map::height(void)
{
  return Syndicate::Data::MapTile::_height + Syndicate::Data::MapSubTile::_height * (_map_data->nb_i() - 1 + _map_data->nb_j() - 1 + _map_data->nb_k() - 1);
}

void Map::get(uint8_t *pixels, unsigned pitch, unsigned i, unsigned j, unsigned k)
{
  /* we get the id of the tile */
  unsigned id = 0;
  _map_data->get(i, j, k, id);

  /* we get the pixels of the tile */
  if(id > 5)
    {
      /* we get the coordinates of the tile */
      unsigned x = 0, y = 0;
      coord(i, j, k, x, y);
      //printf("%u %u %u %u %u \n", i, j, k , x, y);

      _map_tile->get(id, pixels + y * pitch + x, pitch);
    }
}

void Map::get(uint8_t *pixels, unsigned pitch)
{
  /* We get the origin */
  //  unsigned x, y;
  //  coord(0, 0, _map_data->nb_k() - 1, x, y);

  if(pitch >= width())
    {
      /* For each floor */
      for(unsigned k = 0; k < _map_data->nb_k(); k++)
	{
	  /* for each row */
	  for(unsigned j = 0; j < _map_data->nb_j(); j++)
	    {
	      /* for each col */
	      for(unsigned i = 0; i < _map_data->nb_i(); i++)
		{
		  get(pixels, pitch, i, j, k);
		}
	    }
	}
    }

}
