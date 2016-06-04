#include "fileSystemImpl.h"
#include "fsserialization.h"
#include <experimental\filesystem>
#include <exception>

namespace fs = std::experimental::filesystem;

fileSystemImpl::fileSystemImpl()
{

}

fileSystemImpl::~fileSystemImpl()
{

}

bool fileSystemImpl::createDirectory(const std::wstring& _directoryPath)
{
	return createEntry(_directoryPath, EntryType::Directory);
}

bool fileSystemImpl::createFile(const std::wstring& _filePath)
{
	return createEntry(_filePath, EntryType::RegularFile);
}

bool fileSystemImpl::removeDirectory(const std::wstring& _directoryPath)
{
	return removeEntry(_directoryPath);
}

std::vector<std::wstring> fileSystemImpl::getDirectoryContentList(const std::wstring& _directoryPath,
	EntryType _type /*= EntryType::NotSet*/)
{
	std::vector<EntryData> content;
	std::vector<std::wstring> contentNames;
	auto directoryDesc = getPathDescriptor(_directoryPath);
	if (directoryDesc.isValid() && getDirectoryContent(directoryDesc, content))
	{
		for (const auto& elem : content)
		{
			if (_type == EntryType::NotSet || infos[elem.infoIdx].entryType == _type)
				contentNames.push_back(elem.name);
		}
	}

	return contentNames;
}

bool fileSystemImpl::createEntry(const std::wstring& _entryPath, EntryType _type)
{
	fs::path newPath(_entryPath);
	//check if directory already exists
	if (_type == EntryType::NotSet || !fileSystemSource.is_open() || !checkEntryName(newPath.filename().native()) || exists(_entryPath))
		return false;

	auto parentDescriptor = getPathDescriptor(newPath.parent_path());
	if (parentDescriptor.isValid() && parentDescriptor.entryType == EntryType::Directory)
	{
		try
		{
			//make new directories objects
			InfoDescriptor entryInfo;
			entryInfo.entryType = _type;
			entryInfo.blocksNum = 1;
			entryInfo.updateLastAccess();
			entryInfo.block_direct[0] = getFreeBlockIdx();

			EntryData newEntry;
			newEntry.type = _type;
			newEntry.name = newPath.filename().native();
			newEntry.infoIdx = getFreeInfoIdx();


			//get parent directory content
			std::vector<EntryData> content;
			getDirectoryContent(parentDescriptor, content);
			content.push_back(newEntry);

			// save dirinfo to stream;
			setDirectoryContent(parentDescriptor, content);
			//change header data
			dataBlocks[entryInfo.block_direct[0]] = true;
			infoBlocks[newEntry.infoIdx] = true;
			infos[newEntry.infoIdx] = entryInfo;

			SaveToStreamT(fileSystemSource, *this, std::streampos(0));
		}
		catch (const std::exception&)
		{
			// get last confirmed fs state
			LoadFromStreamT(fileSystemSource, *this, std::streampos(0));
			return false;
		}
		return true;
	}

	return false;
}

bool fileSystemImpl::removeEntry(const std::wstring& _entryPath)
{
	auto pathDescIdx = getPathDescriptorIdx(_entryPath);
	if (pathDescIdx == infos.size() || pathDescIdx == 0) // do not allow to remove root dir
	{
		return false;
	}

	//now check its directory or file
	if (infos[pathDescIdx].entryType == EntryType::Directory)
	{
		//directory should be empty 
		std::vector<EntryData> content;
		getDirectoryContent(infos[pathDescIdx], content);
		if (!content.empty())
		{
			return false;
		}
	}

	//update parent dir content
	fs::path dirPath(_entryPath);
	auto parentPath = dirPath.parent_path();
	if (dirPath.filename().native() == L".")
		parentPath = parentPath.parent_path();
	auto parentDesc = getPathDescriptor(parentPath);
	std::vector<EntryData> parentContent;
	getDirectoryContent(parentDesc, parentContent);
	auto iter = std::remove_if(parentContent.begin(), parentContent.end(), [&pathDescIdx](auto& elem)
	{
		return elem.infoIdx == pathDescIdx;
	});
	if (iter != parentContent.end())
	{
		parentContent.erase(iter, parentContent.end());
		setDirectoryContent(parentDesc, parentContent);
	}
	//remove dir information
	for (const auto& index : infos[pathDescIdx].block_direct)
	{
		if(index != 0) //root directory data block
			dataBlocks[index] = false;
	}
	if (infos[pathDescIdx].blocksNum > InfoDescriptor::DIRECT_BLOCKS)
	{
		for (const auto& index : getEntryInderectBlocks(infos[pathDescIdx]).blocks)
		{
			dataBlocks[index] = false;
		}
		dataBlocks[infos[pathDescIdx].block_indirect] = false;
	}
	infoBlocks[pathDescIdx] = false;
	infos[pathDescIdx] = InfoDescriptor();
	SaveToStreamT(fileSystemSource, *this, std::streampos(0));
	return true;
}

bool fileSystemImpl::renameEntry(const std::wstring& _entryPath, const std::wstring& _newName)
{
	auto pathDescIdx = getPathDescriptorIdx(_entryPath);
	if (!checkEntryName(_newName) || pathDescIdx == infos.size() || pathDescIdx == 0) // do not allow to rename root dir
	{
		return false;
	}
	//update parent dir content
	fs::path dirPath(_entryPath);
	auto parentPath = dirPath.parent_path();
	auto parentDesc = getPathDescriptor(parentPath);
	std::vector<EntryData> parentContent;
	getDirectoryContent(parentDesc, parentContent);
	auto entryIter = std::find_if(parentContent.begin(), parentContent.end(), [&pathDescIdx](auto& elem)
	{
		return elem.infoIdx == pathDescIdx;
	});
	entryIter->name = _newName;
	setDirectoryContent(parentDesc, parentContent);
	//update entryinformation
	infos[pathDescIdx].updateLastAccess();
	SaveToStreamT(fileSystemSource, *this, std::streampos(0));
	return true;
}

bool fileSystemImpl::checkEntryName(const std::wstring& _entryName)
{
	return !_entryName.empty() && _entryName.length() < MAX_ENTRYNAME_LENGTH;
}

std::vector<std::wstring> fileSystemImpl::getDirectoriesList(const std::wstring& _directoryPath)
{
	return getDirectoryContentList(_directoryPath, EntryType::Directory);
}

std::vector<std::wstring> fileSystemImpl::getFilesList(const std::wstring& _directoryPath)
{
	return getDirectoryContentList(_directoryPath, EntryType::RegularFile);
}

bool fileSystemImpl::removeFile(const std::wstring& _fullName)
{
	return removeEntry(_fullName);
}

bool fileSystemImpl::exists(const std::wstring& _path)
{
	return getPathDescriptor(_path).isValid();
}

bool fileSystemImpl::openFileSystem(const std::wstring& _pathToFile, bool _createNew/* = false*/)
{
	if (_pathToFile.empty())
		return false;

	auto openMode = std::ios::binary | std::ios::in | std::ios::out;
	if (_createNew)
		openMode |= std::ios::trunc;

	fileSystemSource.open(_pathToFile, openMode);

	return fileSystemSource.is_open();
}

bool fileSystemImpl::closeFileSystem()
{
	if (fileSystemSource.is_open())
	{
		try
		{
			if (mainSector.isValid())
			{
				SaveToStreamT(fileSystemSource, *this, std::streampos(0));
			}
			mainSector = FileSystemHeader();
		}
		catch (const std::exception&)
		{
			fileSystemSource.close();
			return false;
		}
		fileSystemSource.close();
		return true;
	}
	return false;
}

FileDescriptor fileSystemImpl::openFile(const std::wstring& _pathToFile, bool _seekToBegin/*=true*/)
{
	auto file = FileDescriptor();
	auto infoIdx = getPathDescriptorIdx(_pathToFile);
	if (infoIdx != infos.size() && infos[infoIdx].entryType == EntryType::RegularFile)
	{
		file.infoDescIdx = infoIdx;
		file.fileLength = infos[infoIdx].flength;
		if (!_seekToBegin)
			file.seekPos = file.fileLength;
	}
	return file;
}

size_t fileSystemImpl::writeToFile(FileDescriptor & _file, const char * _data, size_t _count)
{
	//check descriptor
	if (_file.infoDescIdx < infos.size() && infos[_file.infoDescIdx].entryType == EntryType::RegularFile && _count > 0)
	{
		auto& fileInfoDesc = infos[_file.infoDescIdx];
		//file seek pos should be not greater than current file length
		if (_file.seekPos <= fileInfoDesc.flength)
		{
			decltype(_count) writedCount = { 0 };
			while (writedCount != _count)
			{
				auto seekPosInBlock = getFileOffset(_file);
				if (seekPosInBlock == std::streampos(0))
				{
					//old file is too small to write all data add new data block
					if (addEntryDataBlock(fileInfoDesc))
					{
						continue;
					}
					else
					{
						break;
					}
				}
				auto maxlentowrite = getLengthToWrite(_file);
				auto countToWrite = maxlentowrite <= _count - writedCount ? maxlentowrite : _count - writedCount;
				fileSystemSource.seekp(seekPosInBlock);
				fileSystemSource.write(&_data[writedCount], countToWrite);
				if (!fileSystemSource.good())
				{
					break;
				}
				writedCount += countToWrite;
				_file.seekPos += countToWrite;
			}
			//update file lenght in case we add some data to end of file
			if (_file.seekPos > fileInfoDesc.flength)
			{
				fileInfoDesc.flength = _file.seekPos;
				_file.fileLength = _file.seekPos;
			}
			
			SaveToStreamT(fileSystemSource, *this, std::streampos(0));
			return writedCount;
		}
	}
	return size_t(0);
}

size_t fileSystemImpl::readFromFile(FileDescriptor & _file, char * _data, size_t _count)
{
	if (_file.infoDescIdx < infos.size() && infos[_file.infoDescIdx].entryType == EntryType::RegularFile && _count > 0)
	{
		auto& fileInfoDesc = infos[_file.infoDescIdx];
		if (_file.seekPos <= fileInfoDesc.flength)
		{
			decltype(_count) readedCount = { 0 };
			while (readedCount != _count)
			{
				auto seekPosInBlock = getFileOffset(_file);
				if (seekPosInBlock == std::streampos(0))
				{
					break;
				}
				auto maxlentowrite = getLengthToWrite(_file); //get max avaible data in this block 
				auto countToRead = maxlentowrite <= _count - readedCount ? maxlentowrite : _count - readedCount;
				fileSystemSource.seekp(seekPosInBlock);
				fileSystemSource.read(&_data[readedCount], countToRead);
				if (!fileSystemSource.good())
				{
					break;
				}
				readedCount += countToRead;
				_file.seekPos += countToRead;
			}
			return readedCount;
		}
	}
	return size_t{ 0 };
}

bool fileSystemImpl::createFileSystem(const std::wstring& _pathToFile, size_t _blockSize, size_t _blocksCount)
{
	if (fileSystemSource.is_open() || _blockSize == 0 || _blocksCount == 0 || _blockSize * _blocksCount > MAX_FS_SIZE)
	{
		return false;
	}
	if (openFileSystem(_pathToFile, true))
	{
		//create a file system with default scheme
		mainSector = FileSystemHeader(512, _blockSize, _blocksCount);
		//create a root directory
		infos.insert(infos.begin(), 512, InfoDescriptor());
		infos[0].lastAccess = system_clock::now();
		infos[0].blocksNum = 1;
		infos[0].block_direct[0] = 0;
		infos[0].entryType = EntryType::Directory;
		infoBlocks.insert(infoBlocks.begin(), 512, false);
		dataBlocks.insert(dataBlocks.begin(), mainSector.getFileSystemSize(), false);
		infoBlocks[0] = true;
		dataBlocks[0] = true;

		try
		{
			SaveToStreamT(fileSystemSource, *this, std::streampos(0));
			mainSector.setDataOffset(fileSystemSource.tellp());
			SaveToStreamT(fileSystemSource, mainSector, std::streampos(0));
			
			fileSystemSource.seekp(mainSector.getDataOffset());
			char* buf = new char[mainSector.getBlockSize()];
			memset(&buf[0], 'd', mainSector.getBlockSize());
			for(decltype(mainSector.getFileSystemSize()) i = 0; i < mainSector.getFileSystemSize(); ++i)
			{
				fileSystemSource.write(&buf[0], mainSector.getBlockSize());
			}
		}
		catch (const std::exception&)
		{
			closeFileSystem();
			return false;
		}
		return true;
	}
	return false;
}

bool fileSystemImpl::initializeFileSystem()
{
	if (!fileSystemSource.is_open())
		return false;

	return LoadFromStreamT(fileSystemSource, *this, std::streampos(0));
}

std::streampos fileSystemImpl::getBlockOffset(size_t _blockNum)
{
	if (_blockNum < mainSector.getFileSystemSize())
	{
		return mainSector.getDataOffset() + mainSector.getBlockSize() * _blockNum;
	}
	
	throw std::exception("Block number is not valid");
}

std::streampos fileSystemImpl::getFileOffset(FileDescriptor & _file)
{
	auto posInBlock = _file.seekPos % mainSector.getBlockSize();

	auto blockNumber = _file.seekPos / mainSector.getBlockSize();

	auto blockIdx = getEntryBlockIdx(infos[_file.infoDescIdx], blockNumber);

	if (blockIdx > 0)
	{
		return posInBlock + getBlockOffset(blockIdx);
	}

	return std::streampos(0);
}

size_t fileSystemImpl::getLengthToWrite(FileDescriptor & _file)
{
	auto posInBlock = _file.seekPos % mainSector.getBlockSize();
	return mainSector.getBlockSize() - posInBlock;
}

size_t fileSystemImpl::getEntryBlockIdx(InfoDescriptor & _entryDesc, size_t _blockNumInFile)
{
	if (fileSystemSource.is_open() && _blockNumInFile < _entryDesc.blocksNum)
	{
		if (_blockNumInFile < InfoDescriptor::DIRECT_BLOCKS)
		{
			return _entryDesc.block_direct[_blockNumInFile];
		}
		else
		{
			auto indirectBlocks = getEntryInderectBlocks(_entryDesc);
			const auto indirectIdx = _blockNumInFile - InfoDescriptor::DIRECT_BLOCKS;
			if (indirectIdx < indirectBlocks.blocks.size())
			{
				return indirectBlocks.blocks[indirectIdx];
			}
		}
	}
	return size_t{ 0 };
}

EntryBlocks fileSystemImpl::getEntryInderectBlocks(InfoDescriptor & _entryDesc)
{
	if (fileSystemSource.is_open() && _entryDesc.blocksNum > InfoDescriptor::DIRECT_BLOCKS)
	{
		EntryBlocks indirectBlocks;
		if (LoadFromStreamT(fileSystemSource, indirectBlocks, getBlockOffset(_entryDesc.block_indirect)))
		{
			return indirectBlocks;
		}
	}
	return EntryBlocks();
}

bool fileSystemImpl::setEntryInderectBlocks(InfoDescriptor & _entryDesc, EntryBlocks & _inderectBlocks)
{
	try
	{
		if (fileSystemSource.is_open() && _entryDesc.blocksNum >= InfoDescriptor::DIRECT_BLOCKS)
		{
			//check if indirect block not exists
			if (_entryDesc.block_indirect == 0)
			{
				auto newInderectBlockIdx = getFreeBlockIdx();
				if (newInderectBlockIdx != 0)
				{
					dataBlocks[newInderectBlockIdx] = true;
					_entryDesc.block_indirect = newInderectBlockIdx;
					SaveToStreamT(fileSystemSource, *this, std::streampos(0));
				}
				else
				{
					return false;
				}
			}

			return SaveToStreamT(fileSystemSource, _inderectBlocks, getBlockOffset(_entryDesc.block_indirect));
		}
	}
	catch (const std::exception&)
	{

	}
	return false;
}

bool fileSystemImpl::addEntryDataBlock(InfoDescriptor & _entry)
{
	if (fileSystemSource.is_open() && _entry.entryType == EntryType::RegularFile) //directory always have 1 data block
	{
		try
		{
			auto newDataBlockIdx = getFreeBlockIdx();
			dataBlocks[newDataBlockIdx] = true;
			if (_entry.blocksNum < InfoDescriptor::DIRECT_BLOCKS)
			{
				_entry.block_direct[_entry.blocksNum] = newDataBlockIdx;
			}
			else
			{
				auto inderectBlocks = getEntryInderectBlocks(_entry);
				inderectBlocks.blocks.push_back(newDataBlockIdx);
				setEntryInderectBlocks(_entry, inderectBlocks);
			}
			++_entry.blocksNum;
			return true;
		}
		catch (std::exception&)
		{

		}
	}
	return false;
}

size_t fileSystemImpl::getFreeBlockIdx()
{
	auto blockIter = std::find_if_not(dataBlocks.begin(), dataBlocks.end(), [](auto val)
	{
		return val; 
	});
	if (blockIter != dataBlocks.end())
	{
		return std::distance(dataBlocks.begin(), blockIter);
	}
	throw std::exception("filesystem is full, not enought data blocks");
}

size_t fileSystemImpl::getFreeInfoIdx()
{
	auto infoIter = std::find_if_not(infoBlocks.begin(), infoBlocks.end(), [](auto val)
	{
		return val;
	});
	if (infoIter != infoBlocks.end())
	{
		return std::distance(infoBlocks.begin(), infoIter);
	}
	throw std::exception("filesystem is full, not enought info blocks");
}

InfoDescriptor fileSystemImpl::getPathDescriptor(const std::wstring& _path)
{
	auto idx = getPathDescriptorIdx(_path);
	if (idx != infos.size())
		return infos[idx];

	return InfoDescriptor();
}

size_t fileSystemImpl::getPathDescriptorIdx(const std::wstring& _path)
{
	fs::path pathTofile(_path);
	if (pathTofile.has_root_path() && pathTofile.root_path() == "/") //check is valid 
	{
		//separate path to directoies list
		fs::path temp(pathTofile);
		std::vector<std::wstring> directories;
		while (temp.has_parent_path())
		{
			if (temp.filename() != fs::path("."))
			{
				directories.push_back(temp.filename().native());
			}
			temp = temp.parent_path();
		}
		auto dirsIter = directories.rbegin();
		decltype(infos.size()) currInfoIdx = 0L;
		auto currInfo = infos[currInfoIdx];
		while (currInfo.isValid() && dirsIter != directories.rend())
		{
			currInfoIdx = getChildDescriptorIdx(currInfo, *dirsIter);
			if (currInfoIdx == infos.size())
			{
				break;
			}
			
			currInfo = infos[currInfoIdx];
			std::advance(dirsIter, 1);
		}
		return currInfoIdx;
	}
	return infos.size();
}

InfoDescriptor fileSystemImpl::getChildDescriptor(InfoDescriptor& _parentDir, const std::wstring& _childName)
{
	if (fileSystemSource.is_open())
	{
		auto idx = getChildDescriptorIdx(_parentDir, _childName);
		if (idx != infos.size())
			return infos[idx];
	}
	return InfoDescriptor();
}

size_t fileSystemImpl::getChildDescriptorIdx(InfoDescriptor& _parentDir, const std::wstring& _childName)
{
	auto retValue = infos.size();
	if ( _parentDir.entryType != EntryType::Directory)
		return retValue;

	std::vector<EntryData> content;
	if (LoadFromStreamT(fileSystemSource, content, getBlockOffset(_parentDir.block_direct[0])))
	{
		auto result = std::find_if(content.begin(), content.end(), [&_childName](auto& info)
		{
			return info.name == _childName;
		});
		if (result != content.end())
		{
			return result->infoIdx;
		}
	}

	return retValue;
}

bool fileSystemImpl::removeDescriptorIdx(size_t _descIdx)
{
	try
	{
		auto desc = infos.at(_descIdx);
		if (desc.isValid() && fileSystemSource.is_open())
		{
			if (desc.entryType == EntryType::RegularFile)
			{
				//mark as free infoblocks and datablocks
				infoBlocks[_descIdx] = false;
				for (const auto index : desc.block_direct)
				{
					dataBlocks[index] = false;
				}
				
				dataBlocks[desc.block_indirect] = false;

				//remove info from directory

				SaveToStreamT(fileSystemSource, *this, std::streampos(0));
			}
		}
	}
	catch (const std::exception&)
	{

	}
	
	return false;
}

bool fileSystemImpl::getDirectoryContent(InfoDescriptor& _desc, std::vector<EntryData>& _content)
{
	if (_desc.isValid()
		&& _desc.entryType == EntryType::Directory
		&& fileSystemSource.is_open())
	{
		return LoadFromStreamT(fileSystemSource, _content, getBlockOffset(getEntryBlockIdx(_desc,0)));
	}

	return false;
}

bool fileSystemImpl::setDirectoryContent(InfoDescriptor& _desc, std::vector<EntryData>& _content)
{
	if (_desc.isValid()
		&& _desc.entryType == EntryType::Directory
		&& fileSystemSource.is_open())
	{
		return SaveToStreamT(fileSystemSource, _content, getBlockOffset(getEntryBlockIdx(_desc, 0)));
	}

	return false;
}