#include "fileSystemImpl.h"
#include <fstream>
#include <string>



fileSystemImpl::fileSystemImpl()
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

bool fileSystemImpl::OpenFileSystem(std::string pathToFile)
{
	if (pathToFile.empty())
		return false;

	fileSystemSource.open(pathToFile, std::ios::binary || std::ios::in || std::ios::app);
	auto retVal = fileSystemSource.is_open();
	return retVal;
}

bool fileSystemImpl::CloseFileSystem()
{
	if (fileSystemSource.is_open())
	{
		fileSystemSource.close();
		return true;
	}
	return true;
}

bool fileSystemImpl::InitializeFileSystem()
{
	if (!fileSystemSource.is_open())
		return false;

	fileSystemSource.seekg(0, std::ios::beg);

	return false;
}
