#ifndef __Syndicate_Engine_Game_h__
#define __Syndicate_Engine_Game_h__

#include "Syndicate/Data/Game.h"
#include "Syndicate/Data/Palette.h"
#include "Syndicate/Data/MapData.h"
#include "Syndicate/Data/MapTile.h"
#include "Syndicate/Data/MapColumn.h"
#include "Syndicate/Data/SpriteAnim.h"
#include "Syndicate/Data/SpriteFrame.h"
#include "Syndicate/Data/SpriteElement.h"
#include "Syndicate/Data/SpriteTab.h"
#include "Syndicate/Data/SpriteData.h"

class Game
{
public:
  Game(void);

  ~Game(void);

  void init(void);

  void load(unsigned game_id);

  /**
   * Give the width of the map in number of tiles * 256
   * \return width
   */
  unsigned map_width(void)
  {
    return _map_data->nb_i() * 0x100;
  }

  /**
   * Give the height of the map in number of tiles * 256
   * \return height
   */
  unsigned map_height(void)
  {
    return _map_data->nb_j() * 0x100;
  }

  /**
   * Give the depth of the map in number of tiles * 128
   * \return depth
   */
  unsigned map_depth(void)
  {
    return _map_data->nb_k() * 0x80;
  }

#if 0
  void map_trim(int &i, int &j, int &k)
  {
    if(i < 0)
      {
	i = 0;
      }
    if((map_width() - 1 - i) < 0) 
      {
	i = map_width() - 1;
      }

    if(j < 0)
      {
	j = 0;
      } 
    if((map_height() - 1 - j) < 0) 
      {
	j = map_height() - 1;
      }

    if(k < 0)
      {
	k = 0;
      } 
    if((map_depth() - 1 - k) < 0) 
      {
	k = map_depth() - 1;
      }
  }
#endif

  /**
   * Give the width of the map in number of pixels on the screen
   * \return number of pixels width
   */
  unsigned screen_width(void)
  {
    return Syndicate::Data::MapTile::_width + 
      Syndicate::Data::MapSubTile::_width * (_map_data->nb_i() - 1 + _map_data->nb_j() - 1) ;
  }

  /**
   * Give the height of the map in number of pixels on the screen
   * \return number of pixels height
   */
  unsigned screen_height(void)
  {
    return Syndicate::Data::MapTile::_height + 
      Syndicate::Data::MapSubTile::_height * (_map_data->nb_i() - 1 + _map_data->nb_j() - 1 + _map_data->nb_k() - 1);
  }

#if 0
  void screen_trim(int &x, int &y)
  {
    if(x < 0)
      {
	x = 0;
      }
    if((map_width() - 1 - x)  < 0) 
      {
	x = map_width() - 1;
      }

    if(y < 0)
      {
	y = 0;
      }
    if((map_height() - 1 - y)  < 0) 
      {
	y = map_height() - 1;
      }
  }
#endif

  /**
   * Transform map coordinates to screen coordinate
   * \param[in] i one tile every 0x100
   * \param[in] j one tile every 0x100
   * \param[in] k one tile every 0x80
   * \param[out] x
   * \param[out] y
   */
  void mapCoord2screenCoord(int i, int j, int k, int &x, int &y)
  {
    x = _x0 + (i - j) / 8;
    y = _y0 + (i + j) / 16 - k / 8;
  }

  /**
   * Transform screen coordinates to map coordinate for a given k
   * \param[in] x
   * \param[in] y
   * \param[in] k
   * \param[out] i
   * \param[out] i
   */
  void screenCoord2mapCoord(int x, int y, int k, int &i, int &j)
  {
    i = _i0 + 4 * x + 8 * y + k;
    j = _j0 - 4 * x + 8 * y + k;
  }

#if 0
  /**
   * Transform map vector to screen vector
   * \param[in] i
   * \param[in] j
   * \param[in] k
   * \param[out] x
   * \param[out] y
   */
  void mapVect2screenVect(int i, int j, int k, int &x, int &y)
  {
    x = (i - j) / 8;
    y = (i + j) / 16 - k / 8;
  }

  /**
   * Transform screen vector to map vector for a given k
   * \param[in] x
   * \param[in] y
   * \param[in] k
   * \param[out] i
   * \param[out] i
   */
  void screenVect2mapVect(int x, int y, int k, int &i, int &j)
  {
    i =   4 * x + 8 * y + k;
    j = - 4 * x + 8 * y + k;
  }
#endif

  void showGameInfo(void);

  void getFrameInfo(unsigned frame_index, unsigned &frame_width, unsigned &frame_height, int &frame_x_offset, int &frame_y_offset) const;

  template<typename TColors>
  void getFrame(unsigned frame_index, TColors *pixels, unsigned pitch, const TColors *colors = Syndicate::Data::Colors<TColors>::_default) const;

  Syndicate::Data::Game          *_game;
  Syndicate::Data::Palette       *_palette;
  Syndicate::Data::MapData       *_map_data;
  Syndicate::Data::MapTile       *_map_tile;
  Syndicate::Data::MapColumn     *_map_column;
  Syndicate::Data::SpriteAnim    *_spr_anim;
  Syndicate::Data::SpriteFrame   *_spr_frame;
  Syndicate::Data::SpriteElement *_spr_element;
  Syndicate::Data::SpriteTab     *_spr_tab;
  Syndicate::Data::SpriteData    *_spr_data;

  unsigned _game_id;
  char     _game_name[16];
  unsigned _palette_id;
  char     _palette_name[16];
  unsigned _map_id;
  char     _map_name[16];

  int _x0;
  int _y0;
  int _i0;
  int _j0;

};

#endif /* __Syndicate_Engine_Game_h__ */

// Local Variables:
// mode: C++
// End:
