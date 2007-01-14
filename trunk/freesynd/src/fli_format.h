
/****************************************\
|*  http://www.compuphase.com/flic.htm  *|
\****************************************/

const Uint16 MAGIC=0xAF12;

typedef struct FliHeader {
  Uint32 size;
  Uint16 type; /* 0xAF12. Or else! */
  Uint16 numFrames;
  Uint16 width;
  Uint16 height;
} __attribute__((packed)) FliHeader;

typedef struct ChunkHeader {
  Uint32 size;
  Uint16 type;
} __attribute__((packed)) ChunkHeader;

typedef struct FrameTypeChunkHeader {
  Uint32 size;
  Uint16 type;
  Uint16 numChunks;
  Uint16 delay;
  Uint16 reserved; /* always zero (why? says who? is it contagious?) */
  Uint16 widthOverride;
  Uint16 heightOverride;
} __attribute__((packed)) FrameTypeChunkHeader;

enum flc_opcodes { pktcnt=0, undef, last, skip };
