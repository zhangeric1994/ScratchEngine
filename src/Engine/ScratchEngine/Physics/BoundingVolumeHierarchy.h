#pragma once

#include "../Memory/Allocator.h"

#include "BoundingVolumeHierarchyNode.h"

namespace ScratchEngine
{
	namespace Physics
	{
		template<class T>
		class BoundingVolumeHierarchy
		{
		private:
			BoundingVolumeHierarchyNode<T>* root;

		public:
			BoundingVolumeHierarchy()
			{
				root = nullptr;
				allocator = nullptr;
			}

			BoundingVolumeHierarchyNode<T>* Insert(AxisAlignedBoundingBox box, T* data)
			{
				BoundingVolumeHierarchyNode<T>* leaf = new BoundingVolumeHierarchyNode<T>(box, data);

				if (!root)
					root = leaf;
				else
				{
					BoundingVolumeHierarchyNode<T>* currentNode = root;
					
					register BoundingVolumeHierarchyNode<T>* l;
					register BoundingVolumeHierarchyNode<T>* r;
					float v, cost, costI, costL, costR;
					while (!currentNode->IsLeaf())
					{
						l = currentNode->leftChild;
						r = currentNode->rightChild;

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

					BoundingVolumeHierarchyNode<T>* node = new BoundingVolumeHierarchyNode<T>();
					node->parent = currentNode->parent;
					node->leftChild = currentNode;
					node->rightChild = leaf;
					node->UpdateBox();
					node->UpdateHeight();

					if (node->parent)
					{
						if (node->parent->leftChild == currentNode)
							node->parent->leftChild = node;
						else
							node->parent->rightChild = node;
					}
					else
						root = node;

					currentNode->parent = node;
					leaf->parent = node;

					for (currentNode = node; currentNode; currentNode = currentNode->parent)
					{
						currentNode = _balance(currentNode);

						currentNode->UpdateBox();
						currentNode->UpdateHeight();
					}
				}

				return leaf;
			}

			void Remove(BoundingVolumeHierarchyNode<T>* node)
			{
				if (node == root)
					root == nullptr;
				else
				{
					register BoundingVolumeHierarchyNode<T>* parent = node->parent;
					register BoundingVolumeHierarchyNode<T>* grandParent = parent->parent;
					register BoundingVolumeHierarchyNode<T>* sibling = parent->leftChild == node ? parent->rightChild : parent->leftChild;
					
					sibling->parent = grandParent;

					if (grandParent)
					{
						if (grandParent->leftChild == parent)
							grandParent->leftChild = sibling;
						else
							grandParent->rightChild = sibling;

						for (BoundingVolumeHierarchyNode<T>* currentNode = grandParent; currentNode; currentNode = currentNode->parent)
						{
							currentNode = _balance(currentNode);

							currentNode->UpdateBox();
							currentNode->UpdateHeight();
						}
					}
					else
						root = sibling;

					delete parent;
				}
			}

			__forceinline BoundingVolumeHierarchyNode<T>* _balance(BoundingVolumeHierarchyNode<T>* node)
			{
				register i32 balance;
				register BoundingVolumeHierarchyNode<T>* l1;
				register BoundingVolumeHierarchyNode<T>* r1;
				register BoundingVolumeHierarchyNode<T>* l2;
				register BoundingVolumeHierarchyNode<T>* r2;

				if (node->height > 1)
				{
					l1 = node->leftChild;
					r1 = node->rightChild;

					balance = l1->height - r1->height;

					if (balance > 1)
					{
						l2 = r1->leftChild;
						r2 = r1->rightChild;

						r1->left = node;
						r1->parent = node->parent;

						node->parent = r1;

						if (r1->parent)
						{
							if (r1->parent->left == node)
								r1->parent->left = right;
							else
								r1->parent->right = right;
						}
						else
							root = r1;

						if (l2->height > r2->height)
						{
							r1->right = l2;
							node->right = r2;

							r2->parent = node;
						}
						else
						{
							r1->right = r2;
							node->right = l2;

							l2->parent = node;
						}

						node->UpdateBox();
						node->UpdateHeight();

						r1->UpdateBox();
						r1->UpdateHeight();

						return r1;
					}
					
					if (balance < -1)
					{
						l2 = l1->leftChild;
						r2 = l1->rightChild;

						l1->leftChild = node;
						l1->parent = node->parent;

						node->parent = l1;

						if (l1->parent)
						{
							if (l1->parent->left == node)
								l1->parent->left = l1;
							else
								l1->parent->right = l1;
						}
						else
							root = l1;

						if (l2->height > r2->height)
						{
							l1->right = l2;
							node->left = r2;

							r2->parent = node;
						}
						else
						{
							l1->right = r2;
							node->left = l2;

							l2->parent = node;
						}

						node->UpdateBox();
						node->UpdateHeight();

						l1->UpdateBox();
						l1->UpdateHeight();

						return l1;
					}

					return node;
				}
			}
		};
	}
}