#include "stdafx.h"
#include "CppUnitTest.h"
#include "fileSystemImpl.h"
#include "FileSystemHeader.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace filesystem;

namespace FileSystemTests
{
	TEST_CLASS(FileSystemHeader_test)
	{
	public:
		FileSystemHeader node;
				
		TEST_METHOD(SaveToStream)
		{
			Assert::IsTrue(node.SaveToStream(std::fstream("C:\\Projects\\FileSystemHeader.txt", std::ios::binary | std::ios::out | std::ios::trunc)));
		}

		TEST_METHOD(LoadFromStream)
		{
			FileSystemHeader loadedNode(1, 1, 1);
			loadedNode.LoadFromStream(std::fstream("C:\\Projects\\FileSystemHeader.txt", std::ios::binary | std::ios::in));
			Assert::IsTrue(loadedNode == node);
		}
	};

	TEST_CLASS(fileSystemImpl_test)
	{
	public:
		
		fileSystemImpl impl;

		TEST_METHOD(CloseFileSystem)
		{
			Assert::IsTrue(impl.OpenFileSystem("C:\\Projects\\filesys.txt"));
			Assert::IsTrue(impl.CloseFileSystem());
		}

		TEST_METHOD(InitializeFileSytem)
		{
			Assert::IsTrue(impl.OpenFileSystem("C:\\Projects\\filesys.txt"));
			Assert::IsTrue(impl.InitializeFileSystem());
			Assert::IsTrue(impl.CloseFileSystem());
		}

		TEST_METHOD(CreateFileSystem)
		{
			Assert::IsTrue(impl.CreateFileSystem("C:\\Projects\\newFilesys.txt", 512, 10000));
			Assert::IsTrue(impl.CloseFileSystem());
		}
	};
}