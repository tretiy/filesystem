#include <string>
#include <vector>
#include "FileDescriptor.h"
#pragma once
struct IBaseFileSystem
{
	//filesystem operations
	virtual bool createDirectory(std::wstring _directoryPath) = 0;
	virtual bool removeDirectory(std::wstring _directoryPath) = 0;
	virtual std::vector<std::wstring> getDirectoriesList(std::wstring _directoryPath) = 0;
	virtual std::vector<std::wstring> getFilesList(std::wstring _directoryPath) = 0;
	virtual FileDescriptor createFile(std::wstring _fullName) = 0;
	virtual bool removeFile(std::wstring _fullName) = 0;
	virtual bool exists(std::wstring _path) = 0;
	//file operations
	virtual bool OpenFileSystem(std::string pathToFile, bool createNew = false) = 0;
	virtual bool CloseFileSystem() = 0;
};

