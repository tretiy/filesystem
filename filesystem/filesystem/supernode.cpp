#include "supernode.h"
#include "portable_serialization\portable_binary_iarchive.hpp"
#include "portable_serialization\portable_binary_oarchive.hpp"

#include <exception>


supernode::supernode()
{
}

supernode::~supernode()
{
}

supernode&& supernode::LoadFromStream(std::fstream & file)
{
	if (!file.is_open())
		throw std::invalid_argument("stream without file");
	
		portable_binary_iarchive ia(file);
		supernode node;
		ia >> node;
		return std::move(node);
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



