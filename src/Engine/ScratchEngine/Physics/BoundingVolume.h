#ifndef BOUNDING_VOLUME_H
#define BOUNDING_VOLUME_H

#include <cmath>
#include <DirectXMath.h>

#include "../Common/Typedefs.h"

using namespace DirectX;
using namespace ScratchEngine;


namespace ScratchEngine
{
	namespace Physics
	{
		template<class T1, class T2> static bool TestOverlap(T1* boundingVolume1, T2* boundingVolume2);

		enum BoundingVolumeType
		{
			AABB,
			OBB,
			Sphere,
		};


		struct BoundingVolume { };


		struct __declspec(dllexport) AxisAlignedBoundingBox : public BoundingVolume
		{
			template<class T1, class T2> friend bool TestOverlap(T1*, T2*);
			//friend class OverlapCheck;
			friend class DynamicAABBTree;
			friend class DynamicAABBTreeNode;
			template<typename T> friend class DynamicBVH;
			template<typename T> friend class DynamicBVHNode;


		private:
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


		struct OrientedBoundingBox : public BoundingVolume
		{
			friend bool GetSeparatingPlane(XMVECTOR, XMVECTOR, OrientedBoundingBox*, OrientedBoundingBox*);
			template<class T1, class T2> friend bool TestOverlap(T1*, T2*);
			//friend class OverlapCheck;
			friend class PhysicsEngine;


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
			OrientedBoundingBox(XMVECTOR position, XMVECTOR rotation, XMVECTOR size);


		public:
			XMVECTOR GetHalfSize() const;

			void SetData(XMVECTOR position, XMVECTOR rotation, XMVECTOR size);
		};


		struct BoundingSphere : public BoundingVolume
		{
			template<class T1, class T2> friend bool TestOverlap(T1*, T2*);
			//friend class OverlapCheck;


		private:
			XMVECTOR center;
			f32 radius;
		};


		//struct OverlapCheck
		//{
		//private:
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


		//public:
			template<class T1, class T2> bool TestOverlap(T1* boundingVolume1, T2* boundingVolume2)
			{
				throw "NOT IMPLEMENTED";
			}

			template<> bool TestOverlap(AxisAlignedBoundingBox* aabb1, AxisAlignedBoundingBox* aabb2)
			{
				if (XMVector3Less(aabb1->max, aabb2->min) || XMVector3Less(aabb2->max, aabb1->min))
					return false;

				return true;
			}

			template<> bool TestOverlap(AxisAlignedBoundingBox* aabb, OrientedBoundingBox* obb)
			{
				throw "NOT IMPLEMENTED";
			}

			template<> bool TestOverlap(AxisAlignedBoundingBox* aabb, BoundingSphere* sphere)
			{
				throw "NOT IMPLEMENTED";
			}

			template<> bool TestOverlap(OrientedBoundingBox* obb1, OrientedBoundingBox* obb2)
			{
				XMVECTOR RPos = XMVectorSubtract(obb1->center, obb2->center);

				if (GetSeparatingPlane(RPos, obb1->axisX, obb1, obb2) ||
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
					GetSeparatingPlane(RPos, XMVector3Cross(obb1->axisZ, obb2->axisZ), obb1, obb2))
					return false;

				//XMVECTOR collisionPoint = getCollisionPoint(a, b);

				//XMVECTOR planeA = getPlaneNormal(a, collisionPoint);
				//XMVECTOR planeB = getPlaneNormal(b, collisionPoint);

				//XMVECTOR aNormal = GetCollisionNormal(b, planeB); // sphere don't need to pass the collision detail
				//XMVECTOR bNormal = GetCollisionNormal(a, planeA);

				//ForceCalculation(a, b, aNormal, bNormal, collisionPoint, totalTime);

				return true;
			}

			template<> bool TestOverlap(OrientedBoundingBox* obb, BoundingSphere* sphere)
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

			template<> bool TestOverlap(BoundingSphere* sphere1, BoundingSphere* sphere2)
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
		//};

		//template<VolumeType T> XMVECTOR GetCollisionNormal(OrientedBoundingBox* a, XMVECTOR plane = { 0, 0, 0, 0 })
		//{
		//	/*if (a->type == Sphere) {
		//		return { 1.0,1.0,1.0, 0 };
		//	}
		//	else if (a->type == OBB) {
		//		BoxCollider* item = reinterpret_cast<BoxCollider*> (a);
		//		XMVECTOR AB = {0.0,0.0,0.0};
		//		XMVECTOR AD = {0.0,0.0,0.0};
		//		XMVECTOR ret;
		//		if (item->maxX == collisionPoint.x) {
		//			AB = { 0,item->maxY - collisionPoint.y,item->minZ - collisionPoint.z };
		//			AD = { 0,item->minY - collisionPoint.y,item->minZ - collisionPoint.z };
		//		}
		//		else if (item->minX == collisionPoint.x) {
		//			AB = { 0,item->minY - collisionPoint.y,item->minZ - collisionPoint.z };
		//			AD = { 0,item->maxY - collisionPoint.y,item->minZ - collisionPoint.z };
		//		}
		//		else if (item->maxY == collisionPoint.y) {
		//			AB = { item->maxX - collisionPoint.x,0,item->minZ - collisionPoint.z };
		//			AD = { item->minX - collisionPoint.x,0,item->minZ - collisionPoint.z };
		//		}
		//		else if (item->minY == collisionPoint.y) {
		//			AB = { item->minX - collisionPoint.x,0,item->minZ - collisionPoint.z };
		//			AD = { item->maxX - collisionPoint.x,0,item->minZ - collisionPoint.z };
		//		}
		//		else if (item->maxZ == collisionPoint.z) {
		//			AB = { item->maxX - collisionPoint.x,item->maxY - collisionPoint.y,0 };
		//			AD = { item->minX - collisionPoint.x,item->minY - collisionPoint.y,0 };
		//		}
		//		else if (item->minZ == collisionPoint.z) {
		//			AB = { item->minX - collisionPoint.x,item->minY - collisionPoint.y,0 };
		//			AD = { item->minX - collisionPoint.x,item->maxY - collisionPoint.y,0 };
		//		}
		//		ret = XMVector3Cross(AB, AD);
		//		ret.m128_f32[0] = abs(ret.m128_f32[0]) > 0 ? (ret.m128_f32[0] < 0 ? -1.0f : 1.0f) : 0.0f;
		//		ret.m128_f32[1] = abs(ret.m128_f32[1]) > 0 ? (ret.m128_f32[1] < 0 ? -1.0f : 1.0f) : 0.0f;
		//		ret.m128_f32[2] = abs(ret.m128_f32[2]) > 0 ? (ret.m128_f32[2] < 0 ? -1.0f : 1.0f) : 0.0f;
		//		return ret;
		//	}
		//	return XMVECTOR();*/

		//	return T == Sphere ? { -1.0, -1.0, -1.0, 0 } : plane;
		//}

		//template<VolumeType T1, VolumeType T2> void ForceCalculation(BoundingVolume<T1>* a, BoundingVolume<T2>* b, XMVECTOR aNormal, XMVECTOR bNormal, XMVECTOR collisionPoint, float totalTime)
		//{
		//	XMVECTOR aVelocity = XMLoadFloat3(&a->velocity);
		//	XMVECTOR bVelocity = XMLoadFloat3(&b->velocity);

		//	XMVECTOR aPos = XMLoadFloat3(&a->Position);
		//	XMVECTOR bPos = XMLoadFloat3(&b->Position);
		//	XMFLOAT3 aForce;
		//	XMFLOAT3 bForce;

		//	if (b->Static) {
		//		// then the momentum is inifinity
		//		// reverse the force

		//		XMVECTOR aF = (aVelocity * a->mass) * 1.9f * aNormal;
		//		//float aRatio = AngularForceCalculation(a, collisionPoint, aF, totalTime);
		//		XMStoreFloat3(&aForce, aF);
		//		//XMStoreFloat3(&aForce, -(aVelocity * a->Mass) * 1.9f * bNormal);
		//		a->ApplyForce(aForce);

		//		//printf("normal:  %f,  %f,  %f \n", aNormal.m128_f32[0], aNormal.m128_f32[1], aNormal.m128_f32[2]);
		//		//printf("force:  %f,  %f,  %f \n",aForce.x, aForce.y, aForce.z);
		//		return;
		//	}
		//	else {
		//		XMStoreFloat3(&aForce, (((a->mass - b->mass)*aVelocity + 2 * b->mass * bVelocity) / (a->mass + b->mass)) * 0.9f * a->mass * aNormal);
		//	}

		//	if (a->Static) {
		//		// then the momentum is inifinity
		//		// reverse the force
		//		XMVECTOR bF = (bVelocity * b->mass) * 1.9f * bNormal;
		//		//float bRatio = AngularForceCalculation(b, collisionPoint, bF, totalTime);
		//		XMStoreFloat3(&bForce, bF);
		//		b->ApplyForce(bForce);
		//		//printf("normal:  %f,  %f,  %f \n", bNormal.m128_f32[0], bNormal.m128_f32[1], bNormal.m128_f32[2]);
		//		//printf("force:  %f,  %f,  %f \n", bForce.x, bForce.y, bForce.z);
		//		return;
		//	}
		//	else {
		//		XMStoreFloat3(&bForce, (((b->mass - a->mass)*bVelocity + 2 * a->mass * bVelocity) / (a->mass + b->mass)) * 0.9f * b->mass * bNormal);
		//		//XMStoreFloat3(&bForce, -(aVelcity * a->Mass));
		//	}

		//	if (a->CollidedWith[b] != 0 || b->CollidedWith[a] != 0) {
		//		//collided already
		//	}
		//	else {
		//		XMFLOAT3 vSum;
		//		XMStoreFloat3(&vSum, XMVector3Length(aVelocity) + XMVector3Length(bVelocity));
		//		if (abs(vSum.x) >= 0.10f) {
		//			a->CollidedWith[b] = totalTime;
		//			b->CollidedWith[a] = totalTime;
		//			XMVECTOR aF = XMLoadFloat3(&aForce);
		//			XMVECTOR bF = XMLoadFloat3(&bForce);
		//			float aRatio = AngularForceCalculation(a, collisionPoint, aF, totalTime);
		//			float bRatio = AngularForceCalculation(b, collisionPoint, bF, totalTime);
		//			XMStoreFloat3(&aForce, aF * (1 - aRatio));
		//			XMStoreFloat3(&bForce, bF * (1 - bRatio));
		//			a->ApplyForce(aForce);
		//			b->ApplyForce(bForce);

		//			XMVECTOR f1 = XMVectorSet(b->Position.x - a->Position.x, b->Position.y - a->Position.y, b->Position.z - a->Position.z, 0.0f);
		//			XMVECTOR f2 = XMVectorSet(a->Position.x - b->Position.x, a->Position.y - b->Position.y, a->Position.z - b->Position.z, 0.0f);
		//			XMFLOAT3 force1;
		//			XMFLOAT3 force2;
		//			XMStoreFloat3(&force1, f1);
		//			XMStoreFloat3(&force2, f2);
		//			a->ApplyForce(force2);
		//			b->ApplyForce(force1);
		//		}
		//	}
		//}

		//XMVECTOR getCollisionPoint(BoxCollider * a, BoxCollider * b)
		//{
		//	// return 
		//	// 1. collisionPoint
		//	// 2. intersectedPlaneA
		//	// 3. intersectedPlaneB
		//	XMVECTOR pLinePoint1 = { 0.0f,0.0f,0.0f };
		//	XMVECTOR pLinePoint2 = { 0.0f,0.0f,0.0f };
		//	std::vector <XMVECTOR> points;
		//	a->UpdateVertex();
		//	b->UpdateVertex();
		//	//printf("===============================================================================\n");
		//	for (int i = 0; i < a->planes.size(); i++) {
		//		//printf("new plane\n");
		//		//printf("-------------------------------------------------------------------------\n");

		//		for (int j = 0; j < b->planes.size(); j++) {
		//			XMPlaneIntersectPlane(&pLinePoint1, &pLinePoint2, b->planes[j], a->planes[i]);

		//			if (!isnan(pLinePoint1.m128_f32[0])) {
		//				float maxX = __min(a->maxX, b->maxX);
		//				float maxY = __min(a->maxY, b->maxY);
		//				float maxZ = __min(a->maxZ, b->maxZ);
		//				float minX = __max(a->minX, b->minX);
		//				float minY = __max(a->minY, b->minY);
		//				float minZ = __max(a->minZ, b->minZ);
		//				XMVECTOR collisionPoint = getLineIntersection(pLinePoint1, pLinePoint2, a->planesInfo[i],
		//					{ __max(maxX,minX), __max(maxY,minY), __max(maxZ,minZ) }, { __min(minX,maxX), __min(minY,maxY), __min(minZ,maxZ) });
		//				//XMFLOAT3 cp;
		//				//XMStoreFloat3(&cp, collisionPoint);
		//				if (!isnan(collisionPoint.m128_f32[0])) {
		//					points.push_back(collisionPoint);
		//				}

		//				// if there is a line
		//				//printf("pLinePoint1:  %f,  %f,  %f \n", pLinePoint1.m128_f32[0], pLinePoint1.m128_f32[1], pLinePoint1.m128_f32[2]);
		//				//printf("pLinePoint2:  %f,  %f,  %f \n", pLinePoint2.m128_f32[0], pLinePoint2.m128_f32[1], pLinePoint2.m128_f32[2]);

		//				//if (pLinePoint1.m128_f32[0] > maxX && pLinePoint2.m128_f32[0] > maxX) {
		//				//	// both of them are larger than max value, not valid
		//				//	printf("diffX:  %f,  %f \n", pLinePoint2.m128_f32[0] - maxX, pLinePoint2.m128_f32[0] - maxX);
		//				//	continue;
		//				//}

		//				//if (pLinePoint1.m128_f32[1] > maxY&& pLinePoint2.m128_f32[1] > maxY) {
		//				//	// both of them are larger than max value, not valid
		//				//	printf("diffY:  %f,  %f \n", pLinePoint1.m128_f32[1] - maxY, pLinePoint2.m128_f32[1] - maxY);
		//				//	continue;
		//				//}

		//				//if (pLinePoint1.m128_f32[2] > maxZ&& pLinePoint2.m128_f32[2] > maxZ) {
		//				//	// both of them are larger than max value, not valid
		//				//	printf("diffZ:  %f,  %f \n", pLinePoint1.m128_f32[2] - maxZ, pLinePoint2.m128_f32[2] - maxZ);
		//				//	continue;
		//				//}
		//				//// if there is a line

		//				//if (pLinePoint1.m128_f32[0] < minX && pLinePoint2.m128_f32[0] < minX) {
		//				//	// both of them are less than min value, not valid
		//				//	printf("diffXmin:  %f,  %f \n", pLinePoint1.m128_f32[0] - minX, pLinePoint2.m128_f32[0] - minX);
		//				//	continue;
		//				//}

		//				//if (pLinePoint1.m128_f32[1] < minY && pLinePoint2.m128_f32[1] < minY) {
		//				//	// both of them are less than min value, not valid
		//				//	printf("diffYmin:  %f,  %f \n", pLinePoint1.m128_f32[1] - minY, pLinePoint2.m128_f32[1] - minY);
		//				//	continue;
		//				//}

		//				//if (pLinePoint1.m128_f32[2] < minZ && pLinePoint2.m128_f32[2] < minZ) {
		//				//	// both of them are less than min value, not valid
		//				//	printf("diffZmin:  %f,  %f \n", pLinePoint1.m128_f32[2] - minZ, pLinePoint2.m128_f32[2] - minZ);
		//				//	continue;
		//				//}
		//				//pLinePoint1 = XMVectorClamp(pLinePoint1, { a->minX,a->minY,a->minZ }, { a->maxX,a->maxY,a->maxZ });
		//				//pLinePoint1 = XMVectorClamp(pLinePoint1, { b->minX,b->minY,b->minZ }, { b->maxX,b->maxY,b->maxZ });
		//				//pLinePoint2 = XMVectorClamp(pLinePoint2, { a->minX,a->minY,a->minZ }, { a->maxX,a->maxY,a->maxZ });
		//				//pLinePoint2 = XMVectorClamp(pLinePoint2, { b->minX,b->minY,b->minZ }, { b->maxX,b->maxY,b->maxZ });

		//				//XMStoreFloat3(&cp, (pLinePoint1 + pLinePoint2) / 2);


		//				//if (cp.x <= a->maxX && cp.y <= a->maxY && cp.z <= a->maxZ
		//				//	&& cp.x >= a->minX && cp.y >= a->minY && cp.z >= a->minZ) {
		//				//	printf("cp:  %f,  %f,  %f \n", cp.x, cp.y, cp.z);

		//				//	return collisionPoint;
		//				//}
		//					/*if (isUnique(points, (pLinePoint1 + pLinePoint2) / 2)) {
		//						points.push_back((pLinePoint1 + pLinePoint2) / 2);
		//					}
		//					XMVECTOR aPos = XMLoadFloat3(&a->Position);*/
		//					//return std::tuple<XMVECTOR, XMVECTOR, XMVECTOR>((pLinePoint1 + pLinePoint2) / 2, {0,1,0}, b->planes[j]);
		//				//}
		//			}
		//			else {
		//				//printf(" Plane  Parallel!\n");
		//			}
		//		}
		//	}
		//	//return { 1,1,1 };
		//	XMVECTOR ret = { 0,0,0 };
		//	for (int i = 0; i < points.size(); i++) {
		//		ret += points[i];
		//	}
		//	if (points.size() == 0) {
		//		return { 0,0,0 };
		//	}
		//	ret /= (float)points.size();
		//	//printf("cp:  %f,  %f,  %f \n", ret.m128_f32[0], ret.m128_f32[1], ret.m128_f32[2]);
		//	return ret;
		//}

		//float AngularForceCalculation(Collider * a, XMVECTOR collisionPoint, XMVECTOR force, float totalTime)
		//{

		//	if (a->Static) {
		//		// no angular force apply
		//		return 0.0f;
		//	}
		//	else {
		//		XMVECTOR aVelocity = XMLoadFloat3(&a->velocity);
		//		XMVECTOR aPos = XMLoadFloat3(&a->Position);

		//		XMVECTOR velocityNorm = XMVector3Normalize(aVelocity);
		//		XMVECTOR CollisionNorm = XMVector3Normalize(collisionPoint - aPos);
		//		//float angle = XMConvertToDegrees(acos(XMVector3Dot(velocityNorm, CollisionNorm).m128_f32[0]));
		//		float angularRatio = 1 - abs(XMVector3Dot(velocityNorm, CollisionNorm).m128_f32[0]);
		//		XMFLOAT3 f;
		//		XMStoreFloat3(&f, aVelocity * angularRatio * 20);
		//		XMFLOAT3 transformed = { f.y,f.x,f.z };
		//		//a->ApplyAngularForce(transformed);

		//		return angularRatio;
		//	}

		//}
	}
}
#endif