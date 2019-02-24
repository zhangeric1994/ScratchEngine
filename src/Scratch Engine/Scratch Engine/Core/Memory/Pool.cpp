#include <exception>

#include "Pool.h"

ScratchEngine::Memory::Pool::Pool(size_t size)
{
	memory = malloc(size);

	Block* block = reinterpret_cast<Block*>(memory);

	block->status = FREED;
	block->size = size;
	block->parent = block;
	block->left = nullptr;
	block->right = nullptr;
	block->height = 0;
	block->next = block;
	block->previous = block;

	*reinterpret_cast<Block**>(reinterpret_cast<byte*>(block) + block->size - sizeof(Block*)) = block;

	capacity = size;
	this->size = 0;
	numBlocks = 1;
	numAllocated = 0;
	root = block;
}

ScratchEngine::Memory::Pool::~Pool()
{
	free(memory);
}

void* ScratchEngine::Memory::Pool::Allocate(size_t size)
{
	size = __max(__min_block_size, size + __data_offset + sizeof(Block*));

	register Block* t = root;
	register Block* b = nullptr;

	/* Try to find a memory block with the same size */
	while (t)
	{
		if (size == t->size)
		{
			t = t->next;

			RemoveFromPool(this, t);

			t->status = ALLOCATED;

			numAllocated++;
			this->size += size;

			return t->data;
		}
		else if (size < t->size)
		{
			b = t->next;
			t = t->left;
		}
		else
			t = t->right;
	}

	/* Use the best-fit strategy if there is no memory block with the same size is found */
	if (b)
	{
		RemoveFromPool(this, b);

		b->status = ALLOCATED;

		if (b->size >= size + __min_block_size)
		{
			t = reinterpret_cast<Block*>(reinterpret_cast<byte*>(b) + size);

			t->status = FREED;
			t->size = b->size - size;
			*reinterpret_cast<Block**>(reinterpret_cast<byte*>(t) + t->size - sizeof(Block*)) = t;

			root = AddToTree(root, t);
			root->parent = root;

			b->size = size;
			*reinterpret_cast<Block**>(reinterpret_cast<byte*>(b) + b->size - sizeof(Block*)) = b;

			numBlocks++;
		}

		numAllocated++;
		this->size += b->size;

		return b->data;
	}

	/* Throw an error if there is no best-fit memory block (the required size is larger than that of any available blocks) */
	throw new exception("Not enough memory.");
}

void ScratchEngine::Memory::Pool::Free(void* p)
{
	Block* block = reinterpret_cast<Block*>(reinterpret_cast<byte*>(p) - __data_offset);

	block->status = FREED;

	numAllocated--;
	size -= block->size;

	AddToPool(this, block);
}
