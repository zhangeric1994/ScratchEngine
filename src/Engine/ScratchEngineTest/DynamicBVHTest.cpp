#include "CppUnitTest.h"
#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace ScratchEngine;
using namespace ScratchEngine::Physics;

namespace ScratchEngineTest
{
	TEST_CLASS(DynamicBVHTest)
	{
		TEST_METHOD(Insert1)
		{
			DynamicBVH<AxisAlignedBoundingBox> tree;


			i32 node = tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));


			Assert::AreEqual(0, node);
		}

		TEST_METHOD(Insert2)
		{
			DynamicBVH<AxisAlignedBoundingBox> tree;


			i32 node1 = tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node2 = tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node3 = tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node4 = tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node5 = tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));


			Assert::AreEqual(0, node1);
			Assert::AreEqual(1, node2);
			Assert::AreEqual(3, node3);
			Assert::AreEqual(5, node4);
			Assert::AreEqual(7, node5);
		}

		TEST_METHOD(Insert3)
		{
			DynamicBVH<AxisAlignedBoundingBox> tree;


			for (int i = 0; i < 64; i++)
				tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));

			tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
		}

		TEST_METHOD(Remove1)
		{
			DynamicBVH<AxisAlignedBoundingBox> tree;


			i32 node = tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			tree.Remove(node);
		}

		TEST_METHOD(Remove2)
		{
			DynamicBVH<AxisAlignedBoundingBox> tree;


			i32 node1 = tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node2 = tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node3 = tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node4 = tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node5 = tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));

			Assert::AreEqual(0, node1);
			Assert::AreEqual(1, node2);
		}

		TEST_METHOD(InsertAfterRemove1)
		{
			DynamicBVH<AxisAlignedBoundingBox> tree;


			i32 node1 = tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node2 = tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));


			Assert::AreEqual(0, node1);
			Assert::AreEqual(1, node2);


			tree.Remove(node1);

			node1 = tree.Insert(new AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), AxisAlignedBoundingBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));


			Assert::AreEqual(2, node1);
		}
	};
}
