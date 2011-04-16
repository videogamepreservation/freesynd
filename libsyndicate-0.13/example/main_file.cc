#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "Syndicate/Data/File.h"


int main(int argc, char ** argv)
{
  const char * input_filename = 0;
  const char * output_filename = 0;

  if(argc >= 2)
    {
      input_filename = argv[1];
      if(argc >= 3)
	{
	  output_filename = argv[2];
	}
      else
	{
	  output_filename = "out.dat";
	}
    }
  else
    {
      printf("Usage %s input [output = out.dat]\n", argv[0]);
      return 1;
    }


  uint8_t * output = 0;
  Syndicate::Data::File file(&output);
  file.load(input_filename);
  unsigned output_length = file.getLength();

 if(output)
    {
      printf("Open %s\n", output_filename);
      int fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd == -1)
	{
	  printf("Erreur opening %s : %s\n", output_filename, strerror(errno));
	  delete output;
	  return 0;
	}

      printf("Transfert %d bytes from %s to %s :", output_length, input_filename, output_filename);
      if(write(fd, output, output_length) != output_length)
	{
	  printf("NOK\nWrite error : %s\n", strerror(errno));
	}
      printf("OK\n");

      close(fd);

      delete output;
      output = 0;
    }

  return 0;
}
