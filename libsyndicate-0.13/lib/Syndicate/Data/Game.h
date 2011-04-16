#ifndef __Syndicate_Data_Game_h__
#define __Syndicate_Data_Game_h__

#include "Syndicate/Data/File.h"
#include "Syndicate/Data/Int.h"

namespace Syndicate
{ 
  namespace Data
  {
    class Game : public File
    {
    public:
      Game(void) : File(reinterpret_cast<uint8_t **>(&_data)) {}

      ~Game(void) {}

      struct GameItem
      {
	le_uint16_t _offset_next;
	le_uint16_t _offset_prev;
	le_uint16_t _tile_i;
	le_uint16_t _tile_j;
	le_uint16_t _tile_k;
	le_uint8_t  _unkn10;
	le_uint8_t  _unkn11;
	le_uint8_t  _unkn12[2];
	le_uint16_t _index_base_anim;
	le_uint16_t _index_current_frame;
	le_uint16_t _index_current_anim;
	le_int16_t  _health;
	le_uint16_t _offset_unknown;
	le_uint8_t  _type;
	le_uint8_t  _status;
	le_uint8_t  _orientation;
      };

      GameItem *getItem(unsigned offset)
      {
	static uint8_t *raw_data = reinterpret_cast<uint8_t *>(_data);
	return reinterpret_cast<GameItem *>(raw_data + 32774 + offset);	
      }

      struct GameStruct
      {
	le_uint8_t _header[6];
	
	le_uint16_t _offsets[128 * 128]; // '_offset + 32774' gives the offset in this file of the first object of this tile. if zero no object
	le_uint16_t _offset_ref; // this field is at 32774
	
	// This struct size is 92.
	// The first height pedestrians seems to be our agents.
	// But why is it used for ?
	struct Pedestrian
	{
	  // - 00
	  le_uint16_t _offset_next; // '_offset + 32774' gives the offset in this file of the next object
	  le_uint16_t _offset_prev; // '_offset + 32774' gives the offset in this file of the previous object (sometimes weapon, or the next target for example ???)
	  le_uint16_t _tile_i;
	  le_uint16_t _tile_j;
	  le_uint16_t _tile_k;
	  // - 10
	  le_uint8_t  _unkn10;      // always to 4 ? this bit may be connected with minimap in bottom left
	  le_uint8_t  _unkn11;      // this bit is connected with control
	  le_uint8_t  _unkn12;
	  le_uint8_t  _unkn13;
	  le_uint16_t _index_base_anim;  // index in (HSTA-0.ANI)
	  le_uint16_t _index_current_frame; // index in (HFRA-0.ANI)
	  le_uint16_t _index_current_anim; // index in (HSTA-0.ANI)
	  // - 20
	  le_int16_t  _health; // something to do with amo / health
	  le_uint16_t _offset_last_enemy;
	  le_uint8_t  _type; //0x01 ped; 0x02 vehicle; 0x04 weapon; 0x05 object; allow to display a target, a pickup, and for minimap
	  le_uint8_t  _status;
	  le_uint8_t  _orientation;
	  le_uint8_t  _unkn27;
	  le_uint8_t  _unkn28;      // when 01 pedestrian, 02 agent, 04 police, 08 guard : change IA and minimap
	  le_uint8_t  _unkn29;
	  // - 30
	  le_uint16_t _unkn30;
	  le_uint16_t _offset_of_persuader;
	  le_uint16_t _unkn34;
	  le_uint16_t _offset_of_vehicle;
	  le_uint16_t _offset_scenario_1;
	  // - 40
	  le_uint16_t _offset_scenario_2;
	  le_uint16_t _unkn42;
	  le_uint16_t _offset_of_vehicle_2; // ??
	  le_uint16_t _goto_tile_i;
	  le_uint16_t _goto_tile_j;
	  // - 50
	  le_uint16_t _goto_tile_k;
	  le_uint8_t  _unkn52[6];
	  le_uint16_t _offset_equipment;
	  // - 60
	  le_uint16_t _mods_info;
	  le_uint8_t  _unkn62[6];
	  le_uint16_t _offset_cur_weapon;
	  // - 70
	  le_uint8_t  _unkn70;
	  le_uint8_t  _adrena_amount;
	  le_uint8_t  _adrena_dependency;
	  le_uint8_t  _adrena_effect;
	  le_uint8_t  _unkn74;
	  le_uint8_t  _inteli_amount;
	  le_uint8_t  _inteli_dependency;
	  le_uint8_t  _inteli_effect;
	  le_uint8_t  _unkn78;
	  le_uint8_t  _percep_amount;
	  le_uint8_t  _percep_dependency;
	  le_uint8_t  _percep_effect;
	  le_uint8_t  _unkn82;
	  le_uint8_t  _unkn83[9];
	} _pedestrians[256];
	
	// This struct size is 42.
	struct Vehicle 
	{
	  le_uint16_t _offset_next;
	  le_uint16_t _offset_prev;
	  le_uint16_t _tile_i;
	  le_uint16_t _tile_j;
	  le_uint16_t _tile_k;
	  le_uint8_t  _unkn10;
	  le_uint8_t  _unkn11;
	  le_uint8_t  _unkn12;
	  le_uint8_t  _unkn13;
	  le_uint16_t _index_base_anim;
	  le_uint16_t _index_current_frame;
	  le_uint16_t _index_current_anim;
	  le_int16_t  _health;
	  le_uint16_t _offset_last_enemy;
	  le_uint8_t  _type;
	  le_uint8_t  _sub_type;
	  le_uint8_t  _orientation;
	  le_uint8_t  _unkn27;
	  le_uint8_t  _offset_of_ped;
	  le_uint8_t  _unkn30[13];
	} _vehicles[64];

	// This struct size is 30.
	struct Object
	{
	  le_uint16_t _offset_next;
	  le_uint16_t _offset_prev;
	  le_uint16_t _tile_i;
	  le_uint16_t _tile_j;
	  le_uint16_t _tile_k;
	  le_uint8_t  _unkn10;
	  le_uint8_t  _unkn11;
	  le_uint8_t  _unkn12;
	  le_uint8_t  _unkn13;
	  le_uint16_t _index_base_anim;
	  le_uint16_t _index_current_frame;
	  le_uint16_t _index_current_anim;
	  le_uint8_t  _unkn20[4];
	  le_uint8_t  _type;
	  le_uint8_t  _sub_type; // 0x0C door; 0x12 open window; 0x13 close window; 0x16 tree
	  le_uint8_t  _orientation;
	  le_uint8_t  _unkn27[3]; // always 0 ?
	} _objects[400];

	// This struct size is 36.
	struct Weapon
	{
	  le_uint16_t _offset_next;
	  le_uint16_t _offset_prev;
	  le_uint16_t _tile_i;
	  le_uint16_t _tile_j;
	  le_uint16_t _tile_k;
	  le_uint8_t  _unkn10;
	  le_uint8_t  _unkn11;
	  le_uint8_t  _unkn12;
	  le_uint8_t  _unkn13;
	  le_uint16_t _index_base_anim;
	  le_uint16_t _index_current_frame;
	  le_uint16_t _index_current_anim;
	  le_int16_t  _nb_amos;
	  le_uint16_t _unkn22;
	  le_uint8_t  _type;
	  le_uint8_t  _sub_type; //0x01 persuadertron; 0x02 pistol; 0x03 gauss gun; 0x04 shotgun; 0x05 uzi; 0x06 minigun; 0x07 laser; 0x08 flamer; 0x09 long range; 0x0A scanner; 0x0B medikit; 0x0C time bomb; 0x0D access card; 0x0E invalid; 0x0F invalid; 0x10 invalid; 0x11 energy shield;
	  le_uint16_t _unkn26;
	  le_uint16_t _offset_next_inventory;
	  le_uint16_t _offset_prev_inventory;
	  le_uint16_t _offset_owner;
	  le_uint16_t _unkn34;
	} _weapons[512];

	struct Sfx
	{
	  le_uint16_t _offset_next;
	  le_uint16_t _offset_prev;
	  le_uint16_t _tile_i;
	  le_uint16_t _tile_j;
	  le_uint16_t _tile_k;
	  le_uint16_t _unkn10;
	  le_uint16_t _unkn12;
	  le_uint16_t _index_base_anim;
	  le_uint16_t _index_current_frame;
	  le_uint16_t _index_current_anim;
	  le_uint16_t _unkn20;
	  le_uint16_t _unkn22;
	  le_uint16_t _unkn24;
	  le_uint16_t _unkn26;
	  le_uint16_t _offset_owner;
	} _sfx[256];

	struct Scenario
	{
	  le_uint16_t _next;
	  le_uint16_t _offset_object;
	  le_uint8_t  _i_factor;
	  le_uint8_t  _j_factor;
	  le_uint8_t  _k_factor;
	  le_uint8_t  _type;	  
	} _scenarios[2048];

	le_uint8_t _unkn08[448];

	struct Mapinfos
	{
	  le_uint16_t _map;
	  le_uint16_t _min_x;
	  le_uint16_t _min_y;
	  le_uint16_t _max_x;
	  le_uint16_t _max_y;
	  le_uint8_t  _unkn10[4];
	} _mapinfos;

	struct Objectives
	{
	  le_uint16_t _type; // 0x00 ??? ;0x01 persuade; 0x02 assassinate; 0x03 protect; 0x05 equipment aquisition; 0x0a combat sweep (police); 0x0b combat sweep; 0x0d raid and rescue; 0x0e use vehicle; 0x10 evacuate
	  // if protect, the type is zero, and the next objectives are the goals, the list finish with zero and the offset of the protected item ?
	  le_uint16_t _offset; // '_offset + 32774' gives the offset in this file of the first objective
	  le_uint16_t _tile_i;
	  le_uint16_t _tile_j;
	  le_uint16_t _tile_k;
	  le_uint8_t  _unkn10[4];
	} _objectives[10];

	le_uint8_t _unkn11[1896];

      } *_data;

    };

  }

}

#endif /* __Syndicate_Data_Game_h__ */

// Local Variables:
// mode: C++
// End:
