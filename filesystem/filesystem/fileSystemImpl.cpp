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

bool fileSystemImpl::createDirectory(std::wstring _directoryPath)
{
	return createEntry(_directoryPath, EntryType::Directory);
}

bool fileSystemImpl::createFile(std::wstring _filePath)
{
	return createEntry(_filePath, EntryType::RegularFile);
}

bool fileSystemImpl::removeDirectory(std::wstring _directoryPath)
{
	return removeEntry(_directoryPath);
}

std::vector<std::wstring> fileSystemImpl::getDirectoryContentList(std::wstring _directoryPath,
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

bool fileSystemImpl::createEntry(std::wstring _entryPath, EntryType _type)
{
	//check if directory already exists
	if (_type == EntryType::NotSet || !fileSystemSource.is_open() || !checkEntryName(_entryPath) || exists(_entryPath))
		return false;

	fs::path newPath(_entryPath);
	auto parentDescriptor = getPathDescriptor(newPath.parent_path());
	if (parentDescriptor.isValid())
	{
		try
		{
			//make new directories objects
			InfoDescriptor entryInfo;
			entryInfo.entryType = _type;
			entryInfo.blocksNum = 1;
			entryInfo.updateLastAccess();
			entryInfo.firstBlock = getFreeBlockIdx();

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
			dataBlocks[entryInfo.firstBlock] = true;
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

bool fileSystemImpl::removeEntry(std::wstring _entryPath)
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
	auto parentDesc = getPathDescriptor(parentPath);
	std::vector<EntryData> parentContent;
	getDirectoryContent(parentDesc, parentContent);
	std::remove_if(parentContent.begin(), parentContent.end(), [&pathDescIdx](auto& elem)
	{
		return elem.infoIdx == pathDescIdx;
	});
	setDirectoryContent(parentDesc, parentContent);
	//remove dir information
	dataBlocks[infos[pathDescIdx].firstBlock] = false;
	infoBlocks[pathDescIdx] = false;
	infos[pathDescIdx] = InfoDescriptor();
	SaveToStreamT(fileSystemSource, *this, std::streampos(0));
	return true;
}

bool fileSystemImpl::checkEntryName(std::wstring _entryName)
{
	return !_entryName.empty() && _entryName.length() < MAX_ENTRYNAME_LENGTH;
}

std::vector<std::wstring> fileSystemImpl::getDirectoriesList(std::wstring _directoryPath)
{
	return getDirectoryContentList(_directoryPath, EntryType::Directory);
}

std::vector<std::wstring> fileSystemImpl::getFilesList(std::wstring _directoryPath)
{
	return getDirectoryContentList(_directoryPath, EntryType::RegularFile);
}

bool fileSystemImpl::removeFile(std::wstring _fullName)
{
	return removeEntry(_fullName);
}

bool fileSystemImpl::exists(std::wstring _path)
{
	return getPathDescriptor(_path).isValid();
}

bool fileSystemImpl::openFileSystem(std::string _pathToFile, bool _createNew/* = false*/)
{
	if (_pathToFile.empty())
		return false;
	auto openMode = std::ios::binary | std::ios::in | std::ios::out;
	if (_createNew)
		openMode |= std::ios::trunc;
	fileSystemSource.open(_pathToFile, openMode);
	auto retVal = fileSystemSource.is_open();
	return retVal;
}

bool fileSystemImpl::closeFileSystem()
{
	if (fileSystemSource.is_open())
	{
		try
		{
			SaveToStreamT(fileSystemSource, *this, std::streampos(0));
		}
		catch (const std::exception&)
		{
			return false;
		}
		fileSystemSource.close();
		return true;
	}
	return false;
}

bool fileSystemImpl::createFileSystem(std::string _pathToFile, size_t _blockSize, size_t _blocksCount)
{
	if (fileSystemSource.is_open())
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
		infos[0].firstBlock = 0;
		infos[0].entryType = EntryType::Directory;
		infoBlocks.insert(infoBlocks.begin(), 512, false);
		dataBlocks.insert(dataBlocks.begin(), mainSector.getBlockSize(), false);
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

InfoDescriptor fileSystemImpl::getPathDescriptor(std::wstring _path)
{
	auto idx = getPathDescriptorIdx(_path);
	if (idx != infos.size())
		return infos[idx];

	return InfoDescriptor();
}

size_t fileSystemImpl::getPathDescriptorIdx(std::wstring _path)
{
	fs::path pathTofile(_path);
	if (pathTofile.has_root_path() && pathTofile.root_path() == "/") //check is valid 
	{
		//separate path to directoies list
		fs::path temp(pathTofile);
		std::vector<std::wstring> directories;
		while (temp.has_parent_path())
		{
			directories.push_back(temp.filename().native());
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

InfoDescriptor fileSystemImpl::getChildDescriptor(InfoDescriptor _parentDir, std::wstring _childName)
{
	if (fileSystemSource.is_open())
	{
		auto idx = getChildDescriptorIdx(_parentDir, _childName);
		if (idx != infos.size())
			return infos[idx];
	}
	return InfoDescriptor();
}

size_t fileSystemImpl::getChildDescriptorIdx(InfoDescriptor _parentDir, std::wstring _childName)
{
	auto retValue = infos.size();
	if ( _parentDir.entryType != EntryType::Directory)
		return retValue;

	std::vector<EntryData> content;
	if (LoadFromStreamT(fileSystemSource, content, getBlockOffset(_parentDir.firstBlock)))
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
				dataBlocks[desc.firstBlock] = false;

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

bool fileSystemImpl::getDirectoryContent(InfoDescriptor _desc, std::vector<EntryData>& _content)
{
	if (_desc.isValid()
		&& _desc.entryType == EntryType::Directory
		&& fileSystemSource.is_open())
	{
		return LoadFromStreamT(fileSystemSource, _content, getBlockOffset(_desc.firstBlock));
	}

	return false;
}

bool fileSystemImpl::setDirectoryContent(InfoDescriptor _desc, std::vector<EntryData>& _content)
{
	if (_desc.isValid()
		&& _desc.entryType == EntryType::Directory
		&& fileSystemSource.is_open())
	{
		return SaveToStreamT(fileSystemSource, _content, getBlockOffset(_desc.firstBlock));
	}

	return false;
}