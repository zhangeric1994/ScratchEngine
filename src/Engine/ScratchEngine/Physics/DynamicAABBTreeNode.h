#pragma once

#include <cstdlib>

#include "AxisAlignedBoundingBox.h"
#include "Collider.h"
#include "RigidBody.h"

namespace ScratchEngine
{
	namespace Physics
	{

		class DynamicAABBTreeNode
		{
			friend class DynamicAABBTree;

		private:
			AxisAlignedBoundingBox aabb;
			Collider* rigidBody;

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