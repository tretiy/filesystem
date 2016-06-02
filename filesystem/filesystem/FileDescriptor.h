#pragma once
struct FileDescriptor
{
	size_t infoDescIdx;
	size_t seekPos;

	bool isValid()
	{
		return infoDescIdx > 0;
	}
};