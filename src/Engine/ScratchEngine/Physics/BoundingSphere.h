#pragma once

#include "BoundingVolume.h"
#include "RigidBody.h"

namespace ScratchEngine
{
	namespace Physics
	{
		struct BoundingSphere : public BoundingVolume
		{
			template<typename T1, typename T2> friend bool CollisionCheck(T1*, T2*, RigidBody*, RigidBody*, float);


		private:
			XMVECTOR center;
			f32 radius;
		};


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