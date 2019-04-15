#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#pragma once

#include "../Memory/DynamicPoolAllocator.hpp"

using namespace ScratchEngine::Memory;

namespace ScratchEngine
{
	template<class T> struct LinkedListNode
	{
		T data;
		i32 next;
		i32 previous;
	};

	template<class T> struct LinkedList
	{
	private:
		DynamicPoolAllocator<LinkedListNode<T>> allocator;
		i32 first;
		i32 last;
		i32 size;

	public:
		LinkedList();
		LinkedList(i32 size);

		T GetFirst() const;
		T GetLast() const;
		i32 GetSize() const;

		i32 AddFirst(T data);
		i32 AddLast(T data);
		T Remove(i32 id);
		T RemoveFirst();
		T RemoveLast();
	};


	template<class T> inline LinkedList<T>::LinkedList() : allocator()
	{
		first = null_index;
		last = null_index;
		size = 0;
	}

	template<class T> inline LinkedList<T>::LinkedList(i32 size) : allocator(size)
	{
		first = null_index;
		last = null_index;
		this->size = 0;
	}

	template<class T> inline T LinkedList<T>::GetFirst() const
	{
		if (size == 0)
			throw "[LinkedList] Invalid Access!";

		return allocator[first].data;
	}

	template<class T> inline T LinkedList<T>::GetLast() const
	{
		if (size == 0)
			throw "[LinkedList] Invalid Access!";

		return allocator[last].data;
	}

	template<class T> inline i32 LinkedList<T>::GetSize() const
	{
		return size;
	}

	template<class T> inline i32 LinkedList<T>::AddFirst(T data)
	{
		int id = allocator.Allocate();

		LinkedListNode<T> node = allocator[id];
		node.data = data;
		node->previous = null_index;
		node->next = first;

		first = id;

		if (++size == 1)
			last = id;
		else
			allocator[first].previous = id;

		return id;
	}

	template<class T> inline i32 LinkedList<T>::AddLast(T data)
	{
		int id = allocator.Allocate();

		LinkedListNode<T> node = allocator[id];
		node.data = data;
		node.previous = last;
		node.next = null_index;

		if (++size == 1)
			first = id;
		else
			allocator[last].next = id;

		last = id;

		return id;
	}

	template<class T> inline T LinkedList<T>::Remove(i32 id)
	{
		if (size == 0)
			throw "[LinkedList] Invalid Access!";

		LinkedListNode<T>& node = allocator[id];
		T data = node.data;
		i32 next = node.next;
		i32 previous = node.previous;

		if (previous == null_index)
			first = next;
		else
			allocator[previous].next = next;

		if (next == null_index)
			last = previous;
		else
			allocator[next].previous = previous;

		--size;

		allocator.Free(id);

		return data;
	}

	template<class T> inline T LinkedList<T>::RemoveFirst()
	{
		if (size == 0)
			throw "[LinkedList] Invalid Access!";

		i32 id = first;

		LinkedListNode<T>& node = allocator[id];
		T data = node.data;
		i32 next = node.next;

		first = next;

		if (next == null_index)
			last = null_index;
		else
			allocator[next].previous = null_index;

		--size;

		allocator.Free(id);

		return data;
	}

	template<class T> inline T LinkedList<T>::RemoveLast()
	{
		if (size == 0)
			throw "[LinkedList] Invalid Access!";

		i32 id = last;

		LinkedListNode<T>& node = allocator[id];
		T data = node.data;
		i32 previous = node.previous;

		if (previous == null_index)
			first = null_index;
		else
			allocator[previous].next = null_index;			

		last = previous;

		--size;

		allocator.Free(id);

		return data;
	}
}
#endif