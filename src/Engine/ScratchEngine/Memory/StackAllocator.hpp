#ifndef STACK_ALLOCATOR_HPP
#define STACK_ALLOCATOR_HPP
#pragma once

#include <algorithm>

#include "../Common/Settings.h"

#include "Allocator.h"

namespace ScratchEngine
{
	namespace Memory
	{
		template<class T> class DynamicStackAllocator : DynamicAllocator
		{
		private:
			T* memory;
			i32 capacity;
			i32 numAllocated;


		public:
			DynamicStackAllocator();
			DynamicStackAllocator(i32 capacity);
			~DynamicStackAllocator();

			T& operator[](i32 id);
			T operator[](i32 id) const;

			i32 GetCapacity() const;
			i32 GetNumAllocated() const;
			void* GetMemoryAddress();

			i32 Allocate();
			
			void Free(i32 id);
			void Flush();

			void Sort(bool(*compare)(T, T));
			void Sort(bool(*compare)(T, T), i32 begin, i32 end);
		};

		template<class T> inline DynamicStackAllocator<T>::DynamicStackAllocator() : DynamicStackAllocator(DEFAULT_DYNAMIC_STACK_ALLOCATOR_CAPACITY)
		{
		}

		template<class T> inline DynamicStackAllocator<T>::DynamicStackAllocator(i32 capacity)
		{
			memory = capacity == 0 ? nullptr : reinterpret_cast<T*>(_aligned_malloc(capacity * sizeof(T), 16));

			this->capacity = capacity;
			this->numAllocated = 0;
		}

		template<class T> inline DynamicStackAllocator<T>::~DynamicStackAllocator()
		{
			_aligned_free(memory);
		}

		template<class T> inline T & DynamicStackAllocator<T>::operator[](i32 id)
		{
			if (id < 0 || id >= numAllocated)
				throw "[DynamicStackAllocator] Invalid Access!";

			return memory[id];
		}

		template<class T> inline T DynamicStackAllocator<T>::operator[](i32 id) const
		{
			if (id < 0 || id >= numAllocated)
				throw "[DynamicStackAllocator] Invalid Access!";

			return memory[id];
		}

		template<class T> inline i32 DynamicStackAllocator<T>::GetCapacity() const
		{
			return capacity;
		}

		template<class T> inline i32 DynamicStackAllocator<T>::GetNumAllocated() const
		{
			return numAllocated;
		}

		template<class T> inline void * DynamicStackAllocator<T>::GetMemoryAddress()
		{
			return memory;
		}

		template<class T> inline i32 DynamicStackAllocator<T>::Allocate()
		{
			if (numAllocated == capacity)
			{
				if (capacity == 0)
					capacity = DEFAULT_DYNAMIC_STACK_ALLOCATOR_CAPACITY;
				else
					capacity *= 2;

				memory = reinterpret_cast<T*>(_aligned_realloc(memory, capacity * sizeof(T), 16));
			}

			return numAllocated++;
		}

		template<class T> inline void DynamicStackAllocator<T>::Free(i32 n)
		{
			numAllocated -= n;
		}

		template<class T> inline void DynamicStackAllocator<T>::Flush()
		{
			numAllocated = 0;
		}

		template<class T> inline void DynamicStackAllocator<T>::Sort(bool(*compare)(T, T))
		{
			Sort(compare, 0, numAllocated);
		}

		template<class T> inline void DynamicStackAllocator<T>::Sort(bool (*compare)(T, T), i32 begin, i32 end)
		{
			sort(memory + begin, memory + end, compare);
		}
	}
}
#endif