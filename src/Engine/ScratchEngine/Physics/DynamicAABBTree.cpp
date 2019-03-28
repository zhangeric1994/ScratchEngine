#include <cstring>

#include "DynamicAABBTree.h"

ScratchEngine::Physics::DynamicAABBTree::DynamicAABBTree() : allocator(128)
{
	root = null_index;
}

ScratchEngine::i32 ScratchEngine::Physics::DynamicAABBTree::Insert(Collider* collider)
{
	assert(collider);

	i32 iLeaf = allocator.Allocate();
	DynamicAABBTreeNode& leaf = allocator[iLeaf];

	leaf.aabb = collider->GetBoundingVolume();
	leaf.rigidBody = collider;
	leaf.height = 0;
	leaf.parent = null_index;
	leaf.left = null_index;
	leaf.right = null_index;

	if (root == null_index)
		root = iLeaf;
	else
	{
		i32 iCurrentNode = root;

		i32 iL, iR;
		f32 v, cost, costI, costL, costR;
		while (!allocator[iCurrentNode].IsLeaf())
		{
			iL = allocator[iCurrentNode].left;
			iR = allocator[iCurrentNode].right;

			DynamicAABBTreeNode& l = allocator[iL];
			DynamicAABBTreeNode& r = allocator[iR];

			v = allocator[iCurrentNode].GetUnionVolume(leaf);

			cost = 2 * v;
			costI = 2 * (v - allocator[iCurrentNode].GetVolume());

			if (l.IsLeaf())
				costL = l.GetUnionVolume(leaf) + costI;
			else
				costL = (l.GetUnionVolume(leaf) - l.GetVolume()) + costI;

			if (r.IsLeaf())
				costR = r.GetUnionVolume(leaf) + costI;
			else
				costR = (r.GetUnionVolume(leaf) - r.GetVolume()) + costI;

			if (cost < costL && cost < costR)
				break;

			if (costL < costR)
				iCurrentNode = iL;
			else
				iCurrentNode = iR;
		}
		
		i32 iNode = allocator.Allocate();
		DynamicAABBTreeNode& node = allocator[iNode];
		node.rigidBody = nullptr;
		node.height = 0;
		node.parent = allocator[iCurrentNode].parent;
		node.left = iCurrentNode;
		node.right = iLeaf;

		_update_box_and_height(node);

		if (node.parent == null_index)
			root = iNode;
		else
		{
			if (allocator[node.parent].left == iCurrentNode)
				allocator[node.parent].left = iNode;
			else
				allocator[node.parent].right = iNode;
		}			

		allocator[iCurrentNode].parent = iNode;
		allocator[iLeaf].parent = iNode;

		for (iCurrentNode = iNode; iCurrentNode != null_index; iCurrentNode = allocator[iCurrentNode].parent)
		{
			iCurrentNode = _balance(iCurrentNode);
			_update_box_and_height(allocator[iCurrentNode]);
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
		allocator.Free(id);
	}
	else
	{
		i32 iParent = allocator[id].parent;
		DynamicAABBTreeNode& parent = allocator[iParent];

		i32 iGrandParent = parent.parent;
		i32 iSibling = parent.left == id ? parent.right : parent.left;

		allocator[iSibling].parent = iGrandParent;

		if (iGrandParent == null_index)
			root = iSibling;
		else
		{
			if (allocator[iGrandParent].left == iParent)
				allocator[iGrandParent].left = iSibling;
			else
				allocator[iGrandParent].left = iSibling;
			
			for (i32 iCurrentNode = iGrandParent; iCurrentNode != null_index; iCurrentNode = allocator[iCurrentNode].parent)
			{
				iCurrentNode = _balance(iCurrentNode);
				_update_box_and_height(allocator[iCurrentNode]);
			}
		}

		allocator.Free(iParent);
	}
}

ScratchEngine::i32 ScratchEngine::Physics::DynamicAABBTree::_balance(i32 id)
{
	DynamicAABBTreeNode& node = allocator[id];

	if (node.height > 1)
	{
		i32 iL1 = node.left;
		i32 iR1 = node.right;

		DynamicAABBTreeNode& l1 = allocator[iL1];
		DynamicAABBTreeNode& r1 = allocator[iR1];

		i32 balanceFactor = l1.height - r1.height;
		if (balanceFactor < -1)
		{
			i32 iL2 = r1.left;
			i32 iR2 = r1.right;

			DynamicAABBTreeNode& l2 = allocator[iL2];
			DynamicAABBTreeNode& r2 = allocator[iR2];

			r1.left = id;
			r1.parent = node.parent;

			node.parent = iR1;

			if (r1.parent == null_index)
				root = iR1;
			else
			{
				if (allocator[r1.parent].left == id)
					allocator[r1.parent].left = iR1;
				else
					allocator[r1.parent].right = iR1;
			}

			if (l2.height > r2.height)
			{
				r1.right = iL2;
				node.right = iR2;

				r2.parent = id;
			}
			else
			{
				r1.right = iR2;
				node.right = iL2;

				l2.parent = id;
			}

			_update_box_and_height(node);
			_update_box_and_height(r1);

			return iR1;
		}

		if (balanceFactor > 1)
		{
			i32 iL2 = l1.left;
			i32 iR2 = l1.right;

			DynamicAABBTreeNode& l2 = allocator[iL2];
			DynamicAABBTreeNode& r2 = allocator[iR2];

			l1.left = id;
			l1.parent = node.parent;

			node.parent = iL1;

			if (l1.parent == null_index)
				root = iL1;
			else
			{
				if (allocator[l1.parent].left == id)
					allocator[l1.parent].left = iL1;
				else
					allocator[l1.parent].right = iL1;
			}

			if (l2.height > r2.height)
			{
				l1.right = iL2;
				node.left = iR2;

				r2.parent = id;
			}
			else
			{
				l1.right = iR2;
				node.left = iL2;

				l2.parent = id;
			}

			_update_box_and_height(node);
			_update_box_and_height(l1);

			return iL1;
		}
	}

	return id;
}

void ScratchEngine::Physics::DynamicAABBTree::_update_box_and_height(DynamicAABBTreeNode& node)
{
	DynamicAABBTreeNode& l = allocator[node.left];
	DynamicAABBTreeNode& r = allocator[node.right];

	node.aabb = l.aabb;
	node.aabb.Union(r.aabb);

	node.height = __max(l.height, r.height) + 1;
}
