#ifndef COLLIDER_H
#define COLLIDER_H

#include <DirectXMath.h>
#include <tuple>
#include <unordered_map>

#include "../Common/Typedefs.h"
#include "../Core/GameObject.h"

#include "BoundingVolume.h"

using namespace DirectX;
using namespace ScratchEngine;

namespace ScratchEngine
{
	namespace Physics
	{
		//XMVECTOR gravity = XMVectorSet(0, -9.8f, 0, 0);
		//XMVECTOR AxisX = XMVectorSet(1, 0, 0, 0);
		//XMVECTOR AxisY = XMVectorSet(0, 1, 0, 0);
		//XMVECTOR AxisZ = XMVectorSet(0, 0, 1, 0);

		class __declspec(dllexport) Collider : public GameComponent
		{
		protected:
			BoundingVolumeType type;
			BoundingVolume* boundingVolume;


		public:
			BoundingVolumeType GetType();
			BoundingVolume* GetBoundingVolume();

			bool IsOverlappingWith(Collider* other, float currentTime);
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
#endif