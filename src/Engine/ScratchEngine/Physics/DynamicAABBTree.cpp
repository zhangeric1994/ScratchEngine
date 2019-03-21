#include <cstring>

#include "DynamicAABBTree.h"

ScratchEngine::Physics::DynamicAABBTree::DynamicAABBTree()
{
	numAllocated = 0;
	capacity = 128;

	memory = reinterpret_cast<DynamicAABBTreeNode*>(malloc(capacity * sizeof(DynamicAABBTreeNode)));

	_get_node(capacity - 1)->next = null_index;
	for (i32 id = capacity - 2; id >= 0; --id)
		_get_node(id)->next = id + 1;

	freeList = 0;
	root = null_index;
}

ScratchEngine::i32 ScratchEngine::Physics::DynamicAABBTree::Insert(Collider* collider)
{
	i32 iLeaf = _allocate_node(collider);
	DynamicAABBTreeNode* leaf = _get_node(iLeaf);

	if (!root)
		root = iLeaf;
	else
	{
		i32 iCurrentNode = root;
		DynamicAABBTreeNode* currentNode = _get_node(iCurrentNode);

		i32 iL;
		i32 iR;
		DynamicAABBTreeNode* l;
		DynamicAABBTreeNode* r;
		float v, cost, costI, costL, costR;
		while (!currentNode->IsLeaf())
		{
			iL = currentNode->left;
			iR = currentNode->right;

			l = _get_node(iL);
			r = _get_node(iR);

			v = currentNode->GetUnionVolume(leaf);

			cost = 2 * v;
			costI = 2 * (v - currentNode->GetVolume());

			if (l->IsLeaf())
				costL = l->GetUnionVolume(leaf) + costI;
			else
				costL = (l->GetUnionVolume(leaf) - l->GetVolume()) + costI;

			if (l->IsLeaf())
				costR = r->GetUnionVolume(leaf) + costI;
			else
				costR = (r->GetUnionVolume(leaf) - r->GetVolume()) + costI;

			if (cost < costL && cost < costR)
				break;

			if (costL < costR)
				currentNode = l;
			else
				currentNode = r;
		}
		
		i32 iNode = _allocate_node();
		DynamicAABBTreeNode* node = _get_node(iNode);
		node->parent = currentNode->parent;
		node->left = iCurrentNode;
		node->right = iLeaf;

		_update_box_and_height(node);

		if (node->parent == null_index)
			root = iNode;
		else
		{
			if (_get_node(node->parent)->left == iCurrentNode)
				_get_node(node->parent)->left = iNode;
			else
				_get_node(node->parent)->right = iNode;
		}			

		currentNode->parent = iNode;
		leaf->parent = iNode;

		for (iCurrentNode = iNode; iCurrentNode != null_index; iCurrentNode = currentNode->parent)
		{
			iCurrentNode = _balance(iCurrentNode);

			currentNode = _get_node(iCurrentNode);
			_update_box_and_height(currentNode);
		}
	}

	return iLeaf;
}

void ScratchEngine::Physics::DynamicAABBTree::Update(i32 id)
{
}

void ScratchEngine::Physics::DynamicAABBTree::Remove(i32 id)
{
	if (id == root)
	{
		root = null_index;
		_free_node(id);
	}
	else
	{
		DynamicAABBTreeNode* node = _get_node(id);

		i32 iParent = node->parent;
		DynamicAABBTreeNode* parent = _get_node(iParent);

		i32 iGrandParent = parent->parent;
		
		i32 iSibling = parent->left == id ? parent->right : parent->left;
		DynamicAABBTreeNode* sibling = _get_node(iSibling);

		sibling->parent = iGrandParent;

		if (iGrandParent == null_index)
			root = iSibling;
		else
		{
			DynamicAABBTreeNode* grandParent = _get_node(iGrandParent);

			if (grandParent->left == iParent)
				grandParent->left = iSibling;
			else
				grandParent->right = iSibling;
			
			DynamicAABBTreeNode* currentNode;
			for (i32 iCurrentNode = iGrandParent; iCurrentNode != null_index; iCurrentNode = currentNode->parent)
			{
				iCurrentNode = _balance(iCurrentNode);

				currentNode = _get_node(iCurrentNode);
				_update_box_and_height(currentNode);
			}
		}

		_free_node(iParent);
	}
}

ScratchEngine::i32 ScratchEngine::Physics::DynamicAABBTree::_allocate_node(Collider* collider)
{
	if (freeList == null_index)
	{
		i32 previousCapacity = capacity;
		DynamicAABBTreeNode* previousMemory = memory;

		capacity += 128;

		memory = reinterpret_cast<DynamicAABBTreeNode*>(malloc(capacity * sizeof(DynamicAABBTreeNode)));

		memcpy(memory, previousMemory, previousCapacity * sizeof(DynamicAABBTreeNode));

		_get_node(capacity - 1)->next = null_index;
		for (i32 id = capacity - 2; id >= previousCapacity; --id)
			_get_node(id)->next = id + 1;

		freeList = previousCapacity;
	}

	i32 id = freeList;
	DynamicAABBTreeNode* node = _get_node(id);
	node->box = collider->GetBoundingVolume();
	node->collider = collider;
	node->height = 0;
	node->parent = null_index;
	node->left = null_index;
	node->right = null_index;

	freeList = node->next;
	++numAllocated;

	return id;
}

ScratchEngine::Physics::DynamicAABBTreeNode* ScratchEngine::Physics::DynamicAABBTree::_get_node(i32 id)
{
	//return reinterpret_cast<DynamicAABBTreeNode*>(reinterpret_cast<uptr>(memory) + id);
	return memory + id;
}

void ScratchEngine::Physics::DynamicAABBTree::_free_node(i32 id)
{
	DynamicAABBTreeNode* node = _get_node(id);
	node->next = freeList;

	freeList = id;
	--numAllocated;
}

ScratchEngine::i32 ScratchEngine::Physics::DynamicAABBTree::_balance(i32 id)
{
	DynamicAABBTreeNode* node = _get_node(id);

	if (node->height > 1)
	{
		i32 iL1 = node->left;
		i32 iR1 = node->right;

		DynamicAABBTreeNode* l1 = _get_node(iL1);
		DynamicAABBTreeNode* r1 = _get_node(iR1);

		i32 balanceFactor = l1->height - r1->height;

		i32 iL2, iR2;
		DynamicAABBTreeNode* l2;
		DynamicAABBTreeNode* r2;

		if (balanceFactor > 1)
		{
			iL2 = r1->left;
			iR2 = r1->right;

			l2 = _get_node(iL2);
			r2 = _get_node(iR2);

			r1->left = id;
			r1->parent = node->parent;

			node->parent = iR1;

			if (r1->parent == null_index)
				root = iR1;
			else
			{
				if (_get_node(r1->parent)->left == id)
					_get_node(r1->parent)->left = iR1;
				else
					_get_node(r1->parent)->right = iR1;
			}

			if (l2->height > r2->height)
			{
				r1->right = iL2;
				node->right = iR2;

				r2->parent = id;
			}
			else
			{
				r1->right = iR2;
				node->right = iL2;

				l2->parent = id;
			}

			_update_box_and_height(node);
			_update_box_and_height(r1);

			return iR1;
		}

		if (balanceFactor < -1)
		{
			iL2 = l1->left;
			iR2 = l1->right;

			l2 = _get_node(iL2);
			r2 = _get_node(iR2);

			l1->left = id;
			l1->parent = node->parent;

			node->parent = iL1;

			if (l1->parent == null_index)
				root = iL1;
			else
			{
				if (_get_node(l1->parent)->left == id)
					_get_node(l1->parent)->left = iL1;
				else
					_get_node(l1->parent)->right = iL1;
			}

			if (l2->height > r2->height)
			{
				l1->right = iL2;
				node->left = iR2;

				r2->parent = id;
			}
			else
			{
				l1->right = iR2;
				node->left = iL2;

				l2->parent = id;
			}

			_update_box_and_height(node);
			_update_box_and_height(l1);

			return iL1;
		}
	}

	return id;
}

void ScratchEngine::Physics::DynamicAABBTree::_update_box_and_height(DynamicAABBTreeNode * node)
{
	DynamicAABBTreeNode* l = _get_node(node->left);
	DynamicAABBTreeNode* r = _get_node(node->right);

	node->box = l->box;
	node->box.Union(r->box);

	node->height = __max(l->height, r->height) + 1;
}
