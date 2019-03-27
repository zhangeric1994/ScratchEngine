#pragma once

#include <cstdlib>

#include "BoundingVolume.h"

namespace ScratchEngine
{
	namespace Physics
	{
		class Collider
		{
		public:
			virtual AxisAlignedBoundingBox GetBoundingVolume() = 0;
		};

		class DynamicAABBTreeNode
		{
			friend class DynamicAABBTree;

		private:
			AxisAlignedBoundingBox box;
			Collider* collider;

			i32 height : 32;
			i32 parent : 32;

			union
			{
				struct
				{
					i32 left : 32;
					i32 right : 32;
				};

				struct
				{
					i32 next : 32;
					i32 previous : 32;
				};
			};

			bool IsLeaf() const;
			f32 GetVolume() const;
			f32 GetUnionVolume(const DynamicAABBTreeNode& other) const;
		};
	}
}