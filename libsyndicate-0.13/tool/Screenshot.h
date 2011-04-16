#ifndef __Screenshot_h__
#define __Screenshot_h__

#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

class Screenshot
{
 public:

  static Screenshot *getInstance(void)
    {
      static Screenshot instance;
      return &instance;
    }

  /** 
   *
   */
  bool init(pid_t pid = 0);

  pid_t pid(void) { return _pid; }

  /** 
   * \param filename an .xpm file where the screenshot will be stored
   */
  bool save(char *filename);

 private:

  Screenshot(void);

  ~Screenshot(void);

  Display *_dpy;

  int _scr;

  Window _w;

  pid_t _pid;

  XImage *_image;

  unsigned char *_buffer;
  unsigned int _width;
  unsigned int _height;
  unsigned int _depth;

  XShmSegmentInfo _shm_info;

  long _shm_alloc;

  bool _initialized;
};

#endif /* __Screenshot_h__ */

// Local Variables:
// mode: C++
// End:

