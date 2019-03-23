#include <cstring>
#include <malloc.h>

#include "../Utility/Typedefs.h"

#include "Block.h"

namespace ScratchEngine
{
	namespace Memory
	{
		class DynamicPoolBlock
		{
		public:
			BlockStatus status : 32;
			i32 next : 32;
		};

		template<class T> class __declspec(dllexport) DynamicPoolAllocator
		{
		private:
			T* memory;

			i32 capacity;
			i32 numAllocated;
			i32 freeList;

		public:
			DynamicPoolAllocator(i32 capacity)
			{
				memory = reinterpret_cast<T*>(_aligned_malloc(capacity * sizeof(T), 16));

				reinterpret_cast<DynamicPoolBlock*>(memory + capacity - 1)->status = FREED;
				reinterpret_cast<DynamicPoolBlock*>(memory + capacity - 1)->next = null_index;

				for (i32 id = capacity - 2; id >= 0; --id)
				{
					reinterpret_cast<DynamicPoolBlock*>(memory + id)->status = FREED;
					reinterpret_cast<DynamicPoolBlock*>(memory + id)->next = id + 1;
				}

				this->capacity = capacity;
				numAllocated = 0;
				freeList = 0;
			}

			i32 Allocate()
			{
				if (freeList == null_index)
				{
					void* previousMemory = memory;
					i32 previousCapacity = capacity;

					capacity *= 2;

					memory = reinterpret_cast<T*>(_aligned_malloc(capacity * sizeof(T), 16));
					memcpy(memory, previousMemory, previousCapacity * sizeof(T));

					reinterpret_cast<DynamicPoolBlock*>(memory + capacity - 1)->status = FREED;
					reinterpret_cast<DynamicPoolBlock*>(memory + capacity - 1)->next = null_index;

					for (i32 id = capacity - 2; id >= previousCapacity; --id)
					{
						reinterpret_cast<DynamicPoolBlock*>(memory + id)->status = FREED;
						reinterpret_cast<DynamicPoolBlock*>(memory + id)->next = id + 1;
					}

					freeList = previousCapacity;

					_aligned_free(previousMemory);
				}

				i32 id = freeList;
				reinterpret_cast<DynamicPoolBlock*>(memory + id)->status = ALLOCATED;

				freeList = reinterpret_cast<DynamicPoolBlock*>(memory + id)->next;
				++numAllocated;

				return id;
			}

			__inline T& operator[](i32 id)
			{
				if (reinterpret_cast<DynamicPoolBlock*>(memory + id)->status == FREED)
					throw "[DynamicPoolAllocator] Invalid Access!";

				return memory[id];
			}

			__inline T operator[](i32 id) const
			{
				if (reinterpret_cast<DynamicPoolBlock*>(memory + id)->status == FREED)
					throw "[DynamicPoolAllocator] Invalid Access!";

				return memory[id];
			}

			void Free(i32 id)
			{
				reinterpret_cast<DynamicPoolBlock*>(memory + id)->status = FREED;
				reinterpret_cast<DynamicPoolBlock*>(memory + id)->next = freeList;

				freeList = id;
				--numAllocated;
			}
		};
	}
}