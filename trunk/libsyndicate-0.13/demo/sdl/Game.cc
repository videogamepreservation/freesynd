#include <cstdio>
#include <cstring>

#include "Game.h"

Game::Game(void) :
  _game(0),
  _palette(0),
  _map_data(0),
  _map_tile(0),
  _map_column(0),
  _spr_anim(0),
  _spr_frame(0),
  _spr_element(0),
  _spr_tab(0),
  _spr_data(0),
  _game_id(0),
  _palette_id(0),
  _map_id(0),
  _x0(0),
  _y0(0),
  _i0(0),
  _j0(0)
{
  _game = new Syndicate::Data::Game();

  _palette = new Syndicate::Data::Palette();

  _map_data = new Syndicate::Data::MapData();

  _map_tile = new Syndicate::Data::MapTile();

  _map_column = new Syndicate::Data::MapColumn();

  _spr_anim = new Syndicate::Data::SpriteAnim();

  _spr_frame = new Syndicate::Data::SpriteFrame();

  _spr_element = new Syndicate::Data::SpriteElement();

  _spr_tab = new Syndicate::Data::SpriteTab();

  _spr_data = new Syndicate::Data::SpriteData();

}

Game::~Game(void)
{
  delete _game;
  delete _palette;
  delete _map_data;
  delete _map_tile;
  delete _spr_anim;
  delete _spr_frame;
  delete _spr_element;
  delete _spr_tab;
  delete _spr_data;
}

void Game::showGameInfo(void)
{

}

void Game::init(void)
{
  /****************************************/
  /* init common ressources               */
  /****************************************/
  _map_tile->load("HBLK01.DAT");

  _map_column->load("COL01.DAT");

  _spr_anim->load("HSTA-0.ANI");

  _spr_frame->load("HFRA-0.ANI");

  _spr_element->load("HELE-0.ANI");

  _spr_tab->load("HSPR-0.TAB");

  _spr_data->load("HSPR-0.DAT");
}

void Game::load(unsigned game_id)
{
  /****************************************/
  /* init this level ressources           */
  /****************************************/
  _game_id = game_id;
  snprintf(_game_name, sizeof(_game_name), "GAME%02d.DAT", _game_id);
  _game->load(_game_name);

  _palette_id = 1 + (_game_id % 5);
  snprintf(_palette_name, sizeof(_palette_name), "HPAL%02d.DAT", _palette_id);
  _palette->load(_palette_name);

  _map_id = _game->_data->_mapinfos._map;
  snprintf(_map_name, sizeof(_map_name), "MAP%02d.DAT", _map_id);

  _map_data->load(_map_name);

  _x0 = _map_data->nb_j() * 32;
  _y0 = (_map_data->nb_k() - 1) * 16;
  _i0 = - _map_data->nb_j() * 128 - (_map_data->nb_k() - 1) * 128;
  _j0 =   _map_data->nb_j() * 128 - (_map_data->nb_k() - 1) * 128;

}

void Game::getFrameInfo(unsigned frame_index, unsigned &frame_width, unsigned &frame_height, int &frame_x_offset, int &frame_y_offset) const
{
  /* we get frame infos and the next frame */
  unsigned element_index = 0;
  unsigned frame_flags = 0;
  _spr_frame->get(frame_index, element_index, frame_width, frame_height, frame_flags, frame_index);

  /*
    printf("%u\t%u\t%u\t%u\t\n", 
    element_index,
    frame_width, 
    frame_height,
    frame_flags);
  */

  /*
    printf("\tsprite\tindex\tx off\ty off\twidth\theight\tflipped\tdata index\n");
  */

  frame_x_offset = frame_width;
  frame_y_offset = frame_height;  

  if(frame_width == 0 || frame_height == 0)
    {
      return ;
    } 

  while(element_index)
    {
      /* we get element infos */
      unsigned sprite_index = 0;
      int      sprite_x_offset = 0;
      int      sprite_y_offset = 0;
      bool     sprite_flipped = 0;

      _spr_element->get(element_index, sprite_index, sprite_x_offset, sprite_y_offset, sprite_flipped, element_index);
     
	unsigned sprite_data_index = 0;
	unsigned sprite_width = 0;
	unsigned sprite_height = 0;
	_spr_tab->get(sprite_index, sprite_data_index, sprite_width, sprite_height); 

	/*
	printf("\t\t%u\t%d\t%d\t%u\t%u\t%u\t%u\n", 
	sprite_index, 
	sprite_x_offset, 
	sprite_y_offset,
	sprite_width,
	sprite_height,
	sprite_flipped,
	sprite_data_index);
      */
      
      if(frame_x_offset > sprite_x_offset)
	{
	  frame_x_offset = sprite_x_offset ;
	}
      if(frame_y_offset > sprite_y_offset)
	{
	  frame_y_offset = sprite_y_offset ;
	}
    }

}

template<typename TColors>
void Game::getFrame(unsigned frame_index, TColors *pixels, unsigned pitch, const TColors *colors) const
{
  /* we get frame infos and the next frame */
  unsigned element_index = 0;
  unsigned frame_width = 0;
  unsigned frame_height = 0;
  unsigned frame_flags = 0;
  _spr_frame->get(frame_index, element_index, frame_width, frame_height, frame_flags, frame_index);

  if(frame_width == 0 || frame_height == 0)
    {
      return ;
    } 

  while(element_index)
    {
      /* we get element infos */
      unsigned sprite_index = 0;
      int      sprite_x_offset = 0;
      int      sprite_y_offset = 0;
      unsigned sprite_width = 0;
      unsigned sprite_height = 0;
      bool     sprite_flipped = 0;
      unsigned sprite_data_index = 0;

      _spr_element->get(element_index, sprite_index, sprite_x_offset, sprite_y_offset, sprite_flipped, element_index);

      _spr_tab->get(sprite_index, sprite_data_index, sprite_width, sprite_height); 
     
      TColors *pix = pixels + sprite_y_offset * pitch / sizeof(*pixels) + sprite_x_offset;

      _spr_data->get(sprite_data_index, sprite_width, sprite_height, sprite_flipped, pix, pitch, colors);
    }
}

template void Game::getFrame(unsigned frame_index, uint8_t *pixels, unsigned pitch, const uint8_t *colors) const;
template void Game::getFrame(unsigned frame_index, uint32_t *pixels, unsigned pitch, const uint32_t *colors) const;
