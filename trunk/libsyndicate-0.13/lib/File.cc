#include <cstdio>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

#include "Syndicate/Data/Debug.h"
#include "Syndicate/Data/Rnc.h"
#include "Syndicate/Data/File.h"

using namespace Syndicate::Data;

File::File(uint8_t **data) : 
  _data(data), 
  _data_length(0) 
{
  Assert( _data != 0 );
  *_data = 0;
}

File::~File(void)
{
  if(*_data)
    {
      delete[] *_data;
    }
}

void File::info(void)
{
  Debug("The file %s is loaded at %p and is %s bytes\n", _filename, *_data, _data_length);
}

bool File::load(char const * filename)
{
  char full_path[PATH_MAX] = {0};
  int fd = -1;

  if(filename[0] != '.' && filename[0] != '/')
    {
      char * syndicate_data_path = getenv("SYNDICATE_DATA_PATH");
      if(syndicate_data_path)
	{
	  snprintf(full_path, sizeof(full_path), "%s/%s", syndicate_data_path, filename);

	  // try with this filename
	  fd = open(full_path, O_RDONLY);
	  if (fd == -1)
	    {
	      Error("open %s : %s\n", full_path, strerror(errno));
	      // try with uppercase filename
	      for(unsigned i = strlen(syndicate_data_path) + 1; i < strlen(full_path); i++)
		{
		  full_path[i] = toupper(full_path[i]);
		}
	      fd = open(full_path, O_RDONLY);
	      if (fd == -1)
		{
		  Error("open %s : %s\n", full_path, strerror(errno));
		  // try with lowercase filename
		  for(unsigned i = strlen(syndicate_data_path) + 1; i < strlen(full_path); i++)
		    {
		      full_path[i] = tolower(full_path[i]);
		    }
		  fd = open(full_path, O_RDONLY);
		  if (fd == -1)
		    {
		      Error("open %s : %s\n", full_path, strerror(errno));
		      return false;
		    }
		}
	    }
	}
      else
	{
	  Error("SYNDICATE_DATA_PATH not set : try to find the data in the current dir.\n");
	  snprintf(full_path, sizeof(full_path), "./%s", filename);
	  fd = open(full_path, O_RDONLY);
	  if (fd == -1)
	    {
	      Error("open %s : %s\n", full_path, strerror(errno));
	      return false;
	    }
	}      
    }
  else
    {
      strncpy(full_path, filename, sizeof(full_path));
      fd = open(full_path, O_RDONLY);
      if (fd == -1)
	{
	  Error("open %s : %s\n", full_path, strerror(errno));
	  return false;
	}
    }


  // Get the data length
  struct stat file_stats;
  fstat(fd, &file_stats);
  off_t input_length = file_stats.st_size; 
  
  // Alloc the input buffer
  uint8_t *input = new uint8_t [input_length];
  if(!input)
    {
      Error("allocation error.\n");
      close(fd);
      return false;
    }
  

  // Fill the input buffer with the data of the file
  if(read(fd, input, input_length) != input_length)
    {
      Error("read from %s : %s\n", filename, strerror(errno));
      delete[] input;
      close(fd);
      return false;
    }  

  close(fd);

  uint8_t *output = input;
  unsigned output_length = input_length;

  // Try to unpack data.
  if(Rnc::getInstance()->isRnc(input))
    {
      output = Rnc::getInstance()->unpack(input, output_length);
      delete[] input;
      if(output == 0)
	{
	  Error("error unpacking %s\n", full_path);
	  return false;
	}
    }

  // Erase old data if any
  if(*_data)
    {
      delete [] *_data;
    }

  // Store the result
  *_data = output;
  _data_length = output_length;
  strncpy(_filename, full_path, sizeof(_filename));

  return true;
}

