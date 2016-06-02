#pragma once
#include <vector>
#include "boost\serialization\access.hpp"
#include "boost\serialization\vector.hpp"

namespace filesystem
{
	class EntryBlocks
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & blocks;
		}
	public:
		std::vector<size_t> blocks;
	};
}
