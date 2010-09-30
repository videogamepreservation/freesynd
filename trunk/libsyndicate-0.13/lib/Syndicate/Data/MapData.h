#ifndef __Syndicate_Data_MapData_h__
#define __Syndicate_Data_MapData_h__

#include "Syndicate/Data/File.h"
#include "Syndicate/Data/Int.h"

namespace Syndicate
{
  namespace Data
  {
    /**
     * This class allow to access map[xx].dat files.
     */
    class MapData : public File
    {
    public:
  
      MapData(void) : File(reinterpret_cast<uint8_t **>(&_data)) {}

      ~MapData(void) {}

      unsigned nb_i(void) const; 

      unsigned nb_j(void) const;
    
      unsigned nb_k(void) const;

      void get(unsigned i, unsigned j, unsigned k, unsigned &id) const;

      static const unsigned _max_i = 128;
      static const unsigned _max_j = 128;
      static const unsigned _max_k = 13;

    private:
  
      /**
       * The data array is 
       */
      struct Data 
      {
	le_uint32_t _nb_i;
	le_uint32_t _nb_j;
	le_uint32_t _nb_k;
	le_uint32_t _offsets[/*_nb_i * _nb_j*/];
	/*le_uint8_t  _tile_index[_nb_z * ??];*/
      };
       
      Data    *_data;
 
    };
  }
}

#endif /* __Syndicate_Data_MapData_h__ */

// Local Variables:
// mode: C++
// End:
