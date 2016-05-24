#include "fileSystemImpl.h"
#include <fstream>
#include "portable_serialization\portable_binary_iarchive.hpp"
#include "portable_serialization\portable_binary_oarchive.hpp"
#include <experimental\filesystem>
#include <exception>


fileSystemImpl::fileSystemImpl()
{
	groups.reserve(512);
	infos.reserve(512);
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

bool fileSystemImpl::OpenFileSystem(std::string pathToFile)
{
	if (pathToFile.empty())
		return false;

	fileSystemSource.open(pathToFile, std::ios::binary | std::ios::in | std::ios::out);
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
			auto pos = fileSystemSource.tellg();
			portable_binary_oarchive oa(fileSystemSource);
			oa << *this;
			pos = fileSystemSource.tellg();
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

bool fileSystemImpl::InitializeFileSystem()
{
	if (!fileSystemSource.is_open())
		return false;

	fileSystemSource.seekg(0, std::ios::beg);
	portable_binary_iarchive ia(fileSystemSource);
	ia >> *this;

	return true;
}
