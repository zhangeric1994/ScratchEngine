#pragma once

#include "AxisAlignedBoundingBox.h"
#include "OrientedBoundingBox.h"
#include "BoundingSphere.h"

namespace ScratchEngine
{
	namespace Physics
	{
		static bool GetSeparatingPlane(XMVECTOR RPos, XMVECTOR Plane, OrientedBoundingBox* obb1, OrientedBoundingBox* obb2)
		{
			XMVECTOR halfSizeA = obb1->GetHalfSize();
			XMVECTOR halfSizeB = obb2->GetHalfSize();

			return (abs(XMVector3Dot(RPos, Plane).m128_f32[0]) >
				(abs(XMVector3Dot(XMVectorScale(obb1->axisX, halfSizeA.m128_f32[0]), Plane).m128_f32[0]) +
					abs(XMVector3Dot(XMVectorScale(obb1->axisY, halfSizeA.m128_f32[1]), Plane).m128_f32[0]) +
					abs(XMVector3Dot(XMVectorScale(obb1->axisZ, halfSizeA.m128_f32[2]), Plane).m128_f32[0]) +
					abs(XMVector3Dot(XMVectorScale(obb2->axisX, halfSizeB.m128_f32[0]), Plane).m128_f32[0]) +
					abs(XMVector3Dot(XMVectorScale(obb2->axisY, halfSizeB.m128_f32[1]), Plane).m128_f32[0]) +
					abs(XMVector3Dot(XMVectorScale(obb2->axisZ, halfSizeB.m128_f32[2]), Plane).m128_f32[0])));
		}

		template<class T1, class T2> bool CollisionCheck(T1* boundingVolume1, T2* boundingVolume2, RigidBody* rigidBody1, RigidBody* rigidBody2, float currentTime)
		{
			throw "NOT IMPLEMENTED";
		}

		template<> bool CollisionCheck(AxisAlignedBoundingBox* aabb1, AxisAlignedBoundingBox* aabb2, RigidBody* aabbRb1, RigidBody* aabbRb2, float currentTime)
		{
			if (XMVector3Less(aabb1->max, aabb2->min) || XMVector3Less(aabb2->max, aabb1->min))
				return false;

			return true;
		}

		template<> bool CollisionCheck(AxisAlignedBoundingBox* aabb, OrientedBoundingBox* obb, RigidBody* aabbRb, RigidBody* obbRb, float currentTime)
		{
			throw "NOT IMPLEMENTED";
		}

		template<> bool CollisionCheck(AxisAlignedBoundingBox* aabb, BoundingSphere* sphere, RigidBody* aabbRb, RigidBody* sphereRb, float currentTime)
		{
			throw "NOT IMPLEMENTED";
		}

		template<> bool CollisionCheck(OrientedBoundingBox* obb1, OrientedBoundingBox* obb2, RigidBody* obbRb1, RigidBody* obbRb2, float currentTime)
		{
			XMVECTOR RPos = XMVectorSubtract(obb1->center, obb2->center);

			if (!(GetSeparatingPlane(RPos, obb1->axisX, obb1, obb2) ||
				  GetSeparatingPlane(RPos, obb1->axisY, obb1, obb2) ||
				  GetSeparatingPlane(RPos, obb1->axisZ, obb1, obb2) ||
				  GetSeparatingPlane(RPos, obb2->axisX, obb1, obb2) ||
				  GetSeparatingPlane(RPos, obb2->axisY, obb1, obb2) ||
				  GetSeparatingPlane(RPos, obb2->axisZ, obb1, obb2) ||
				  GetSeparatingPlane(RPos, XMVector3Cross(obb1->axisX, obb2->axisX), obb1, obb2) ||
				  GetSeparatingPlane(RPos, XMVector3Cross(obb1->axisX, obb2->axisY), obb1, obb2) ||
				  GetSeparatingPlane(RPos, XMVector3Cross(obb1->axisX, obb2->axisZ), obb1, obb2) ||
				  GetSeparatingPlane(RPos, XMVector3Cross(obb1->axisY, obb2->axisX), obb1, obb2) ||
				  GetSeparatingPlane(RPos, XMVector3Cross(obb1->axisY, obb2->axisY), obb1, obb2) ||
				  GetSeparatingPlane(RPos, XMVector3Cross(obb1->axisY, obb2->axisZ), obb1, obb2) ||
				  GetSeparatingPlane(RPos, XMVector3Cross(obb1->axisZ, obb2->axisX), obb1, obb2) ||
				  GetSeparatingPlane(RPos, XMVector3Cross(obb1->axisZ, obb2->axisY), obb1, obb2) ||
				  GetSeparatingPlane(RPos, XMVector3Cross(obb1->axisZ, obb2->axisZ), obb1, obb2)))
			{
				//XMVECTOR collisionPoint = getCollisionPoint(a, b);

				//XMVECTOR planeA = getPlaneNormal(a, collisionPoint);
				//XMVECTOR planeB = getPlaneNormal(b, collisionPoint);

				//XMVECTOR aNormal = GetCollisionNormal(b, planeB); // sphere don't need to pass the collision detail
				//XMVECTOR bNormal = GetCollisionNormal(a, planeA);

				//ForceCalculation(a, b, aNormal, bNormal, collisionPoint, totalTime);

				return true;
			}

			return false;
		}

		template<> bool CollisionCheck(OrientedBoundingBox* obb, BoundingSphere* sphere, RigidBody* obbRb, RigidBody* sphereRb, float currentTime)
		{
			XMVECTOR halfDiag = XMVectorSubtract(obb->H, obb->center);

			XMVECTOR diff = XMVectorSubtract(obb->center, sphere->center);

			if (XMVector3Dot(halfDiag, halfDiag).m128_f32[0] < XMVector3Dot(diff, diff).m128_f32[0])
				return false;

			XMVECTOR bPos = sphere->center;

			for (int i = 0; i < 6; i++)
			{
				XMVECTOR plane = obb->surfacePlanes[i];
				float distance = XMVector3Dot(plane, bPos).m128_f32[0] + plane.m128_f32[3];

				if (abs(distance) <= sphere->radius)
				{
					XMVECTOR collisionPoint = bPos - plane * distance;

					if (XMVector3LessOrEqual(collisionPoint, obb->H) && XMVector3GreaterOrEqual(collisionPoint, obb->A))
					{
						//XMVECTOR planeA = a->surfacePlanes[i];

						//XMVECTOR aNormal = GetCollisionNormal(b);

						////sphere don't need to pass the collision detail, sphere reflect everything
						//XMVECTOR bNormal = GetCollisionNormal(a, planeA);

						//ForceCalculation(a, b, aNormal, bNormal, collisionPoint, totalTime);

						return true;
					}
				}
			}

			//edge check
			for (int i = 0; i < 12; i++)
			{
				XMVECTOR start = obb->vertices[OrientedBoundingBox::iEdges[i][0]];
				XMVECTOR end = obb->vertices[OrientedBoundingBox::iEdges[i][1]];

				float distance = XMVector3LinePointDistance(start, end, bPos).m128_f32[0];

				if (abs(distance) <= sphere->radius)
				{
					XMVECTOR AB = end - start;
					XMVECTOR AP = bPos - start;
					XMVECTOR collisionPoint = start + XMVector3Dot(AP, AB) / XMVector3Dot(AB, AB) * AB;

					if (XMVector3LessOrEqual(collisionPoint, obb->H) && XMVector3GreaterOrEqual(collisionPoint, obb->A))
					{
						//XMVECTOR planeA = { 1, 1, 1 };

						//XMVECTOR aNormal = GetCollisionNormal(a, planeA);

						////sphere don't need to pass the collision detail, sphere reflect everything
						//XMVECTOR bNormal = GetCollisionNormal(b);

						//ForceCalculation(a, b, aNormal, bNormal, collisionPoint, totalTime);

						return true;
					}
				}
			}

			return false;
		}

		template<> bool CollisionCheck(BoundingSphere* sphere1, BoundingSphere* sphere2, RigidBody* sphereRb1, RigidBody* sphereRb2, float currentTime)
		{
			XMVECTOR normal = XMVectorSubtract(sphere1->center, sphere2->center);
			float squared_distance = XMVector3LengthSq(normal).m128_f32[0];

			//calculate the sum of the radius squared
			float radius_sum_squared = (sphere1->radius + sphere2->radius) * (sphere1->radius + sphere2->radius);

			if (squared_distance < radius_sum_squared)
			{
				//XMVECTOR collisionPoint = XMVectorSubtract(a->center, XMVectorScale(normal, sqrt(squared_distance)));

				////create normalized vectors to apply the forces in the correct direction
				//XMVECTOR aNormal = GetCollisionNormal(b); // sphere don't need to pass the collision detail
				//XMVECTOR bNormal = GetCollisionNormal(a);

				//ForceCalculation(a, b, aNormal, bNormal, collisionPoint, totalTime);

				return true;
			}

			return false;
		}
	}
}