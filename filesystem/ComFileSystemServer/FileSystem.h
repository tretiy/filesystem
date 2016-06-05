#pragma once
#include "IFileSystem.h"
#include "..\filesystem\fileSystemImpl.h"

extern long g_lObjs;
extern long g_lLocks;


class FileSystem : public IFileSystem
{
	fileSystemImpl fsImpl;
protected:
	// Reference count
	long          m_lRef;

public:
	FileSystem();
	~FileSystem();

public:
	// IUnknown
	STDMETHOD(QueryInterface(REFIID, void**));
	STDMETHOD_(ULONG, AddRef());
	STDMETHOD_(ULONG, Release());

	//filesystem operations
	STDMETHOD(createDirectory(BSTR _directoryPath));
	STDMETHOD(removeDirectory(BSTR _directoryPath)) ;
	STDMETHOD_(SAFEARRAY*, getDirectoriesList(BSTR _directoryPath));
	STDMETHOD_(SAFEARRAY*, getFilesList(BSTR _directoryPath));
	STDMETHOD(createFile(BSTR _filePath));
	STDMETHOD(removeFile(BSTR _filePath));
	STDMETHOD(exists(BSTR _path));
	STDMETHOD(openFile(BSTR _pathToFile, bool _seekToBegin, size_t* _fileIdx, size_t* _position));
	STDMETHOD_(size_t, writeToFile(size_t _fileIdx, size_t* _position, VARIANT* _data, size_t _count));
	STDMETHOD_(size_t, readFromFile(size_t _fileIdx, size_t* _position, VARIANT& _data, size_t _count));
	//file operations
	STDMETHOD(rename(BSTR _pathTo, BSTR _newName));
	STDMETHOD(openFileSystem(BSTR _pathToFile, bool _createNew));
	STDMETHOD(createFileSystem(BSTR _pathToFile, size_t _blockSize, size_t _blocksCount));
	STDMETHOD(closeFileSystem());
};

class FileSystemClassFactory : public IClassFactory
{
protected:
	long       m_lRef;

public:
	FileSystemClassFactory();
	~FileSystemClassFactory();

	// IUnknown
	STDMETHOD(QueryInterface(REFIID, void**));
	STDMETHOD_(ULONG, AddRef());
	STDMETHOD_(ULONG, Release());

	// IClassFactory
	STDMETHOD(CreateInstance(LPUNKNOWN, REFIID, void**));
	STDMETHOD(LockServer(BOOL));
};