#ifndef __Syndicate_Data_HuffmanTable_h__
#define __Syndicate_Data_HuffmanTable_h__

#include <inttypes.h>

#include "Syndicate/Data/Bitstream.h"

namespace Syndicate
{
  namespace Data
  {
    /**
     * This class implement a huffman tree
     * specific to the RNC bitsream format.
     * This implementation is not realy performant because 
     * we build a "complete" tree, which is easier to 
     * understand when we read the code i think.
     */
    class HuffmanTable
    {
    public:

      /**
       * Instanciate a tree with the given stream.
       * \param[in] bs a pointer to the stream
       */
      HuffmanTable(Bitstream *bs);

      ~HuffmanTable(void);

      /**
       * Give the value of the tree according to the stream.
       * \param[in] bs is a pointer to a stream. If not given take the original stream.
       * \return the value.
       */
      unsigned getValue(Bitstream *bs = 0);

    private:

      /** 
       * This is the base structure of the tree.
       */
      struct Node 
      {
	/**
	 * The constructor for a leaf
	 */
	Node(unsigned val)
	{
	  _val = val;
	  _left = 0;
	  _right = 0;
	}

	/**
	 * The constructor for a node
	 */
	Node(Node *left, Node *right)
	{
	  _val = 0;
	  _left = left;
	  _right = right;
	}

	~Node(void)
	{
	  if(_left)
	    {
	      delete _left;
	    }
	  if(_right)
	    {
	      delete _right;
	    }
	}

	unsigned _val;
	Node *_left;
	Node *_right;
      } ;

      Node *_root;
      Bitstream *_bs;

    };

  }

}

#endif /* __Syndicate_Data_HuffmanTable_h__ */

// Local Variables:
// mode: C++
// End:
