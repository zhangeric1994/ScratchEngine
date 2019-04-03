#pragma once

#include <cstring>
#include <malloc.h>

#include "../Common/Settings.h"
#include "../Common/Typedefs.h"

#include "Allocator.h"
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


		template<class T> struct __declspec(dllexport) DynamicPoolPointer : DynamicAllocator
		{
			template<class T> friend class DynamicPoolAllocator;

		private:
			DynamicPoolAllocator<T>* allocator;
			i32 id;

		public:
			T* operator->();
		};


		template<class T> class __declspec(dllexport) DynamicPoolAllocator
		{
			template<class T> friend struct DynamicPoolPointer;

		private:
			T* memory;

			i32 capacity;
			i32 numAllocated;
			i32 freeList;


		public:
			DynamicPoolAllocator();
			DynamicPoolAllocator(i32 capacity);
			~DynamicPoolAllocator();

			i32 Allocate()
			{
				if (freeList == null_index)
				{
					i32 previousCapacity = capacity;

					if (capacity == 0)
						capacity = DEFAULT_DYNAMIC_POOL_ALLOCATOR_CAPACITY;
					else
						capacity *= 2;

					memory = reinterpret_cast<T*>(_aligned_realloc(memory, capacity * sizeof(T), 16));

					reinterpret_cast<DynamicPoolBlock*>(memory + capacity - 1)->status = FREED;
					reinterpret_cast<DynamicPoolBlock*>(memory + capacity - 1)->next = null_index;

					for (i32 id = capacity - 2; id >= previousCapacity; --id)
					{
						reinterpret_cast<DynamicPoolBlock*>(memory + id)->status = FREED;
						reinterpret_cast<DynamicPoolBlock*>(memory + id)->next = id + 1;
					}

					freeList = previousCapacity;
				}

				i32 id = freeList;
				reinterpret_cast<DynamicPoolBlock*>(memory + id)->status = ALLOCATED;

				freeList = reinterpret_cast<DynamicPoolBlock*>(memory + id)->next;
				++numAllocated;

				return id;
			}

			//DynamicPoolPointer<T> AllocateDynamicPoolPointer()
			//{
			//	if (freeList == null_index)
			//	{
			//		void* previousMemory = memory;
			//		i32 previousCapacity = capacity;

			//		capacity *= 2;

			//		memory = reinterpret_cast<T*>(_aligned_malloc(capacity * sizeof(T), 16));
			//		memcpy(memory, previousMemory, previousCapacity * sizeof(T));

			//		reinterpret_cast<DynamicPoolBlock*>(memory + capacity - 1)->status = FREED;
			//		reinterpret_cast<DynamicPoolBlock*>(memory + capacity - 1)->next = null_index;

			//		for (i32 id = capacity - 2; id >= previousCapacity; --id)
			//		{
			//			reinterpret_cast<DynamicPoolBlock*>(memory + id)->status = FREED;
			//			reinterpret_cast<DynamicPoolBlock*>(memory + id)->next = id + 1;
			//		}

			//		freeList = previousCapacity;

			//		_aligned_free(previousMemory);
			//	}

			//	i32 id = freeList;
			//	reinterpret_cast<DynamicPoolBlock*>(memory + id)->status = ALLOCATED;

			//	freeList = reinterpret_cast<DynamicPoolBlock*>(memory + id)->next;
			//	++numAllocated;

			//	return { this, id };
			//}
				
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

		template<class T>inline T * DynamicPoolPointer<T>::operator->()
		{
			return allocator.memory + id;
		}

		template<class T> inline DynamicPoolAllocator<T>::DynamicPoolAllocator() : DynamicPoolAllocator(DEFAULT_DYNAMIC_POOL_ALLOCATOR_CAPACITY)
		{
		}

		template<class T> inline DynamicPoolAllocator<T>::DynamicPoolAllocator(i32 capacity)
		{
			if (capacity == 0)
				memory == nullptr;
			else
			{
				memory = reinterpret_cast<T*>(_aligned_malloc(capacity * sizeof(T), 16));

				reinterpret_cast<DynamicPoolBlock*>(memory + capacity - 1)->status = FREED;
				reinterpret_cast<DynamicPoolBlock*>(memory + capacity - 1)->next = null_index;

				for (i32 id = capacity - 2; id >= 0; --id)
				{
					reinterpret_cast<DynamicPoolBlock*>(memory + id)->status = FREED;
					reinterpret_cast<DynamicPoolBlock*>(memory + id)->next = id + 1;
				}
			}

			this->capacity = capacity;
			numAllocated = 0;
			freeList = 0;
		}

		template<class T> inline DynamicPoolAllocator<T>::~DynamicPoolAllocator()
		{
			_aligned_free(memory);
		}
	}
}