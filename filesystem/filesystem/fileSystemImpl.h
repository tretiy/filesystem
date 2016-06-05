#pragma once
#include "IBaseFileSystem.h"
#include "fstypes.h"
#include "FileSystemHeader.h"
#include "InfoDescriptor.h"
#include "EntryData.h"
#include "EntryBlocks.h"

#include <fstream>
#include <mutex>
#include "boost\serialization\vector.hpp"
using namespace filesystem;

class fileSystemImpl :public IBaseFileSystem
{
	static const size_t MAX_ENTRYNAME_LENGTH = 255;
	static const size_t MAX_FS_SIZE = 1024 * 1024 * 1024;
	std::fstream fileSystemSource;
	FileSystemHeader mainSector;
	std::vector<bool> dataBlocks;
	std::vector<bool> infoBlocks;
	std::vector<InfoDescriptor> infos;

	std::recursive_mutex lockObj;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & mainSector;
		ar & dataBlocks;
		ar & infoBlocks;
		ar & infos;
	}

	std::streampos getBlockOffset(size_t _blockNum);
	std::streampos getFileOffset(FileDescriptor& _file);
	size_t getLengthToWrite(FileDescriptor& _file);

	size_t getEntryBlockIdx(InfoDescriptor& _entryDesc, size_t _blockNumInFile);
	EntryBlocks getEntryInderectBlocks(InfoDescriptor& _entryDesc);
	bool setEntryInderectBlocks(InfoDescriptor& _entryDesc, EntryBlocks& _inderectBlocks);
	bool addEntryDataBlock(InfoDescriptor& _entry);

	size_t getFreeBlockIdx();
	size_t getFreeInfoIdx();
	InfoDescriptor getPathDescriptor(const std::wstring& _path);
	size_t getPathDescriptorIdx(const std::wstring& _path);
	InfoDescriptor getChildDescriptor(InfoDescriptor& _parentDir, const std::wstring& _childName);
	size_t getChildDescriptorIdx(InfoDescriptor& _parentDir, const std::wstring& _childName);
	bool removeDescriptorIdx(size_t _descIdx);
	bool getDirectoryContent(InfoDescriptor& _desc, std::vector<EntryData>& _content);
	bool setDirectoryContent(InfoDescriptor& _desc, std::vector<EntryData>& _content);
	std::vector<std::wstring> getDirectoryContentList(const std::wstring& _directoryPath,
		EntryType _type = EntryType::NotSet);
	bool createEntry(const std::wstring& _entryPath, EntryType _type);
	bool removeEntry(const std::wstring& _entryPath);
	bool checkEntryName(const std::wstring& _entryName);

public:
	fileSystemImpl();
	virtual ~fileSystemImpl();

	// Inherited via IBaseFileSystem
	// directories operations
	virtual bool createDirectory(const std::wstring& _directoryPath) override;
	virtual bool removeDirectory(const std::wstring& _directoryPath) override;
	virtual std::vector<std::wstring> getDirectoriesList(const std::wstring& _directoryPath) override;
	virtual bool exists(const std::wstring& _path) override;
	// files operations
	virtual std::vector<std::wstring> getFilesList(const std::wstring& _directoryPath) override;
	virtual bool createFile(const std::wstring& _filePath) override;
	virtual bool removeFile(const std::wstring& _filePath) override;
	virtual FileDescriptor openFile(const std::wstring& _pathToFile, bool _seekToBegin = true) override;
	virtual size_t writeToFile(FileDescriptor& _file, const char* _data, size_t _count) override;
	virtual size_t readFromFile(FileDescriptor& _file, char* _data, size_t _count) override;

	virtual bool renameEntry(const std::wstring& _entryPath, const std::wstring& _newName) override;

	virtual bool openFileSystem(const std::wstring& _pathToFile, bool _createNew = false) override;
	virtual bool closeFileSystem() override;

	virtual bool createFileSystem(const std::wstring& _pathToFile, size_t _blockSize, size_t _blocksCount) override;
	bool initializeFileSystem();
};

