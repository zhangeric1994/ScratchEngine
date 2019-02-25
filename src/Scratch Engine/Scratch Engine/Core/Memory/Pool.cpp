#include <exception>

#include "Pool.h"

ScratchEngine::Memory::Pool::Pool()
{
	memory = nullptr;
	capacity = 0;
	this->size = 0;
	numBlocks = 0;
	numAllocated = 0;
	root = nullptr;
}

ScratchEngine::Memory::Pool::Pool(size_t size)
{
	memory = _aligned_malloc(size, 16);

	Block* block = reinterpret_cast<Block*>(memory);

	block->status = FREED;
	block->size = size;
	block->parent = block;
	block->left = nullptr;
	block->right = nullptr;
	block->height = 1;
	block->next = block;
	block->previous = block;

	*reinterpret_cast<Block**>(reinterpret_cast<uptr>(block) + block->size - __block_tail_size) = block;

	capacity = size;
	this->size = 0;
	numBlocks = 1;
	numAllocated = 0;
	root = block;
}

ScratchEngine::Memory::Pool::~Pool()
{
	_aligned_free(memory);
}

void* ScratchEngine::Memory::Pool::Allocate(size_t size)
{
	size = __max(__min_block_size, __data_offset + size + __block_tail_size);

	register Block* t = root;
	register Block* b = nullptr;

	/* Try to find a memory block with the same size */
	while (t)
	{
		if (size == t->size)
		{
			t = t->next;

			Remove(t);

			t->status = ALLOCATED;

			numAllocated++;
			this->size += size;

			return reinterpret_cast<void*>(t->data);
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
		Remove(b);

		b->status = ALLOCATED;

		if (b->size >= size + __min_block_size)
		{
			t = reinterpret_cast<Block*>(reinterpret_cast<uptr>(b) + size);

			t->status = FREED;
			t->size = b->size - size;
			*reinterpret_cast<Block**>(reinterpret_cast<uptr>(t) + t->size - __block_tail_size) = t;

			root = AddToTree(root, t);
			root->parent = root;

			b->size = size;
			*reinterpret_cast<Block**>(reinterpret_cast<uptr>(b) + b->size - __block_tail_size) = b;

			numBlocks++;
		}

		numAllocated++;
		this->size += b->size;

		return reinterpret_cast<void*>(b->data);
	}

	/* Throw an error if there is no best-fit memory block (the required size is larger than that of any available blocks) */
	throw new exception("Not enough memory.");
}

void ScratchEngine::Memory::Pool::Free(void* p)
{
	Block* block = reinterpret_cast<Block*>(reinterpret_cast<uptr>(p) - __data_offset);

	block->status = FREED;

	numAllocated--;
	size -= block->size;

	Add(block);
}

void ScratchEngine::Memory::Pool::Add(Block * block)
{
	/* Try to merge with the block right after */
	register uptr p = reinterpret_cast<uptr>(block) + block->size;

	if (p < reinterpret_cast<uptr>(memory) + capacity)
	{
		Block* blockAfter = reinterpret_cast<Block*>(p);
		if (blockAfter->status == FREED)
		{
			Remove(blockAfter);
			block->size += blockAfter->size;
			numBlocks--;
		}
	}

	/* Try to merge with the block right before */
	p = reinterpret_cast<uptr>(block) - __block_tail_size;

	if (p > reinterpret_cast<uptr>(memory))
	{
		Block* blockBefore = reinterpret_cast<Block*>(p);
		if (block->status == FREED)
		{
			Remove(blockBefore);
			blockBefore->size += block->size;
			block = blockBefore;
			numBlocks--;
		}
	}

	/* Update the tail of the block */
	*reinterpret_cast<Block**>(reinterpret_cast<uptr>(block) + block->size - __block_tail_size) = block;

	AddToTree(root, block);
}

void ScratchEngine::Memory::Pool::Remove(Block * block)
{
	Block* nextBlock = block->next;
	Block* parentBlock = block->parent;

	/* Adjust the tree if the block is a node */
	if (parentBlock)
	{
		if (block->next != block) // The block can be substituted by another one so that the node need not to be removed from the tree
		{
			if (parentBlock != block) // The block is not the root of the tree
			{
				if (nextBlock->size < parentBlock->size)
					parentBlock->left = nextBlock;
				else
					parentBlock->right = nextBlock;

				nextBlock->parent = parentBlock;
			}
			else
				root = nextBlock;

			nextBlock->left = block->left;
			nextBlock->right = block->right;

			if (block->left)
				block->left->parent = nextBlock;
			if (block->right)
				block->right->parent = nextBlock;
		}
		else
		{
			root = RemoveFromTree(root, block);
			if (root)
				root->parent = root;
		}
	}

	/* Remove the block from its linked list */
	nextBlock->previous = block->previous;
	block->previous->next = nextBlock;
}
