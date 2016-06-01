#pragma once
#include <string>
#include "boost\serialization\access.hpp"
namespace filesystem
{
	class DirectoryInfo
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & infoIdx;
			ar & name;
		}
	public:
		size_t infoIdx;   //	Inode number
		std::wstring  name;		//	Directory name

		DirectoryInfo() = default;
		~DirectoryInfo() = default;
	};
}
