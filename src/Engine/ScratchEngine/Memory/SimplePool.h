#pragma once

#include <queue>

#include "../Utility/Typedefs.h"

#include "SimpleBlock.h"

namespace ScratchEngine
{
	namespace Memory
	{
		class __declspec(dllexport) SimplePool
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
