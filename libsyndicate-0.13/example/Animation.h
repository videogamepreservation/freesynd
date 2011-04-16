// -*- Mode: c++ -*-

#ifndef __Animation_h__
#define __Animation_h__

#include "Syndicate/Data/SpriteAnim.h"
#include "Syndicate/Data/SpriteFrame.h"
#include "Syndicate/Data/SpriteElement.h"
#include "Syndicate/Data/SpriteTab.h"
#include "Syndicate/Data/SpriteData.h"

class Animation
{
public:
  Animation(const char *anim_filename,
	    const char *frame_filename,
	    const char *element_filename,
	    const char *tab_filename,
	    const char *data_filename);

  ~Animation(void);

  void set(unsigned anim_index);

  void get(uint8_t *pixels, unsigned pitch, unsigned frame_num);

  unsigned get_frame(uint8_t *pixels, unsigned pitch, unsigned frame_index);
  unsigned get_frame_info(unsigned frame_index, unsigned &frame_width, unsigned &frame_height, int &frame_x_offset, int &frame_y_offset);

  Syndicate::Data::SpriteAnim    *_anim;
  Syndicate::Data::SpriteFrame   *_frame;
  Syndicate::Data::SpriteElement *_element;
  Syndicate::Data::SpriteTab     *_tab;
  Syndicate::Data::SpriteData    *_data;

  unsigned _anim_index;
};

#endif /* __Animation_h__ */
