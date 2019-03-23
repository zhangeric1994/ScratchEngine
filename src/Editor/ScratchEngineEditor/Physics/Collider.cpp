#include "Collider.h"
#include "Physics.h"
#include <tuple>

using namespace DirectX;
float collisionCheckBack = 0.00f;
namespace Colliders{
	void Collider::Update(float dt, float totalTime)
	{
		if (!Static) {
			Position = Item->individualPositon;

			Position.x += Velocity.x *dt;
			Position.y += Velocity.y *dt;
			Position.z += Velocity.z *dt;

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
			XMStoreFloat3(&Velocity, (XMLoadFloat3(&Velocity)*(1.0f - dt * 0.5f)));
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
		planes.push_back(XMPlaneFromPoints(A, C, B));
		planes.push_back(XMPlaneFromPoints(A, E, C));
		planes.push_back(XMPlaneFromPoints(A, E, F));
		planes.push_back(XMPlaneFromPoints(E, F, G));
		planes.push_back(XMPlaneFromPoints(C, G, D));
		planes.push_back(XMPlaneFromPoints(B, D, F));

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
		for (int i = 0; i < a->planes.size(); i++) {
			for (int j = 0; j < b->planes.size(); j++) {
				XMPlaneIntersectPlane(&pLinePoint1, &pLinePoint2, a->planes[i], b->planes[j]);
				if (!isnan(pLinePoint1.m128_f32[0])) {
					// if there is a line
					if (pLinePoint1.m128_f32[0] > a->maxX && pLinePoint2.m128_f32[0] > a->maxX) {
						// both of them are larger than max value, not valid
						continue;
					}
					if (pLinePoint1.m128_f32[1] > a->maxY && pLinePoint2.m128_f32[1] > a->maxY) {
						// both of them are larger than max value, not valid
						continue;
					}
					if (pLinePoint1.m128_f32[2] > a->maxZ && pLinePoint2.m128_f32[2] > a->maxZ) {
						// both of them are larger than max value, not valid
						continue;
					}
					// if there is a line
					if (pLinePoint1.m128_f32[0] < a->minX && pLinePoint2.m128_f32[0] < a->minX) {
						// both of them are less than min value, not valid
						continue;
					}
					if (pLinePoint1.m128_f32[1] < a->minY && pLinePoint2.m128_f32[1] < a->minY) {
						// both of them are less than min value, not valid
						continue;
					}
					if (pLinePoint1.m128_f32[2] < a->minZ && pLinePoint2.m128_f32[2] < a->minZ) {
						// both of them are less than min value, not valid
						continue;
					}
					pLinePoint1 = XMVectorClamp(pLinePoint1, { a->minX,a->minY,a->minZ }, { a->maxX,a->maxY,a->maxZ });
					pLinePoint1 = XMVectorClamp(pLinePoint1, { b->minX,b->minY,b->minZ }, { b->maxX,b->maxY,b->maxZ });
					pLinePoint2 = XMVectorClamp(pLinePoint2, { a->minX,a->minY,a->minZ }, { a->maxX,a->maxY,a->maxZ });
					pLinePoint2 = XMVectorClamp(pLinePoint2, { b->minX,b->minY,b->minZ }, { b->maxX,b->maxY,b->maxZ });
					XMFLOAT3 cp;
					XMStoreFloat3(&cp, (pLinePoint1 + pLinePoint2) / 2);
					return (pLinePoint1 + pLinePoint2) / 2;
					//XMStoreFloat3(&cp,(pLinePoint1 + pLinePoint2) / 2);
				/*	if (cp.x <= a->maxX && cp.y <= a->maxY && cp.z <= a->maxZ
						&& cp.x >= a->minX && cp.y >= a->minY && cp.z >= a->minZ) {
						printf("cp:  %f,  %f,  %f \n", cp.x, cp.y, cp.z);
						
						
						if (isUnique(points, (pLinePoint1 + pLinePoint2) / 2)) {
							points.push_back((pLinePoint1 + pLinePoint2) / 2);
						}
						XMVECTOR aPos = XMLoadFloat3(&a->Position);*/
						//return std::tuple<XMVECTOR, XMVECTOR, XMVECTOR>((pLinePoint1 + pLinePoint2) / 2, {0,1,0}, b->planes[j]);
					//}
				}
			}
		}
		return { 1,1,1 };
		//XMVECTOR ret = {0,0,0};
		//for (int i = 0; i < points.size(); i++) {
			//ret += points[i];
		//}
		//ret /= (float)points.size();
		//return ret;
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

			XMVECTOR aF = (aVelocity * a->Mass) * 1.9f * aNormal;
			//float aRatio = AngularForceCalculation(a, collisionPoint, aF, totalTime);
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
			XMVECTOR bF = (bVelocity * b->Mass) * 1.9f * bNormal;
		//	float bRatio = AngularForceCalculation(b, collisionPoint, bF, totalTime);
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
			if (abs(vSum.x) >= 0.10f) {
				a->CollidedWith[b] = totalTime;
				b->CollidedWith[a] = totalTime;
				XMVECTOR aF = XMLoadFloat3(&aForce);
				XMVECTOR bF = XMLoadFloat3(&bForce);
				float aRatio = AngularForceCalculation(a, collisionPoint, aF, totalTime);
				float bRatio = AngularForceCalculation(b, collisionPoint, bF, totalTime);
				XMStoreFloat3(&aForce, aF * (1 - aRatio));
				XMStoreFloat3(&bForce, bF * (1 - bRatio));
				a->ApplyForce(aForce);
				b->ApplyForce(bForce);

				XMVECTOR f1 = XMVectorSet(b->Position.x - a->Position.x, b->Position.y - a->Position.y, b->Position.z - a->Position.z, 0.0f);
				XMVECTOR f2 = XMVectorSet(a->Position.x - b->Position.x, a->Position.y - b->Position.y, a->Position.z - b->Position.z, 0.0f);
				XMFLOAT3 force1;
				XMFLOAT3 force2;
				XMStoreFloat3(&force1, f1);
				XMStoreFloat3(&force2, f2);
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
}
	