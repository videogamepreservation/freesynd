#include <list>

#include "Syndicate/Data/Debug.h"
#include "Syndicate/Data/HuffmanTable.h"

using namespace Syndicate::Data;

HuffmanTable::HuffmanTable(Bitstream *bs)
{
  _root = 0;
  _bs = bs;

  Assert( (bs != 0) );

  unsigned max_val = _bs->get(5);
  Debug("max_val = %d\n", max_val);

  int max_depth = 1;
  int *leaf_depth = new int [max_val];
  for (unsigned val = 0; val < max_val; val++)
    {
      leaf_depth[val] = _bs->get(4);
      Debug("\tleafdepth[%d] %d\n", val, leaf_depth[val]);
      if (max_depth < leaf_depth[val])
	{
	  max_depth = leaf_depth[val];
	}
    }

  std::list<Node *> *old_nodes = 0;
  for(int depth = max_depth; depth >= 0; depth--)
    {
      std::list<Node *> *nodes = new std::list<Node *>;

      if(depth != 0)
	{
	  for (unsigned val = 0; val < max_val; val++)
	    {
	      if(leaf_depth[val] == depth)
		{
		  nodes->push_back(new Node(val));
		}
	    }
	}

      if(old_nodes)
	{
	  while(old_nodes->size() != 0)
	    {
	      Node *left = 0;
	      if(old_nodes->size() != 0)
		{
		  left = old_nodes->front();
		  old_nodes->pop_front();
		}
	      Node *right = 0;
	      if(old_nodes->size() != 0)
		{
		  right = old_nodes->front();
		  old_nodes->pop_front();
		}
	      nodes->push_back(new Node(left, right)); 
	    }	
	  delete old_nodes;
	}

      old_nodes = nodes;
    }
  delete[] leaf_depth;
  
  Assert( (old_nodes && (old_nodes->size() == 1)) );

  if(old_nodes && old_nodes->size() == 1)
    {
      _root = old_nodes->front();
      old_nodes->pop_front();
      delete old_nodes;
    }

  Assert( (_root != 0) );
}

HuffmanTable::~HuffmanTable(void)
{
  if(_root)
    {
      delete _root;
    }
}

unsigned HuffmanTable::getValue(Bitstream *bs)
{
  if(bs)
    {
      _bs = bs;
    }

  Node *node = _root;
  while(node->_left || node->_right)
    {
      unsigned bit = _bs->get(1);
      node = (bit == 0)?node->_left:node->_right;
      if(!node)
	{
	  return -1;
	}
    } 

  unsigned val = 0;
  val = node->_val;
  if (val >= 2)
    {
      unsigned mask = (1 << (val - 1)) - 1;
      val = (1 << (val - 1)) | (_bs->get(node->_val - 1) & mask);
    }

  return val;
}
