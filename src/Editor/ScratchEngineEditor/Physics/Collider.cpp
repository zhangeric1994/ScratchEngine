#include "Collider.h"
#include "Physics.h"
#include <tuple>

using namespace DirectX;
float collisionCheckBack = 0.2f;
namespace Colliders
{
	void Collider::Update(float dt, float totalTime)
	{
		if (!Static) {
			Position = Item->individualPositon;
			//printf("Pos before: %f,%f,%f\n", Position.x, Position.y, Position.z);
			//printf("vol: %f,%f,%f\n", Velocity.x, Velocity.y, Velocity.z);
			//printf("dt: %f\n", dt);
			Position.x += Velocity.x *dt;
			Position.y += Velocity.y *dt;
			Position.z += Velocity.z *dt;

			//printf("pos: %f,%f,%f\n", Position.x, Position.y, Position.z);
			Rotation = Item->quaternion;
			Rotation.x += AngularVelocity.x *dt;
			Rotation.y += AngularVelocity.y *dt;
			Rotation.z += AngularVelocity.z *dt;
			std::unordered_map<Collider*, float>::iterator it = CollidedWith.begin();
			while (it != CollidedWith.end())
			{
				float totalVel = it->first->Velocity.x + it->first->Velocity.y + it->first->Velocity.z;
				if (it->second + collisionCheckBack < totalTime && totalVel>=0.1f) {
					it->second = 0.0f;
				}
				it++;
			}
			//linear drag 
			XMStoreFloat3(&Velocity, (XMLoadFloat3(&Velocity)*(1.0f - dt * 0.7f)));
			XMStoreFloat3(&AngularVelocity, (XMLoadFloat3(&AngularVelocity)*(1.0f - dt * 0.8f)));
			Item->SetTranslation(Position.x, Position.y, Position.z);
			Item->SetRotation(Rotation.x, Rotation.y, Rotation.z);
		}
		
	}

	void Collider::ApplyForce(XMFLOAT3 force)
	{
		if (!Static) {
			Velocity.x += force.x / Mass;
			Velocity.y += force.y / Mass;
			Velocity.z += force.z / Mass;
		}

	}

	void Collider::ApplyAngularForce(XMFLOAT3 force)
	{
		if (!Static) {
			AngularVelocity.x += force.x / Mass;
			AngularVelocity.y += force.y / Mass;
			AngularVelocity.z += force.z / Mass;
		}

	}

	void Collider::ApplyGravity(float dt)
	{
		if (!Static&&UseGravity) {
				Velocity.y -= 9.8f *dt;
		}
	}

	SphereCollider::SphereCollider(Entity * _item, float _radius, float _mass, bool _gravity, bool _static)
	{
		Item = _item;
		Radius = _radius;
		Mass = _mass;
		UseGravity = _gravity;
		Static = _static;
		Position = Item->individualPositon;
		Rotation = Item->quaternion;
		Velocity = { 0,0,0 };
		AngularVelocity = { 0,0,0 };
		type = Sphere;
	}

	SphereCollider::~SphereCollider()
	{
	}


	BoxCollider::BoxCollider(Entity * _item, XMFLOAT3 _size, float _mass, bool _gravity, bool _static)
	{

		Item = _item;
		Mass = _mass;
		UseGravity = _gravity;
		Static = _static;
		Position = Item->individualPositon;
		Rotation = Item->quaternion;
		Velocity = { 0,0,0 };
		AngularVelocity = { 0,0,0 };
		type = Box;
		size = _size;
		defaultMinX = Position.x - size.x / 2;
		defaultMaxX = Position.x + size.x / 2;
		defaultMinY = Position.y - size.y / 2;
		defaultMaxY = Position.y + size.y / 2;
		defaultMinZ = Position.z - size.z / 2;
		defaultMaxZ = Position.z + size.z / 2;
		AxisX = { 1.f, 0.f, 0.f };
		AxisY = { 0.f, 1.f, 0.f };
		AxisZ = { 0.f, 0.f, 1.f };
		UpdateVertex();

	}

	BoxCollider::~BoxCollider()
	{
	}

	void BoxCollider::UpdateVertex()
	{
		defaultMinX = Position.x - size.x / 2;
		defaultMaxX = Position.x + size.x / 2;
		defaultMinY = Position.y - size.y / 2;
		defaultMaxY = Position.y + size.y / 2;
		defaultMinZ = Position.z - size.z / 2;
		defaultMaxZ = Position.z + size.z / 2;
		XMVECTOR rotationQuaternion = XMQuaternionRotationRollPitchYawFromVector({ Item->quaternion.x, Item->quaternion.y, Item->quaternion.z });
		XMVECTOR maxRotated = XMVector3Rotate({ defaultMaxX,defaultMaxY,defaultMaxZ }, rotationQuaternion);
		XMVECTOR minRotated = XMVector3Rotate({ defaultMinX,defaultMinY,defaultMinZ }, rotationQuaternion);

		AxisX = XMVector3Rotate({ 1.f, 0.f, 0.f }, rotationQuaternion);
		AxisY = XMVector3Rotate({ 0.f, 1.f, 0.f }, rotationQuaternion);
		AxisZ = XMVector3Rotate({ 0.f, 0.f, 1.f }, rotationQuaternion);

		minX = minRotated.m128_f32[0];
		maxX = maxRotated.m128_f32[0];
		minY = minRotated.m128_f32[1];
		maxY = maxRotated.m128_f32[1];
		minZ = minRotated.m128_f32[2];
		maxZ = maxRotated.m128_f32[2];

		XMVECTOR A = { minX,minY,minZ };
		XMVECTOR B = { minX,minY,maxZ };
		XMVECTOR C = { minX,maxY,minZ };
		XMVECTOR D = { minX,maxY,maxZ };
		XMVECTOR E = { maxX,minY,minZ };
		XMVECTOR F = { maxX,minY,maxZ };
		XMVECTOR G = { maxX,maxY,minZ };
		XMVECTOR H = { maxX,maxY,maxZ };

		planes.clear();
		planes.push_back(XMPlaneFromPoints(B, A, D));
		planes.push_back(XMPlaneFromPoints(A, E, C));
		planes.push_back(XMPlaneFromPoints(E, A, F));
		planes.push_back(XMPlaneFromPoints(E, F, G));
		planes.push_back(XMPlaneFromPoints(C, G, D));
		planes.push_back(XMPlaneFromPoints(F, B, H));

		planesInfo.clear();
		planesInfo.push_back(std::tuple<XMVECTOR, XMVECTOR, XMVECTOR, XMVECTOR>(B, A, D, C));
		planesInfo.push_back(std::tuple<XMVECTOR, XMVECTOR, XMVECTOR, XMVECTOR>(A, E, C, G));
		planesInfo.push_back(std::tuple<XMVECTOR, XMVECTOR, XMVECTOR, XMVECTOR>(E, A, F, B));
		planesInfo.push_back(std::tuple<XMVECTOR, XMVECTOR, XMVECTOR, XMVECTOR>(E, F, G, H));
		planesInfo.push_back(std::tuple<XMVECTOR, XMVECTOR, XMVECTOR, XMVECTOR>(C, G, D, H));
		planesInfo.push_back(std::tuple<XMVECTOR, XMVECTOR, XMVECTOR, XMVECTOR>(F, B, H, D));

		edges.clear();
		edges.push_back(std::tuple<XMVECTOR, XMVECTOR>(A, B));
		edges.push_back(std::tuple<XMVECTOR, XMVECTOR>(A, C));
		edges.push_back(std::tuple<XMVECTOR, XMVECTOR>(A, E));
		edges.push_back(std::tuple<XMVECTOR, XMVECTOR>(E, G));
		edges.push_back(std::tuple<XMVECTOR, XMVECTOR>(E, F));
		edges.push_back(std::tuple<XMVECTOR, XMVECTOR>(F, H));
		edges.push_back(std::tuple<XMVECTOR, XMVECTOR>(F, B));
		edges.push_back(std::tuple<XMVECTOR, XMVECTOR>(B, D));
		edges.push_back(std::tuple<XMVECTOR, XMVECTOR>(C, G));
		edges.push_back(std::tuple<XMVECTOR, XMVECTOR>(C, D));
		edges.push_back(std::tuple<XMVECTOR, XMVECTOR>(G, H));
		edges.push_back(std::tuple<XMVECTOR, XMVECTOR>(H, D));
	}


	bool getSeparatingPlane(XMVECTOR RPos, XMVECTOR Plane, BoxCollider * a, BoxCollider * b)
	{
		return (abs(XMVector3Dot(RPos , Plane).m128_f32[0]) >
				(abs(XMVector3Dot(a->AxisX*a->size.x/2,Plane).m128_f32[0]) +
				abs(XMVector3Dot((a->AxisY*a->size.y/2),Plane).m128_f32[0]) +
				abs(XMVector3Dot((a->AxisZ*a->size.z/2),Plane).m128_f32[0]) +
				abs(XMVector3Dot((b->AxisX*b->size.x/2),Plane).m128_f32[0]) +
				abs(XMVector3Dot((b->AxisY*b->size.y/2),Plane).m128_f32[0]) +
				abs(XMVector3Dot((b->AxisZ*b->size.z/2),Plane).m128_f32[0])
					)
			);
	}

	XMVECTOR getCollidedNormal(Collider * a,XMVECTOR plane)
	{
		/*if (a->type == Sphere) {
			return { 1.0,1.0,1.0, 0 };
		}
		else if (a->type == Box) {
			BoxCollider* item = reinterpret_cast<BoxCollider*> (a);
			XMVECTOR AB = {0.0,0.0,0.0};
			XMVECTOR AD = {0.0,0.0,0.0};
			XMVECTOR ret;
			if (item->maxX == collisionPoint.x) {
				AB = { 0,item->maxY - collisionPoint.y,item->minZ - collisionPoint.z };
				AD = { 0,item->minY - collisionPoint.y,item->minZ - collisionPoint.z };
			}
			else if (item->minX == collisionPoint.x) {	
				AB = { 0,item->minY - collisionPoint.y,item->minZ - collisionPoint.z };
				AD = { 0,item->maxY - collisionPoint.y,item->minZ - collisionPoint.z };
			}
			else if (item->maxY == collisionPoint.y) {
				AB = { item->maxX - collisionPoint.x,0,item->minZ - collisionPoint.z };
				AD = { item->minX - collisionPoint.x,0,item->minZ - collisionPoint.z };
			}
			else if (item->minY == collisionPoint.y) {
				AB = { item->minX - collisionPoint.x,0,item->minZ - collisionPoint.z };
				AD = { item->maxX - collisionPoint.x,0,item->minZ - collisionPoint.z };
			}
			else if (item->maxZ == collisionPoint.z) {
				AB = { item->maxX - collisionPoint.x,item->maxY - collisionPoint.y,0 };
				AD = { item->minX - collisionPoint.x,item->minY - collisionPoint.y,0 };
			}
			else if (item->minZ == collisionPoint.z) {
				AB = { item->minX - collisionPoint.x,item->minY - collisionPoint.y,0 };
				AD = { item->minX - collisionPoint.x,item->maxY - collisionPoint.y,0 };
			}
			ret = XMVector3Cross(AB, AD);
			ret.m128_f32[0] = abs(ret.m128_f32[0]) > 0 ? (ret.m128_f32[0] < 0 ? -1.0f : 1.0f) : 0.0f;
			ret.m128_f32[1] = abs(ret.m128_f32[1]) > 0 ? (ret.m128_f32[1] < 0 ? -1.0f : 1.0f) : 0.0f;
			ret.m128_f32[2] = abs(ret.m128_f32[2]) > 0 ? (ret.m128_f32[2] < 0 ? -1.0f : 1.0f) : 0.0f;
			return ret;
		}
		return XMVECTOR();*/
		XMVECTOR aNormal;
		if (a->type == Sphere) {
			aNormal = { -1.0,-1.0, -1.0, 0 };
		}
		else {
			aNormal = plane;
		}
		return aNormal;
	}

	XMVECTOR getCollisionPoint(BoxCollider * a, BoxCollider * b)
	{
		// return 
		// 1. collisionPoint
		// 2. intersectedPlaneA
		// 3. intersectedPlaneB
 		XMVECTOR pLinePoint1 = {0.0f,0.0f,0.0f};
		XMVECTOR pLinePoint2 = {0.0f,0.0f,0.0f};
		std::vector <XMVECTOR> points;
		a->UpdateVertex();
		b->UpdateVertex();
		//printf("===============================================================================\n");
		for (int i = 0; i < a->planes.size(); i++) {
			//printf("new plane\n");
			//printf("-------------------------------------------------------------------------\n");

			for (int j = 0; j < b->planes.size(); j++) {
				XMPlaneIntersectPlane(&pLinePoint1, &pLinePoint2, b->planes[j], a->planes[i]);

				if (!isnan(pLinePoint1.m128_f32[0])) {
					float maxX = min(a->maxX, b->maxX);
					float maxY = min(a->maxY, b->maxY);
					float maxZ = min(a->maxZ, b->maxZ);
					float minX = max(a->minX, b->minX);
					float minY = max(a->minY, b->minY);
					float minZ = max(a->minZ, b->minZ);
					XMVECTOR collisionPoint = getLineIntersection(pLinePoint1, pLinePoint2, a -> planesInfo[i],
						{ max(maxX,minX),max(maxY,minY),max(maxZ,minZ) }, { min(minX,maxX),min(minY,maxY),min(minZ,maxZ)});
					//XMFLOAT3 cp;
					//XMStoreFloat3(&cp, collisionPoint);
					if (!isnan(collisionPoint.m128_f32[0])) {
						points.push_back(collisionPoint);
					}
					
					// if there is a line
					//printf("pLinePoint1:  %f,  %f,  %f \n", pLinePoint1.m128_f32[0], pLinePoint1.m128_f32[1], pLinePoint1.m128_f32[2]);
					//printf("pLinePoint2:  %f,  %f,  %f \n", pLinePoint2.m128_f32[0], pLinePoint2.m128_f32[1], pLinePoint2.m128_f32[2]);
					
					//if (pLinePoint1.m128_f32[0] > maxX && pLinePoint2.m128_f32[0] > maxX) {
					//	// both of them are larger than max value, not valid
					//	printf("diffX:  %f,  %f \n", pLinePoint2.m128_f32[0] - maxX, pLinePoint2.m128_f32[0] - maxX);
					//	continue;
					//}
					
					//if (pLinePoint1.m128_f32[1] > maxY&& pLinePoint2.m128_f32[1] > maxY) {
					//	// both of them are larger than max value, not valid
					//	printf("diffY:  %f,  %f \n", pLinePoint1.m128_f32[1] - maxY, pLinePoint2.m128_f32[1] - maxY);
					//	continue;
					//}
					
					//if (pLinePoint1.m128_f32[2] > maxZ&& pLinePoint2.m128_f32[2] > maxZ) {
					//	// both of them are larger than max value, not valid
					//	printf("diffZ:  %f,  %f \n", pLinePoint1.m128_f32[2] - maxZ, pLinePoint2.m128_f32[2] - maxZ);
					//	continue;
					//}
					//// if there is a line
					
					//if (pLinePoint1.m128_f32[0] < minX && pLinePoint2.m128_f32[0] < minX) {
					//	// both of them are less than min value, not valid
					//	printf("diffXmin:  %f,  %f \n", pLinePoint1.m128_f32[0] - minX, pLinePoint2.m128_f32[0] - minX);
					//	continue;
					//}

					//if (pLinePoint1.m128_f32[1] < minY && pLinePoint2.m128_f32[1] < minY) {
					//	// both of them are less than min value, not valid
					//	printf("diffYmin:  %f,  %f \n", pLinePoint1.m128_f32[1] - minY, pLinePoint2.m128_f32[1] - minY);
					//	continue;
					//}
					
					//if (pLinePoint1.m128_f32[2] < minZ && pLinePoint2.m128_f32[2] < minZ) {
					//	// both of them are less than min value, not valid
					//	printf("diffZmin:  %f,  %f \n", pLinePoint1.m128_f32[2] - minZ, pLinePoint2.m128_f32[2] - minZ);
					//	continue;
					//}
					//pLinePoint1 = XMVectorClamp(pLinePoint1, { a->minX,a->minY,a->minZ }, { a->maxX,a->maxY,a->maxZ });
					//pLinePoint1 = XMVectorClamp(pLinePoint1, { b->minX,b->minY,b->minZ }, { b->maxX,b->maxY,b->maxZ });
					//pLinePoint2 = XMVectorClamp(pLinePoint2, { a->minX,a->minY,a->minZ }, { a->maxX,a->maxY,a->maxZ });
					//pLinePoint2 = XMVectorClamp(pLinePoint2, { b->minX,b->minY,b->minZ }, { b->maxX,b->maxY,b->maxZ });
					
					//XMStoreFloat3(&cp, (pLinePoint1 + pLinePoint2) / 2);


					//if (cp.x <= a->maxX && cp.y <= a->maxY && cp.z <= a->maxZ
					//	&& cp.x >= a->minX && cp.y >= a->minY && cp.z >= a->minZ) {
					//	printf("cp:  %f,  %f,  %f \n", cp.x, cp.y, cp.z);

					//	return collisionPoint;
					//}
						/*if (isUnique(points, (pLinePoint1 + pLinePoint2) / 2)) {
							points.push_back((pLinePoint1 + pLinePoint2) / 2);
						}
						XMVECTOR aPos = XMLoadFloat3(&a->Position);*/
						//return std::tuple<XMVECTOR, XMVECTOR, XMVECTOR>((pLinePoint1 + pLinePoint2) / 2, {0,1,0}, b->planes[j]);
					//}
				}
				else {
					//printf(" Plane  Parallel!\n");
				}
			}
		}
		//return { 1,1,1 };
		XMVECTOR ret = {0,0,0};
		for (int i = 0; i < points.size(); i++) {
			ret += points[i];
		}
		if (points.size() == 0) {
			return { 0,0,0 };
		}
		ret /= (float)points.size();
		//printf("cp:  %f,  %f,  %f \n", ret.m128_f32[0], ret.m128_f32[1], ret.m128_f32[2]);
		return ret;
	}

	void ForceCalculation(Collider * a, Collider * b, XMVECTOR aNormal,XMVECTOR bNormal, XMVECTOR collisionPoint, float totalTime)
	{
		XMVECTOR aVelocity = XMLoadFloat3(&a->Velocity);
		XMVECTOR bVelocity = XMLoadFloat3(&b->Velocity);

		XMVECTOR aPos = XMLoadFloat3(&a->Position);
		XMVECTOR bPos = XMLoadFloat3(&b->Position);
		XMFLOAT3 aForce;
		XMFLOAT3 bForce;

		if (b->Static) {
			// then the momentum is inifinity
			// reverse the force

			XMVECTOR aF = (aVelocity * a->Mass) * 1.5f * aNormal;
			float aRatio = AngularForceCalculation(a, collisionPoint, aF, totalTime);
			XMStoreFloat3(&aForce, aF);
			//XMStoreFloat3(&aForce, -(aVelocity * a->Mass) * 1.9f * bNormal);
			a->ApplyForce(aForce);
		
			//printf("normal:  %f,  %f,  %f \n", aNormal.m128_f32[0], aNormal.m128_f32[1], aNormal.m128_f32[2]);
			//printf("force:  %f,  %f,  %f \n",aForce.x, aForce.y, aForce.z);
			return;
		}
		else {
			XMStoreFloat3(&aForce, (((a->Mass - b->Mass)*aVelocity + 2 * b->Mass * bVelocity) / (a->Mass + b->Mass)) * 0.9f * a->Mass * aNormal);
		}


		if (a->Static) {
			// then the momentum is inifinity
			// reverse the force
			XMVECTOR bF = (bVelocity * b->Mass) * 1.5f * bNormal;
		    //float bRatio = AngularForceCalculation(b, collisionPoint, bF, totalTime);
			XMStoreFloat3(&bForce, bF);
			b->ApplyForce(bForce);
			//printf("normal:  %f,  %f,  %f \n", bNormal.m128_f32[0], bNormal.m128_f32[1], bNormal.m128_f32[2]);
			//printf("force:  %f,  %f,  %f \n", bForce.x, bForce.y, bForce.z);
			return;
		}
		else {
			XMStoreFloat3(&bForce, (((b->Mass - a->Mass)*bVelocity + 2 * a->Mass * bVelocity) / (a->Mass + b->Mass)) * 0.9f * b->Mass * bNormal);
			//XMStoreFloat3(&bForce, -(aVelcity * a->Mass));
		}
	
		if (a->CollidedWith[b] != 0 || b -> CollidedWith[a] != 0) {
			//collided already
		}
		else {
			XMFLOAT3 vSum;
			XMStoreFloat3(&vSum, XMVector3Length(aVelocity) + XMVector3Length(bVelocity));
			if (abs(vSum.x) >= 5.0f) {
				a->CollidedWith[b] = totalTime;
				b->CollidedWith[a] = totalTime;
				XMVECTOR aF = XMLoadFloat3(&aForce);
				XMVECTOR bF = XMLoadFloat3(&bForce);
				//float aRatio = AngularForceCalculation(a, collisionPoint, aF, totalTime);
				//float bRatio = AngularForceCalculation(b, collisionPoint, bF, totalTime);
				XMStoreFloat3(&aForce, aF);
				XMStoreFloat3(&bForce, bF);
				a->ApplyForce(aForce);
				b->ApplyForce(bForce);
			}
			else {
				XMVECTOR f1 = XMVectorSet(b->Position.x - a->Position.x, b->Position.y - a->Position.y, b->Position.z - a->Position.z, 0.0f);
				XMVECTOR f2 = XMVectorSet(a->Position.x - b->Position.x, a->Position.y - b->Position.y, a->Position.z - b->Position.z, 0.0f);
				XMFLOAT3 force1;
				XMFLOAT3 force2;
				XMStoreFloat3(&force1, XMVector3Normalize(f1)*0.5f);
				XMStoreFloat3(&force2, XMVector3Normalize(f2)*0.5f);
				a->ApplyForce(force2);
				b->ApplyForce(force1);
			}
		}
	}

	float AngularForceCalculation(Collider * a, XMVECTOR collisionPoint, XMVECTOR force, float totalTime)
	{

		if (a->Static) {
			// no angular force apply
			return 0.0f;
		}
		else {
			XMVECTOR aVelocity = XMLoadFloat3(&a->Velocity);
			XMVECTOR aPos = XMLoadFloat3(&a->Position);

			XMVECTOR velocityNorm = XMVector3Normalize(aVelocity);
			XMVECTOR CollisionNorm = XMVector3Normalize(collisionPoint - aPos);
			//float angle = XMConvertToDegrees(acos(XMVector3Dot(velocityNorm, CollisionNorm).m128_f32[0]));
			float angularRatio = 1 - abs(XMVector3Dot(velocityNorm, CollisionNorm).m128_f32[0]);
			XMFLOAT3 f;
			XMStoreFloat3(&f, aVelocity * angularRatio * 20);
			XMFLOAT3 transformed = { f.y,f.x,f.z };
			//a->ApplyAngularForce(transformed);

			return angularRatio;
		}

	}

	bool CollisionCheck(Collider * a, Collider * b, float totalTime)
	{
		if (a->type == Sphere && b->type == Sphere) {
			return CollisionCheck(reinterpret_cast<SphereCollider*> (a), reinterpret_cast<SphereCollider*> (b), totalTime);
		}
		else if (a->type == Sphere && b->type == Box) {
			return CollisionCheck(reinterpret_cast<BoxCollider*> (b), reinterpret_cast<SphereCollider*> (a), totalTime);
		}
		else if (a->type == Box && b->type == Box) {
			return CollisionCheck(reinterpret_cast<BoxCollider*> (a), reinterpret_cast<BoxCollider*> (b), totalTime);
		}
		else if (a->type == Box && b->type == Sphere) {
			return CollisionCheck(reinterpret_cast<BoxCollider*> (a), reinterpret_cast<SphereCollider*> (b), totalTime);
		}
		return false;
	}

	bool CollisionCheck(SphereCollider * a, SphereCollider * b, float totalTime)
	{
		float squared_distance = (a->Position.x - b->Position.x) * (a->Position.x - b->Position.x) +
			(a->Position.y - b->Position.y) * (a->Position.y - b->Position.y) + (a->Position.z - b->Position.z) * (a->Position.z - b->Position.z);
		//calculate the sum of the radii squared
		float radius_sum_squared = (a->Radius + b->Radius) * (a->Radius + b->Radius);
		if (squared_distance < radius_sum_squared) {
			XMVECTOR normal = XMVector3Normalize(XMLoadFloat3(&b->Position) - XMLoadFloat3(&a->Position));
			XMVECTOR collisionPoint = XMLoadFloat3(&a->Position) - normal * sqrt(squared_distance);
			//create normalized vectors to apply the forces in the correct direction
			XMVECTOR aNormal = getCollidedNormal(b); // sphere don't need to pass the collision detail
			XMVECTOR bNormal = getCollidedNormal(a);
			ForceCalculation(a, b, aNormal,bNormal, collisionPoint,totalTime);
			return true;
		}
		else {
			return false;
		}
	}

	bool CollisionCheck(BoxCollider * a, BoxCollider * b, float totalTime)
	{
		XMVECTOR RPos;
		XMVECTOR aPos = XMLoadFloat3(&a->Position);
		XMVECTOR bPos = XMLoadFloat3(&b->Position);
		RPos = aPos - bPos;
		a->UpdateVertex();
		b->UpdateVertex();
		if (!(getSeparatingPlane(RPos, a->AxisX, a, b) ||
			getSeparatingPlane(RPos, a->AxisY, a, b) ||
			getSeparatingPlane(RPos, a->AxisZ, a, b) ||
			getSeparatingPlane(RPos, b->AxisX, a, b) ||
			getSeparatingPlane(RPos, b->AxisY, a, b) ||
			getSeparatingPlane(RPos, b->AxisZ, a, b) ||
			getSeparatingPlane(RPos, XMVector3Cross(a->AxisX, b->AxisX), a, b) ||
			getSeparatingPlane(RPos, XMVector3Cross(a->AxisX, b->AxisY), a, b) ||
			getSeparatingPlane(RPos, XMVector3Cross(a->AxisX, b->AxisZ), a, b) ||
			getSeparatingPlane(RPos, XMVector3Cross(a->AxisY, b->AxisX), a, b) ||
			getSeparatingPlane(RPos, XMVector3Cross(a->AxisY, b->AxisY), a, b) ||
			getSeparatingPlane(RPos, XMVector3Cross(a->AxisY, b->AxisZ), a, b) ||
			getSeparatingPlane(RPos, XMVector3Cross(a->AxisZ, b->AxisX), a, b) ||
			getSeparatingPlane(RPos, XMVector3Cross(a->AxisZ, b->AxisY), a, b) ||
			getSeparatingPlane(RPos, XMVector3Cross(a->AxisZ, b->AxisZ), a, b))) {
			//XMFLOAT3 ret = getCollisionPoint(a, b);
			XMVECTOR collisionPoint = getCollisionPoint(a, b);
			XMFLOAT3 cp;
			XMStoreFloat3(&cp, collisionPoint);
			XMVECTOR planeA = getPlaneNormal(a, collisionPoint);
			XMVECTOR planeB = getPlaneNormal(b, collisionPoint);
			//XMVECTOR planeA = { 0,1,0 };
			//XMVECTOR planeB = { 0,1,0 };
			XMVECTOR aNormal = getCollidedNormal(b, planeB); // sphere don't need to pass the collision detail
			XMVECTOR bNormal = getCollidedNormal(a, planeA);
			ForceCalculation(a, b, aNormal, bNormal, collisionPoint,totalTime);
			return true;
		}
		return false;
	}
	bool CollisionCheck(BoxCollider * a, SphereCollider * b, float totalTime)
	{
		a->UpdateVertex();
		XMVECTOR vertex = { a->maxX,a->maxY ,a->maxZ };
		XMVECTOR halfDiag = vertex - XMLoadFloat3(&a->Position);

		XMVECTOR diff = { a->Position.x - b->Position.x,a->Position.y - b->Position.y,a->Position.z - b->Position.z };
		if(sqrtf(XMVector3Dot(halfDiag, halfDiag).m128_f32[0]) < sqrtf(XMVector3Dot(diff, diff).m128_f32[0])) return false;


		XMVECTOR bPos;
		bPos = XMLoadFloat3(&b->Position);
		for (int i = 0; i < a->planes.size(); i++) {
			XMVECTOR plane = a->planes[i];
			float distance = XMVector3Dot(plane, bPos).m128_f32[0] + plane.m128_f32[3];
			if (abs(distance) <= b->Radius) {
				XMVECTOR collisionPoint = bPos - plane * distance;
				XMFLOAT3 cp;
				XMStoreFloat3(&cp, collisionPoint);
				if (cp.x <= a->maxX && cp.y <= a->maxY && cp.z <= a->maxZ
					&& cp.x >= a->minX && cp.y >= a->minY && cp.z >= a->minZ) {
					XMVECTOR planeA = a->planes[i];
					
					XMVECTOR aNormal = getCollidedNormal(b); 

					//sphere don't need to pass the collision detail, sphere reflect everything
					XMVECTOR bNormal = getCollidedNormal(a, planeA);

					ForceCalculation(a, b, aNormal, bNormal, collisionPoint,totalTime);
					return true;
				}
			}
		}

		//edge check
		for (int i = 0; i < a->edges.size(); i++) {
			std::tuple < XMVECTOR, XMVECTOR > temp = a->edges[i];
			XMVECTOR start = std::get<0>(temp);
			XMVECTOR end = std::get<1>(temp);
			float distance = XMVector3LinePointDistance(start, end, bPos).m128_f32[0];
			if (abs(distance) <= b->Radius) {
				XMVECTOR AB = end - start;
				XMVECTOR AP = bPos - start;
				XMVECTOR collisionPoint = start + XMVector3Dot(AP, AB) / XMVector3Dot(AB, AB) * AB;
				XMFLOAT3 cp;
				XMStoreFloat3(&cp, collisionPoint);
				if (cp.x <= a->maxX && cp.y <= a->maxY && cp.z <= a->maxZ
					&& cp.x >= a->minX && cp.y >= a->minY && cp.z >= a->minZ) {
					XMVECTOR planeA = {1,1,1};

					XMVECTOR aNormal = getCollidedNormal(a, planeA);

					//sphere don't need to pass the collision detail, sphere reflect everything
					XMVECTOR bNormal = getCollidedNormal(b);

					ForceCalculation(a, b, aNormal, bNormal, collisionPoint, totalTime);
					return true;
				}
			}
		}
		return false;
	}
	bool isUnique(std::vector<XMVECTOR> points, XMVECTOR test)
	{
		bool add = true;
		for (int i = 0; i < points.size(); i++) {
			XMVECTOR diff = points[i] - test;
			if (abs(diff.m128_f32[0]) + abs(diff.m128_f32[1]) + abs(diff.m128_f32[2]) < 0.1f) {
				add = false;
			}
		}
		return add;
	}
	XMVECTOR getPlaneNormal(BoxCollider * Entity, XMVECTOR point)
	{
		float x = point.m128_f32[0];
		float y = point.m128_f32[1];
		float z = point.m128_f32[2];
		XMVECTOR ret = { 0,0,0 };

		for (int i = 0; i < Entity->planes.size(); i++) {
			XMVECTOR plane = Entity->planes[i];
			float A = plane.m128_f32[0];
			float B = plane.m128_f32[1];
			float C = plane.m128_f32[2];
			float D = plane.m128_f32[3];
			if (abs(A * x + B * y + C * z + D) <= 0.1f) {
					ret = plane;
			}
		}

		/*if (XMVector3Equal(ret, { 0,0,0 })) {
			printf("6 plane! \n");
			for (int i = 0; i < Entity->planes.size(); i++) {
				XMVECTOR plane = Entity->planes[i];
				float A = plane.m128_f32[0];
				float B = plane.m128_f32[1];
				float C = plane.m128_f32[2];
				float D = plane.m128_f32[3];
				printf("point:  %f,  %f,  %f \n", point.m128_f32[0], point.m128_f32[1], point.m128_f32[2]);
				printf("plane:  %f,  %f,  %f \n", plane.m128_f32[0], plane.m128_f32[1], plane.m128_f32[2]);
				printf("%f \n", abs(A * x + B * y + C * z + D));
			}
		}*/
		return ret;
	}
	XMVECTOR getLineIntersection(XMVECTOR start, XMVECTOR end, std::tuple<XMVECTOR, XMVECTOR, XMVECTOR, XMVECTOR> planeVertetices, 
		XMFLOAT3 Max, XMFLOAT3 Min)
	{
		std::vector<XMVECTOR> planeVerteticesVec;
		planeVerteticesVec.push_back(std::get<0>(planeVertetices));
		planeVerteticesVec.push_back(std::get<1>(planeVertetices));
		planeVerteticesVec.push_back(std::get<2>(planeVertetices));
		planeVerteticesVec.push_back(std::get<3>(planeVertetices));
		XMVECTOR v1 = end - start;
		XMVECTOR ip = { 0,0,0 };
		int counter = 0;
		for (int i = 0; i < planeVerteticesVec.size()-1; i++)
		{
			XMVECTOR A = planeVerteticesVec[i];
			for (int j = i + 1; j < planeVerteticesVec.size(); j++)
			{
				XMVECTOR B = planeVerteticesVec[j];
				XMVECTOR v2 = XMVector3Normalize(B - A);
				//printf("v1:  %f,  %f,  %f \n", v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2]);
				//printf("v2:  %f,  %f,  %f \n", v2.m128_f32[0], v2.m128_f32[1], v2.m128_f32[2]);
				if (abs(XMVector3Dot(v1, v2).m128_f32[0]) == 1)
				{
					// 两线平行
					//printf("line parallel \n ");
					continue;
				}

				XMVECTOR diag = XMVector3Normalize(A - start);
				XMVECTOR vecS1 = XMVector3Cross(v1, v2);            // 有向面积1
				XMVECTOR vecS2 = XMVector3Cross(diag, v2); // 有向面积2
				float num = XMVector3Dot(diag, vecS1).m128_f32[0];

				// 判断两这直线是否共面
				//if (num >= 1E-05f || num <= -1E-05f)
				//{
				//	continue;
				//}

				// 有向面积比值，利用点乘是因为结果可能是正数或者负数
				float num2 = (XMVector3Dot(vecS2, vecS1) / XMVector3LengthSq(vecS1)).m128_f32[0];
				XMVECTOR point = start + v1 * num2;
				
				point = XMVectorClamp(point, { Min.x,Min.y,Min.z }, { Max.x,Max.y,Max.z });
				ip += point;
				counter++;
				//if (point.m128_f32[0] <= Max.x && point.m128_f32[0] >= Min.x &&
				//	point.m128_f32[1] <= Max.y && point.m128_f32[1] >= Min.y &&
				//	point.m128_f32[2] <= Max.z && point.m128_f32[2] >= Min.z) {
			
				//	printf("add!\n");
				//}
				//else {
				//	printf("out of bound!\n");
				//}
			
			}
		}
		if (counter == 0) {
			return { NAN,NAN,NAN };
		}
		XMVECTOR ret = ip / (float)counter;
		return ret;
	}
}
	