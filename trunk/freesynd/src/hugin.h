
extern struct lua_State *L;

int          get_int(struct lua_State *L, int ix, const char *n,const int alt);
const char * get_str(struct lua_State *L, int ix, const char *n);
double       get_num(struct lua_State *L, const char *n);
void         set_num(struct lua_State *L, const char *n, const double d);
void         set_str(struct lua_State *L, const char *n, const char *s);

void debug_lua_table(struct lua_State *L, int ind);
void debug_lua_stack(struct lua_State *L, const char *n);

int init_hugin(void);

#define foreach(L, ix) for(lua_pushnil(L); lua_next(L, ix-1); lua_pop(L, 1))
