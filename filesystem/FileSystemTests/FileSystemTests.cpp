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
			Assert::IsTrue(impl.initializeFileSystem());
			Assert::IsFalse(impl.getDirectoriesList(L"/").empty());
			Assert::IsTrue(impl.closeFileSystem());
		}

		TEST_METHOD(FileOperations)
		{
			Assert::IsTrue(impl.openFileSystem("C:\\Projects\\newFilesys.txt"));
			Assert::IsTrue(impl.initializeFileSystem());
			Assert::IsTrue(impl.exists(L"/testDir/testDir1"));
			Assert::IsTrue(impl.createFile(L"/testDir/testDir1/testFile.txt"));
			Assert::IsTrue(impl.exists(L"/testDir/testDir1/testFile.txt"));
			auto sizeToWrite = 512 * 5 + 1; 
			char* array = new char[sizeToWrite];
			memset(&array[0], 'l', sizeToWrite);
			array[0] = 's';
			array[sizeToWrite - 1] = 's';
			auto file = impl.openFile(L"/testDir/testDir1/testFile.txt");
			Assert::IsTrue(file.isValid());
			Assert::IsTrue(impl.writeToFile(file, &array[0], sizeToWrite) == sizeToWrite);
			Assert::IsTrue(impl.closeFileSystem());
		}

		TEST_METHOD(CheckFile)
		{
			Assert::IsTrue(impl.openFileSystem("C:\\Projects\\newFilesys.txt"));
			Assert::IsTrue(impl.initializeFileSystem());
			Assert::IsTrue(impl.exists(L"/testDir/testDir1/testFile.txt"));
			auto file = impl.openFile(L"/testDir/testDir1/testFile.txt"); 
			Assert::IsTrue(file.isValid());
			Assert::IsTrue(impl.closeFileSystem());
		}
	};
}