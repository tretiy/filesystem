#pragma once
#include "boost\serialization\access.hpp"
namespace filesystem
{
	class FileSystemHeader
	{
		size_t dataOffset;			// Start offset of data 
		size_t infosCount;			// Total number of info descriptors
		size_t blockSize;			// Block size
		size_t fileSystemSize;		// File Sytem size in blocks

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & dataOffset;
			ar & infosCount;
			ar & blockSize;
			ar & fileSystemSize;
		}
	public:
		FileSystemHeader();
		FileSystemHeader(size_t _infosCount, size_t _blockSize, size_t _fileSystemSize);
		FileSystemHeader(FileSystemHeader&& node) = default;
		FileSystemHeader& operator=(const FileSystemHeader& newOne) = default;
		friend bool operator==(const FileSystemHeader& left, const FileSystemHeader& right);
		size_t getBlockSize() const;
		size_t getFileSystemSize() const;
		void setDataOffset(size_t offset);
		size_t getDataOffset();
		~FileSystemHeader();
	};
}
