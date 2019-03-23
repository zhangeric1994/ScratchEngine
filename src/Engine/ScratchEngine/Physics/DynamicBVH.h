#pragma once

#include "../Memory/DynamicPoolAllocator.h"

#include "DynamicBVHNode.h"

namespace ScratchEngine
{
	namespace Physics
	{
		template<class T> class __declspec(dllexport) DynamicBVH
		{
		private:
			DynamicPoolAllocator<DynamicBVHNode<T>> allocator;
			i32 root;

		public:
			DynamicBVH() : allocator(128)
			{
				root = null_index;
			}

			i32 Insert(T* data, const AxisAlignedBoundingBox& box)
			{
				assert(data);

				i32 iLeaf = allocator.Allocate();
				DynamicBVHNode<T>& leaf = allocator[iLeaf];

				leaf.box = box;
				leaf.data = data;
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

						DynamicBVHNode<T>& l = allocator[iL];
						DynamicBVHNode<T>& r = allocator[iR];

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
					DynamicBVHNode<T>& node = allocator[iNode];
					node.data = nullptr;
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

			void Remove(i32 id)
			{
				if (id == root)
				{
					root = null_index;
					allocator.Free(id);
				}
				else
				{
					i32 iParent = allocator[id].parent;
					DynamicBVHNode<T>& parent = allocator[iParent];

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

		private:
			__forceinline i32 _balance(i32 id)
			{
				DynamicBVHNode<T>& node = allocator[id];

				if (node.height > 1)
				{
					i32 iL1 = node.left;
					i32 iR1 = node.right;

					DynamicBVHNode<T>& l1 = allocator[iL1];
					DynamicBVHNode<T>& r1 = allocator[iR1];

					i32 balanceFactor = l1.height - r1.height;
					if (balanceFactor < -1)
					{
						i32 iL2 = r1.left;
						i32 iR2 = r1.right;

						DynamicBVHNode<T>& l2 = allocator[iL2];
						DynamicBVHNode<T>& r2 = allocator[iR2];

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

						DynamicBVHNode<T>& l2 = allocator[iL2];
						DynamicBVHNode<T>& r2 = allocator[iR2];

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

			__forceinline void _update_box_and_height(DynamicBVHNode<T>& node)
			{
				DynamicBVHNode<T>& l = allocator[node.left];
				DynamicBVHNode<T>& r = allocator[node.right];

				node.box = l.box;
				node.box.Union(r.box);

				node.height = __max(l.height, r.height) + 1;
			}
		};
	}
}