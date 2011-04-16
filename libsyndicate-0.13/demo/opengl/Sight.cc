#include <cstring>
#include <cstdio>
#include <cmath>

#include "Sight.h"
#include "Tiles.h"
#include "Data.h"


/*
const GLfloat Sight::_planeValueS[] = 
  {
    1.0  / (2.0 * _tile_side), 
    -1.0 / (2.0 * _tile_side),
    0.0,
    0.5
  };

const GLfloat Sight::_planeValueT[] = 
  {
    1.0 / (4.0 * _tile_side),
    1.0 / (4.0 * _tile_side),
    1.0 / (2.0 * _tile_side),
    0.25
  };
*/

extern uint8_t tile_id;

const GLfloat Sight::_planeValueS[] = 
  {
    0.5, 
    -0.5,
    0.0,
    0.5
  };

const GLfloat Sight::_planeValueT[] = 
  {
    0.25,
    0.25,
    0.5,
    0.25
  };

Sight::Sight(Data *data, unsigned width, unsigned height) :
  _data(data),
  _width(width),
  _height(height),
  _nb_i(0),
  _nb_j(0),
  _nb_k(0),
  _nb_colors(0),
  _colors(0),
  _dark_brown(0),
  _dark_red(0),
  _dark_blue(0),
  _white(0),
  _light_brown(0),
  _light_red(0),
  _light_blue(0),
  _alpha_color(0),
  _nb_tiles(0),
  _tile_texture_names(0),
  _nb_frames(0),
  _frame_texture_names(0),
  _frame_list_base(0),
  _nb_fonts(0),
  _nb_fonts_col(0),
  _nb_fonts_row(0),
  _fonts_width(0),
  _fonts_height(0),
  _fonts_texture_name(0),
  _basis(0),
  _distance(8000),
  _zenith(-60),
  _azimuth(45),
  _pan_x(-2000),
  _pan_y(-18000),
  _zoom(1.0),
  _perspective(false),
  _smooth(false),
  _change_size(true),
  _change_zoom(true),
  _change_pan(true),
  _on_screen_mode(false)
{
}

// only rgba
void textureTreatment(uint8_t *texture, unsigned width, unsigned height)
{
  int kern_off[9] = 
    {
      - width - 1, - width, - width + 1,
      -1,          0,       1,
      width - 1,   width,   width + 1,
    };
  int kern_weight[9] = 
    {
      1, 1, 1,
      1, 1, 1,
      1, 1, 1
    };

  for (unsigned k = 0; k < height * width; k++) 
    {
      if(texture[3] == 0)
	{
	  unsigned n = 0;
	  unsigned r = 0;
	  unsigned g = 0;
	  unsigned b = 0;

	  for(unsigned l = 0; l < sizeof(kern_off) / sizeof(*kern_off); l++)
	    {
	      if((0 <= (k + kern_off[l])) && 
		 ((k + kern_off[l]) < (width * height)) && 
		 texture[kern_off[l] * 4 + 3] == 255)
		{
		  n ++;
		  r += kern_weight[l] * texture[kern_off[l] * 4 + 0]; 
		  g += kern_weight[l] * texture[kern_off[l] * 4 + 1]; 
		  b += kern_weight[l] * texture[kern_off[l] * 4 + 2]; 
		}
	    }
	  if(n != 0)
	    {
	      texture[0] = r / n;
	      texture[1] = g / n;
	      texture[2] = b / n;
	    }
	}
      texture+=4;
    }
}


bool Sight::initMap(void)
{
  printf("Initializing map ");
  _nb_i = _data->_map_data->nb_i();
  _nb_j = _data->_map_data->nb_j();
  _nb_k = _data->_map_data->nb_k();

  printf("Ok\n");
  return true;
}

bool Sight::initPalette(void)
{
  printf("Initializing palette ");

  _nb_colors = _data->_palette->size();
  _colors = new uint32_t [_nb_colors];

  for(unsigned index = 0; index < _nb_colors; index++)
    {
      union 
      {
	uint8_t  _tab[4];
	uint32_t _val;
      } color;
      if(index == _nb_colors - 1)
	{
	  color._tab[3] = 0; // transparent
	}
      else
	{
	  color._tab[3] = 0xff; // opaque
	}
      _data->_palette->get(index, color._tab[0], color._tab[1], color._tab[2]);
      _colors[index] = color._val;
    }  

  _dark_brown = _colors[5];
  _dark_red = _colors[6];
  _dark_blue = _colors[7];
  _white = _colors[12];
  _light_brown = _colors[13];
  _light_red = _colors[14];
  _light_blue = _colors[15];
  _alpha_color = _colors[_nb_colors - 1];

  printf("(%d colors) Ok\n", _nb_colors);
  return true;
}

bool Sight::initTiles(void)
{
  printf("Initializing tiles vertices ");

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer (3, GL_FLOAT, 0, tiles_vertices);

  printf("Ok\n");

  printf("Initializing tiles textures ");

  _nb_tiles = _data->_map_tile->_nb_tiles;

  _tile_texture_names = new GLuint[_nb_tiles];

  glGenTextures(_nb_tiles, _tile_texture_names);

  for(unsigned tile_index = 0; tile_index < _nb_tiles; tile_index++)
    {
      unsigned texture_width = 64;
      unsigned texture_height = 64;

      uint32_t tile_texture[texture_width * texture_height];
      memset(tile_texture, 0, texture_width * texture_height * sizeof(*tile_texture));
      _data->_map_tile->get(tile_index, tile_texture, texture_width * sizeof(*tile_texture), _colors);
      textureTreatment((uint8_t*)tile_texture, texture_width, texture_height);

      glBindTexture(GL_TEXTURE_2D, _tile_texture_names[tile_index]);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      if(_smooth)
	{
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
      else
	{
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

      gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, texture_width, texture_height,
			GL_RGBA, GL_UNSIGNED_BYTE, tile_texture);

    }
  printf("(%d tiles) Ok\n", _nb_tiles);
  return true;
}

bool Sight::renderTile(unsigned id)
{
  /*
    glColor3f(1.0, 1.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_QUADS, sizeof(_shape_dflt) / sizeof(*_shape_dflt), GL_UNSIGNED_BYTE, _shape_dflt);
    glCallList(_xyz);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  */

  if(id >= 5)
    {
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_TEXTURE_GEN_S);
      glEnable(GL_TEXTURE_GEN_T);

      glBindTexture(GL_TEXTURE_2D, _tile_texture_names[id]);

      if(_smooth)
	{
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
      else
	{
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
      glTexGenfv(GL_S, GL_OBJECT_PLANE, _planeValueS);

      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
      glTexGenfv(GL_T, GL_OBJECT_PLANE, _planeValueT);

      glPushMatrix();
      glScalef(_tile_side, _tile_side, _tile_side);
      glDrawElements(GL_QUADS, tiles[id]._nb_vertices, GL_UNSIGNED_BYTE, tiles[id]._faces);
#if 0
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0, 0.0, 0);
      glDrawElements(GL_QUADS, tiles[id]._nb_vertices, GL_UNSIGNED_BYTE, tiles[id]._faces);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
      glPopMatrix();

      glDisable(GL_TEXTURE_2D);
      glDisable(GL_TEXTURE_GEN_S);
      glDisable(GL_TEXTURE_GEN_T);

      glBindTexture(GL_TEXTURE_2D, 0);
    }


  return true;
}

bool Sight::renderTiles(void)
{
  glPushMatrix();

  for(int k = 0; k < _nb_k; k+=1)
    {
      glPushMatrix();
      for(int j = 0; j < _nb_j; j+=1)
	{
	  glPushMatrix();
	  for(int i = 0; i < _nb_i; i+=1)
	    {
	      unsigned id = 0;

	      /* we get the id of the tile */
	      _data->_map_data->get(i, j, k, id);

	      renderTile(id);

	      glTranslatef(_tile_side, 0.0, 0.0);
	    }
	  glPopMatrix();
	  glTranslatef(0.0, _tile_side, 0.0);
	}
      glPopMatrix();
      glTranslatef(0.0, 0.0, -_tile_side / 2.0);
    }

  glPopMatrix();

  return true;
}


bool Sight::initFrames(void)
{
  printf("Initializing frames texture ");

  _nb_frames = _data->_spr_frame->size();

  _frame_texture_names = new GLuint[_nb_frames];

  glGenTextures(_nb_frames, _frame_texture_names);

  _frame_list_base = glGenLists(_nb_frames);

  for(unsigned frame_index = 0; frame_index < _nb_frames; frame_index++)
    {
      unsigned frame_width = 0;
      unsigned frame_height = 0;
      int frame_x_offset = 0;
      int frame_y_offset = 0;

      _data->getFrameInfo(frame_index, frame_width, frame_height, frame_x_offset, frame_y_offset);

      if(frame_width == 0 || frame_height == 0)
	{
	  continue;
	}

      unsigned texture_width = 1;
      unsigned texture_height = 1;

      while(texture_width < frame_width) texture_width <<= 1;
      while(texture_height < frame_height) texture_height <<= 1;

      uint32_t frame_texture[texture_width * texture_height];
      memset(frame_texture, 0, texture_width * texture_height * sizeof(*frame_texture));
      uint32_t *pixels = frame_texture - frame_y_offset * texture_width - frame_x_offset;
      _data->getFrame(frame_index, pixels, texture_width * sizeof(*frame_texture), _colors);
      
      // manage texture
      glBindTexture(GL_TEXTURE_2D, _frame_texture_names[frame_index]);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

      if(_smooth)
	{
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
      else
	{
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

      gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, texture_width, texture_height,
			GL_RGBA, GL_UNSIGNED_BYTE, frame_texture);

      glNewList(_frame_list_base + frame_index, GL_COMPILE);
      //glColor3f(1.0, 1.0, 0.0);
      glBegin(GL_QUADS);
      glTexCoord2f(0.0, 0.0); 
      glVertex3f(frame_x_offset, frame_y_offset, 0.0);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(frame_x_offset, frame_y_offset + texture_height, 0.0);
      glTexCoord2f(1.0, 1.0);
      glVertex3f(frame_x_offset + texture_width, frame_y_offset + texture_height, 0.0);
      glTexCoord2f(1.0, 0.0);
      glVertex3f(frame_x_offset + texture_width, frame_y_offset, 0.0);
      glEnd();
      glEndList();

    }
  printf("(%d frames) Ok\n", _nb_frames);
  return true;
}

bool Sight::renderFrame(unsigned id)
{
  if(id >= _nb_frames) return false;

  glEnable(GL_TEXTURE_2D);

#if 0
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
#endif

  glBindTexture(GL_TEXTURE_2D, _frame_texture_names[id]);

  if(_smooth)
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
  else
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

#if 0
  GLfloat planeValueS[] = 
    {
      1.0 / (8.0 * texture_width), 
      -1.0 / (8.0 * texture_width),
      0.0,
      - (float)frame_x_offset / (float)texture_width
    };
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glTexGenfv(GL_S, GL_OBJECT_PLANE, planeValueS);

  GLfloat planeValueT[] = 
    {
      1.0 / (16.0 * texture_height), 
      1.0 / (16.0 * texture_height), 
      1.0 / (8.0 * texture_height), 
      - (float)frame_y_offset / (float)texture_height
    };
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glTexGenfv(GL_T, GL_OBJECT_PLANE, planeValueT);
#endif

  static const float mult = 4.0 * sqrtf(2.0);
  if(_on_screen_mode)
    {
      glCallList(_frame_list_base + id);
    }
  else
    {
      glPushMatrix();
      glScalef(mult, mult, mult);
      glCallList(_frame_list_base + id);
      glPopMatrix();
    }

  glDisable(GL_TEXTURE_2D);
#if 0
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
#endif

  glBindTexture(GL_TEXTURE_2D, 0);

}

bool Sight::renderFrames(void)
{
  for(int j = 0; j < _nb_j; j+=1)
    {
      for(int i = 0; i < _nb_i; i+=1)
	{
	  unsigned id = 0;
	  unsigned offset = _data->_game->_data->_offsets[j * _nb_i + i];
	  while(offset)
	    {
	      /* we get the item to draw */
	      Syndicate::Data::Game::GameItem *item = _data->_game->getItem(offset);
	      id = item->_index_current_frame;
	      int frame_I = item->_tile_i;
	      int frame_J = item->_tile_j;
	      int frame_K = item->_tile_k;
	      int frame_orientation = item->_orientation;
	      float rotation = (float)frame_orientation * 90.0 / 64.0;
	      
	      glPushMatrix();
	      
	      glTranslatef(frame_I, frame_J, -frame_K);

  	      //renderBasis();

	      glRotatef(_azimuth, 0.0, 0.0, -1.0);
	      glRotatef(_zenith, -1.0, 0.0, 0.0);

  	      renderFrame(id);

  	      //renderBasis();

	      glPopMatrix();

	      offset = item->_offset_next;
	    } // while offset
	}
    }
  return true;
}

bool Sight::initBasis(void)
{
  printf("Initializing basis ");

  _basis = glGenLists(1);
  glNewList(_basis,GL_COMPILE);
  glBegin(GL_LINES);
  glColor3f(1.0,0.0,0.0);
  glVertex3f(0.0,0.0,0.0); 
  glVertex3f(128,0.0,0.0);
  glColor3f(0.0,1.0,0.0);
  glVertex3f(0.0,0.0,0.0);
  glVertex3f(0.0,128,0.0);
  glColor3f(0.0,0.0,1.0);
  glVertex3f(0.0,0.0,0.0);
  glVertex3f(0.0,0.0,128);
  glEnd();
  glEndList();

  printf("Ok\n");
  return true;
}

bool Sight::initFonts(void)
{
  printf("Initializing fonts ");

  _nb_fonts = _data->_req->size();
  _nb_fonts_col = 32;
  _nb_fonts_row = _nb_fonts / _nb_fonts_col + 1;
  _fonts_width = Syndicate::Data::Req::_width;
  _fonts_height = Syndicate::Data::Req::_height;

  unsigned texture_width = 1;
  unsigned texture_height = 1;

  while(texture_width < _nb_fonts_col * _fonts_width) texture_width <<= 1;
  while(texture_height < _nb_fonts_row * _fonts_height) texture_height <<= 1;

  _nb_fonts_col = texture_width / _fonts_width;
  _nb_fonts_row = _nb_fonts / _nb_fonts_col + 1;

  uint32_t texture[texture_width * texture_height];
  memset(texture, 0, texture_width * texture_height * sizeof(*texture));
      
  for(unsigned id = 0; id < _nb_fonts; id++)
    {
      uint32_t *pixels = texture + (id / _nb_fonts_col) * texture_width * _fonts_height + (id % _nb_fonts_col) * _fonts_width;
      _data->_req->get(id, pixels, texture_width * sizeof(*texture), _colors);
    }

  // manage texture
  glGenTextures(1, &_fonts_texture_name);
  glBindTexture(GL_TEXTURE_2D, _fonts_texture_name);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  if(_smooth)
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
  else
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, texture_width, texture_height,
		    GL_RGBA, GL_UNSIGNED_BYTE, texture);

  printf("(%d fonts) Ok\n", _nb_fonts);
  return true;
}

bool Sight::renderFont(unsigned id)
{
  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, _fonts_texture_name);

  if(_smooth)
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
  else
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glBegin(GL_QUADS);

  /*
    glTexCoord2f((float)(id % _nb_fonts_col) / (float)_nb_fonts_col, (float)(id / _nb_fonts_col) / (float)_nb_fonts_row); 
    glVertex2f(0, 0);

    glTexCoord2f((float)(id % _nb_fonts_col) / (float)_nb_fonts_col, (float)(id / _nb_fonts_col + 1) / (float)_nb_fonts_row); 
    glVertex2f(0, 0.5);

    glTexCoord2f((float)(id % _nb_fonts_col + 1) / (float)_nb_fonts_col, (float)(id / _nb_fonts_col + 1) / (float)_nb_fonts_row); 
    glVertex2f(0.5, 0.5);

    glTexCoord2f((float)(id % _nb_fonts_col + 1) / (float)_nb_fonts_col, (float)(id / _nb_fonts_col) / (float)_nb_fonts_row); 
    glVertex2f(0.5, 0);
  */

  glTexCoord2f(0.0, 0.0); 
  glVertex2f(0, 0);

  glTexCoord2f(1, 0.0);
  glVertex2f(_nb_fonts_col * _fonts_width, 0.0);

  glTexCoord2f(1, 1);
  glVertex2f(_nb_fonts_col * _fonts_width, _nb_fonts_row * _fonts_height);

  glTexCoord2f(0, 1);
  glVertex2f(0, _nb_fonts_row * _fonts_height);

  glEnd();

  glDisable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);

  return true;
}

bool Sight::renderFonts(void)
{
  for(int i = 0; i < 60; i++)
    {
      renderFont(i);
    }
  return true;
}

bool Sight::renderMeasure(void)
{
  glBegin(GL_QUADS);
  glVertex2f(0, 0);
  glVertex2f(0, 1);
  glVertex2f(1, 1);
  glVertex2f(1, 0);
  glEnd();

  return true;
}

bool Sight::renderControlPanel(void)
{
  // Render agents
  unsigned ped_id = 0;
  while(ped_id < 4)
    {
      //pushMatrix();
      switch(ped_id)
	{
	case 0:
	  renderFrame(1374);
	  break;
	case 1:
	  renderFrame(1385);
	  break;
	case 2:
	  renderFrame(1386);
	  break;
	case 3:
	  renderFrame(1392);
	  break;
	default:
	  break;
	}

      if(_data->_game->_data->_pedestrians[ped_id]._type)
	{
	  // sprite
	  glPushMatrix();
	  glTranslatef(32,40,0);
	  renderFrame(_data->_game->_data->_pedestrians[ped_id]._index_current_frame);
	  glPopMatrix();

	  // health
	  int health = _data->_game->_data->_pedestrians[ped_id]._health;
	  if(health)
	    {
	      glPushMatrix();
	      glTranslatef(52,42,0);
	      glScalef(6, -health * 36 / 0x10, 1);
	      glColor4ubv((const GLubyte *)&_white);
	      renderMeasure();
	      glPopMatrix();
	    }

	  // Adrenalin
	  {
	    int depend = _data->_game->_data->_pedestrians[ped_id]._adrena_dependency * 55 / 0xff;
	    int effect = _data->_game->_data->_pedestrians[ped_id]._adrena_effect * 55 / 0xff;
	    int amount = _data->_game->_data->_pedestrians[ped_id]._adrena_amount * 55 / 0xff;

	    glPushMatrix();
	    glTranslatef(4 + depend,48,0);

	    glPushMatrix();
	    glScalef(effect - depend, 10, 1);
	    glColor4ubv((const GLubyte *)&_light_red);
	    renderMeasure();
	    glPopMatrix();

	    glPushMatrix();
	    glScalef(amount - depend, 10, 1);
	    glColor4ubv((const GLubyte *)&_dark_red);
	    renderMeasure();
	    glPopMatrix();

	    glPushMatrix();
	    glScalef(1, 10, 1);
	    glColor4ubv((const GLubyte *)&_white);
	    renderMeasure();
	    glPopMatrix();

	    glPopMatrix();
	  }

	  // Perception
	  {
	    int depend = _data->_game->_data->_pedestrians[ped_id]._percep_dependency * 55 / 0xff;
	    int effect = _data->_game->_data->_pedestrians[ped_id]._percep_effect * 55 / 0xff;
	    int amount = _data->_game->_data->_pedestrians[ped_id]._percep_amount * 55 / 0xff;

	    glPushMatrix();
	    glTranslatef(4 + depend,62,0);

	    glPushMatrix();
	    glScalef(effect - depend, 10, 1);
	    glColor4ubv((const GLubyte *)&_light_blue);
	    renderMeasure();
	    glPopMatrix();

	    glPushMatrix();
	    glScalef(amount - depend, 10, 1);
	    glColor4ubv((const GLubyte *)&_dark_blue);
	    renderMeasure();
	    glPopMatrix();

	    glPushMatrix();
	    glScalef(1, 10, 1);
	    glColor4ubv((const GLubyte *)&_white);
	    renderMeasure();
	    glPopMatrix();

	    glPopMatrix();
	  }

	  // Intelligence
	  {
	    int depend = _data->_game->_data->_pedestrians[ped_id]._inteli_dependency * 55 / 0xff;
	    int effect = _data->_game->_data->_pedestrians[ped_id]._inteli_effect * 55 / 0xff;
	    int amount = _data->_game->_data->_pedestrians[ped_id]._inteli_amount * 55 / 0xff;

	    glPushMatrix();
	    glTranslatef(4 + depend,76,0);

	    glPushMatrix();
	    glScalef(effect - depend, 10, 1);
	    glColor4ubv((const GLubyte *)&_light_brown);
	    renderMeasure();
	    glPopMatrix();

	    glPushMatrix();
	    glScalef(amount - depend, 10, 1);
	    glColor4ubv((const GLubyte *)&_dark_brown);
	    renderMeasure();
	    glPopMatrix();

	    glPushMatrix();
	    glScalef(1, 10, 1);
	    glColor4ubv((const GLubyte *)&_white);
	    renderMeasure();
	    glPopMatrix();

	    glPopMatrix();
	  }

	}

      ped_id++;

      switch(ped_id)
	{
	case 1:
	  glTranslatef(64,0,0);
	  break;
	case 2:
	  glTranslatef(-64,90,0);
	  
	  // every agent button
	  renderFrame(8229);
	  glTranslatef(0,10,0);
	  break;
	case 3:
	  glTranslatef(64,0,0);
	  break;
	case 4:
	  glTranslatef(-64,90,0);
	  
	  // mission infos
	  renderFrame(1572);
	  glTranslatef(0,10,0);
	  break;
	default:
	  break;
	}

    }

  // inventory
  ped_id = 0; // TODO the selected ped
  unsigned offset_eqpt = _data->_game->_data->_pedestrians[ped_id]._offset_equipment;
  unsigned offset_curr = _data->_game->_data->_pedestrians[ped_id]._offset_cur_weapon;
  for(int i = 0; i < 8; i++)
    {
      glPushMatrix();

      glTranslatef(32 * (i % 4), 32 * (i / 4), 0);

      unsigned item_type = 0;
      int item_amos = 0;
      if(38242 <= offset_eqpt && offset_eqpt < 56674)
	{
	  Syndicate::Data::Game::GameStruct::Weapon *item = reinterpret_cast<Syndicate::Data::Game::GameStruct::Weapon *>(_data->_game->getItem(offset_eqpt));
	  item_type = item->_sub_type;
	  item_amos = item->_nb_amos;
	  offset_eqpt = item->_offset_next_inventory;
	}

      if(item_type < Data::NbItems)
	{
	  renderFrame(Data::_items_frames[item_type] + ((offset_eqpt == offset_curr)?3:0));

	  if((item_amos >= 0) && Data::_items_amo[item_type])
	    {
	      glPushMatrix();
	      glTranslatef(4, 24, 0);
	      // The measure calcul isn't very beautiful
	      glScalef((item_amos + 1) * 23 / (Data::_items_amo[item_type] + 1), 4, 1);
	      glColor4ubv((const GLubyte *)&_light_red);
	      renderMeasure();

	      glPopMatrix();
	    }
	}

      glPopMatrix();
    }

  return true;
}

bool Sight::renderBasis(void)
{
  glCallList(_basis);
  return true;
}

bool Sight::enterOnScreenMode(void)
{
  glPushAttrib(GL_TRANSFORM_BIT | GL_VIEWPORT_BIT) ; 

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  glOrtho(0.0, _width, _height, 0.0, 0.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  _on_screen_mode = true;

  return true;
}

bool Sight::leaveOnScreenMode(void)
{
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glPopAttrib();

  _on_screen_mode = false;

  return true;
}

bool Sight::init(void)
{
  /****************************************/
  /* init opengl                          */
  /****************************************/
  printf("Initializing opengl.\n");
  glClearColor(0.8,0.8,0.8,1.0);

  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.5);
  
  //glEnable(GL_DEPTH_TEST);

  /*
  glClearDepth(1.0);  // Enables Clearing Of The Depth Buffer
  glDepthFunc(GL_LESS);  // The Type Of Depth Test To Do
  */

  //glShadeModel(GL_SMOOTH);  // Enables Smooth Color Shading
  //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective Calculations

  /****************************************/
  /* init map                             */
  /****************************************/
  initMap();

  /****************************************/
  /* init palette                         */
  /****************************************/
  initPalette();

  /****************************************/
  /* init tiles                           */
  /****************************************/
  initTiles();

  /****************************************/
  /* init frames                          */
  /****************************************/
  initFrames();

  /****************************************/
  /* init fonts                           */
  /****************************************/
  initFonts();

  /****************************************/
  /* init basis                           */
  /****************************************/
  initBasis();

  return true;
}

bool Sight::render(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(_change_size)
    {
      glViewport (0, 0, _width, _height);
    }

#if 1

  /**********************/
  /** render in world  **/
  /**********************/
  glMatrixMode (GL_PROJECTION);

  if(_change_size || _change_zoom)
    {
      glLoadIdentity ();

      if(_perspective)
	{
	  gluPerspective (45.0 * _zoom,(float)_width/(float)_height, 0.0, 10000.0);
	}
      else
	{
	  GLdouble horizontal_clip = 1800.0 * _zoom;
	  GLdouble vertical_clip = horizontal_clip * (float)_height / (float)_width;
	  glOrtho(-horizontal_clip, horizontal_clip, 
		  -vertical_clip, vertical_clip, 
		  -10000.0, 10000.0);
	}
    }
 
  glMatrixMode(GL_MODELVIEW);

  if(_change_size || _change_zoom || _change_pan)
    {
      glLoadIdentity();
      glRotatef(180.0, 1.0, 0.0, 0.0);
      
      if(_perspective)
	{
	  glTranslatef(0.0, 0.0, _distance * _zoom);
	}
      glRotatef(_zenith, 1.0, 0.0, 0.0);
      glTranslatef(_pan_x, _pan_y, 0);
      glRotatef(_azimuth, 0.0, 0.0, 1.0);
    }

  /*** render tiles   ***/
  renderTiles();
  //renderTile(tile_id);

  /*** render frames  ***/
  renderFrames();

#endif

#if 1
  /**********************/
  /** render on screen **/
  /**********************/
  enterOnScreenMode();

  /*** render fonts  ***/
  renderControlPanel();

  leaveOnScreenMode();
#endif

  _change_size = false;
  _change_zoom = false;
  _change_pan = false;

  return true;
}

bool Sight::draw()
{
  glFlush();
  return true;
}

