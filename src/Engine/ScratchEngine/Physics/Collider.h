#pragma once

#include <DirectXMath.h>
#include <tuple>
#include <unordered_map>

#include "../Core/GameObject.h"
#include "../Utility/Typedefs.h"

#include "BoundingVolume.h"

using namespace DirectX;
using namespace ScratchEngine;

namespace ScratchEngine
{
	namespace Physics
	{
		XMVECTOR gravity = XMVectorSet(0, -9.8f, 0, 0);
		XMVECTOR AxisX = XMVectorSet(1, 0, 0, 0);
		XMVECTOR AxisX = XMVectorSet(0, 1, 0, 0);
		XMVECTOR AxisX = XMVectorSet(0, 0, 1, 0);

		
		template<class T> class __declspec(dllexport) Collider : public GameComponent
		{
		protected:
			T* boundingVolume;
			std::unordered_map<gameObject*, float> CollidedWith;

		public:
			template<class V> bool Query(Collider<V>* other, float currentTime)
			{
				return GetBoundingVolume()->Query(other->GetBoundingVolume(), currentTime);
			}

		private:
			BoundingVolume<T> GetBoundingVolume()
			{
				return boundingVolume;
			}
		};

		
		class BoxCollider : public Collider<OrientedBoundingBox>
		{
		private:
			XMVECTOR size;

			//XMVECTOR min;
			//XMVECTOR max;

			// float minX, maxX, minY, maxY, minZ, maxZ;
			// float defaultMinX, defaultMaxX, defaultMinY, defaultMaxY, defaultMinZ, defaultMaxZ;

			//std::vector<XMVECTOR> planes;
			//std::vector<std::tuple<XMVECTOR, XMVECTOR, XMVECTOR, XMVECTOR>> planesInfo;
			//std::vector<std::tuple<XMVECTOR, XMVECTOR>> edges;

		public:
			BoxCollider(XMVECTOR size);
			~BoxCollider();
		};

		
		class SphereCollider : public Collider<BoundingSphere>
		{
		private:
			f32 radius;

		public:
			SphereCollider(f32 radius);
			~SphereCollider();
		};


		//bool GetSeparatingPlane(XMVECTOR RPos, XMVECTOR Plane, BoxCollider* a, BoxCollider* b);
		//XMVECTOR getCollidedNormal(Collider* Entity, XMVECTOR reflectPlane = { 0,0,0 });
		//XMVECTOR getCollisionPoint(BoxCollider* a, BoxCollider* b);
		//void ForceCalculation(Collider* a, Collider* b, XMVECTOR aNormal, XMVECTOR bNormal, XMVECTOR collisionPoint, float totalTime);
		//float AngularForceCalculation(Collider* a, XMVECTOR collisionPoint, XMVECTOR force, float totalTime);

		////Sphere check
		//bool CollisionCheck(SphereCollider* a, SphereCollider* b, float totalTime);

		////Box check
		//bool CollisionCheck(BoxCollider* a, BoxCollider* b, float totalTime);
		//bool CollisionCheck(BoxCollider* a, SphereCollider* b, float totalTime);

		//// Helper
		//bool isUnique(std::vector<XMVECTOR>points, XMVECTOR test);
		//XMVECTOR getPlaneNormal(BoxCollider* Entity, XMVECTOR point);
		//XMVECTOR getLineIntersection(XMVECTOR start, XMVECTOR end, std::tuple<XMVECTOR, XMVECTOR, XMVECTOR, XMVECTOR> planeVertetices, XMFLOAT3 Max, XMFLOAT3 Min);
	}
}
