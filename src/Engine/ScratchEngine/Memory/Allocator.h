#pragma once

namespace ScratchEngine
{
	namespace Memory
	{
		class Allocator
		{
		public:
			virtual void* Allocate(size_t size) = 0;
			virtual void Free(void* p) = 0;
		};
	}
}