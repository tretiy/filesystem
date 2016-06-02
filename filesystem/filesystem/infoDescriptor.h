#pragma once
#include <chrono>
#include "boost\serialization\access.hpp"
#include "boost\serialization\binary_object.hpp"

using namespace std::chrono;
namespace filesystem
{
	class InfoDescriptor
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & entryType;
			ar & flength;
			ar & boost::serialization::make_binary_object(&lastAccess, sizeof(lastAccess));
			ar & blocksNum;
			ar & firstBlock;
		}

	public:
		EntryType entryType = { EntryType::NotSet };					//Entry type
		short owner = { 0 };											//Owner identification
		size_t flength = { 0 };											//File length in bytes
		system_clock::time_point  lastAccess = system_clock::now();		//Time of last file access
		size_t blocksNum = { 0 };										//Number of data blocks
		size_t firstBlock = { 0 };										//First data block 
		/*14 more pointers to data blocks*/
		void updateLastAccess()
		{
			lastAccess = system_clock::now();
		};
		bool isValid()
		{
			return blocksNum > 0 && entryType != EntryType::NotSet;
		}
	};
}
