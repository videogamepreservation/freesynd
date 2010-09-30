#include <cstdio>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
      
#include <X11/cursorfont.h>
#include <X11/Xmu/WinUtil.h>
#include <X11/xpm.h>

#include "Screenshot.h"

Screenshot::Screenshot(void) :
  _dpy(0),
  _scr(0),
  _w(0),
  _pid(0),
  _image(0),
  _buffer(0),
  _width(0),
  _height(0),
  _depth(0),
  _shm_alloc(0),
  _initialized(false)
{
  printf("Screenshot hello !!\n");
}

Screenshot::~Screenshot(void)
{
  if(_shm_alloc)
    {
      XShmDetach (_dpy, &_shm_info);
      XDestroyImage (_image);
      shmdt (_shm_info.shmaddr);
      shmctl (_shm_info.shmid, IPC_RMID, 0);
    }
  else
    {
      if(_buffer)
	{
	  delete [] _buffer;
	}
    }

  printf("Screenshot bye !!\n");
}

bool Screenshot::init(pid_t pid)
{
  /******************************/
  /*** Grab the target window ***/
  /******************************/
  char *displayname = NULL; /* name of server to contact */

  _dpy = XOpenDisplay (displayname);
  if (!_dpy) 
    {
      fprintf (stderr, "unable to open display \"%s\"\n", XDisplayName (displayname));
      return false;
    }

  _scr = DefaultScreen (_dpy); /* screen number of _dpy */

  Window root = RootWindow (_dpy, _scr); /* the current root */

  Cursor cursor = XCreateFontCursor (_dpy, XC_target); /* cursor to use when selecting */
  if (cursor == None) 
    {
      fprintf (stderr, "unable to create selection cursor\n");
      return false;
    }

  XSync (_dpy, 0);

  int mask = (ButtonPressMask | ButtonReleaseMask);
  if (XGrabPointer (_dpy, root, False, mask, GrabModeSync, GrabModeAsync, 
		    None, cursor, CurrentTime) != GrabSuccess) 
    {
      fprintf (stderr, "unable to grab cursor\n");
      return false;
    }

  int pressed = 0; /* count of number of buttons pressed */
  _w = None; /* the window that got selected */
  while (_w == None || pressed != 0) 
    {
      XAllowEvents (_dpy, SyncPointer, CurrentTime);
      XEvent event;
      XWindowEvent (_dpy, root, mask, &event);
      switch (event.type) 
	{
	case ButtonPress:
	  if (_w == None) 
	    {
	      _w = ((event.xbutton.subwindow != None) ?
		    event.xbutton.subwindow : root);
	    }
	  pressed++;
	  continue;
	case ButtonRelease:
	  if (pressed > 0) pressed--;
	  continue;
	}
    }

  XUngrabPointer (_dpy, CurrentTime);

  XFreeCursor (_dpy, cursor);

  XSync (_dpy, 0);

  if(_w == root)
    {
      fprintf (stderr, "can't select root window\n");
      return false;
    }

  XID w_client = XmuClientWindow(_dpy, _w);
  if(_w == w_client) 
    {
      fprintf (stderr, "can't select wm window\n");
      return false;
    }

  _w = w_client;

  Window root_return;
  int x_return, y_return;
  unsigned int border_width_return;
  XGetGeometry(_dpy, _w, &root_return, &x_return, &y_return,
	       &_width, &_height, &border_width_return, &_depth);

  /******************************/
  /*** Get the pid            ***/
  /******************************/
  if(pid)
    {
      _pid = pid;
    }
  else
    {
      int atom_nb = 0;
      Atom *atom_list = XListProperties(_dpy, _w, &atom_nb);
      Atom atom;
      char *atom_name = 0;
      int i;
      for(i = 0; i < atom_nb; i++)
	{
	  atom_name = XGetAtomName (_dpy, atom_list[i]);
	  //printf("%s\n", atom_name);
	  if(strstr(atom_name, "WM_PID"))
	    {
	      atom = atom_list[i];
	      break;
	    }
	}
      XFree(atom_list);
      if(i == atom_nb)
	{
	  fprintf (stderr, "can't find any pid information\n");
	}
      else
	{
	  Atom actual_type;
	  int actual_format;
	  unsigned long nitems;
	  unsigned long bytes_after;
	  unsigned char *prop;
	  int status = XGetWindowProperty(_dpy, _w, atom, 0, 1024,
					  False, AnyPropertyType,
					  &actual_type,
					  &actual_format, 
					  &nitems,
					  &bytes_after,
					  &prop);

	  if(status) 
	    {
	      fprintf (stderr, "can't get %s\n", atom_name);
	      return false;
	    }

	  if(!prop) 
	    {
	      fprintf (stderr, "no properties for %s\n", atom_name);
	      return false;
	    }
  
	  _pid = prop[0] + prop[1] * 256;
	  //printf("pid of window 0x%x = %d\n", (unsigned int)_w, _pid);
	}
    }

  /******************************/
  /*** Get the image          ***/
  /******************************/
  _shm_alloc = XShmQueryExtension(_dpy);
  if(!_shm_alloc) 
    {
      fprintf (stderr, "needs code\n");
      return false;
      /*
	_buffer = new unsigned char [_image->bytes_per_line * _image->height];
	if(!_buffer) 
	{
	fprintf (stderr, "malloc failed\n");
	return false;
	}
      */
    }
  else
    { 
      _image = XShmCreateImage(_dpy, DefaultVisual(_dpy, _scr),
			       _depth, ZPixmap, 0, &_shm_info,
			       _width, _height);

      _shm_info.shmid = shmget(IPC_PRIVATE, _image->bytes_per_line * _image->height, 
			       IPC_CREAT | 0777);
      _shm_info.shmaddr = _image->data = (char *)shmat(_shm_info.shmid, NULL, 0);
      if(_shm_info.shmaddr == (void *) -1)
	{
	  perror("shmat failed\n");
	  return false;
	}    
      _shm_info.readOnly = False;

      XShmAttach(_dpy, &_shm_info);
 
    }  

  _initialized = true;

  return true;
}

bool Screenshot::save(char * filename)
{
  if(!_initialized)
    {
      fprintf(stderr, "not initialized\n");      
      return false;
    }

  if(!filename)
    {
      fprintf(stderr, "need a filename for saving the screenshot\n");
      return false;
    }

  printf("Write to %s\n", filename);

  if(!_shm_alloc) 
    {
      _image = XGetImage(_dpy, _w, 0, 0, _width, _height, AllPlanes, ZPixmap);
    }
  else
    {
      XShmGetImage (_dpy, _w, _image, 0, 0, AllPlanes);
    }

  XpmWriteFileFromImage(_dpy, filename,  _image, 0, NULL);

  if(!_shm_alloc) 
    {
      XFree(_image);
    }

  return true;
}

