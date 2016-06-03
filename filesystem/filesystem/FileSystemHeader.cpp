#include "FileSystemHeader.h"
#include <exception>

namespace filesystem
{
	FileSystemHeader::FileSystemHeader()
		:FileSystemHeader(0, 0, 0)
	{
	}

	FileSystemHeader::FileSystemHeader(size_t _infosCount, size_t _blockSize, size_t _fileSystemSize)
		: infosCount(_infosCount), blockSize(_blockSize), fileSystemSize(_fileSystemSize), dataOffset(0)
	{

	}

	FileSystemHeader::~FileSystemHeader()
	{

	}
	size_t FileSystemHeader::getBlockSize() const
	{
		return blockSize;
	}

	size_t FileSystemHeader::getFileSystemSize() const
	{
		return fileSystemSize;
	}

	void FileSystemHeader::setDataOffset(size_t offset)
	{
		dataOffset = offset;
	}

	size_t FileSystemHeader::getDataOffset()
	{
		return dataOffset;
	}
	bool FileSystemHeader::isValid()
	{
		return dataOffset > 0 && infosCount > 0 && blockSize > 0 && fileSystemSize > 0;
	}
}