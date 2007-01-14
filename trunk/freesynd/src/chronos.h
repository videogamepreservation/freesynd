#ifndef _SDL_events_h
#include "SDL_events.h"
#endif

extern Uint16 frame_time;
extern int frame_number;

Uint16 set_frame_time(const Uint16 t);
int init_chronos(void);

#define FRAMEEVENT (SDL_USEREVENT+1)
#define MUSICENDED (SDL_USEREVENT+2)
