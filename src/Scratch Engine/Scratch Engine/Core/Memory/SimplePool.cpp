#include <cstdlib>

#include "SimplePool.h"

ScratchEngine::Memory::SimplePool::SimplePool(size_t objectSize, size_t capacity)
{
	register size_t size = objectSize * capacity;

	this->size = size;
	this->objectSize = objectSize;
	this->capcity = capacity;
	this->count = 0;

	memory = malloc(size);
}
