#include "groupDescriptor.h"


namespace filesystem
{
	groupDescriptor::groupDescriptor()
	{
	}

	groupDescriptor::groupDescriptor(unsigned int _infoNumber, std::string & _entryName, bool _isDirectory)
		:infoNumber(_infoNumber), isDirectory(_isDirectory), entryName(_entryName)
	{
	}


	groupDescriptor::~groupDescriptor()
	{
	}
}
