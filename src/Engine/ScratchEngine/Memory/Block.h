#pragma once

#include "../Common/Typedefs.h"

#include "Allocator.h"

namespace ScratchEngine
{
	namespace Memory
	{
		struct Block					// 64 bytes
		{
			BlockStatus status;			// 8 bytes
			size_t size;				// 8 bytes
			union						// 48 bytes <-- actual data starts from here
			{
				byte data[1];

				struct
				{
					i64 height;
					Block* parent;
					Block* left;
					Block* right;
					Block* next;
					Block* previous;
				};
			};
		};

#define __data_offset (sizeof(BlockStatus) + sizeof(size_t))
#define __block_tail_size 16
#define __min_block_size (sizeof(Block) + __block_tail_size)
	}
}