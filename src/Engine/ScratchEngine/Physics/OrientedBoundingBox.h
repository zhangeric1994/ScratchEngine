#pragma once

#include "BoundingVolume.h"
#include "RigidBody.h"

namespace ScratchEngine
{
	namespace Physics
	{
		struct OrientedBoundingBox : public BoundingVolume
		{
			template<typename T1, typename T2> friend bool CollisionCheck(T1*, T2*, RigidBody*, RigidBody*, float);

			friend bool GetSeparatingPlane(XMVECTOR, XMVECTOR, OrientedBoundingBox*, OrientedBoundingBox*);


		private:
			static const int iSurfaces[6][4];
			static const int iEdges[12][2];

			XMVECTOR center;
			XMVECTOR axisX;
			XMVECTOR axisY;
			XMVECTOR axisZ;

			union
			{
				XMVECTOR vertices[8];

				struct
				{
					XMVECTOR A;
					XMVECTOR B;
					XMVECTOR C;
					XMVECTOR D;
					XMVECTOR E;
					XMVECTOR F;
					XMVECTOR G;
					XMVECTOR H;
				};
			};

			union
			{
				XMVECTOR surfacePlanes[6];

				struct
				{
					XMVECTOR negativeX;
					XMVECTOR negativeZ;
					XMVECTOR negativeY;
					XMVECTOR positiveX;
					XMVECTOR positiveY;
					XMVECTOR positiveZ;
				};
			};

			OrientedBoundingBox() {}
			OrientedBoundingBox(XMVECTOR size, XMVECTOR position, XMVECTOR rotation);


		public:
			XMVECTOR GetHalfSize() const;
		};
	}
}