#ifndef POOL_ALLOCATOR_INL
#define POOL_ALLOCATOR_INL
#pragma once

using namespace ScratchEngine::Memory;


template<size_t BLOCKSIZE> inline ScratchEngine::Memory::PoolArena<BLOCKSIZE>::PoolArena(u32 capacity, PoolArena* next)
{
	this->capacity = capacity;
	this->numAllocated = 0;
	this->next = next;
	this->data = _aligned_malloc(BLOCKSIZE * capacity, 16);


	PoolBlock* previousBlock = reinterpret_cast<PoolBlock*>(data);

	_ASSERT(previousBlock);

	previousBlock->previous = nullptr;

	for (i32 i = 1; i < capacity; ++i)
	{
		PoolBlock* block = reinterpret_cast<PoolBlock*>(reinterpret_cast<i8*>(data) + i * BLOCKSIZE);

		block->next = previousBlock;
		previousBlock->next = block;


		previousBlock = block;
	}

	previousBlock->next = nullptr;
}

template<size_t BLOCKSIZE> inline ScratchEngine::Memory::PoolArena<BLOCKSIZE>::~PoolArena()
{
	_aligned_free(data);
}

template<size_t BLOCKSIZE> inline void* ScratchEngine::Memory::PoolArena<BLOCKSIZE>::operator new(size_t size)
{
	return _aligned_malloc(size, 16);
}

template<size_t BLOCKSIZE> inline void ScratchEngine::Memory::PoolArena<BLOCKSIZE>::operator delete(void* p)
{
	return _aligned_free(p);
}


template<size_t BLOCKSIZE> inline ScratchEngine::Memory::PoolAllocator<BLOCKSIZE>::PoolAllocator() : PoolAllocator(16)
{
}

template<size_t BLOCKSIZE> inline ScratchEngine::Memory::PoolAllocator<BLOCKSIZE>::PoolAllocator(u32 capacity)
{
	_ASSERT(BLOCKSIZE >= sizeof(PoolBlock) && BLOCKSIZE % 4 == 0);


	this->capacity = capacity;
	this->numAllocated = 0;


	if (capacity == 0)
	{
		this->arenaList = nullptr;
		this->freeList = nullptr;
	}
	else
	{
		this->arenaList = new PoolArena<BLOCKSIZE>(capacity);
		this->freeList = reinterpret_cast<PoolBlock*>(arenaList->data);
	}
}

template<size_t BLOCKSIZE> inline ScratchEngine::Memory::PoolAllocator<BLOCKSIZE>::~PoolAllocator()
{
	PoolArena<BLOCKSIZE>* arena = arenaList;

	while (arena)
	{
		PoolArena<BLOCKSIZE>* nextArena = arena->next;

		delete arena;

		arena = nextArena;
	}
}

template<size_t BLOCKSIZE> inline void* ScratchEngine::Memory::PoolAllocator<BLOCKSIZE>::Allocate(size_t size)
{
	_ASSERT(size <= BLOCKSIZE);


	if (!freeList)
	{
		if (capacity == 0)
		{
			arenaList = new PoolArena<BLOCKSIZE>(16, arenaList);
			freeList = reinterpret_cast<PoolBlock*>(arenaList->data);
			capacity = 16;
		}
		else
		{
			arenaList = new PoolArena<BLOCKSIZE>(capacity, arenaList);
			freeList = reinterpret_cast<PoolBlock*>(arenaList->data);
			capacity *= 2;
		}
	}


	PoolBlock* block = freeList;


	++numAllocated;
	freeList = freeList->next;


	return block;
}

template<size_t BLOCKSIZE> inline void ScratchEngine::Memory::PoolAllocator<BLOCKSIZE>::Free(void* p)
{
	PoolBlock* block = reinterpret_cast<PoolBlock*>(p);
	block->next = freeList;
	block->previous = nullptr;


	if (freeList)
		freeList->previous = block;

	freeList = block;
}

#endif