#ifndef __Data_h__
#define __Data_h__

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
#include "Syndicate/Data/Req.h"

struct Data
{
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
  Syndicate::Data::Req           *_req;

  unsigned _game_id;
  char     _game_name[16];
  unsigned _palette_id;
  char     _palette_name[16];
  unsigned _map_id;
  char     _map_name[16];

   Data(void);

  ~Data(void);

  void init(void);

  void load(unsigned game_id);

  void getFrameInfo(unsigned frame_index, unsigned &frame_width, unsigned &frame_height, int &frame_x_offset, int &frame_y_offset) const;

  template<typename TColors>
  void getFrame(unsigned frame_index, TColors *pixels, unsigned pitch, const TColors *colors = Syndicate::Data::Colors<TColors>::_default) const;

  enum Items
    {
      None,
      Persuadertron,
      Pistol, // Air raid
      Gaussgun,
      Shotgun,
      Uzi,
      Minigun,
      Laser,
      Flamer,
      Longrange,
      Scanner,
      Medikit,
      Timebomb,
      Accesscard, // clone shield
      Item14,
      Item15,
      Item16,
      Energyshield,
      NbItems
    };

  static const unsigned _items_amo[NbItems];
  static const unsigned _items_frames[NbItems];

};

#endif /* __Data_h__ */

// Local Variables:
// mode: C++
// End:
