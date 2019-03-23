#pragma once

#include <cstdlib>

#include "AxisAlignedBoundingBox.h"

namespace ScratchEngine
{
	namespace Physics
	{
		template<class T>
		class BoundingVolumeHierarchyNode
		{
			friend class BoundingVolumeHierarchy;

		private:
			AxisAlignedBoundingBox box;
			T* data;

			BoundingVolumeHierarchyNode<T>* parent;
			BoundingVolumeHierarchyNode<T>* leftChild;
			BoundingVolumeHierarchyNode<T>* rightChild;
			i32 height;

			BoundingVolumeHierarchyNode(AxisAlignedBoundingBox box, T* data)
			{
				this->box = box;
				this->data = data;

				parent = nullptr;
				leftChild = nullptr;
				rightChild = nullptr;

				height = 0;
			}

			i32 GetHeight() const
			{
				return height;
			}

			f32 GetVolume() const
			{
				return box.GetVolume();
			}

			bool IsLeaf() const
			{
				return leftChild == nullptr;
			}

			f32 GetUnionVolume(const BoundingVolumeHierarchyNode& other) const
			{
				return box.GetUnionVolume(other.box);
			}

			__inline void UpdateBox()
			{
				if (!leftChild)
					box = rightChild->box;
				else if (!rightChild)
					box = leftChild->box;
				else
					box = leftChild->box.Union(rightChild->box);
			}

			__inline void UpdateHeight()
			{
				height = leftChild ? __max(leftChild->GetHeight(), rightChild->GetHeight()) : 0;
			}
		};
	}
}