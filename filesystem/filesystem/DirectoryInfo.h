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
		unsigned int infoIdx;   //	Inode number
		std::string  name;		//	Directory name

		DirectoryInfo();
		~DirectoryInfo();
	};
}
