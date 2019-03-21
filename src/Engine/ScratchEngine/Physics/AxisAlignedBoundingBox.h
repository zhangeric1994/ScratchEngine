#pragma once

#include <DirectXMath.h>

#include "../Utility/Typedefs.h"

using namespace DirectX;

namespace ScratchEngine
{
	namespace Physics
	{
		struct __declspec(dllexport) AxisAlignedBoundingBox
		{
			friend class DynamicAABBTree;
			friend class DynamicAABBTreeNode;

		protected:
			union
			{
				struct
				{
					XMVECTOR min;
					XMVECTOR max;
				};

				struct
				{
					f32 minX;
					f32 minY;
					f32 minZ;
					f32 minW;
					f32 maxX;
					f32 maxY;
					f32 maxZ;
					f32 maxW;
				};
			};
			

		public:
			AxisAlignedBoundingBox();
			AxisAlignedBoundingBox(f32 minX, f32 minY, f32 minZ, f32 maxX, f32 maxY, f32 maxZ);
			AxisAlignedBoundingBox(XMVECTOR point1, XMVECTOR point2);

			f32 GetMinX() const;
			f32 GetMaxX() const;
			f32 GetMinY() const;
			f32 GetMaxY() const;
			f32 GetMinZ() const;
			f32 GetMaxZ() const;
			f32 GetVolume() const;

			void* operator new(size_t size);
			void operator delete(void* p);

		private:
			void SetMinX(f32 value);
			void SetMaxX(f32 value);
			void SetMinY(f32 value);
			void SetMaxY(f32 value);
			void SetMinZ(f32 value);
			void SetMaxZ(f32 value);

			void Union(const AxisAlignedBoundingBox& other);

			f32 GetUnionVolume(const AxisAlignedBoundingBox& other) const;

			__forceinline f32 _get_box_volume(XMVECTOR lwh) const;
		};
	}
}