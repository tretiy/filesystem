#pragma once
#include <fstream>
#include <chrono>
#include "boost\serialization\access.hpp"
#include "boost\serialization\binary_object.hpp"

using namespace std::chrono;
namespace filesystem
{
	class infoDescriptor
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & fileType;
			ar & flength;
			ar & boost::serialization::make_binary_object(&lastAccess, sizeof(lastAccess));
			ar & blocksNum;
			ar & firstBlock;
		}

	public:
		short fileType;								//File type 0 for Directory
		short owner;								//Owner identification
		unsigned long flength;						//File length in bytes
		system_clock::time_point  lastAccess;		//Time of last file access
		unsigned long blocksNum;					//Number of data blocks
		unsigned long firstBlock;					//First data block 
		/*14 more pointers to data blocks*/
		enum FileType
		{
			Directory = 0,
			RegularFile
		};
	};
}
