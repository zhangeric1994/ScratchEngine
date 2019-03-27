#include "CppUnitTest.h"
#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace ScratchEngine;
using namespace ScratchEngine::Physics;

namespace ScratchEngineTest
{
	TEST_CLASS(DynamicAABBTreeTest)
	{
		class AABBCollider : public AxisAlignedBoundingBox, public Collider
		{
		public:
			AABBCollider(f32 minX, f32 minY, f32 minZ, f32 maxX, f32 maxY, f32 maxZ)
			{
				this->minX = minX;
				this->minY = minY;
				this->minZ = minZ;
				this->maxX = maxX;
				this->maxY = maxY;
				this->maxZ = maxZ;
			}

			AxisAlignedBoundingBox GetBoundingVolume()
			{
				AxisAlignedBoundingBox aabb(min, max);
				return aabb;
			}
		};

		TEST_METHOD(NodeSize)
		{
			Assert::AreEqual((size_t)0, sizeof(DynamicAABBTreeNode) % 16);
		}

		TEST_METHOD(Insert1)
		{
			DynamicAABBTree tree;


			i32 node = tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));


			Assert::AreEqual(0, node);
		}

		TEST_METHOD(Insert2)
		{
			DynamicAABBTree tree;


			i32 node1 = tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node2 = tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node3 = tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node4 = tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node5 = tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));


			Assert::AreEqual(0, node1);
			Assert::AreEqual(1, node2);
			Assert::AreEqual(3, node3);
			Assert::AreEqual(5, node4);
			Assert::AreEqual(7, node5);
		}

		TEST_METHOD(Insert3)
		{
			DynamicAABBTree tree;


			for (int i = 0; i < 64; i++)
				tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));

			tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
		}

		TEST_METHOD(Remove1)
		{
			DynamicAABBTree tree;


			i32 node = tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			tree.Remove(node);
		}

		TEST_METHOD(Remove2)
		{
			DynamicAABBTree tree;


			i32 node1 = tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node2 = tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node3 = tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node4 = tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node5 = tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));

			Assert::AreEqual(0, node1);
			Assert::AreEqual(1, node2);
		}

		TEST_METHOD(InsertAfterRemove1)
		{
			DynamicAABBTree tree;


			i32 node1 = tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
			i32 node2 = tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));


			Assert::AreEqual(0, node1);
			Assert::AreEqual(1, node2);


			tree.Remove(node1);

			node1 = tree.Insert(new AABBCollider(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));


			Assert::AreEqual(2, node1);
		}
	};
}
