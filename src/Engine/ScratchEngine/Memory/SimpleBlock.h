#pragma once

namespace ScratchEngine
{
	namespace Memory
	{
		struct SimpleBlock				// 16 bytes
		{
			SimpleBlock* next;			// 8 bytes
			SimpleBlock* previous;		// 8 bytes
		};
	}
}