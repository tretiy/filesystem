#pragma once
#include "IBaseFileSystem.h"
#include "supernode.h"
#include <fstream>
class fileSystemImpl :public IBaseFileSystem
	
{
	std::ifstream fileSystemSource;
	supernode mainSector;

public:
	fileSystemImpl();
	virtual ~fileSystemImpl();

	// Inherited via IBaseFileSystem
	virtual bool createDirectory(std::string _directoryPath) override;
	virtual bool removeDirectory(std::string _directoryPath, bool _isRecursive = false) override;
	virtual std::vector<std::string> getDirectoriesList(std::string _directoryPath) override;
	virtual std::vector<std::string> getFilesList(std::string _directoryPath) override;
	virtual FileDescriptor createFile(std::string _fullName) override;
	virtual bool removeFile(std::string _fullName) override;
	virtual bool exists(std::string _path) override;
	virtual bool OpenFileSystem(std::string pathToFile) override;
	virtual bool CloseFileSystem() override;

	bool InitializeFileSystem();
};

