#ifndef DYNAMIC_BVH_HPP
#define DYNAMIC_BVH_HPP

#include <stack>

#include "../Memory/DynamicPoolAllocator.hpp"

#include "DynamicBVHNode.hpp"

using namespace ScratchEngine::Memory;

namespace ScratchEngine
{
	namespace Physics
	{
		class IDynamicBVHQueryCallback
		{
			template<typename T> friend class DynamicBVH;


		private:
			virtual bool DynamicBVHTestOverlapCallback(i32 node1, i32 node2) = 0;
		};

		template<class T> class __declspec(dllexport) DynamicBVH
		{
			friend class PhysicsEngine;


		private:
			DynamicPoolAllocator<DynamicBVHNode<T>> allocator;
			i32 root;


		public:
			DynamicBVH();


			i32 Insert(T* data, const AxisAlignedBoundingBox& aabb);

			void Remove(i32 id);
			

		private:
			void Query(int targetID, IDynamicBVHQueryCallback* callback);


			i32 __Balance(i32 id);
			void __UpdateBoxAndHeight(DynamicBVHNode<T>& node);
		};


		template<class T> inline DynamicBVH<T>::DynamicBVH() : allocator(128)
		{
			root = null_index;
		}

		template<class T> inline i32 DynamicBVH<T>::Insert(T * data, const AxisAlignedBoundingBox & aabb)
		{
			assert(data);

			i32 iLeaf = allocator.Allocate();
			DynamicBVHNode<T>& leaf = allocator[iLeaf];

			leaf.aabb = aabb;
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

				__UpdateBoxAndHeight(node);

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
					iCurrentNode = __Balance(iCurrentNode);
					__UpdateBoxAndHeight(allocator[iCurrentNode]);
				}
			}

			return iLeaf;
		}

		template<class T> inline void DynamicBVH<T>::Remove(i32 id)
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
						iCurrentNode = __Balance(iCurrentNode);
						__UpdateBoxAndHeight(allocator[iCurrentNode]);
					}
				}

				allocator.Free(iParent);
			}
		}

		template<class T> inline void DynamicBVH<T>::Query(int targetID, IDynamicBVHQueryCallback * callback)
		{
			DynamicBVHNode<T>& targetNode = allocator[targetID];

			stack<int> s;

			s.push(root);

			while (!s.empty())
			{
				int id = s.top();
				s.pop();

				if (id == null_index)
					continue;

				DynamicBVHNode<T>& node = allocator[id];

				if (ScratchEngine::Physics::TestOverlap(&targetNode.aabb, &node.aabb, nullptr, nullptr, 0))
				{
					if (node.IsLeaf())
					{
						if (!callback->DynamicBVHTestOverlapCallback(targetID, id))
							return;
					}
					else
					{
						s.push(node.left);
						s.push(node.right);
					}
				}
			}
		}

		template<class T> __forceinline i32 DynamicBVH<T>::__Balance(i32 id)
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

					__UpdateBoxAndHeight(node);
					__UpdateBoxAndHeight(r1);

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

					__UpdateBoxAndHeight(node);
					__UpdateBoxAndHeight(l1);

					return iL1;
				}
			}

			return id;
		}

		template<class T> __forceinline void DynamicBVH<T>::__UpdateBoxAndHeight(DynamicBVHNode<T>& node)
		{
			DynamicBVHNode<T>& l = allocator[node.left];
			DynamicBVHNode<T>& r = allocator[node.right];

			node.aabb = l.aabb;
			node.aabb.Union(r.aabb);

			node.height = __max(l.height, r.height) + 1;
		}
	}
}
#endif