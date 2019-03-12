#include "Collider.h"
#include "Physics.h"
namespace Colliders{
	void Collider::Update(float dt)
	{
		if (!Static) {

			if (UseGravity) {
				Velocity.y += 9.8f * Mass *dt;
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
		return squared_distance < radius_sum_squared;
	}

	bool CollisionCheck(SphereCollider * a, BoxCollider * b)
	{
		float x = max(b->minX, min(a->Position.x, b->maxX));
		float y = max(b->minY, min(a->Position.y, b->maxY));
		float z = max(b->minZ, min(a->Position.z, b->maxZ));

		float distance = sqrt((x - a->Position.x) * (x - a->Position.x) +
			(y - a->Position.y) * (y - a->Position.y) +
			(z - a->Position.z) * (z - a->Position.z));

		return distance < a->Radius;
	}

	bool CollisionCheck(BoxCollider * a, BoxCollider * b)
	{
		return (a->minX <= b->maxX && a->maxX >= b->minX) &&
			(a->minY <= b->maxY && a->maxY >= b->minY) &&
			(a->minZ <= b->maxZ && a->maxZ >= b->minZ);
	}

	bool CollisionCheck(BoxCollider * a, SphereCollider * b)
	{
		float x = max(a->minX, min(b->Position.x, a->maxX));
		float y = max(a->minY, min(b->Position.y, a->maxY));
		float z = max(a->minZ, min(b->Position.z, a->maxZ));

		float distance = sqrt((x - b->Position.x) * (x - b->Position.x) +
			(y - b->Position.y) * (y - b->Position.y) +
			(z - b->Position.z) * (z - b->Position.z));

		return distance < b->Radius;
	}

}
	