#ifndef __Syndicate_Data_File_h__
#define __Syndicate_Data_File_h__

#include <inttypes.h>
#include <climits>

namespace Syndicate
{ 
  namespace Data
  { 
    class File
    { 
    public:

      /** 
       * The constructor initialise the pointer to the 
       * memory where the data will be located.
       * \param[in] data a pointer to the pointer which will hold
       *   the data.
       */
      File(uint8_t **data);

      virtual ~File(void);

      /**
       * In debug mode it display the informations.
       */
      virtual void info(void);

      /**
       * This method load the content (unpacked) of the file
       * at the adress pointed by _data.
       * \param[in] filename the name of the file.
       * \retval true if success.
       * \retval false if fails. If previous file is 
       *                  loaded, the data keep unaffected.
       */
      bool load(char const * filename);

      unsigned getLength(void) {return _data_length;}

    private:
      char      _filename[PATH_MAX];
      uint8_t **_data;

    protected:
      unsigned  _data_length;
    };
  }
}

#endif /* __Syndicate_Data_File_h__ */

// Local Variables:
// mode: C++
// End:
