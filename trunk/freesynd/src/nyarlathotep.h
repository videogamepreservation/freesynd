
typedef void(*framefunc_t)(void);
typedef void(*mode_ends_t)(void);

void enter_mode(framefunc_t f);
void exit_mode ();

/*void set_end_cb(mode_ends_t f);*/

void main_loop(void);

int init_nyarlathotep(void);
