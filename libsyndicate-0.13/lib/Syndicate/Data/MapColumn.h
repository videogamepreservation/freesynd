#ifndef __Syndicate_Data_MapColumn_h__
#define __Syndicate_Data_MapColumn_h__

#include "Syndicate/Data/File.h"
#include "Syndicate/Data/Int.h"

namespace Syndicate
{
  namespace Data
  {
    /**
     * This class allow to access the col01.dat file.
     */
    class MapColumn : public File
    {
    public:

      MapColumn(void) : File(reinterpret_cast<uint8_t **>(&_types)) {}

      ~MapColumn(void) {}

      /**
       * Given the index of the tile, returns his type.
       * \param[in] tile the *index* of the tile
       * \param[out] type the type of tile
       */
      void get(uint8_t tile, uint8_t &type) const;

      enum MapColType
	{
	  None,
	  SlopeSN,
	  SlopeNS,
	  SlopeEW,
	  SlopeWE,
	  Ground,
	  RoadSideEW,
	  RoadSideWE,
	  RoadSideSN,
	  RoadSideNS,
	  Wall,
	  RoadCurve,
	  HandrailLight,
	  Roof,
	  RoadPedCross,
	  RoadMark,
	  NbTypes
	};

    private:

      uint8_t  *_types;

    };
  }
}

#endif /* __Syndicate_Data_MapColumn_h__ */

// Local Variables:
// mode: C++
// End:

