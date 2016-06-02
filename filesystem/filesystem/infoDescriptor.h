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
			ar & block_direct;
			ar & block_indirect;
		}

	public:
		static const short DIRECT_BLOCKS = 5;
		EntryType entryType = { EntryType::NotSet };					//Entry type
		short owner = { 0 };											//Owner identification
		size_t flength = { 0 };											//File length in bytes
		system_clock::time_point  lastAccess = { system_clock::now() };	//Time of last file access
		size_t blocksNum = { 0 };										//Number of data blocks
		size_t block_direct[DIRECT_BLOCKS] = { 0, 0, 0, 0, 0 };			//Direct positioning
		size_t block_indirect = { 0 };									//Data block for inderect positioning

		void updateLastAccess()
		{
			lastAccess = system_clock::now();
		};
		bool isValid()
		{
			return blocksNum > 0 && entryType != EntryType::NotSet;
		}
		void setflength(size_t _length)
		{
			flength = _length;
			updateLastAccess();
		}
	};
}
