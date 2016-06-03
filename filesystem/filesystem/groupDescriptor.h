#pragma once
#include <stddef.h>
#include <string>
#include "boost\serialization\access.hpp"

namespace filesystem
{
	class groupDescriptor
	{
		int infoNumber;			 // Inode number
		size_t entrySize;		 // This directory entry's length
		size_t entryNameLength;  // File name length
		bool isDirectory;		 // true is directory
		std::string entryName;	 // file or directory name
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & infoNumber;
			ar & entrySize;
			ar & entryNameLength;
			ar & isDirectory;
			ar & entryName;
		}
	};
}

