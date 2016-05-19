#pragma once
#include <fstream>
#include "boost\serialization\access.hpp"
class supernode
{

	static const int LENGTH = 12;		// size of serialized supernode
	

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & infosCount;
		ar & blockSize;
		ar & fileSystemSize;
	}
public:
	unsigned int infosCount;			// Total number of inodes
	unsigned int blockSize;				// Block size
	unsigned int fileSystemSize;		// File Sytem size in blocks

	supernode();
	supernode&& LoadFromStream(std::fstream& file);
	supernode(supernode&& node) = default;
	bool SaveToStream(std::fstream& file);
	~supernode();

};

