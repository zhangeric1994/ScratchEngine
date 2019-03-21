#pragma once

#include <queue>

#include "../Utility/Typedefs.h"

#include "Allocator.h"
#include "SimpleBlock.h"

namespace ScratchEngine
{
	namespace Memory
	{
		template<size_t BLOCKSIZE>
		class __declspec(dllexport) PoolAllocator : Allocator
		{
		private:
			void* memory;

			size_t numAllocated;
			size_t capcity;
			size_t count;

			SimpleBlock* freeList;

		public:
			PoolAllocator(size_t capacity)
			{
				_ASSERT(BLOCKSIZE >= sizeof(SimpleBlock));

				register size_t numAllocated = BLOCKSIZE * capacity;

				this->numAllocated = numAllocated;
				this->capcity = capacity;
				this->count = 0;

				memory = _aligned_malloc(numAllocated, 16);

				freeList = reinterpret_cast<SimpleBlock*>(memory);
				freeList->previous = nullptr;

				register SimpleBlock* a = freeList;
				register SimpleBlock* b = reinterpret_cast<SimpleBlock*>(reinterpret_cast<uptr>(a) + BLOCKSIZE);
				freeList = a;

				for (size_t i = 1; i < capacity; i++)
				{
					a->next = b;
					b->previous = a;

					a = b;
					b = reinterpret_cast<SimpleBlock*>(reinterpret_cast<uptr>(b) + BLOCKSIZE);
				}

				b->next = nullptr;
			}

			~PoolAllocator()
			{
				_aligned_free(memory);
			}

			void* Allocate(size_t numAllocated = BLOCKSIZE)
			{
				void* p = freeList;

				freeList = freeList->next;

				return p;
			}

			void Free(void* p)
			{
				register SimpleBlock* b = reinterpret_cast<SimpleBlock*>(p);
				b->next = freeList;
				b->previous = nullptr;

				if (freeList)
				{
					freeList->previous = b;
				}

				freeList = b;
			}
		};
	}
}
