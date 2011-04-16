#include <stdio.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/mman.h>
#include <errno.h>

#include "Memshot.h"

static void sigHandler(int x) 
{
  printf("Received sig %d\n", x);
  Memshot::getInstance()->detach();
  printf("Rethrow sig %d\n", x);
  kill(getpid(), x);
}

Memshot::Memshot(void) :
  _pid(0),
  _attached(false),
  _pattern_offset(0),
  _pattern_length(0),
  _mem_fd(-1),
  _buffer_offset(0),
  _buffer_length(0),
  _initialized(0),
  _sigIntHdl(0),
  _sigQuitHdl(0),
  _sigHupHdl(0),
  _sigPipeHdl(0),
  _sigTermHdl(0),
  _sigSegvHdl(0),
  _sigBusHdl(0),
  _sigIllHdl(0)
{
  memset(_filename, 0, sizeof(_filename));

  printf("Memshot hello !!\n");
}

Memshot::~Memshot(void)
{
  detach();

  if(_mem_fd != -1)
    {
      close(_mem_fd);
    }

  printf("Memshot bye !!\n");
}

bool Memshot::reset(void)
{
  detach();

  _pid = 0;

  memset(_filename, 0, sizeof(_filename));

  _pattern_offset = 0;
  
  _pattern_length = 0;

  if(_mem_fd != -1)
    {
      close(_mem_fd);
    }

  _buffer_offset = 0;

  _buffer_length = 0;

  _initialized = false;

  _sigIntHdl = 0;
  _sigQuitHdl = 0;
  _sigHupHdl = 0;
  _sigPipeHdl = 0;
  _sigTermHdl = 0;
  _sigSegvHdl = 0;
  _sigBusHdl = 0;
  _sigIllHdl = 0;

  return true;
}

bool Memshot::attach(void)
{
  if(!_attached)
    {
      if (ptrace(PTRACE_ATTACH, _pid, 0, 0)) 
	{
	  fprintf(stderr, "Ptrace attach to %d failed (%s).\n", _pid, strerror(errno));
	  return false;
	}

      int st = 0;
      if (wait(&st) <= 0 || !WIFSTOPPED(st)) 
	{
	  if (errno == ECHILD && !kill(_pid, 0)) 
	    {
	      fprintf(stderr, "Process %d gone during attach (%s).\n", _pid, strerror(errno));
	      return false;
	    }
	}

      _sigIntHdl  = signal(SIGINT,  sigHandler);
      _sigQuitHdl = signal(SIGQUIT, sigHandler);
      _sigHupHdl  = signal(SIGHUP,  sigHandler);
      _sigPipeHdl = signal(SIGPIPE, sigHandler);
      _sigTermHdl = signal(SIGTERM, sigHandler);
      _sigSegvHdl = signal(SIGSEGV, sigHandler);
      _sigBusHdl  = signal(SIGBUS,  sigHandler);
      _sigIllHdl  = signal(SIGILL,  sigHandler);

      _attached = true;

      printf("Attached to PID %d.\n", _pid);
    }

  return true;
}
  
bool Memshot::detach(void)
{
  if(_attached)
    {
      ptrace(PTRACE_DETACH, _pid, 0, 0);

      signal(SIGINT,  _sigIntHdl);
      signal(SIGQUIT, _sigQuitHdl);
      signal(SIGHUP,  _sigHupHdl);
      signal(SIGPIPE, _sigPipeHdl);
      signal(SIGTERM, _sigTermHdl);
      signal(SIGSEGV, _sigSegvHdl);
      signal(SIGBUS,  _sigBusHdl);
      signal(SIGILL,  _sigIllHdl);

      _attached = false;

      printf("Detached from PID %d.\n", _pid);
    }
  return true;
}

bool Memshot::init(int pid, const char *filename, unsigned pattern_offset, unsigned pattern_length)
{
  if(!filename)
    {
      fprintf(stderr, "%s : first parameters can't be null!\n", __PRETTY_FUNCTION__);
      return false;      
    }

  reset();

  strncpy(_filename, filename, sizeof(_filename));

  _pattern_offset = pattern_offset;
  _pattern_length = pattern_length;

  _pid = pid;
     
  //printf("_filename = %s\n_pattern_offset = %u\n_pattern_length = %u\n_pid = %d\n", _filename, _pattern_offset, _pattern_length, _pid);
  //printf("_buffer_length = %u\n", _buffer_length);


  // INIT PATTERN
  int pattern_fd = open(_filename, O_RDONLY);
  if(pattern_fd < 0)
    {
      fprintf(stderr, "Error opening %s (%s)\n", _filename, strerror(errno));
      return false;
    }

  // INIT PATTERN DESCRIPTION
  struct stat buffer_stat;
  fstat(pattern_fd, &buffer_stat);
  _buffer_length = buffer_stat.st_size;
  if(_pattern_offset + _pattern_length > _buffer_length)
    {
      fprintf(stderr, "Pattern length not correct\n");
      close(pattern_fd);
      return false;
    }

  uint8_t *buffer = (uint8_t *)mmap(0, _buffer_length, PROT_READ, MAP_SHARED, pattern_fd, 0);
  if(buffer == (uint8_t *)-1)
    {
      fprintf(stderr, "Error mmaping %s (%s)\n", _filename, strerror(errno));
      close(pattern_fd);
      return false;
    }

  // INIT MAP AND MEM FILES
  char mem_filename[PATH_MAX] = {0};
  sprintf(mem_filename, "/proc/%d/mem", pid);
  int _mem_fd = open(mem_filename, O_RDONLY);
  if (_mem_fd < 0)
    {
      fprintf(stderr, "Cannot open %s (%s).\n", mem_filename, strerror(errno)); 
      munmap(buffer, _buffer_length);
      close(pattern_fd);
      return 0;
    }

  char map_filename[PATH_MAX] = {0};
  sprintf(map_filename, "/proc/%d/maps", pid);
  FILE *map_fp = fopen(map_filename, "r");
  if (!map_fp)
    {
      fprintf(stderr, "Cannot open %s (%s).\n", map_filename, strerror(errno)); 
      close(_mem_fd);
      _mem_fd = -1;
      munmap(buffer, _buffer_length);
      close(pattern_fd);
      return 0;
    }

  // ATTACH TO PROCESS
  if(!attach())
    {
      fprintf(stderr, "Attach failed.\n"); 
      fclose(map_fp);
      close(_mem_fd);
      _mem_fd = -1;
      munmap(buffer, _buffer_length);
      close(pattern_fd);
      return 0;
    }

  // BROWSE THE MAP
  char map_line[512] = {0};
  while (fgets(map_line, sizeof(map_line), map_fp) && !_buffer_offset) 
    {
      unsigned st = 0;
      unsigned en = 0;
      unsigned len = 0;

      // continue if parse error
      if (sscanf(map_line, "%x-%x", &st, &en) != 2) 
	{
	  printf("[!] Parse error in %s : %s", map_filename, map_line);
	  continue;
	}
      len = en - st;

      // continue if it is a map
      if (strchr(map_line, '/')) 
	{
	  printf("[!] Skipping map at 0x%08x (%d bytes).\n", st, len);
	  continue;
	}

      // continue if size is wrong
      if(_buffer_length > len)
	{
	  printf("[!] Skipping mem at 0x%08x (%d bytes).\n", st, len);
	  continue;
	}

      printf("    Processing mem at 0x%08x (%d bytes)\n", st, len);

      // alloc buffer for receiving data
      uint8_t *mem_ptr = new uint8_t[len];

      // read data
      if (lseek(_mem_fd, st, SEEK_SET) == -1 || read(_mem_fd, mem_ptr, len) == -1) 
	{
	  errno = 0;
	  for (unsigned i = 0; i < len / 4 && !errno; i++)
	    {
	      *reinterpret_cast<uint32_t *>(mem_ptr + i * 4) = ptrace(PTRACE_PEEKDATA, pid, st + i * 4, 0);
	    }
	  if(errno)
	    {
	      fprintf(stderr, "Ptrace error peeking data (%s).\n", strerror(errno));
	      delete [] mem_ptr;
	      continue;
	    }
	}

      // look for the data
      for(unsigned i = _pattern_offset; i < (len - (_buffer_length - _pattern_offset)) && !_buffer_offset; i++)
	{
	  if((mem_ptr[i] == buffer[_pattern_offset]) && (memcmp(mem_ptr + i, buffer + _pattern_offset, _pattern_length) == 0))
	    {
	      _buffer_offset = st + i - _pattern_offset;
	    }
	}
      
      // free memory
      delete [] mem_ptr;
    }

  detach();

  fclose(map_fp);

  munmap(buffer, _buffer_length);

  close(pattern_fd);

  if(_buffer_offset)
    {
      printf("Pattern found !\n");
      _initialized = true;
      return true;
    }
  else
    {
      printf("Pattern not found :(\n");
      return false;
    }

}

bool Memshot::save(const char * filename)
{
  if(!_initialized)
    {
      fprintf(stderr, "Not initialized\n");      
      return false;
    }

  filename = filename?filename:_filename;

  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if(fd < 0)
    {
      fprintf(stderr, "Error opening %s (%s)\n", filename, strerror(errno));
      return false;
    }

  printf("Write to %s\n", filename);

  uint8_t *buffer = new uint8_t[_buffer_length];

  attach();
  
  // read data
  if (lseek(_mem_fd, _buffer_offset, SEEK_SET) == -1 || read(_mem_fd, buffer, _buffer_length) == -1) 
    {
      errno = 0;
      for (unsigned i = 0; i < _buffer_length / 4 && !errno; i++)
	{
	  *reinterpret_cast<uint32_t *>(buffer + i * 4) = ptrace(PTRACE_PEEKDATA, _pid, _buffer_offset + i * 4, 0);
	}
      if(errno)
	{
	  fprintf(stderr, "Ptrace error peeking data (%s).\n", strerror(errno));
	}
    }

  detach();

  write(fd, buffer, _buffer_length);

  delete [] buffer;

  close(fd);

  return errno?false:true;
}

bool Memshot::load(const char * filename)
{
  if(!_initialized)
    {
      fprintf(stderr, "Not initialized\n");      
      return false;
    }

  filename = filename?filename:_filename;

  int fd = open(filename, O_RDONLY);
  if(fd < 0)
    {
      fprintf(stderr, "Error opening %s (%s)\n", filename, strerror(errno));
      return false;
    }

  struct stat file_stat;
  fstat(fd, &file_stat);
  if(file_stat.st_size < _buffer_length)
    {
      fprintf(stderr, "Wrong size file %s\n", filename);
      return false;      
    }

  printf("Read from %s\n", filename);

  uint8_t *buffer = (uint8_t *)mmap(0, _buffer_length, PROT_READ, MAP_SHARED, fd, 0);
  if(buffer == (uint8_t *)-1)
    {
      fprintf(stderr, "Error mmaping %s (%s)\n", filename, strerror(errno));
      close(fd);
      return false;
    }

  attach();

  if (lseek(_mem_fd, _buffer_offset, SEEK_SET) == -1 || write(_mem_fd, buffer, _buffer_length) == -1) 
    {
      errno = 0;
      for (unsigned i = 0; i < _buffer_length / 4 && !errno; i++)
	{
	  ptrace(PTRACE_POKEDATA, _pid, _buffer_offset + i * 4, *reinterpret_cast<uint32_t *>(buffer + i * 4));
	}
      if(errno)
	{
	  fprintf(stderr, "Ptrace error peeking data (%s).\n", strerror(errno));
	}
    }

  detach();

  munmap(buffer, _buffer_length);

  close(fd);

  return errno?false:true;
}
