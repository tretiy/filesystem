#include <windows.h>
#include <tchar.h>
#include <iostream>

#include <initguid.h>
#include "..\ComFileSystemServer\IFileSystem.h"
#include "..\ComFileSystemServer\AutomationHelpers.h"

using namespace std;

int main(int argc, char *argv[])
{
	cout << "Initializing COM" << endl;

	if (FAILED(CoInitialize(NULL)))
	{
		cout << "Unable to initialize COM" << endl;
		return -1;
	}

	char* szProgID = "FileSystem.Component.1";
	WCHAR  szWideProgID[128];
	CLSID  clsid;
	long lLen = MultiByteToWideChar(CP_ACP,
		0,
		szProgID,
		strlen(szProgID),
		szWideProgID,
		sizeof(szWideProgID));

	szWideProgID[lLen] = '\0';
	HRESULT hr = ::CLSIDFromProgID(szWideProgID, &clsid);
	if (FAILED(hr))
	{
		cout.setf(ios::hex, ios::basefield);
		cout << "Unable to get CLSID from ProgID. HR = " << hr << endl;
		return -1;
	}

	IClassFactory* pCF;
	// get a classfactory
	hr = CoGetClassObject(clsid,
		CLSCTX_INPROC,
		NULL,
		IID_IClassFactory,
		(void**)&pCF);
	if (FAILED(hr))
	{
		cout.setf(ios::hex, ios::basefield);
		cout << "Failed to GetClassObject server instance. HR = " << hr << endl;
		return -1;
	}

	// get IUnknown iface
	IUnknown* pUnk;
	hr = pCF->CreateInstance(NULL, IID_IUnknown, (void**)&pUnk);

	// Release the class factory
	pCF->Release();

	if (FAILED(hr))
	{
		cout.setf(ios::hex, ios::basefield);
		cout << "Failed to create server instance. HR = " << hr << endl;
		return -1;
	}

	cout << "Instance created" << endl;

	IFileSystem* pFileSystem = NULL;
	hr = pUnk->QueryInterface(IID_IFileSystem, (LPVOID*)&pFileSystem);
	pUnk->Release();
	if (FAILED(hr))
	{
		cout << "QueryInterface() for IMath failed" << endl;
		return -1;
	}

	//check bstr
	//SAFEARRAY* retVal = pFileSystem->TESTBSTR();
	BSTR bstrPath = SysAllocString(L"C:\\Projects\\comFS");
	auto ret = false;
	ret = SUCCEEDED(pFileSystem->createFileSystem(bstrPath, 512, 20000));
	SysFreeString(bstrPath);
	bstrPath = SysAllocString(L"/comDir");
	ret = SUCCEEDED(pFileSystem->createDirectory(bstrPath));
	SysFreeString(bstrPath);
	bstrPath = SysAllocString(L"/comDir2");
	ret = SUCCEEDED(pFileSystem->createDirectory(bstrPath));
	SysFreeString(bstrPath);
	bstrPath = SysAllocString(L"/");
	auto list = pFileSystem->getDirectoriesList(bstrPath);
	SysFreeString(bstrPath);
	bstrPath = SysAllocString(L"/comDir2/testFile.txt");
	ret = SUCCEEDED(pFileSystem->createFile(bstrPath));
	size_t fileDesc{ 0 };
	size_t filePos{ 0 };
	ret = SUCCEEDED(pFileSystem->openFile(bstrPath, true, &fileDesc, &filePos));
	
	auto buffSize{ 1025 };
	std::vector<char> bytes(buffSize, 'm');
	bytes[0] = 'a';
	bytes[buffSize - 1] = 'a';
	VARIANT* data = makeByteVariant(bytes);
	auto writed = pFileSystem->writeToFile(fileDesc, &filePos, data, buffSize);
	VariantClear(data);
	filePos = 0;
	VariantInit(data);
	auto readed = pFileSystem->readFromFile(fileDesc, &filePos, *data, buffSize);
	SysFreeString(bstrPath);
	pFileSystem->closeFileSystem();
	cout << "Releasing instance" << endl;
	pFileSystem->Release();

	cout << "Shuting down COM" << endl;
	CoUninitialize();

	return 0;
}