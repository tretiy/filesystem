#include "stdafx.h"
#include "CppUnitTest.h"
#include "fileSystemImpl.h"
#include "FileSystemHeader.h"
#include <string>
#include <experimental/filesystem>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace filesystem;
using namespace std::experimental::filesystem;

namespace FileSystemTests
{
	TEST_CLASS(fileSystemImpl_test)
	{
	public:
		
		fileSystemImpl impl;

		TEST_METHOD(CreateFileSystem)
		{
			Assert::IsTrue(impl.createFileSystem("C:\\Projects\\newFilesys.txt", 512, 10000));
			Assert::IsTrue(impl.createDirectory(L"/testDir"));
			Assert::IsTrue(impl.createDirectory(L"/testDir/testDir1"));
			Assert::IsTrue(impl.createDirectory(L"/testDir/testDir1/testDir2"));
			Assert::IsTrue(impl.exists(L"/testDir/testDir1"));
			Assert::IsTrue(impl.exists(L"/testDir/testDir1/testDir2"));
			Assert::IsTrue(impl.removeDirectory(L"/testDir/testDir1/testDir2"));
			Assert::IsFalse(impl.exists(L"/testDir/testDir1/testDir2"));
			Assert::IsTrue(impl.closeFileSystem());
			Assert::IsTrue(impl.openFileSystem("C:\\Projects\\newFilesys.txt"));
			Assert::IsFalse(impl.getDirectoriesList(L"/").empty());
			Assert::IsTrue(impl.closeFileSystem());
		}
	};
}