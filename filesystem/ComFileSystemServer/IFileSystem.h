#pragma once

// {A888F560-58E4-11d0-A68A-0000837E3100}
DEFINE_GUID(CLSID_Math,
	0xa888f560, 0x58e4, 0x11d0, 0xa6, 0x8a, 0x0, 0x0, 0x83, 0x7e, 0x31, 0x0);

// {A888F561-58E4-11d0-A68A-0000837E3100}
DEFINE_GUID(IID_IMath,
	0xa888f561, 0x58e4, 0x11d0, 0xa6, 0x8a, 0x0, 0x0, 0x83, 0x7e, 0x31, 0x0);

class IFileSystem : public IUnknown
{
public:
	//filesystem operations
	STDMETHOD(createDirectory(BSTR _directoryPath)) PURE;
	STDMETHOD(removeDirectory(BSTR _directoryPath)) PURE;
	STDMETHOD_(SAFEARRAY*, getDirectoriesList(BSTR _directoryPath)) PURE;
	STDMETHOD_(SAFEARRAY*, getFilesList(BSTR _directoryPath)) PURE;
	STDMETHOD(createFile(BSTR _filePath)) PURE;
	STDMETHOD(removeFile(BSTR _filePath)) PURE;
	STDMETHOD(exists(BSTR _path)) PURE;
	STDMETHOD_(SAFEARRAY*, openFile(BSTR _pathToFile, bool _seekToBegin)) PURE;
	STDMETHOD_(size_t, writeToFile(SAFEARRAY*_file, SAFEARRAY* _data, size_t _count)) PURE;
	STDMETHOD_(size_t, readFromFile(SAFEARRAY* _file, SAFEARRAY* _data, size_t _count)) PURE;
	//file operations
	STDMETHOD(openFileSystem(BSTR _pathToFile, bool _createNew)) PURE;
	STDMETHOD(closeFileSystem()) PURE;
};