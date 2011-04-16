#include <cstdio>
#include <cstring>
#include <assert.h>

#include "Syndicate/Data/MapData.h"
#include "TestMapData.h"

int main(int argc, char **argv) 
{
  Syndicate::Data::MapData *map_data = new Syndicate::Data::MapData();

  map_data->load(ref_map_filename);

  assert( map_data->nb_i() == ref_map_nb_i );

  assert( map_data->nb_j() == ref_map_nb_j );

  assert( map_data->nb_k() == ref_map_nb_k );

  for(unsigned k = 0; k < map_data->nb_k(); k++)
    {
      for(unsigned j = 0; j < map_data->nb_j(); j++)
	{
	  for(unsigned i = 0; i < map_data->nb_i(); i++)
	    {
	      unsigned id = 0;
	      map_data->get(i, j, k, id);
	      assert( id == ref_data_indexes[k][j][i] );
	    }
	}
    }

#if 0
  printf("map_data->nb_i() = %u\n", map_data->nb_i());

  printf("map_data->nb_j() = %u\n", map_data->nb_j());

  printf("map_data->nb_k() = %u\n", map_data->nb_k());

  printf("{");
  for(unsigned k = 0; k < map_data->nb_k(); k++)
    {
      printf("{");
      for(unsigned j = 0; j < map_data->nb_j(); j++)
	{
	  printf("{");
	  for(unsigned i = 0; i < map_data->nb_i(); i++)
	    {
	      unsigned id = 0;
	      map_data->get(i, j, k, id);
	      printf("0x%02x, ", id);
	    }
	  printf("},\n");
	}
      printf("},\n");
    }
  printf("\n");
#endif

  delete map_data;

  return 0;  
}

