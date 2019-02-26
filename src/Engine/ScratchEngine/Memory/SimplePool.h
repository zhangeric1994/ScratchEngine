#pragma once

#include <queue>

#include "SimpleBlock.h"
#include "../Utility/Typedefs.h"

namespace ScratchEngine
{
	namespace Memory
	{
		class SimplePool
		{
		private:
			void* memory;

			size_t size;
			size_t objectSize;
			size_t capcity;
			size_t count;

			SimpleBlock* freeList;

		public:
			SimplePool(size_t objectSize, size_t capacity);
			~SimplePool();

			void* Get();
			void Recycle(void* object);
		};
	}
}
