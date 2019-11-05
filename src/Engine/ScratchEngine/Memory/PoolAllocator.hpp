#ifndef POOL_ALLOCATOR_HPP
#define POOL_ALLOCATOR_HPP
#pragma once

#include "../header.h"

#include "../Common/Typedefs.h"

#include "Allocator.h"


namespace ScratchEngine
{
	namespace Memory
	{
		template<size_t BLOCKSIZE> class PoolAllocator;
		template<size_t BLOCKSIZE> struct PoolArena;


		struct PoolBlock
		{
			template<size_t S> friend class PoolAllocator;

			template<size_t S> friend struct PoolArena;


		private:
			PoolBlock* next;
			PoolBlock* previous;
		};


		template<size_t BLOCKSIZE> struct PoolArena
		{
			friend class PoolAllocator<BLOCKSIZE>;


		private:
			u32 capacity;
			u32 numAllocated;
			PoolArena* next;
			void* data;


			PoolArena(u32 capacity, PoolArena* next = nullptr);
			~PoolArena();


			void* operator new(size_t size);
			void operator delete(void* p);
		};


		template<size_t BLOCKSIZE> class __declspec(dllexport) PoolAllocator : Allocator
		{
		private:
			u32 capacity;
			u32 numAllocated;
			PoolArena<BLOCKSIZE>* arenaList;
			PoolBlock* freeList;


		public:
			PoolAllocator();
			PoolAllocator(u32 capacity);
			~PoolAllocator();


			void* Allocate(size_t size = BLOCKSIZE);
			void Free(void* p);
		};
	}
}


#include "PoolAllocator.inl"

#endif