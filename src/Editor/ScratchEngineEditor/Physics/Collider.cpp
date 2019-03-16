#include "Collider.h"
#include "Physics.h"

using namespace DirectX;
namespace Colliders{
	void Collider::Update(float dt)
	{
		if (!Static) {

			if (UseGravity) {
				Velocity.y -= 9.8f * Mass *dt;
			}
			Position = Item->individualPositon;
			Position.x += Velocity.x *dt;
			Position.y += Velocity.y *dt;
			Position.z += Velocity.z *dt;

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

	BoxCollider::BoxCollider(Entity * _item, XMFLOAT3 size, float _mass, bool _gravity, bool _static)
	{

		Item = _item;
		Mass = _mass;
		UseGravity = _gravity;
		Static = _static;
		Position = Item->individualPositon;
		Velocity = { 0,0,0 };
		type = Box;
		minX = Position.x - size.x / 2;
		maxX = Position.x + size.x / 2;
		minY = Position.y - size.y / 2;
		maxY = Position.y + size.y / 2;
		minZ = Position.z - size.z / 2;
		maxZ = Position.z + size.z / 2;
	}

	BoxCollider::~BoxCollider()
	{
	}

	void ForceCalculation(Collider * a, XMVECTOR forceToA, Collider * b, XMVECTOR forceToB)
	{
		XMVECTOR aVelcity = XMLoadFloat3(&a->Velocity);
		XMVECTOR bVelcity = XMLoadFloat3(&b->Velocity);
		XMFLOAT3 force3;
		XMStoreFloat3(&force3, XMVector3Length(aVelcity * a->Mass + bVelcity * b->Mass) / (a->Mass + b->Mass));
		//float distance = sqrt((point1.x - point2.x) * (point1.x - point2.x) +
		//	(point1.y - point2.y) * (point1.y - point2.y) +
		//	(point1.z - point2.z) * (point1.z - point2.z));
		float force = force3.x*3.5f;
		XMFLOAT3 force1;
		XMFLOAT3 force2;
		XMStoreFloat3(&force1, XMVector3Normalize(forceToA) * force);
		XMStoreFloat3(&force2, XMVector3Normalize(forceToB) * force);
		a->ApplyForce(force1);
		b->ApplyForce(force2);
	}

	bool CollisionCheck(Collider * a, Collider * b)
	{
		if (a->type == Sphere && b->type == Sphere) {
			return CollisionCheck(reinterpret_cast<SphereCollider*> (a), reinterpret_cast<SphereCollider*> (b));
		}
		else if (a->type == Sphere && b->type == Box) {
			return CollisionCheck(reinterpret_cast<SphereCollider*> (a), reinterpret_cast<BoxCollider*> (b));
		}
		else if (a->type == Box && b->type == Box) {
			return CollisionCheck(reinterpret_cast<BoxCollider*> (a), reinterpret_cast<BoxCollider*> (b));
		}
		else if (a->type == Box && b->type == Sphere) {
			return CollisionCheck(reinterpret_cast<BoxCollider*> (a), reinterpret_cast<SphereCollider*> (b));
		}
		return false;
	}

	bool CollisionCheck(SphereCollider * a, SphereCollider * b)
	{
		float squared_distance = (a->Position.x - b->Position.x) * (a->Position.x - b->Position.x) +
			(a->Position.y - b->Position.y) * (a->Position.y - b->Position.y) + (a->Position.z - b->Position.z) * (a->Position.z - b->Position.z);
		//calculate the sum of the radii squared
		float radius_sum_squared = (a->Radius + b->Radius) * (a->Radius + b->Radius);
		if (squared_distance < radius_sum_squared) {
			//create normalized vectors to apply the forces in the correct direction
			XMVECTOR f1 = XMVectorSet(b->Position.x - a->Position.x, b->Position.y - a->Position.y, b->Position.z - a->Position.z, 0.0f);
			XMVECTOR f2 = XMVectorSet(a->Position.x - b->Position.x, a->Position.y - b->Position.y, a->Position.z - b->Position.z, 0.0f);
			ForceCalculation(a, f2, b, f1);
			return true;
		}
		else {
			return false;
		}
	}

	bool CollisionCheck(SphereCollider * a, BoxCollider * b)
	{
		float x = max(b->minX, min(a->Position.x, b->maxX));
		float y = max(b->minY, min(a->Position.y, b->maxY));
		float z = max(b->minZ, min(a->Position.z, b->maxZ));

		float distance = sqrt((x - a->Position.x) * (x - a->Position.x) +
			(y - a->Position.y) * (y - a->Position.y) +
			(z - a->Position.z) * (z - a->Position.z));

		if (distance < a->Radius) {
			//create normalized vectors to apply the forces in the correct direction
			XMVECTOR f1 = XMVectorSet(b->Position.x - a->Position.x, b->Position.y - a->Position.y, b->Position.z - a->Position.z, 0.0f);
			XMVECTOR f2 = XMVector3Normalize(XMVectorSet(-(a->Velocity.x), -(a->Velocity.y), -(a->Velocity.z), 0.0f));
			ForceCalculation(a, f2, b, f1);
			return true;
		}
		else {
			return false;
		}
	}

	bool CollisionCheck(BoxCollider * a, BoxCollider * b)
	{
		if ((a->minX <= b->maxX && a->maxX >= b->minX) &&
			(a->minY <= b->maxY && a->maxY >= b->minY) &&
			(a->minZ <= b->maxZ && a->maxZ >= b->minZ)) {
			//create normalized vectors to apply the forces in the correct direction
			XMVECTOR f1 = XMVector3Normalize(XMVectorSet(-(b->Velocity.x), -(b->Velocity.y), -(b->Velocity.z), 0.0f));
			XMVECTOR f2 = XMVector3Normalize(XMVectorSet(-(a->Velocity.x), -(a->Velocity.y), -(a->Velocity.z), 0.0f));
			ForceCalculation(a, f2, b, f1);
			return true;
		}
		else {
			return false;
		}
	}

	bool CollisionCheck(BoxCollider * a, SphereCollider * b)
	{
		float x = max(a->minX, min(b->Position.x, a->maxX));
		float y = max(a->minY, min(b->Position.y, a->maxY));
		float z = max(a->minZ, min(b->Position.z, a->maxZ));

		float distance = sqrt((x - b->Position.x) * (x - b->Position.x) +
			(y - b->Position.y) * (y - b->Position.y) +
			(z - b->Position.z) * (z - b->Position.z));
		if (distance < b->Radius) {
			//create normalized vectors to apply the forces in the correct direction
			XMVECTOR f1 = XMVectorSet(a->Position.x - b->Position.x, a->Position.y - b->Position.y, a->Position.z - b->Position.z, 0.0f);
			XMVECTOR f2 = XMVector3Normalize(XMVectorSet(-(b->Velocity.x), -(b->Velocity.y), -(b->Velocity.z), 0.0f));
			ForceCalculation(a, f2, b, f1);
			return true;
		}
		else {
			return false;
		}
	}

}
	