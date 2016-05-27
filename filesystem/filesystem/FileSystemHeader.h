#pragma once
#include <fstream>
#include "boost\serialization\access.hpp"
namespace filesystem
{
	class FileSystemHeader
	{
		//static const unsigned int SIZEONDISK = 128;
		unsigned int infosCount;			// Total number of inodes
		unsigned int blockSize;				// Block size
		unsigned int fileSystemSize;		// File Sytem size in blocks

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & infosCount;
			ar & blockSize;
			ar & fileSystemSize;
		}
	public:
		FileSystemHeader();
		FileSystemHeader(unsigned int _infosCount, unsigned int _blockSize, unsigned int _fileSystemSize);
		void LoadFromStream(std::fstream& file);
		FileSystemHeader(FileSystemHeader&& node) = default;
		FileSystemHeader& operator=(const FileSystemHeader& newOne) = default;
		friend bool operator==(const FileSystemHeader& left, const FileSystemHeader& right);
		bool SaveToStream(std::fstream& file);
		unsigned int getBlockSize() const;
		unsigned int getFileSystemSize() const;
		~FileSystemHeader();
	};
}
