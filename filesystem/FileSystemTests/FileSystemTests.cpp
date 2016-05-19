#include "stdafx.h"
#include "CppUnitTest.h"
#include "fileSystemImpl.h"
#include "supernode.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FileSystemTests
{		
	TEST_CLASS(supernode_test)
	{
	public:
		supernode node;
		supernode_test()
		{
			 node.blockSize = 1;
			 node.fileSystemSize = 2;
			 node.infosCount = 3;
		};
		
		TEST_METHOD(SaveToStream)
		{
			
			Assert::IsTrue(node.SaveToStream(std::fstream("C:\\Projects\\supernode.txt", std::ios::binary | std::ios::out | std::ios::trunc)));
		}

		TEST_METHOD(LoadFromStream)
		{
			supernode loadedNode(node.LoadFromStream(std::fstream("C:\\Projects\\supernode.txt", std::ios::binary | std::ios::in)));
			Assert::IsTrue(loadedNode.blockSize == node.blockSize);
			Assert::IsTrue(loadedNode.fileSystemSize == node.fileSystemSize);
			Assert::IsTrue(loadedNode.infosCount == node.infosCount);
		}
	};

	TEST_CLASS(fileSystemImpl_test)
	{
	public:
		
		fileSystemImpl impl;

		TEST_METHOD(OpenFileSystem)
		{
			Assert::IsTrue(impl.OpenFileSystem("C:\\Projects\\filesystem.txt"));
		}

		TEST_METHOD(CloseFileSystem)
		{
			Assert::IsTrue(impl.CloseFileSystem());
		}
	};
}