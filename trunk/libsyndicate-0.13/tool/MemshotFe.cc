#include <cstdio>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <inttypes.h>

#include "Memshot.h"
#include "Screenshot.h"

void usage(int argc,char* argv[]) 
{
  printf("Usage: \n"
	 "\t%s filename [pid [pattern_offset [pattern_length]]]\n", argv[0]);
  printf("For example for finding GAME01.DAT try : \n"
	 "\t%s GAME01.DAT \n", argv[0]);

  exit(1);
}

void help(char *default_filename, char *quick_save_filename) 
{
  printf("Help :\n");
  printf(" r             : Re-init \n");
  printf(" S [filename]  : Save from mem to file. If no filename is given,\n"
	 "                 will write to %s.BIN and %s.xpm\n", default_filename, default_filename);
  printf(" s [num] [int] : save 'num' shots from mem to file (will write\n"
	 "                 to %s.BIN and %s.xpm), every 'int' seconds\n", quick_save_filename, quick_save_filename);
  printf(" L [filename]  : load from file to mem. If no filename is given,\n"
	 "                 will read from %s\n", default_filename);
  printf(" q             : Quit\n");
}

int main(int argc, char* argv[]) 
{
  if(argc < 2)
    {
      usage(argc, argv);
    }

  // Init params
  char *filename = argv[1];
  int pid = 0;
  unsigned pattern_offset = 59020; // Search the vehicle section by default
  unsigned pattern_length = 4;

  if(access(filename, F_OK))
    {
      fprintf(stderr, "%s doesn't exist\n", filename);
    }
  if(argc > 2)
    {  
      pid = strtol(argv[2], 0, 0);
    }
  if(argc > 3)
    {  
      pattern_offset = strtol(argv[3], 0, 0);
    }
  if(argc > 4)
    {  
      pattern_length = strtol(argv[4], 0, 0);
    }

  // Init Screenshot
  if(!Screenshot::getInstance()->init())
    {
      return 1;
    }

  if(!pid)
    {
      pid = Screenshot::getInstance()->pid();
      if(!pid)
	{
	  fprintf(stderr, "no pid found\n");
	  return 1;
	}
    }

  // Init Memshot
  if(!Memshot::getInstance()->init(pid, filename, pattern_offset, pattern_length))
    {
      return 1;
    }

  // Init the file names
  char base[PATH_MAX] = {0};
  strncpy(base, filename, sizeof(base));
  *strrchr(base, '.') = 0;
  unsigned index = 1;
  char quick_save_filename[PATH_MAX] = {0};
  snprintf(quick_save_filename, sizeof(quick_save_filename), "%s_%03d", base, index);
  char xpm_filename[PATH_MAX] = {0};
  char bin_filename[PATH_MAX] = {0};

  // Begin
  help(filename, quick_save_filename);
  bool stop = false;
  while(!stop)
    {
      printf("?> ");
      fflush(0);
      char cmd[PATH_MAX] = {0};
      fgets(cmd, sizeof(cmd), stdin);
      switch(*cmd)
	{
	case 'r':
	  {
	    printf("Re-init ");
	    bool res = Memshot::getInstance()->init(pid, filename, pattern_offset, pattern_length);
	    printf("%s\n", res?"Ok":"Nok");
	    index = 1;
	    snprintf(quick_save_filename, sizeof(quick_save_filename), "%s_%03d", base, index);
	    help(filename, quick_save_filename);
	  }
	  break;
	case 's':
	  {
	    unsigned num = 1;
	    timespec interval = {0, 0};
	    {
	      char *nptr = cmd + 1;
	      char *endptr = 0;
	      num = strtoul(nptr, &endptr, 0);
	      if(endptr != nptr && *endptr)
		{		  
		  nptr = endptr;
		  endptr = 0;
		  double tmp = strtod(nptr, &endptr);
		  interval.tv_sec = (time_t)tmp;
		  interval.tv_nsec = (long)(((double)tmp - interval.tv_sec) * 1000000000.0);
		}
	      else
		{
		  num = 1;
		}
	    }
	    printf("Will take %d shot every %d.%09d seconds\n", num, interval.tv_sec, interval.tv_nsec);
	    while(num--)
	      {
		snprintf(bin_filename, sizeof(bin_filename), "%s.BIN", quick_save_filename);
		snprintf(xpm_filename, sizeof(xpm_filename), "%s.xpm", quick_save_filename);

		printf("Save to %s and %s\n", bin_filename, xpm_filename);
		bool res = Memshot::getInstance()->save(bin_filename) && Screenshot::getInstance()->save(xpm_filename);
		printf("%s\n", res?"Ok":"Nok");
		if(res)
		  {
		    index++;
		    snprintf(quick_save_filename, sizeof(quick_save_filename), "%s_%03d", base, index);
		  }
		nanosleep(&interval, 0);
	      }
	  }
	  break;
	case 'S':
	  {
	    char *save_filename = strtok(cmd + 1, " \n");
	    printf("Save to %s\n", save_filename?save_filename:filename);
	    bool res = Memshot::getInstance()->save(save_filename);
	    printf("%s\n", res?"Ok":"Nok");
	  }
	  break;
	case 'L':
	  {
	    char *load_filename = strtok(cmd + 1, " \n");
	    printf("Load from %s\n", load_filename?load_filename:filename);
	    bool res = Memshot::getInstance()->load(load_filename);
	    printf("%s\n", res?"Ok":"Nok");
	  }
	  break;
	case 'q':
	  stop = true;
	  break;
	default:
	  help(filename, quick_save_filename);
	  break;
	}
    }
  
  return 0;
}




