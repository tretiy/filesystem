#include <windows.h>
#include <string>
#include "FileSystem.h"

//
// FileSystem class implementation
//
// Constructors
FileSystem::FileSystem()
{
	m_lRef = 0;

	// ��������� �������� �������� �������� ��������
	InterlockedIncrement(&g_lObjs);
}

// The destructor
FileSystem::~FileSystem()
{
	// ��������� �������� �������� �������� ��������
	InterlockedDecrement(&g_lObjs);
}

STDMETHODIMP FileSystem::QueryInterface(REFIID riid, void** ppv)
{
	*ppv = 0;

	if (riid == IID_IUnknown || riid == IID_IFileSystem)
		*ppv = this;

	if (*ppv)
	{
		AddRef();
		return(S_OK);
	}
	return (E_NOINTERFACE);
}

STDMETHODIMP_(ULONG) FileSystem::AddRef()
{
	return InterlockedIncrement(&m_lRef);
}

STDMETHODIMP_(ULONG) FileSystem::Release()
{
	if (InterlockedDecrement(&m_lRef) == 0)
	{
		delete this;
		return 0;
	}

	return m_lRef;
}

SAFEARRAY* FileSystem::makeBSTRarray(std::vector<std::wstring>& _strings)
{
	if (_strings.empty())
		return nullptr;

	SAFEARRAYBOUND bounds[] = { { (ULONG)_strings.size(), 0 } }; //Array Contains 2 Elements starting from Index �0� 

	SAFEARRAY* stringsBSTR = SafeArrayCreate(VT_VARIANT, 1, bounds); //Create a one-dimensional SafeArray of variants
	long*  lIndex = new long[_strings.size()];
	for (const auto& value : _strings)
	{
		VARIANT var;
		*lIndex = 0; // index of the element being inserted in the array
		var.vt = VT_BSTR; // type of the element being inserted 
		var.bstrVal = SysAllocStringLen(value.data(), (UINT)value.size());
		HRESULT hr = SafeArrayPutElement(stringsBSTR, lIndex, &var); // insert the element 
		std::advance(lIndex, 1);
	}
	
	return stringsBSTR;
}

VARIANT* FileSystem::makeByteVariant(std::vector<char>& _bytes)
{
	SAFEARRAY FAR*  psarray;
	SAFEARRAYBOUND sabounds[1];

	sabounds[0].lLbound = 0;
	sabounds[0].cElements = (ULONG)_bytes.size();

	long nLbound;

	psarray = SafeArrayCreate(VT_UI1, 1, sabounds);
	if (psarray == NULL)
		return nullptr;

	for (nLbound = 0; nLbound < (long)sabounds[0].cElements; nLbound++) {
		if (FAILED(SafeArrayPutElement(psarray, &nLbound, &_bytes[nLbound]))) {
			SafeArrayDestroy(psarray);
			return nullptr;
		}
	}
	VARIANT* vtResult = new VARIANT();
	vtResult->vt = VT_ARRAY | VT_UI1;
	vtResult->parray = psarray;
	return vtResult;
}

std::vector<char>* FileSystem::makeByteArray(VARIANT* _bytes)
{
	if (_bytes->vt != (VT_ARRAY | VT_UI1))
		return nullptr;

	auto arraySize = _bytes->parray->rgsabound[0].cElements;
	auto retVec = new std::vector<char>(arraySize);
	long nLbound;
	for (nLbound = 0; nLbound < (long)_bytes->parray->rgsabound[0].cElements; nLbound++)
	{
		if (FAILED(SafeArrayGetElement(_bytes->parray, &nLbound, &retVec[nLbound]))) 
		{
			delete retVec;
			return nullptr;
		}
	}

	return retVec;
}

//filesystem operations
STDMETHODIMP FileSystem::createDirectory(BSTR _directoryPath)
{
	if (_directoryPath != nullptr)
	{
		std::wstring path(_directoryPath, SysStringLen(_directoryPath));
		if (fsImpl.createDirectory(path))
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

STDMETHODIMP FileSystem::removeDirectory(BSTR _directoryPath)
{
	if (_directoryPath != nullptr)
	{
		std::wstring path(_directoryPath, SysStringLen(_directoryPath));
		if (fsImpl.removeDirectory(path))
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

STDMETHODIMP_(SAFEARRAY*) FileSystem::getDirectoriesList(BSTR _directoryPath)
{
	if (_directoryPath != nullptr)
	{
		std::wstring path(_directoryPath, SysStringLen(_directoryPath));
		auto dirList = fsImpl.getDirectoriesList(path);
		if(!dirList.empty())
		{
			return makeBSTRarray(dirList);
		}
	}
	return nullptr;
}

STDMETHODIMP_(SAFEARRAY*) FileSystem::getFilesList(BSTR _directoryPath)
{
	if (_directoryPath != nullptr)
	{
		std::wstring path(_directoryPath, SysStringLen(_directoryPath));
		auto fileList = fsImpl.getFilesList(path);
		if (!fileList.empty())
		{
			return makeBSTRarray(fileList);
		}
	}
	return nullptr;
}

STDMETHODIMP FileSystem::createFile(BSTR _filePath)
{
	if (_filePath != nullptr)
	{
		std::wstring path(_filePath, SysStringLen(_filePath));
		if (fsImpl.createFile(path))
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

STDMETHODIMP FileSystem::removeFile(BSTR _filePath)
{
	if (_filePath != nullptr)
	{
		std::wstring path(_filePath, SysStringLen(_filePath));
		if (fsImpl.removeFile(path))
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

STDMETHODIMP FileSystem::exists(BSTR _path)
{
	if (_path != nullptr)
	{
		std::wstring path(_path, SysStringLen(_path));
		if (fsImpl.exists(path))
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

STDMETHODIMP FileSystem::openFile(BSTR _pathToFile, bool _seekToBegin, size_t* _fileIdx, size_t* _position)
{
	if (_pathToFile != nullptr)
	{
		std::wstring path(_pathToFile, SysStringLen(_pathToFile));
		auto desc = fsImpl.openFile(path, _seekToBegin);
		if (desc.isValid())
		{
			*_fileIdx = desc.infoDescIdx;
			*_position = desc.seekPos;
			return S_OK;
		}
	}
	return E_FAIL;
}

STDMETHODIMP_(size_t) FileSystem::writeToFile(size_t _fileIdx, size_t* _position, VARIANT* _data, size_t _count)
{
	if (_fileIdx > 0)
	{
		auto bytes = makeByteArray(_data);
		if (bytes != nullptr)
		{
			FileDescriptor fDesc;
			fDesc.infoDescIdx = _fileIdx;
			fDesc.seekPos = *_position;
			auto writed = fsImpl.writeToFile(fDesc, &(*bytes)[0], _count);
			*_position = fDesc.seekPos;
			delete bytes;
			return writed;
		}
	}

	return size_t{ 0 };
}

STDMETHODIMP_(size_t) FileSystem::readFromFile(size_t _fileIdx, size_t* _position, VARIANT* _data, size_t _count)
{
	if (_fileIdx > 0)
	{
		auto bytes = std::vector<char>(_count);
		
		FileDescriptor fDesc;
		fDesc.infoDescIdx = _fileIdx;
		fDesc.seekPos = *_position;
		auto readed = fsImpl.readFromFile(fDesc, &bytes[0], _count);
		*_position = fDesc.seekPos;
		_data = makeByteVariant(bytes);
		return readed;
	}

	return size_t{ 0 };
}
//file operations
STDMETHODIMP FileSystem::openFileSystem(BSTR _pathToFile, bool _createNew)
{
	if (_pathToFile != nullptr)
	{
		std::wstring path(_pathToFile, SysStringLen(_pathToFile));
		if (fsImpl.openFileSystem(path, _createNew))
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

STDMETHODIMP FileSystem::closeFileSystem()
{
	if (fsImpl.closeFileSystem())
		return S_OK;
	return E_FAIL;
}

FileSystemClassFactory::FileSystemClassFactory()
{
	m_lRef = 0;
}

FileSystemClassFactory::~FileSystemClassFactory()
{
}

STDMETHODIMP FileSystemClassFactory::QueryInterface(REFIID riid, void** ppv)
{
	*ppv = 0;

	if (riid == IID_IUnknown || riid == IID_IClassFactory)
		*ppv = this;

	if (*ppv)
	{
		AddRef();
		return S_OK;
	}

	return(E_NOINTERFACE);
}

STDMETHODIMP_(ULONG) FileSystemClassFactory::AddRef()
{
	return InterlockedIncrement(&m_lRef);
}

STDMETHODIMP_(ULONG) FileSystemClassFactory::Release()
{
	if (InterlockedDecrement(&m_lRef) == 0)
	{
		delete this;
		return 0;
	}

	return m_lRef;
}

STDMETHODIMP FileSystemClassFactory::CreateInstance
(LPUNKNOWN pUnkOuter, REFIID riid, void** ppvObj)
{
	FileSystem*      pFileSystem;
	HRESULT    hr;

	*ppvObj = 0;

	pFileSystem = new FileSystem();

	if (pFileSystem == 0)
		return(E_OUTOFMEMORY);

	hr = pFileSystem->QueryInterface(riid, ppvObj);

	if (FAILED(hr))
		delete pFileSystem;

	return hr;
}

STDMETHODIMP FileSystemClassFactory::LockServer(BOOL fLock)
{
	if (fLock)
		InterlockedIncrement(&g_lLocks);
	else
		InterlockedDecrement(&g_lLocks);

	return S_OK;
}