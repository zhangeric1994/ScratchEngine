#ifndef LOCK_FREE_QUEUE_HPP
#define LOCK_FREE_QUEUE_HPP
#pragma once

#include "../header.h"

#include "../Common/Typedefs.h"
#include "../Memory/PoolAllocator.hpp"

using namespace ScratchEngine;
using namespace ScratchEngine::Memory;


namespace ScratchEngine
{
	namespace Multithreading
	{
		//template<class T> class LockFreeQueue;


		//template<class T> struct LockFreeQueueNode
		//{
		//	friend class LockFreeQueue<T>;


		//private:
		//	T* data;
		//	i32 next;
		//	i32 previous;
		//};


		template<class T> class LockFreeQueue
		{
		private:
			i8 pad0[hardware_destructive_interference_size];

			const u32 size;
			T* const data;

			alignas(hardware_destructive_interference_size) atomic<u32> readIndex;
			alignas(hardware_destructive_interference_size) atomic<u32> writeIndex;

			i8 pad1[hardware_destructive_interference_size - sizeof(atomic<u32>)];


		public:
			LockFreeQueue(u32 capacity);
			~LockFreeQueue();


			T* GetFront() const;
			u32 GetCapacity() const;
			u32 GetCount() const;

			bool Push(T const & data);
			template <class... ARGUMENTTYPE> bool Emplace(ARGUMENTTYPE... args);
			T Pop();
		};
	}
}


#include "LockFreeQueue.inl"

#endif