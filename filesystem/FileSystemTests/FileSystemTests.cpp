#include "stdafx.h"
#include "CppUnitTest.h"
#include "fileSystemImpl.h"
#include "FileSystemHeader.h"
#include <string>
#include <vector>
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
		std::wstring fsPath = L"C:\\Projects\\newFilesys.txt";

		TEST_METHOD(CreateCloseFileSystem)
		{
			//check bound conditions 
			Assert::IsFalse(impl.createFileSystem(L"", 512, 10000));
			Assert::IsFalse(impl.createFileSystem(fsPath, 0, 10000));
			Assert::IsFalse(impl.createFileSystem(fsPath, 512, 0));
			//check greater than max size 
			Assert::IsFalse(impl.createFileSystem(fsPath, 1024, 1024 * 1024 + 1));
			//check max size
			Assert::IsTrue(impl.createFileSystem(fsPath, 1024, 1024 * 1024));
			//close			
			Assert::IsTrue(impl.closeFileSystem());
		}

		TEST_METHOD(OpenCloseFileSystem)
		{
			//open previous fs
			Assert::IsTrue(impl.openFileSystem(fsPath));
			Assert::IsTrue(impl.initializeFileSystem());
			Assert::IsTrue(impl.closeFileSystem());

			//check that enpty file is not valid
			Assert::IsTrue(impl.openFileSystem(fsPath, true));
			Assert::IsFalse(impl.initializeFileSystem());
			Assert::IsTrue(impl.closeFileSystem());
		}

		TEST_METHOD(DirectoryOperations)
		{
			//create fs first
			Assert::IsTrue(impl.createFileSystem(fsPath, 1024, 10000));

			//create dirs
			Assert::IsTrue(impl.createDirectory(L"/fDir"));
			Assert::IsTrue(impl.createDirectory(L"/fDir/sDir"));
			Assert::IsTrue(impl.createDirectory(L"/fDir/sDir/tDir"));
			//check if exists
			Assert::IsTrue(impl.exists(L"/fDir"));
			Assert::IsTrue(impl.exists(L"/fDir/sDir"));
			Assert::IsTrue(impl.exists(L"/fDir/sDir/tDir"));
			//can't create same directory
			Assert::IsFalse(impl.createDirectory(L"/fDir/sDir"));
			//check remove 
			Assert::IsTrue(impl.removeDirectory(L"/fDir/sDir/tDir"));
			Assert::IsFalse(impl.exists(L"/fDir/sDir/tDir"));

			Assert::IsTrue(impl.closeFileSystem());
			//check that directory structure is saved to disk
			Assert::IsTrue(impl.openFileSystem(fsPath));
			Assert::IsTrue(impl.initializeFileSystem());
			Assert::IsFalse(impl.getDirectoriesList(L"/").empty());
			Assert::IsFalse(impl.getDirectoriesList(L"/fDir").empty());

			Assert::IsTrue(impl.removeDirectory(L"/fDir/sDir/"));
			Assert::IsTrue(impl.removeDirectory(L"/fDir"));
			// TODO add checks
			// for max directories in directory
			for (auto i = 0; i < 511; ++i)
			{
				std::wstring name = L"/dir" + std::to_wstring(i);
				Assert::IsTrue(impl.createDirectory(name));
			}
			Assert::IsFalse(impl.createDirectory(L"badDirectory"));
		}

		TEST_METHOD(FileOperations)
		{
			//create fs first
			Assert::IsTrue(impl.createFileSystem(fsPath, 1024, 500));
			Assert::IsTrue(impl.createFile(L"/testFile.txt"));
			Assert::IsTrue(impl.exists(L"/testFile.txt"));

			auto sizeToWrite = 512 * 5 + 1; 
			std::vector<char> buffer(sizeToWrite, 'l');
			buffer[0] = 's';
			buffer[sizeToWrite - 1] = 's';
			auto file = impl.openFile(L"/testFile.txt");
			Assert::IsTrue(file.isValid());
			Assert::IsTrue(impl.writeToFile(file, &buffer[0], sizeToWrite) == sizeToWrite);
			std::vector<char> readBuff(sizeToWrite, 'm');
			file.seekPos = file.fileLength - 1;
			Assert::IsTrue(impl.readFromFile(file, &readBuff[0], 1) == 1);
			Assert::IsTrue(impl.closeFileSystem());
		}
	};
}