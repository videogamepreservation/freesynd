
#define REV32(x) ({                                        \
  int n=x;                                                 \
  n = ((n >>  1) & 0x55555555) | ((n <<  1) & 0xaaaaaaaa); \
  n = ((n >>  2) & 0x33333333) | ((n <<  2) & 0xcccccccc); \
  n = ((n >>  4) & 0x0f0f0f0f) | ((n <<  4) & 0xf0f0f0f0); \
  n = ((n >>  8) & 0x00ff00ff) | ((n <<  8) & 0xff00ff00); \
  n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000); \
})

#define REV16(x) ({                                \
  int n=x;                                         \
  n = ((n >>  1) & 0x5555) | ((n <<  1) & 0xaaaa); \
  n = ((n >>  2) & 0x3333) | ((n <<  2) & 0xcccc); \
  n = ((n >>  4) & 0x0f0f) | ((n <<  4) & 0xf0f0); \
  n = ((n >>  8) & 0x00ff) | ((n <<  8) & 0xffff); \
})

#define REV8(x) ({ \
  int n=x;\
  n = ((n >> 1) & 0x55) | ((n << 1) & 0xaa); \
  n = ((n >> 2) & 0x33) | ((n << 2) & 0xcc); \
  n = ((n >> 4) & 0x0f) | ((n << 4) & 0xf0); })


#define CEIL8(x) ((x & 7)? x & ~7 + 8: x)

