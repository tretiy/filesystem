#include "supernode.h"
#include "portable_serialization\portable_binary_iarchive.hpp"
#include "portable_serialization\portable_binary_oarchive.hpp"

#include <exception>


supernode::supernode()
	:supernode(256,512,1024)
{
}

supernode::supernode(unsigned int _infosCount, unsigned int _blockSize, unsigned int _fileSystemSize)
	:infosCount(_infosCount), blockSize(_blockSize), fileSystemSize(_fileSystemSize)
{
}

supernode::~supernode()
{
}

void supernode::LoadFromStream(std::fstream & file)
{
	if (!file.is_open())
		throw std::invalid_argument("stream without file");
	
		portable_binary_iarchive ia(file);
		ia >> *this;
}

bool supernode::SaveToStream(std::fstream & file)
{
	if (!file.is_open())
		throw std::invalid_argument("stream without file");
	try
	{
		portable_binary_oarchive oa(file);
		oa << *this;
	}
	catch (const std::exception&)
	{
		return false;
	}
	return true;
}
bool operator==(const supernode& left, const supernode& right)
{
	return left.blockSize == right.blockSize &&
		left.infosCount == right.infosCount &&
		left.fileSystemSize == right.fileSystemSize;
}


