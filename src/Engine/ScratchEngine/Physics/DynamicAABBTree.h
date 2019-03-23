#pragma once

#include "../Memory/DynamicPoolAllocator.h"

#include "DynamicAABBTreeNode.h"

using namespace ScratchEngine::Memory;

namespace ScratchEngine
{
	namespace Physics
	{
		class __declspec(dllexport) DynamicAABBTree
		{
		private:
			DynamicPoolAllocator<DynamicAABBTreeNode> allocator;

			size_t root;

		public:
			DynamicAABBTree();

			i32 Insert(Collider* collider);
			void Update(i32 id);
			void Remove(i32 id);

		private:
			__forceinline i32 _balance(i32 id);
			__forceinline void _update_box_and_height(DynamicAABBTreeNode& node);
		};
	}
}