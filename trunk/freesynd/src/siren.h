
extern int    frequency;
extern Uint16 format;
extern int    channels;
extern int    chunksize;

/*
  int snd_new      (struct lua_State *L);
  int snd_gc       (struct lua_State *L);
  int snd_play     (struct lua_State *L);
  int snd_stop     (struct lua_State *L);
  int snd_pause    (struct lua_State *L);
  int snd_resume   (struct lua_State *L);
  int snd_volume   (struct lua_State *L);
  int siren_resume (struct lua_State *L);
  int siren_pause  (struct lua_State *L);
*/

int init_siren(void);
