#include <stdio.h>
#include <stdlib.h>

#include <inttypes.h>

#include "Syndicate/Data/Game.h"

char *getOffset(unsigned offset, int base = 32774);

Syndicate::Data::Game *game = 0;

int main(int argc, char *argv[]) 
{
  char *file_name = "GAME01.DAT";

  if(argc >= 2)
    {
      file_name = argv[1];
    }

  game = new Syndicate::Data::Game();
  if(!game->load(file_name))
    {
      return 1;
    }

  printf("Sizeof game : %d\n", sizeof(Syndicate::Data::Game::GameStruct));
  printf("Offsets :\n");
  printf("\theader   : %6u\n", ((uint8_t*)(game->_data->_header) - (uint8_t*)game->_data));
  printf("\toffsets  : %6u\n", ((uint8_t*)(game->_data->_offsets) - (uint8_t*)game->_data));
  printf("\tpeds     : %6u (%d)\n", ((uint8_t*)(game->_data->_pedestrians) - (uint8_t*)game->_data), sizeof(Syndicate::Data::Game::GameStruct::Pedestrian));
  printf("\tvehicles : %6u (%d)\n", ((uint8_t*)(game->_data->_vehicles) - (uint8_t*)game->_data), sizeof(Syndicate::Data::Game::GameStruct::Vehicle));
  printf("\tobjects  : %6u (%d)\n", ((uint8_t*)(game->_data->_objects) - (uint8_t*)game->_data), sizeof(Syndicate::Data::Game::GameStruct::Object));
  printf("\tweapons  : %6u (%d)\n", ((uint8_t*)(game->_data->_weapons) - (uint8_t*)game->_data), sizeof(Syndicate::Data::Game::GameStruct::Weapon));
  printf("\tsfx      : %6u (%d)\n", ((uint8_t*)(game->_data->_sfx) - (uint8_t*)game->_data), sizeof(Syndicate::Data::Game::GameStruct::Sfx));
  printf("\tscenario : %6u (%d)\n", ((uint8_t*)(game->_data->_scenarios) - (uint8_t*)game->_data), sizeof(Syndicate::Data::Game::GameStruct::Scenario));
  printf("\tmapinfo  : %6u (%d)\n", ((uint8_t*)(&game->_data->_mapinfos) - (uint8_t*)game->_data), sizeof(Syndicate::Data::Game::GameStruct::Mapinfos));
  printf("\tobjectiv : %6u (%d)\n", ((uint8_t*)(game->_data->_objectives) - (uint8_t*)game->_data), sizeof(Syndicate::Data::Game::GameStruct::Objectives));

#if 0
  for(unsigned i = 0; i < sizeof(game->_data->_pedestrians) / sizeof(Syndicate::Data::Game::GameStruct::Pedestrian); i++)
    {
      Syndicate::Data::Game::GameStruct::Pedestrian *pedestrian = &(game->_data->_pedestrians[i]);
      if(pedestrian->_tile_i)
	{
	  printf("Pedestrian #%d (%u) (%u)\n", i, ((uint8_t*)pedestrian - (uint8_t*)game->_data), ((uint8_t*)pedestrian - (uint8_t*)game->_data - 32774));
	  printf("\t%d %d\n", (int)pedestrian->_offset_next, (int)pedestrian->_offset_prev);
	  printf("\t%d %d %d\n", (int)pedestrian->_tile_i, (int)pedestrian->_tile_j, (int)pedestrian->_tile_k);
	  printf("\t%d %d %d %d \n", (int)pedestrian->_unkn10, (int)pedestrian->_unkn11, (int)pedestrian->_unkn12, (int)pedestrian->_unkn13);
	  printf("\t%d %d %d\n", (int)pedestrian->_index_base_anim, (int)pedestrian->_index_current_frame, (int)pedestrian->_index_current_anim);
	  printf("\t%d %d\n", (int)pedestrian->_health, (int)pedestrian->_offset_last_enemy);
	  printf("\t%d %d %d\n", (int)pedestrian->_type, (int)pedestrian->_status, (int)pedestrian->_orientation);
	}
    }
#endif
#if 0
  for(unsigned i = 0; i < 64; i++)
    {
      Syndicate::Data::Game::GameStruct::Vehicle *vehicle = &(game->_data->_vehicles[i]);
      if(vehicle->_tile_sub_i)
	{
	  printf("Vehicle #%d (%u) (%u)\n\t", i, ((uint8_t*)vehicle - (uint8_t*)game->_data), ((uint8_t*)vehicle - (uint8_t*)game->_data - 32774));
	  printf("%d %d\n\t%d %d %d %d %d %d\n\t0x%02x 0x%02x 0x%02x 0x%02x\n\t%d %d %d\n\t%d 0x%02x 0x%02x 0x%02x %d %d\n",
		 (int)vehicle->_offset_prev, (int)vehicle->_offset_next,
		 (int)vehicle->_tile_sub_i, (int)vehicle->_tile_i, (int)vehicle->_tile_sub_j, (int)vehicle->_tile_j, (int)vehicle->_tile_sub_k, (int)vehicle->_tile_k,
		 (int)vehicle->_unkn10, (int)vehicle->_unkn11, (int)vehicle->_unkn12[0], (int)vehicle->_unkn12[1], 
		 (int)vehicle->_static_index_anim, (int)vehicle->_static_index_frame, (int)vehicle->_dynamic_index_anim, 
		 (int)vehicle->_unkn20[0], (int)vehicle->_unkn20[1], (int)vehicle->_unkn20[2], (int)vehicle->_unkn20[3], (int)vehicle->_type, (int)vehicle->_sub_type);
	  printf("\t0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
		 (int)vehicle->_unkn26[0], (int)vehicle->_unkn26[1], (int)vehicle->_unkn26[2], (int)vehicle->_unkn26[3],
		 (int)vehicle->_unkn26[4], (int)vehicle->_unkn26[5], (int)vehicle->_unkn26[6], (int)vehicle->_unkn26[7],
		 (int)vehicle->_unkn26[8], (int)vehicle->_unkn26[9], (int)vehicle->_unkn26[10], (int)vehicle->_unkn26[11],
		 (int)vehicle->_unkn26[12], (int)vehicle->_unkn26[13], (int)vehicle->_unkn26[14], (int)vehicle ->_unkn26[15]);
	}
    }
#endif
#if 0
  for(unsigned i = 0; i < 400; i++)
    {
      Syndicate::Data::Game::GameStruct::Object *object = &(game->_data->_objects[i]);
      if(object->_tile_sub_i)
	{
	  printf("Object #%d (%u) (%u)\n\t", i, ((uint8_t*)object - (uint8_t*)game->_data), ((uint8_t*)object - (uint8_t*)game->_data - 32774));
	  printf("%d %d\n\t%d %d %d %d %d %d\n\t0x%02x 0x%02x 0x%02x 0x%02x\n\t%d %d %d\n\t%d 0x%02x 0x%02x 0x%02x %d %d\n",
		 (int)object->_offset_prev, (int)object->_offset_next,
		 (int)object->_tile_sub_i, (int)object->_tile_i, (int)object->_tile_sub_j, (int)object->_tile_j, (int)object->_tile_sub_k, (int)object->_tile_k,
		 (int)object->_unkn10, (int)object->_unkn11, (int)object->_unkn12[0], (int)object->_unkn12[1], 
		 (int)object->_static_index_anim, (int)object->_static_index_frame, (int)object->_dynamic_index_anim, 
		 (int)object->_nb_amos, (int)object->_unkn21[0], (int)object->_unkn21[1], (int)object->_unkn21[2], (int)object->_type, (int)object->_sub_type);
	  printf("\t0x%02x 0x%02x 0x%02x 0x%02x\n",
		 (int)object->_orientation, (int)object->_unkn27[0], (int)object->_unkn27[1], (int)object->_unkn27[2]);
	}
    }
#endif
#if 0
  for(unsigned i = 0; i < 725; i++)
    {
      Syndicate::Data::Game::GameStruct::Weapon *weapon = &(game->_data->_weapons[i]);
      if(weapon->_tile_sub_i)
	{
	  printf("Weapon #%d (%u) (%u)\n\t", i, ((uint8_t*)weapon - (uint8_t*)game->_data), ((uint8_t*)weapon - (uint8_t*)game->_data - 32774));
	  printf("%d %d\n\t%d %d %d %d %d %d\n\t0x%02x 0x%02x 0x%02x 0x%02x\n\t%d %d %d\n\t%d 0x%02x 0x%02x 0x%02x %d %d\n",
		 (int)weapon->_offset_prev, (int)weapon->_offset_next,
		 (int)weapon->_tile_sub_i, (int)weapon->_tile_i, (int)weapon->_tile_sub_j, (int)weapon->_tile_j, (int)weapon->_tile_sub_k, (int)weapon->_tile_k,
		 (int)weapon->_unkn10, (int)weapon->_unkn11, (int)weapon->_unkn12[0], (int)weapon->_unkn12[1], 
		 (int)weapon->_static_index_anim, (int)weapon->_static_index_frame, (int)weapon->_dynamic_index_anim, 
		 (int)weapon->_nb_amos, (int)weapon->_unkn21[0], (int)weapon->_unkn21[1], (int)weapon->_unkn21[2], (int)weapon->_type, (int)weapon->_sub_type);
	  printf("\t0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
		 (int)weapon->_unkn26[0], (int)weapon->_unkn26[1], (int)weapon->_unkn26[2], (int)weapon->_unkn26[3],
		 (int)weapon->_unkn26[4], (int)weapon->_unkn26[5], (int)weapon->_unkn26[6], (int)weapon->_unkn26[7],
		 (int)weapon->_unkn26[8], (int)weapon->_unkn26[9], (int)weapon->_unkn26[10], (int)weapon->_unkn26[11],
		 (int)weapon->_unkn26[12], (int)weapon->_unkn26[13], (int)weapon->_unkn26[14], (int)weapon ->_unkn26[15]);
	}
    }
#endif
#if 1
  for(unsigned n = 0; n < 2048; n++)
    {
      Syndicate::Data::Game::GameStruct::Scenario *scen = &(game->_data->_scenarios[n]);
      
      printf("Scen #%4d", n);
      unsigned offset = scen->_offset_object;
      int i, j, k;
      i = (scen->_i_factor << 7) | 0x0040;
      j = (scen->_j_factor << 7) | 0x0040;
      k = (scen->_k_factor << 7) | 0x0000;
      printf("\tnext %4d offset %6d i %6d j %6d k %6d type %02x %s\n", (int)scen->_next, offset, i, j, k, (int)scen->_type, getOffset(offset));
    }
#endif

  delete game;
  
  return 0;
}

char *getOffset(unsigned offset, int base)
{
  if(32776 - base <= offset && offset < 56328 - base)
    {
      return "Ped";
    }
  else if(56328 - base <= offset && offset < 59016 - base)
    {
      return "Veh";
    }
  else if(59016 - base <= offset && offset < 71016 - base)
    {
      return "Obj";
    }
  else if(71016 - base <= offset && offset < 89448 - base)
    {
      return "Wea";
    }
  else if(89448 - base <= offset && offset < 97128 - base)
    {
      return "Sfx";
    }
  else if(97128 - base <= offset && offset < 113512 - base)
    {
      return "Scen";
    }
  else if(113512 - base <= offset && offset < 113960 - base)
    {
      return "Unk";
    }
  else if(113960 - base <= offset)
    {
      return "Mapino";
    }
  return "???";
}
