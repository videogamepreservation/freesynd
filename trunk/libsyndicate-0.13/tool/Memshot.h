#ifndef __Memshot_h__
#define __Memshot_h__

#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>

class Memshot
{
 public:

  static Memshot *getInstance(void)
    {
      static Memshot instance;
      return &instance;
    }

  /** Init the file and pattern */
  bool init(int pid, const char *filename, unsigned pattern_offset, unsigned pattern_length);

  /** From mem to file 
   * If no filename is given, the mem will be dump to the original file.
   * incremented index in its name
   */
  bool save(const char *filename = 0);

  /** From file to mem 
   * If no filename is given, the original file will be dump to the mem.
   */
  bool load(const char *filename = 0);

  bool detach(void);

 private:

  Memshot(void);

  ~Memshot(void);

  bool attach(void);

  bool reset(void);

  /** The pid of the child process */
  pid_t _pid;
  /** Says if we are attached to child */
  bool _attached;

  /** The file used to match the pattern and to load/save data */
  char _filename[PATH_MAX];
  /** The offset of the pattern in bytes */
  off_t _pattern_offset;
  /** The size of the pattern in bytes */
  size_t _pattern_length;

  /** The file descriptor to the memory */
  int _mem_fd;
  /** The offset of the buffer in memory */
  off_t _buffer_offset;
  /** The size of the buffer */
  size_t _buffer_length;

  bool _initialized;

  void (*_sigIntHdl)(int);
  void (*_sigQuitHdl)(int);
  void (*_sigHupHdl)(int);
  void (*_sigPipeHdl)(int);
  void (*_sigTermHdl)(int);
  void (*_sigSegvHdl)(int);
  void (*_sigBusHdl)(int);
  void (*_sigIllHdl)(int);
};

#endif /* __Memshot_h__ */

// Local Variables:
// mode: C++
// End:

