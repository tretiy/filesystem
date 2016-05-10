#include <string>
#include <vector>
#include "FileDescriptor.h"
#pragma once
class IBaseFileSystem
{
public:
	IBaseFileSystem();
	virtual ~IBaseFileSystem();

	//filesystem operations
	virtual bool createDirectory(std::string _directoryPath);
	virtual bool removeDirectory(std::string _directoryPath, bool _isRecursive = false);
	virtual std::vector<std::string> getDirectoriesList(std::string _directoryPath);
	virtual std::vector<std::string> getFilesList(std::string _directoryPath);
	virtual FileDescriptor createFile(std::string _fullName);
	virtual bool removeFile(std::string _fullName);
	virtual bool exists(std::string _path);
	//file operations
	
};

