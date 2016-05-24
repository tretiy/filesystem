#include "blocksbitmap.h"
#include "portable_serialization\portable_binary_iarchive.hpp"
#include "portable_serialization\portable_binary_oarchive.hpp"

#include <exception>

blocksbitmap::blocksbitmap()
{
	blocksState.reserve(512);
	blocksState.assign(512, false);
}

blocksbitmap::~blocksbitmap()
{

}

void blocksbitmap::LoadFromStream(std::fstream & file)
{
	if (!file.is_open())
		throw std::invalid_argument("stream without file");

	portable_binary_iarchive ia(file);
	ia >> *this;
}

bool blocksbitmap::SaveToStream(std::fstream & file)
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