
#define SCALE 32 /* units per tile in map-space. */
#define W 64     /* tile size */
#define H 32

#define MS_x(x,y,z) ( ( 2*(x) - 2*(y) + 0*(z) ) / 2)
#define MS_y(x,y,z) ( ( 1*(x) + 1*(y) - 1*(z) ) / 2)
#define SM_x(x,y)   ( ( 1*(x) + 2*(y) ) / 2 )
#define SM_y(x,y)   ( (-1*(x) + 2*(y) ) / 2 )
#define TILE(x) ((x) & ~(SCALE-1))
#define SUB(x)  ((x) % SCALE)

#define GLOM(x,y,z) ((z << 16) + (y << 8) + (x << 0))

int init_gaia(void);
