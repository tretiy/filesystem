#include "FileSystemHeader.h"
#include <exception>

namespace filesystem
{
	FileSystemHeader::FileSystemHeader()
		:FileSystemHeader(256, 512, 1024)
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

	bool operator==(const FileSystemHeader& left, const FileSystemHeader& right)
	{
		return left.blockSize == right.blockSize &&
			left.infosCount == right.infosCount &&
			left.fileSystemSize == right.fileSystemSize;
	}
}