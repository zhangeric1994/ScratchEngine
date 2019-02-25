#pragma once

#include "../Typedefs.h"

namespace ScratchEngine
{
	namespace Memory
	{
		class SimplePool
		{
		private:
			size_t size;
			size_t objectSize;
			size_t capcity;
			size_t count;

			void* memory;

		public:
			SimplePool(size_t objectSize, size_t capacity);
		};
	}
}
