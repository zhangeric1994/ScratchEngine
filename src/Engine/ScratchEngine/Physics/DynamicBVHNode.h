#pragma once

#include "AxisAlignedBoundingBox.h"

namespace ScratchEngine
{
	namespace Physics
	{
		template<class T> class DynamicBVHNode
		{
			template<class T> friend class DynamicBVH;

		private:
			AxisAlignedBoundingBox aabb;

			T* data;

			i32 height : 32;
			i32 parent : 32;
			i32 left : 32;
			i32 right : 32;

			f32 GetVolume() const
			{
				return aabb.GetVolume();
			}

			f32 GetUnionVolume(const DynamicBVHNode& other) const
			{
				return aabb.GetUnionVolume(other.aabb);
			}

			bool IsLeaf() const
			{
				return left == null_index;
			}
		};
	}
}