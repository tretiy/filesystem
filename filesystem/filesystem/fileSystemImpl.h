#pragma once
#include "IBaseFileSystem.h"
#include "fstypes.h"
#include "FileSystemHeader.h"
#include "groupDescriptor.h"
#include "infoDescriptor.h"
#include "EntryData.h"
#include <fstream>
#include "boost\serialization\vector.hpp"
using namespace filesystem;

class fileSystemImpl :public IBaseFileSystem
{
	std::fstream fileSystemSource;
	FileSystemHeader mainSector;
	//groupDescriptor groupDesc;
	std::vector<bool> dataBlocks;
	std::vector<bool> infoBlocks;
	std::vector<InfoDescriptor> infos;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & mainSector;
		//ar & groupDesc;
		ar & dataBlocks;
		ar & infoBlocks;
		ar & infos;
	}

	std::streampos GetBlockOffset(size_t blockNum);
	size_t getFreeBlockIdx();
	size_t getFreeInfoIdx();
	InfoDescriptor getPathDescriptor(std::wstring _path);
	size_t getPathDescriptorIdx(std::wstring _path);
	InfoDescriptor getDescriptor(InfoDescriptor _parentDir, std::wstring _name);
	size_t getDescriptorIdx(InfoDescriptor _parentDir, std::wstring _name);
	bool removeDescriptorIdx(size_t _descIdx);
	bool getDirectoryContent(InfoDescriptor _desc, std::vector<EntryData>& _content);
	bool setDirectoryContent(InfoDescriptor _desc, std::vector<EntryData>& _content);
	std::vector<std::wstring> fileSystemImpl::getDirectoryContentList(std::wstring _directoryPath,
		EntryType _type = EntryType::NotSet);

public:
	fileSystemImpl();
	virtual ~fileSystemImpl();

	// Inherited via IBaseFileSystem
	// directories operations
	virtual bool createDirectory(std::wstring _directoryPath) override;
	virtual bool removeDirectory(std::wstring _directoryPath) override;
	virtual std::vector<std::wstring> getDirectoriesList(std::wstring _directoryPath) override;
	virtual bool exists(std::wstring _path) override;

	virtual std::vector<std::wstring> getFilesList(std::wstring _directoryPath) override;
	virtual FileDescriptor createFile(std::wstring _fullName) override;
	virtual bool removeFile(std::wstring _fullName) override;


	virtual bool OpenFileSystem(std::string pathToFile, bool createNew = false) override;
	virtual bool CloseFileSystem() override;

	bool CreateFileSystem(std::string pathToFile, unsigned int blockSize, unsigned int blocksCount);
	bool InitializeFileSystem();
	
	
};

