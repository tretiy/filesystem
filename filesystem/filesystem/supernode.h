#pragma once
#include <fstream>
#include "boost\serialization\access.hpp"
class supernode
{

	static const int LENGTH = 12;		// size of serialized supernode
	unsigned int infosCount;			// Total number of inodes
	unsigned int blockSize;				// Block size
	unsigned int fileSystemSize;		// File Sytem size in blocks

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & infosCount;
		ar & blockSize;
		ar & fileSystemSize;
	}
public:
	supernode();
	supernode(unsigned int _infosCount, unsigned int _blockSize, unsigned int _fileSystemSize);
	void LoadFromStream(std::fstream& file);
	supernode(supernode&& node) = default;
	friend bool operator==(const supernode& left, const supernode& right);
	bool SaveToStream(std::fstream& file);
	~supernode();

};

