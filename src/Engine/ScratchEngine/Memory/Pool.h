#pragma once

#include <stdlib.h>

#include "../Utility/Typedefs.h"

#include "Block.h"

using namespace std;

namespace ScratchEngine
{
	namespace Memory
	{
		class __declspec(dllexport) Pool
		{
			friend class PoolTest;

		private:
			void* memory;
			size_t capacity;
			size_t size;
			u32 numBlocks;
			u32 numAllocated;
			Block* root;

		public:
			Pool();
			Pool(size_t size);
			~Pool();

			void* Allocate(size_t size);
			void Free(void* p);

		private:
			__forceinline void Add(Block* block);
			__forceinline void Remove(Block* block);
		};

#define __height(node) (node ? node->height : 0)
#define __balance_factor(node) (node ? (__height(node->left) - __height(node->right)) : 0)

		__forceinline Block* RightRotate(Block* node)
		{
			Block* leftNode = node->left;
			Block* rightNode = leftNode->right;

			/* Rotate */

			leftNode->right = node;
			node->parent = leftNode;
			node->left = rightNode;
			rightNode->parent = node;

			/* Update heights */

			node->height = __max(__height(node->left), __height(node->right)) + 1;
			leftNode->height = __max(__height(leftNode->left), __height(leftNode->right)) + 1;

			/* Return the new tree root */

			return leftNode;
		}

		__forceinline Block* LeftRotate(Block* node)
		{
			Block* rightNode = node->right;
			Block* leftNode = rightNode->left;

			/* Rotate */

			rightNode->left = node;
			node->parent = rightNode;
			node->right = leftNode;
			leftNode->parent = node;

			/* Update heights */

			node->height = __max(__height(node->left), __height(node->right)) + 1;
			rightNode->height = __max(__height(rightNode->left), __height(rightNode->right)) + 1;

			/* Return the new tree root */

			return rightNode;
		}

		Block* AddToTree(Block* tree, Block* node)
		{
			/* Perform the normal BST insertion */

			if (!tree)
			{
				node->height = 1;
				node->left = nullptr;
				node->right = nullptr;
				node->next = node;
				node->previous = node;

				return node;
			}

			size_t size = node->size;

			if (size < tree->size)
			{
				tree->left = AddToTree(tree->left, node);
				tree->left->parent = tree;
			}
			else if (size > tree->size)
			{
				tree->right = AddToTree(tree->right, node);
				tree->right->parent = tree;
			}
			else
			{
				node->height = 0;
				node->parent = nullptr;
				node->left = nullptr;
				node->right = nullptr;

				node->next = tree->next;
				node->previous = tree;

				tree->next->previous = node;
				tree->next = node;

				return tree;
			}

			/* Update height */

			tree->height = 1 + __max(__height(tree->left), __height(tree->right));

			/* Rebalance the tree if needed */

			i64 balanceFactor = __balance_factor(tree);
			
			if (balanceFactor > 1 && size < tree->left->size)
				return RightRotate(tree);

			if (balanceFactor < -1 && size > tree->right->size)
				return LeftRotate(tree);

			if (balanceFactor > 1 && size > tree->left->size)
			{
				tree->left = LeftRotate(tree->left);
				tree->left->parent = tree;
				return RightRotate(tree);
			}

			if (balanceFactor < -1 && size < tree->right->size)
			{
				tree->right = RightRotate(tree->right);
				tree->right->parent = tree;
				return LeftRotate(tree);
			}

			/* Return the original tree if no rebalancing is performed */

			return tree;
		}

		Block* RemoveFromTree(Block* tree, Block* node)
		{
			// STEP 1: PERFORM STANDARD BST DELETE 
			if (!tree)
				return tree;

			size_t size = node->size;

			// If the key to be deleted is smaller than the 
			// root's key, then it lies in left subtree 
			if (size < tree->size)
			{
				tree->left = RemoveFromTree(tree->left, node);
				tree->left->parent = tree;
			}
			else if (size > tree->size)
			{
				tree->right = RemoveFromTree(tree->right, node);
				tree->right->parent = tree;
			}
			else
			{
				if (tree->left && tree->right)
				{
					Block* rightTree = tree->right;

					while (rightTree->left != NULL)
						rightTree = rightTree->left;

					tree->right = RemoveFromTree(tree->right, rightTree);

					rightTree->left = tree->left;
					rightTree->left->parent = rightTree;

					rightTree->right = tree->right;

					if (rightTree->right)
						rightTree->right->parent = rightTree;

					tree = rightTree;
				}
				else if (tree->left)
					tree = tree->left;
				else if (tree->right)
					tree = tree->right;
				else
					return nullptr;
			}

			tree->height = 1 + __max(__height(tree->left), __height(tree->right));

			i64 balanceFactor = __balance_factor(tree);

			if (balanceFactor > 1 && __balance_factor(tree->left) >= 0)
				return RightRotate(tree);

			if (balanceFactor > 1 && __balance_factor(tree->left) < 0)
			{
				tree->left = LeftRotate(tree->left);
				tree->left->parent = tree;
				return RightRotate(tree);
			}

			if (balanceFactor < -1 && __balance_factor(tree->right) <= 0)
				return LeftRotate(tree);

			if (balanceFactor < -1 && __balance_factor(tree->right) > 0)
			{
				tree->right = RightRotate(tree->right);
				tree->right->parent = tree;
				return LeftRotate(tree);
			}

			return tree;
		}
	}
}