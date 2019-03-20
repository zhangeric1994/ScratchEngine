#include "Collider.h"
#include "Physics.h"

using namespace DirectX;
float collisionCheckBack = 0.08f;
namespace Colliders{
	void Collider::Update(float dt, float totalTime)
	{
		if (!Static) {
			Position = Item->individualPositon;
			Position.x += Velocity.x *dt;
			Position.y += Velocity.y *dt;
			Position.z += Velocity.z *dt;
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

			Item->SetTranslation(Position.x, Position.y, Position.z);
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
		Velocity = { 0,0,0 };
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
		Velocity = { 0,0,0 };
		type = Box;
		size = _size;
		UpdateVertex();
	}

	BoxCollider::~BoxCollider()
	{
	}

	void BoxCollider::UpdateVertex()
	{
		minX = Position.x - size.x / 2;
		maxX = Position.x + size.x / 2;
		minY = Position.y - size.y / 2;
		maxY = Position.y + size.y / 2;
		minZ = Position.z - size.z / 2;
		maxZ = Position.z + size.z / 2;
	}


	XMVECTOR getCollidedNormal(Collider * a, XMFLOAT3 collisionPoint)
	{
		if (a->type == Sphere) {
			return { 1.0,1.0,1.0, 0 };
		}
		else if (a->type == Box) {
			BoxCollider* item = reinterpret_cast<BoxCollider*> (a);
			XMVECTOR AB = {0.0,0.0,0.0};
			XMVECTOR AD = { 0.0,0.0,0.0 };
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
		return XMVECTOR();
	}

	void ForceCalculation(Collider * a, Collider * b, XMFLOAT3 collisionPoint, float totalTime)
	{
		XMVECTOR aVelocity = XMLoadFloat3(&a->Velocity);
		XMVECTOR bVelocity = XMLoadFloat3(&b->Velocity);
		XMFLOAT3 aForce;
		XMFLOAT3 bForce;

		XMVECTOR aNormal = getCollidedNormal(a, collisionPoint);
		XMVECTOR bNormal = getCollidedNormal(b, collisionPoint);

		if (b->Static) {
			// then the momentum is inifinity
			// reverse the force
			XMStoreFloat3(&aForce, -(aVelocity * a->Mass) * 1.9f * bNormal);
			a->ApplyForce(aForce);
			return;
		}
		else {
			XMStoreFloat3(&aForce, (((a->Mass - b->Mass)*aVelocity + 2 * b->Mass * bVelocity) / (a->Mass + b->Mass)) * 0.75f * a->Mass * bNormal);
		}


		if (a->Static) {
			// then the momentum is inifinity
			// reverse the force
			XMStoreFloat3(&bForce, -(bVelocity * b->Mass) * 1.9f * aNormal);
			b->ApplyForce(bForce);
			return;
		}
		else {
			XMStoreFloat3(&bForce, -(((b->Mass - a->Mass)*bVelocity + 2 * a->Mass * bVelocity) / (a->Mass + b->Mass)) * 0.75f * b->Mass * aNormal);
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
				a->ApplyForce(aForce);
				b->ApplyForce(bForce);

				XMVECTOR f1 = XMVectorSet(b->Position.x - a->Position.x, b->Position.y - a->Position.y, b->Position.z - a->Position.z, 0.0f);
				XMVECTOR f2 = XMVectorSet(a->Position.x - b->Position.x, a->Position.y - b->Position.y, a->Position.z - b->Position.z, 0.0f);
				XMFLOAT3 force1;
				XMFLOAT3 force2;
				XMStoreFloat3(&force1, XMVector3Normalize(f1));
				XMStoreFloat3(&force2, XMVector3Normalize(f2));
				a->ApplyForce(force2);
				b->ApplyForce(force1);
			}
		}
	}

	bool CollisionCheck(Collider * a, Collider * b, float totalTime)
	{
		if (a->type == Sphere && b->type == Sphere) {
			return CollisionCheck(reinterpret_cast<SphereCollider*> (a), reinterpret_cast<SphereCollider*> (b), totalTime);
		}
		else if (a->type == Sphere && b->type == Box) {
			return CollisionCheck(reinterpret_cast<SphereCollider*> (a), reinterpret_cast<BoxCollider*> (b), totalTime);
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
			//create normalized vectors to apply the forces in the correct direction
			//XMVECTOR f1 = XMVectorSet(b->Position.x - a->Position.x, b->Position.y - a->Position.y, b->Position.z - a->Position.z, 0.0f);
			//XMVECTOR f2 = XMVectorSet(a->Position.x - b->Position.x, a->Position.y - b->Position.y, a->Position.z - b->Position.z, 0.0f);
			ForceCalculation(a, b, {0,0,0},totalTime);
			return true;
		}
		else {
			return false;
		}
	}

	bool CollisionCheck(SphereCollider * a, BoxCollider * b, float totalTime)
	{
		b->UpdateVertex();
		float x = max(b->minX, min(a->Position.x, b->maxX));
		float y = max(b->minY, min(a->Position.y, b->maxY));
		float z = max(b->minZ, min(a->Position.z, b->maxZ));

		float distance = sqrt((x - a->Position.x) * (x - a->Position.x) +
			(y - a->Position.y) * (y - a->Position.y) +
			(z - a->Position.z) * (z - a->Position.z));

		if (distance < a->Radius) {
			//create normalized vectors to apply the forces in the correct direction
			//XMVECTOR f1 = XMVectorSet(b->Position.x - a->Position.x, b->Position.y - a->Position.y, b->Position.z - a->Position.z, 0.0f);
			//XMVECTOR f2 = XMVector3Normalize(XMVectorSet(-(a->Velocity.x), -(a->Velocity.y), -(a->Velocity.z), 0.0f));
			XMFLOAT3 collisionPoint = { x,y,z };
			ForceCalculation(a, b, collisionPoint, totalTime);
			return true;
		}
		else {
			return false;
		}
	}

	bool CollisionCheck(BoxCollider * a, BoxCollider * b, float totalTime)
	{
		a->UpdateVertex();
		b->UpdateVertex();
		XMVECTOR collisionPoint = { b->maxX,b->maxY,b->maxZ};
		if ((a->minX <= b->maxX && a->maxX >= b->minX) &&
			(a->minY <= b->maxY && a->maxY >= b->minY) &&
			(a->minZ <= b->maxZ && a->maxZ >= b->minZ)) {
			//create normalized vectors to apply the forces in the correct direction
			//XMVECTOR f1 = XMVector3Normalize(XMVectorSet(-(b->Velocity.x), -(b->Velocity.y), -(b->Velocity.z), 0.0f));
			//XMVECTOR f2 = XMVector3Normalize(XMVectorSet(-(a->Velocity.x), -(a->Velocity.y), -(a->Velocity.z), 0.0f));
			ForceCalculation(a, b, { 1,1,1 }, totalTime);
			return true;
		}
		else {
			return false;
		}
	}

	bool CollisionCheck(BoxCollider * a, SphereCollider * b, float totalTime)
	{
		a->UpdateVertex();
		float x = max(a->minX, min(b->Position.x, a->maxX));
		float y = max(a->minY, min(b->Position.y, a->maxY));
		float z = max(a->minZ, min(b->Position.z, a->maxZ));

		float distance = sqrt((x - b->Position.x) * (x - b->Position.x) +
			(y - b->Position.y) * (y - b->Position.y) +
			(z - b->Position.z) * (z - b->Position.z));
		if (distance < b->Radius) {
			//create normalized vectors to apply the forces in the correct direction
			//XMVECTOR f1 = XMVectorSet(a->Position.x - b->Position.x, a->Position.y - b->Position.y, a->Position.z - b->Position.z, 0.0f);
			//XMVECTOR f2 = XMVector3Normalize(XMVectorSet(-(b->Velocity.x), -(b->Velocity.y), -(b->Velocity.z), 0.0f));
			XMFLOAT3 collisionPoint = { x,y,z };
			ForceCalculation(a, b, collisionPoint, totalTime);
			return true;
		}
		else {
			return false;
		}
	}

}
	