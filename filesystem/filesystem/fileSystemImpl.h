#pragma once
#include "IBaseFileSystem.h"
#include "FileSystemHeader.h"
#include "groupDescriptor.h"
#include "infoDescriptor.h"
#include <fstream>
#include "boost\serialization\vector.hpp"
using namespace filesystem;
class fileSystemImpl :public IBaseFileSystem
{
	std::fstream fileSystemSource;
	FileSystemHeader mainSector;
	groupDescriptor groupDesc;
	std::vector<bool> dataBlocks;
	std::vector<bool> infoBlocks;
	std::vector<infoDescriptor> infos;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & mainSector;
		ar & groupDesc;
		ar & dataBlocks;
		ar & infoBlocks;
		ar & infos;
	}
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
	virtual bool OpenFileSystem(std::string pathToFile, bool createNew = false) override;
	virtual bool CloseFileSystem() override;

	bool CreateFileSystem(std::string pathToFile, unsigned int blockSize, unsigned int blocksCount);
	bool InitializeFileSystem();
};

