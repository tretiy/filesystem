#pragma once
struct FileDescriptor
{
	size_t infoDescIdx;
	size_t seekPos;
	size_t fileLength;
	bool isValid() 
	{
		return infoDescIdx > 0 && seekPos <= fileLength;
	}
};