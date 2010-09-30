#ifndef __Sight_h__
#define __Sight_h__

#include <GL/gl.h>
#include <GL/glu.h>

class Data;

class Sight
{
public :
  Sight(Data *data, unsigned width = 640, unsigned height = 480);

  bool init(void);

  bool render(void);

  bool draw(void);

  bool resize(unsigned width, unsigned height)
  {
    _width = width;
    _height = height;
    _change_size = true;
  }
  bool panZoom(int delta_x, int delta_y, int delta_z)
  {
    if(delta_x)
      {
	_pan_x += (delta_x * _delta_pan_x);
	_change_pan = true;
      }
    if(delta_y)
      {
	_pan_y += (delta_y * _delta_pan_y);
	_change_pan = true;
      }
    if(delta_z)
      {
	_zoom += (delta_z * _delta_zoom);
	_change_zoom = true;
      }
  }

  void perspective(bool enable) 
  {
    _perspective = enable;
    _change_zoom = true;
  }

  void smooth(bool enable)
  {
    _smooth = enable;
  }

private:

  Data    *_data;
  unsigned _width;
  unsigned _height;

  unsigned _nb_i;
  unsigned _nb_j;
  unsigned _nb_k;

  unsigned _nb_colors;
  uint32_t  *_colors;
  uint32_t _dark_brown;
  uint32_t _dark_red;
  uint32_t _dark_blue;
  uint32_t _white;
  uint32_t _light_brown;
  uint32_t _light_red;
  uint32_t _light_blue;
  uint32_t _alpha_color;


  unsigned _nb_tiles;
  GLuint *_tile_texture_names;

  unsigned _nb_frames;
  GLuint *_frame_texture_names;
  GLuint _frame_list_base;
  
  unsigned _nb_fonts;
  unsigned _nb_fonts_col;
  unsigned _nb_fonts_row;
  unsigned _fonts_width;
  unsigned _fonts_height;
  GLuint _fonts_texture_name;

  GLuint _basis;

  float _distance;
  float _zenith;
  float _azimuth;
  float _pan_x;
  float _pan_y;
  float _zoom;

  bool _perspective;
  bool _smooth;

  bool _change_size;
  bool _change_zoom;
  bool _change_pan;

  bool _on_screen_mode;

  static const float _tile_side = 256.0;
  static const float _delta_pan_x = 256.0;
  static const float _delta_pan_y = 256.0;
  static const float _delta_zoom = 0.2;

  static const GLfloat _planeValueS[];
  static const GLfloat _planeValueT[];

  bool initMap(void);
  bool initPalette(void);

  bool initTiles(void);
  bool renderTile(unsigned id);
  bool renderTiles(void);

  bool initFrames(void);
  bool renderFrame(unsigned id);
  bool renderFrames(void);

  bool initFonts(void);
  bool renderFont(unsigned id);
  bool renderFonts(void);

  bool renderControlPanel(void);

  bool initBasis(void);
  bool renderBasis(void);

  bool renderMeasure(void);

  bool enterOnScreenMode(void);
  bool leaveOnScreenMode(void);

};


#endif /* __Sight_h__ */

// Local Variables:
// mode: C++
// End:
