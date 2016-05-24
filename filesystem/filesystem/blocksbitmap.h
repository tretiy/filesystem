#pragma once
#include <vector>
#include <fstream>
#include "boost\serialization\access.hpp"
#include "boost\serialization\vector.hpp"

class blocksbitmap
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & blocksState;
	}
public:
	std::vector<bool> blocksState;
	blocksbitmap();
	~blocksbitmap();

	void LoadFromStream(std::fstream& file);
	bool SaveToStream(std::fstream& file);
};

