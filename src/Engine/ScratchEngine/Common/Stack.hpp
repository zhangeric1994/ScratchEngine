#pragma once

#include "../header.h"

#include "../Common/Typedefs.h"


namespace ScratchEngine
{
	template<class T> class Stack
	{
	private:
		T* memory;
		i32 capacity;
		i32 top;

	public:
		Stack();
		Stack(i32 capacity);
		~Stack();

		i32 GetSize() const;

		const T& operator[](i32 id) const;

		i32 Push(T data);
		T Pop();
		void Clear();
	};

	template<class T> inline Stack<T>::Stack() : Stack(16)
	{
	}

	template<class T> inline Stack<T>::Stack(i32 capacity)
	{
		this->capacity = capacity;
		this->memory = reinterpret_cast<T*>(_aligned_malloc(capacity * sizeof(T), 16));
		this->top = null_index;
	}

	template<class T> inline Stack<T>::~Stack()
	{
		_aligned_free(memory);
	}

	template<class T> inline i32 Stack<T>::GetSize() const
	{
		return top + 1;
	}

	template<class T>
	inline const T& Stack<T>::operator[](i32 id) const
	{
		if (id > top)
			throw "[Stack] Invalid Access!";

		return memory[id];
	}

	template<class T> inline i32 Stack<T>::Push(T data)
	{
		if (++top == capacity)
		{
			capacity *= 2;
			memory = reinterpret_cast<T*>(_aligned_realloc(memory, capacity * sizeof(T), 16));
		}

		memory[top] = data;

		return top;
	}

	template<class T> inline T Stack<T>::Pop()
	{
		if (top == null_index)
			throw "[Stack] Invalid Access!";

		return memory[top--];
	}

	template<class T> inline void Stack<T>::Clear()
	{
		top = null_index;
	}
}