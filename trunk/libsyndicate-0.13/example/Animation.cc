#include <cstdio>

#include <inttypes.h>

#include "Animation.h"

Animation::Animation(const char *anim_filename,
		     const char *frame_filename,
		     const char *element_filename,
		     const char *tab_filename,
		     const char *data_filename)
{
  _anim = new Syndicate::Data::SpriteAnim();
  _anim->load(anim_filename);

  _frame = new Syndicate::Data::SpriteFrame();
  _frame->load(frame_filename);

  _element = new Syndicate::Data::SpriteElement();
  _element->load(element_filename);

  _tab = new Syndicate::Data::SpriteTab();
  _tab->load(tab_filename);

  _data = new Syndicate::Data::SpriteData();
  _data->load(data_filename);

  _anim_index = 0;
}

Animation::~Animation(void)
{
  delete _anim;
  delete _frame;
  delete _element;
  delete _tab;
  delete _data;
}	      

void Animation::set(unsigned anim_index)
{
  _anim_index = anim_index;
}

void Animation::get(uint8_t *pixels, unsigned pitch, unsigned frame_num)
{
  static unsigned frame_index = 0;

  if(frame_num == 0)
    {
      _anim->get(_anim_index, frame_index);
    }

  printf("FRAME %u:\n", frame_num);
  frame_index = get_frame(pixels, pitch, frame_index);
}

unsigned Animation::get_frame(uint8_t *pixels, unsigned pitch, unsigned frame_index)
{
  printf("\tframe\tindex\telement\twidth\theight\tflags\n\t\t%u\t", frame_index);

  /* we get frame infos and the next frame */
  unsigned element_index = 0;
  unsigned frame_width = 0;
  unsigned frame_height = 0;
  unsigned frame_flags = 0;
  _frame->get(frame_index, element_index, frame_width, frame_height, frame_flags, frame_index);

  int frame_x_offset = frame_width;
  int frame_y_offset = frame_height;  
  printf("%u\t%u\t%u\t%u\t\n", 
	 element_index,
	 frame_width, 
	 frame_height,
	 frame_flags);
  printf("\tsprite\tindex\tx off\ty off\twidth\theight\tflipped\tdata index\n");
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

      _element->get(element_index, sprite_index, sprite_x_offset, sprite_y_offset, sprite_flipped, element_index);

      _tab->get(sprite_index, sprite_data_index, sprite_width, sprite_height); 
     
      printf("\t\t%u\t%d\t%d\t%u\t%u\t%u\t%u\n", 
	     sprite_index, 
	     sprite_x_offset, 
	     sprite_y_offset,
	     sprite_width,
	     sprite_height,
	     sprite_flipped,
	     sprite_data_index);
      
      uint8_t *screen_ptr = pixels + sprite_y_offset * pitch + sprite_x_offset;

      _data->get(sprite_data_index, sprite_width, sprite_height, sprite_flipped, screen_ptr, pitch);

      if(frame_x_offset > sprite_x_offset)
	{
	  frame_x_offset = sprite_x_offset ;
	}
      if(frame_y_offset > sprite_y_offset)
	{
	  frame_y_offset = sprite_y_offset ;
	}

    }

  return frame_index;
}

unsigned Animation::get_frame_info(unsigned frame_index, unsigned &frame_width, unsigned &frame_height, int &frame_x_offset, int &frame_y_offset)
{
  /* we get frame infos and the next frame */
  unsigned element_index = 0;
  frame_width = 0;
  frame_height = 0;
  unsigned frame_flags = 0;
  _frame->get(frame_index, element_index, frame_width, frame_height, frame_flags, frame_index);

  frame_x_offset = frame_width;
  frame_y_offset = frame_height;  

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

      _element->get(element_index, sprite_index, sprite_x_offset, sprite_y_offset, sprite_flipped, element_index);

      _tab->get(sprite_index, sprite_data_index, sprite_width, sprite_height); 
     
      if(sprite_x_offset < frame_x_offset)
	{
	  frame_x_offset = sprite_x_offset ;
	}
      if(sprite_y_offset < frame_y_offset)
	{
	  frame_y_offset = sprite_y_offset ;
	}
    }

  return frame_index;
}
