#ifndef DYNAMIC_BVH_NODE
#define DYNAMIC_BVH_NODE

#include "BoundingVolume.h"

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

			f32 GetVolume() const;
			f32 GetUnionVolume(const DynamicBVHNode& other) const;
			bool IsLeaf() const;
		};


		template<class T> inline f32 DynamicBVHNode<T>::GetVolume() const
		{
			return aabb.GetVolume();
		}

		template<class T> inline f32 DynamicBVHNode<T>::GetUnionVolume(const DynamicBVHNode & other) const
		{
			return aabb.GetUnionVolume(other.aabb);
		}

		template<class T> inline bool DynamicBVHNode<T>::IsLeaf() const
		{
			return left == null_index;
		}
	}
}
#endif