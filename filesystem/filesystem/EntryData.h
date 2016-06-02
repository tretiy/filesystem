#pragma once
#include "fstypes.h"
#include <string>
#include "boost\serialization\access.hpp"

namespace filesystem
{
	class EntryData
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & infoIdx;
			ar & type;
			ar & name;
		}
	public:
		size_t infoIdx;			//Inode number
		EntryType type;			//Entry type (directory or file)
		std::wstring  name;		//Entry name

		EntryData() = default;
		~EntryData() = default;
	};
}
