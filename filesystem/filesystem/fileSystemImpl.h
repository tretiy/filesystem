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
	static const size_t MAX_ENTRYNAME_LENGTH = 255;
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

	std::streampos getBlockOffset(size_t _blockNum);
	size_t getFreeBlockIdx();
	size_t getFreeInfoIdx();
	InfoDescriptor getPathDescriptor(std::wstring _path);
	size_t getPathDescriptorIdx(std::wstring _path);
	InfoDescriptor getChildDescriptor(InfoDescriptor _parentDir, std::wstring _childName);
	size_t getChildDescriptorIdx(InfoDescriptor _parentDir, std::wstring _childName);
	bool removeDescriptorIdx(size_t _descIdx);
	bool getDirectoryContent(InfoDescriptor _desc, std::vector<EntryData>& _content);
	bool setDirectoryContent(InfoDescriptor _desc, std::vector<EntryData>& _content);
	std::vector<std::wstring> fileSystemImpl::getDirectoryContentList(std::wstring _directoryPath,
		EntryType _type = EntryType::NotSet);
	bool createEntry(std::wstring _entryPath, EntryType _type);
	bool removeEntry(std::wstring _entryPath);
	bool checkEntryName(std::wstring _entryName);

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
	virtual bool createFile(std::wstring _filePath) override;
	virtual bool removeFile(std::wstring _filePath) override;


	virtual bool openFileSystem(std::string _pathToFile, bool _createNew = false) override;
	virtual bool closeFileSystem() override;

	bool createFileSystem(std::string _pathToFile, size_t _blockSize, size_t _blocksCount);
	bool initializeFileSystem();
};

