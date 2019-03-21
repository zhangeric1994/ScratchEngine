#pragma once

#include "DynamicAABBTreeNode.h"

namespace ScratchEngine
{
	namespace Physics
	{
		class DynamicAABBTree
		{
		private:
			DynamicAABBTreeNode* memory;
			i32 freeList : 32;
			i32 numAllocated : 32;
			i32 capacity : 32;
			i32 root : 32;

		public:
			DynamicAABBTree();

			i32 Insert(Collider* collider);
			void Update(i32 id);
			void Remove(i32 id);

		private:
			__forceinline i32 _allocate_node(Collider* collider = nullptr);
			__forceinline DynamicAABBTreeNode* _get_node(i32 id);
			__forceinline void _free_node(i32 id);

			__forceinline i32 _balance(i32 id);
			__forceinline void _update_box_and_height(DynamicAABBTreeNode* node);
		};
	}
}