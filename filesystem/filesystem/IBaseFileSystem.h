#include <string>
#include <vector>
#include "FileDescriptor.h"
#pragma once
struct IBaseFileSystem
{
	//filesystem operations
	virtual bool createDirectory(const std::wstring& _directoryPath) = 0;
	virtual bool removeDirectory(const std::wstring& _directoryPath) = 0;
	virtual std::vector<std::wstring> getDirectoriesList(const std::wstring& _directoryPath) = 0;
	virtual std::vector<std::wstring> getFilesList(const std::wstring& _directoryPath) = 0;
	virtual bool createFile(const std::wstring& _filePath) = 0;
	virtual bool removeFile(const std::wstring& _filePath) = 0;
	virtual bool exists(const std::wstring& _path) = 0;
	virtual FileDescriptor openFile(const std::wstring& _pathToFile, bool _seekToBegin = true) = 0;
	virtual size_t writeToFile(FileDescriptor& _file, const char* _data, size_t _count) = 0;
	virtual size_t readFromFile(FileDescriptor& _file, char* _data, size_t _count) = 0;
	//file operations
	virtual bool openFileSystem(const std::wstring& _pathToFile, bool _createNew = false) = 0;
	virtual bool closeFileSystem() = 0;
};

