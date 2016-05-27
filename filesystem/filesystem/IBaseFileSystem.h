#include <string>
#include <vector>
#include "FileDescriptor.h"
#pragma once
struct IBaseFileSystem
{
	//filesystem operations
	virtual bool createDirectory(std::string _directoryPath) = 0;
	virtual bool removeDirectory(std::string _directoryPath, bool _isRecursive = false) = 0;
	virtual std::vector<std::string> getDirectoriesList(std::string _directoryPath) = 0;
	virtual std::vector<std::string> getFilesList(std::string _directoryPath) = 0;
	virtual FileDescriptor createFile(std::string _fullName) = 0;
	virtual bool removeFile(std::string _fullName) = 0;
	virtual bool exists(std::string _path) = 0;
	//file operations
	virtual bool OpenFileSystem(std::string pathToFile, bool createNew = false) = 0;
	virtual bool CloseFileSystem() = 0;
};

