#pragma once
#include <fstream>
#include <boost\archive\binary_iarchive.hpp>
#include <boost\archive\binary_oarchive.hpp>

using namespace boost::archive;
#define serialize_flags  (archive_flags::no_codecvt | archive_flags::no_header | archive_flags::no_tracking | archive_flags::no_xml_tag_checking)

template <class T>
bool LoadFromStreamT(std::fstream& file, T& obj, std::streampos& pos)
{
	if (!file.is_open())
		throw std::invalid_argument("stream without file");

	try
	{
		file.seekg(pos);
		binary_iarchive ia(file, serialize_flags);
		ia >> obj;
	}
	catch (const std::exception&)
	{
		return false;
	}
	return true;
}

template <class T>
bool SaveToStreamT(std::fstream& file, T& obj, std::streampos& pos)
{
	if (!file.is_open())
		throw std::invalid_argument("stream without file");
	try
	{
		file.seekp(pos);
		binary_oarchive oa(file, serialize_flags);
		oa << obj;
	}
	catch (const std::exception&)
	{
		return false;
	}
	return true;
}
