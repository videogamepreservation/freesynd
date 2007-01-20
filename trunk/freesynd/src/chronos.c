#include "common.h"

int frame_number = 0;
int frame_time   = 0;

static        SDL_TimerID   timer_id = (SDL_TimerID) 0;
static struct SDL_UserEvent ue       = { SDL_USEREVENT+1, 0, NULL, NULL };

static Uint32 push_frame_event(Uint32 ival, void *p)
{
  ue.code = ++frame_number;

  if(SDL_PushEvent((SDL_Event *) &ue) == -1) {
    printf("Critical error, SDL refuses PushEvent!\n");
    error = SDL_GetError();
    printf("%s\n", error);
  }
  return frame_time;
}

Uint16 set_frame_time(const Uint16 t)
{
  int old_time = frame_time;

  frame_time = t;
  if(!old_time)   timer_id = SDL_AddTimer(frame_time, push_frame_event, NULL);
  if(!frame_time)            SDL_RemoveTimer(timer_id), timer_id = 0;
  return old_time;
}

int init_chronos(void)
{
  if(SDL_Init(SDL_INIT_TIMER) < 0) {
    error = SDL_GetError();
    return 0;
  }
  printf("Chronos timer module.\n");

  return 1;
}
