#pragma once
#include <fstream>
#include "boost\serialization\access.hpp"
class infoDescriptor
{
		/*File type and access rights
		Owner identification
		File length in bytes
		Time of last file access
		Time that inode last changed
		Time that file contents last changed
		Time of file deletion
		Group identifier
		Hard links counter
		Number of data blocks of the file
		File flags
		Specific operating system information
		Pointer to first data block
		14 more pointers to data blocks*/

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		
	}
	
public:
	infoDescriptor();
	~infoDescriptor();
};

