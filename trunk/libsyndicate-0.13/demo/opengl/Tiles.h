#ifndef __Tiles_h__
#define __Tiles_h__

#include <GL/gl.h>

extern const GLfloat tiles_vertices[] ;

struct Tile
{
  unsigned  _nb_vertices;
  const GLubyte  *_faces;
};

extern const Tile tiles[];

#endif // __Tiles_h__
