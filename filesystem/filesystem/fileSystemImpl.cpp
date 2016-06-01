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
	fs::path newPath(_directoryPath);
	auto parentDescriptor = getPathDescriptor(newPath.parent_path());
	if (parentDescriptor.isValid())
	{
		try
		{
			//make new directories objects
			infoDescriptor dirInfo;
			dirInfo.fileType = infoDescriptor::FileType::Directory;
			dirInfo.blocksNum = 1;
			dirInfo.updateLastAccess();
			dirInfo.firstBlock = getFreeBlockIdx();

			DirectoryInfo newDir;
			newDir.name = newPath.filename().native();
			newDir.infoIdx = getFreeInfoIdx();

			//get parent directory content
			std::vector<DirectoryInfo> content;
			getDirectoryContent(parentDescriptor, content);
			content.push_back(newDir);

			// save dirinfo to stream;
			setDirectoryContent(parentDescriptor, content);
			//change header data
			dataBlocks[dirInfo.firstBlock] = true;
			infoBlocks[newDir.infoIdx] = true;
			infos[newDir.infoIdx] = dirInfo;

			SaveToStreamT(fileSystemSource, *this, std::streampos(0));
		}
		catch (const std::exception&)
		{
			return false;
		}
		return true;
	}

	return false;
}

bool fileSystemImpl::removeDirectory(std::wstring _directoryPath)
{
	auto pathDescIdx = getPathDescriptorIdx(_directoryPath);
	if (pathDescIdx == infos.size() || pathDescIdx == 0) // do not allow to remove root dir
	{
		return false;
	}
	
	std::vector<DirectoryInfo> content;
	getDirectoryContent(infos[pathDescIdx], content);
	if (content.empty())
	{
		//so this directory is free we can delete it
		//update parent dir content
		fs::path dirPath(_directoryPath);
		auto parentPath = dirPath.parent_path();
		auto parentDesc = getPathDescriptor(parentPath);
		std::vector<DirectoryInfo> parentContent;
		getDirectoryContent(parentDesc, parentContent);
		std::remove_if(parentContent.begin(), parentContent.end(), [&pathDescIdx](auto& elem)
		{
			return elem.infoIdx == pathDescIdx;
		});
		setDirectoryContent(parentDesc, parentContent);
		//remove dir information
		dataBlocks[infos[pathDescIdx].firstBlock] = false;
		infoBlocks[pathDescIdx] = false;
		infos[pathDescIdx] = infoDescriptor();
		SaveToStreamT(fileSystemSource, *this, std::streampos(0));
		return true;
	}
	return false;
}

std::vector<std::wstring> fileSystemImpl::getDirectoryContentList(std::wstring _directoryPath,
	infoDescriptor::FileType _type /*= infoDescriptor::FileType::NotSet*/)
{
	std::vector<DirectoryInfo> content;
	std::vector<std::wstring> contentNames;
	auto directoryDesc = getPathDescriptor(_directoryPath);
	if (directoryDesc.isValid() && getDirectoryContent(directoryDesc, content))
	{
		for (const auto& elem : content)
		{
			if (_type == infoDescriptor::FileType::NotSet || infos[elem.infoIdx].fileType == _type)
				contentNames.push_back(elem.name);
		}
	}

	return contentNames;
}

std::vector<std::wstring> fileSystemImpl::getDirectoriesList(std::wstring _directoryPath)
{
	return getDirectoryContentList(_directoryPath, infoDescriptor::FileType::Directory);
}

std::vector<std::wstring> fileSystemImpl::getFilesList(std::wstring _directoryPath)
{
	return getDirectoryContentList(_directoryPath, infoDescriptor::FileType::RegularFile);
}

FileDescriptor fileSystemImpl::createFile(std::wstring _fullName)
{
	return FileDescriptor();
}

bool fileSystemImpl::removeFile(std::wstring _fullName)
{
	return false;
}

bool fileSystemImpl::exists(std::wstring _path)
{
	return getPathDescriptor(_path).isValid();
}

bool fileSystemImpl::OpenFileSystem(std::string pathToFile, bool createNew/* = false*/)
{
	if (pathToFile.empty())
		return false;
	auto openMode = std::ios::binary | std::ios::in | std::ios::out;
	if (createNew)
		openMode |= std::ios::trunc;
	fileSystemSource.open(pathToFile, openMode);
	auto retVal = fileSystemSource.is_open();
	return retVal;
}

bool fileSystemImpl::CloseFileSystem()
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

bool fileSystemImpl::CreateFileSystem(std::string pathToFile, unsigned int blockSize, unsigned int blocksCount)
{
	if (fileSystemSource.is_open())
	{
		return false;
	}
	if (OpenFileSystem(pathToFile, true))
	{
		//create a file system with default scheme
		mainSector = FileSystemHeader(512, blockSize, blocksCount);
		//create a root directory
		infos.insert(infos.begin(), 512, infoDescriptor());
		infos[0].lastAccess = system_clock::now();
		infos[0].blocksNum = 1;
		infos[0].firstBlock = 0;
		infos[0].fileType = infoDescriptor::FileType::Directory;
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

bool fileSystemImpl::InitializeFileSystem()
{
	if (!fileSystemSource.is_open())
		return false;

	return LoadFromStreamT(fileSystemSource, *this, std::streampos(0));
}

std::streampos fileSystemImpl::GetBlockOffset(size_t blockNum)
{
	if (blockNum < mainSector.getFileSystemSize())
	{
		return mainSector.getDataOffset() + mainSector.getBlockSize() * blockNum;
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

infoDescriptor fileSystemImpl::getPathDescriptor(std::wstring _path)
{
	auto idx = getPathDescriptorIdx(_path);
	if (idx != infos.size())
		return infos[idx];

	return infoDescriptor();
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
			currInfoIdx = getDescriptorIdx(currInfo, *dirsIter);
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
infoDescriptor fileSystemImpl::getDescriptor(infoDescriptor _parentDir, std::wstring _name)
{
	if (fileSystemSource.is_open())
	{
		auto idx = getDescriptorIdx(_parentDir, _name);
		if (idx != infos.size())
			return infos[idx];
	}
	return infoDescriptor();
}

size_t fileSystemImpl::getDescriptorIdx(infoDescriptor _parentDir, std::wstring _name)
{
	auto retValue = infos.size();
	if ( _parentDir.fileType != infoDescriptor::FileType::Directory)
		return retValue;

	std::vector<DirectoryInfo> content;
	if (LoadFromStreamT(fileSystemSource, content, GetBlockOffset(_parentDir.firstBlock)))
	{
		auto result = std::find_if(content.begin(), content.end(), [&_name](auto& info)
		{
			return info.name == _name;
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
			if (desc.fileType == infoDescriptor::FileType::RegularFile)
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

bool fileSystemImpl::getDirectoryContent(infoDescriptor _desc, std::vector<DirectoryInfo>& _content)
{
	if (_desc.isValid()
		&& _desc.fileType == infoDescriptor::FileType::Directory
		&& fileSystemSource.is_open())
	{
		return LoadFromStreamT(fileSystemSource, _content, GetBlockOffset(_desc.firstBlock));
	}

	return false;
}

bool fileSystemImpl::setDirectoryContent(infoDescriptor _desc, std::vector<DirectoryInfo>& _content)
{
	if (_desc.isValid()
		&& _desc.fileType == infoDescriptor::FileType::Directory
		&& fileSystemSource.is_open())
	{
		return SaveToStreamT(fileSystemSource, _content, GetBlockOffset(_desc.firstBlock));
	}

	return false;
}