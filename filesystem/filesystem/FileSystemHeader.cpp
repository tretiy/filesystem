#include "FileSystemHeader.h"
#include "portable_serialization\portable_binary_iarchive.hpp"
#include "portable_serialization\portable_binary_oarchive.hpp"

#include <exception>

namespace filesystem
{
	FileSystemHeader::FileSystemHeader()
		:FileSystemHeader(256, 512, 1024)
	{
	}

	FileSystemHeader::FileSystemHeader(unsigned int _infosCount, unsigned int _blockSize, unsigned int _fileSystemSize)
		: infosCount(_infosCount), blockSize(_blockSize), fileSystemSize(_fileSystemSize)
	{
	}

	FileSystemHeader::~FileSystemHeader()
	{
	}

	void FileSystemHeader::LoadFromStream(std::fstream & file)
	{
		if (!file.is_open())
			throw std::invalid_argument("stream without file");

		portable_binary_iarchive ia(file, boost::archive::archive_flags::no_header);
		ia >> *this;
	}

	bool FileSystemHeader::SaveToStream(std::fstream & file)
	{
		if (!file.is_open())
			throw std::invalid_argument("stream without file");
		try
		{
			portable_binary_oarchive oa(file, boost::archive::archive_flags::no_header);
			oa << *this;
		}
		catch (const std::exception&)
		{
			return false;
		}
		return true;
	}

	unsigned int FileSystemHeader::getBlockSize() const
	{
		return blockSize;
	}

	unsigned int FileSystemHeader::getFileSystemSize() const
	{
		return fileSystemSize;
	}

	bool operator==(const FileSystemHeader& left, const FileSystemHeader& right)
	{
		return left.blockSize == right.blockSize &&
			left.infosCount == right.infosCount &&
			left.fileSystemSize == right.fileSystemSize;
	}
}