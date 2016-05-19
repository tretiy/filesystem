#pragma once
#include <stddef.h>
#include <string>
class groupDescriptor
{
	int infoNumber;			// Inode number
	size_t entrySize;		// This directory entry's length
	size_t fileNameLength;  // File name length
	char entryType;			// (1 = regular file 2 = directory)
	std::string fileName;	// file or directory name

public:
	groupDescriptor();
	~groupDescriptor();
};

