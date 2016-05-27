#include "fileSystemImpl.h"
#include <fstream>
#include "portable_serialization\portable_binary_iarchive.hpp"
#include "portable_serialization\portable_binary_oarchive.hpp"
#include <experimental\filesystem>
#include <exception>

namespace fs = std::experimental::filesystem;

fileSystemImpl::fileSystemImpl()
	:infos(512, infoDescriptor())
{
}

fileSystemImpl::~fileSystemImpl()
{
}

bool fileSystemImpl::createDirectory(std::string _directoryPath)
{
	return false;
}

bool fileSystemImpl::removeDirectory(std::string _directoryPath, bool _isRecursive/* = false*/)
{
	return false;
}

std::vector<std::string> fileSystemImpl::getDirectoriesList(std::string _directoryPath)
{
	return std::vector<std::string>();
}

std::vector<std::string> fileSystemImpl::getFilesList(std::string _directoryPath)
{
	return std::vector<std::string>();
}

FileDescriptor fileSystemImpl::createFile(std::string _fullName)
{
	return FileDescriptor();
}

bool fileSystemImpl::removeFile(std::string _fullName)
{
	return false;
}

bool fileSystemImpl::exists(std::string _path)
{
	return false;
}

bool fileSystemImpl::OpenFileSystem(std::string pathToFile, bool createNew/* = false*/)
{
	if (pathToFile.empty())
		return false;
	auto openMode = std::ios::binary | std::ios::in | std::ios::out;
	if (createNew)
		openMode |= std::ios::trunc;
	fileSystemSource.open(pathToFile, openMode);
	auto retVal = fileSystemSource.is_open();
	return retVal;
}

bool fileSystemImpl::CloseFileSystem()
{
	if (fileSystemSource.is_open())
	{
		fileSystemSource.seekg(0, std::ios::beg);
		try
		{
			portable_binary_oarchive oa(fileSystemSource, boost::archive::archive_flags::no_header);
			oa << *this;
		}
		catch (const std::exception&)
		{
			return false;
		}
		fileSystemSource.close();
		return true;
	}
	return false;
}

bool fileSystemImpl::CreateFileSystem(std::string pathToFile, unsigned int blockSize, unsigned int blocksCount)
{
	if (fileSystemSource.is_open())
	{
		return false;
	}
	if (OpenFileSystem(pathToFile, true))
	{
		//create a file system with default scheme
		mainSector = FileSystemHeader(512, blockSize, blocksCount);
		//create a root directory
		infos[0].lastAccess = system_clock::now();
		infos[0].blocksNum = 1;
		infos[0].firstBlock = 0;
		infos[0].fileType = infoDescriptor::FileType::Directory;
		infoBlocks.insert(infoBlocks.begin(), 512, false);
		dataBlocks.insert(dataBlocks.begin(), mainSector.getBlockSize(), false);
		infoBlocks[0] = true;
		dataBlocks[0] = true;

		fileSystemSource.seekg(0, std::ios::beg);
		try
		{
			portable_binary_oarchive oa(fileSystemSource, boost::archive::archive_flags::no_header);
			oa << *this;
			char* buf = new char[mainSector.getBlockSize()];
			memset(&buf[0], 'd', mainSector.getBlockSize());
			for(decltype(mainSector.getFileSystemSize()) i = 0; i < mainSector.getFileSystemSize(); ++i)
			{
				fileSystemSource.write(&buf[0], mainSector.getBlockSize());
			}
		}
		catch (const std::exception&)
		{
			return false;
		}
		return true;
	}
	return false;
}

bool fileSystemImpl::InitializeFileSystem()
{
	if (!fileSystemSource.is_open())
		return false;

	fileSystemSource.seekg(0, std::ios::beg);
	portable_binary_iarchive ia(fileSystemSource, boost::archive::archive_flags::no_header);
	ia >> *this;

	return true;
}
