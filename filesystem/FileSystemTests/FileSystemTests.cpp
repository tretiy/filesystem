#include "stdafx.h"
#include "CppUnitTest.h"
#include "fileSystemImpl.h"
#include "supernode.h"
#include "blocksbitmap.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FileSystemTests
{
	TEST_CLASS(blocksbitmap_test)
	{
	public:
		blocksbitmap bitmap;

		TEST_METHOD(SaveToStream)
		{
			bitmap.blocksState[0] = true;
			bitmap.blocksState[255] = true;
			bitmap.blocksState[511] = true;
			Assert::IsTrue(bitmap.SaveToStream(std::fstream("C:\\Projects\\bitmap.txt", std::ios::binary | std::ios::out | std::ios::trunc)));
		}

		TEST_METHOD(LoadFromStream)
		{
			blocksbitmap loadedBitmap;
			loadedBitmap.LoadFromStream(std::fstream("C:\\Projects\\bitmap.txt", std::ios::binary | std::ios::in));
			Assert::IsTrue(loadedBitmap.blocksState[0] == true);
			Assert::IsTrue(loadedBitmap.blocksState[255] == true);
			Assert::IsTrue(loadedBitmap.blocksState[511] == true);
		}
	};


	TEST_CLASS(supernode_test)
	{
	public:
		supernode node;
				
		TEST_METHOD(SaveToStream)
		{
			
			Assert::IsTrue(node.SaveToStream(std::fstream("C:\\Projects\\supernode.txt", std::ios::binary | std::ios::out | std::ios::trunc)));
		}

		TEST_METHOD(LoadFromStream)
		{
			supernode loadedNode(1, 1, 1);
			loadedNode.LoadFromStream(std::fstream("C:\\Projects\\supernode.txt", std::ios::binary | std::ios::in));
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
		}
	};
}