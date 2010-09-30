#include <cstdio>
#include <cstring>

#include "Syndicate/Data/Debug.h"

void Syndicate::Data::error(const char *file, 
	   const char *function, 
	   const unsigned line,
	   const char *format, ...)
{
  fprintf(stderr, "%s (%d) ", function, line); 
  va_list ap;
  va_start ( ap, format );
  vfprintf(stderr, format, ap);
  va_end ( ap );
}

#ifdef DEBUG

/**
 * This is an array with all files we can debug
 * Comment some line to disable debug on files.
 */
static const char * files_to_debug [] = 
  {
    //"Bitstream.cc",
    "Block.cc",
    "Colors.cc",
    "Debug.cc",
    "File.cc",
    "Font.cc",
    "Game.cc",
    //"HuffmanTable.cc",
    "Main.cc",
    "MapColumn.cc",
    "MapData.cc",
    "MapSubTile.cc",
    "MapTile.cc",
    "Palette.cc",
    "Raw.cc",
    "Req.cc",
    //"Rnc.cc",
    "SpriteAnim.cc",
    "SpriteData.cc",
    "SpriteElement.cc",
    "SpriteFrame.cc",
    "SpriteTab.cc",
    NULL,
  };

void Syndicate::Data::debug(const char *file, 
			    const char *function, 
			    const unsigned line,
			    const char *format, ...)
{
  unsigned i = 0;
  while(files_to_debug[i] != NULL)
    {
      if (strcmp(file, files_to_debug[i]) == 0)
	{
	  va_list ap;
	  va_start ( ap, format );
	  vprintf(format, ap);
	  va_end ( ap );
	  return;
	}
      i++;
    }
}

#endif
