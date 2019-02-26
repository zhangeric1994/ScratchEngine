#include <cstdlib>

#include "SimplePool.h"

ScratchEngine::Memory::SimplePool::SimplePool(size_t objectSize, size_t capacity)
{
	_ASSERT(objectSize >= sizeof(SimpleBlock));

	register size_t size = objectSize * capacity;

	this->size = size;
	this->objectSize = objectSize;
	this->capcity = capacity;
	this->count = 0;

	memory = _aligned_malloc(size, 16);

	freeList = reinterpret_cast<SimpleBlock*>(memory);
	freeList->previous = nullptr;

	register SimpleBlock* a = freeList;
	register SimpleBlock* b = reinterpret_cast<SimpleBlock*>(reinterpret_cast<uptr>(a) + objectSize);
	freeList = a;

	for (size_t i = 1; i < capacity; i++)
	{
		a->next = b;
		b->previous = a;

		a = b;
		b = reinterpret_cast<SimpleBlock*>(reinterpret_cast<uptr>(b) + objectSize);
	}

	b->next = nullptr;
}

ScratchEngine::Memory::SimplePool::~SimplePool()
{
	_aligned_free(memory);
}

void* ScratchEngine::Memory::SimplePool::Get()
{
	void* p = freeList;

	freeList = freeList->next;

	return p;
}

void ScratchEngine::Memory::SimplePool::Recycle(void* object)
{
	register SimpleBlock* b = reinterpret_cast<SimpleBlock*>(object);
	b->next = freeList;
	b->previous = nullptr;

	if (freeList)
	{
		freeList->previous = b;
	}

	freeList = b;
}
