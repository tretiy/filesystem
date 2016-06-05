#include <windows.h>
#include <tchar.h>
#include <iostream>

#include <initguid.h>
#include "..\ComFileSystemServer\IFileSystem.h"

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
	// Получить фабрику классов для класса Math
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

	// с помощью фабрики классов создать экземпляр
	// компонента и получить интерфейс IUnknown.
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
	

	cout << "Releasing instance" << endl;
	pFileSystem->Release();

	cout << "Shuting down COM" << endl;
	CoUninitialize();

	return 0;
}