#pragma once

#include "Defs.h"

namespace ScratchEngine
{
	namespace Memory
	{
		enum BlockStatus : u32
		{
			ALLOCATED = 0xa110ced,
			FREED = 0xf7ee,
		};

		struct Block
		{
			BlockStatus status;
			size_t size;
			union
			{
				void* data;
				struct
				{
					Block* parent;
					Block* left;
					Block* right;
					u32 height;
					Block* next;
					Block* previous;
				};
			};
		};
	}
}