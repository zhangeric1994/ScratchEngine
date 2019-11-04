#pragma once

namespace ScratchEngine
{
	namespace Memory
	{
		enum BlockStatus : u64
		{
			ALLOCATED = 0xa110ced,
			FREED = 0xf7ee,
		};


		class Allocator
		{
		public:
			virtual void* Allocate(size_t size) = 0;
			virtual void Free(void* p) = 0;
		};


		class DynamicAllocator
		{
		public:
			virtual i32 Allocate() = 0;
			virtual void Free(i32 id) = 0;
		};
	}
}