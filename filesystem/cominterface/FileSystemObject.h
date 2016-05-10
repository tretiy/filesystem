#pragma once
#include "IComFileSystem_h.h"
class FileSystemObject :
	public IFileSystem
{
public:
	FileSystemObject();
	virtual ~FileSystemObject();
};

