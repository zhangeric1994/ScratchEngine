#ifndef LOCK_FREE_QUEUE_INL
#define LOCK_FREE_QUEUE_INL
#pragma once


template<class T> inline ScratchEngine::Multithreading::LockFreeQueue<T>::LockFreeQueue(u32 capacity) : size(capacity + 1), data(reinterpret_cast<T*>(_aligned_malloc((capacity + 1) * sizeof(T), 16))), readIndex(0), writeIndex(0) //allocator(capacity)
{
}

template<class T> inline ScratchEngine::Multithreading::LockFreeQueue<T>::~LockFreeQueue()
{
	if (!is_trivially_destructible<T>::value)
		for (u32 i = readIndex; i != writeIndex; i = (i + 1) % size)
			data[i].~T();

	_aligned_free(data);
}

template<class T> inline T* ScratchEngine::Multithreading::LockFreeQueue<T>::GetFront() const
{
	u32 const currentReadIndex = readIndex.load(memory_order_relaxed);

	if (currentReadIndex == writeIndex.load(memory_order_acquire))
		return nullptr;


	return data[currentReadIndex];
}

template<class T> inline u32 ScratchEngine::Multithreading::LockFreeQueue<T>::GetCapacity() const
{
	return size - 1;
}

template<class T> inline u32 ScratchEngine::Multithreading::LockFreeQueue<T>::GetCount() const
{
	i32 d = writeIndex.load(memory_order_acquire) - readIndex.load(memory_order_acquire);

	return d < 0 ? d + size : d;
}

template<class T> inline bool ScratchEngine::Multithreading::LockFreeQueue<T>::Push(T const & data)
{
	u32 const currentWriteIndex = writeIndex.load(memory_order_relaxed);

	u32 nextWriteIndex = (currentWriteIndex + 1) % size;


	if (nextWriteIndex != readIndex.load(memory_order_acquire))
	{
		this->data[currentWriteIndex] = data;


		writeIndex.store(nextWriteIndex, memory_order_release);


		return true;
	}


	return false;
}

template<class T> template<class ...ARGUMENTTYPE> inline bool ScratchEngine::Multithreading::LockFreeQueue<T>::Emplace(ARGUMENTTYPE ...args)
{
	u32 const currentWriteIndex = writeIndex.load(memory_order_relaxed);

	u32 nextWriteIndex = (currentWriteIndex + 1) % size;


	if (nextWriteIndex != readIndex.load(memory_order_acquire))
	{
		new (&data[currentWriteIndex]) T(args...);


		writeIndex.store(nextWriteIndex, memory_order_release);


		return true;
	}


	return false;
}

template<class T> inline T ScratchEngine::Multithreading::LockFreeQueue<T>::Pop()
{
	u32 const currentReadIndex = readIndex.load(memory_order_relaxed);

	_ASSERT(currentReadIndex != writeIndex.load(memory_order_acquire));


	T result = data[currentReadIndex];

	data[currentReadIndex].~T();


	readIndex.store((currentReadIndex + 1) % size, memory_order_release);


	return result;
}

#endif