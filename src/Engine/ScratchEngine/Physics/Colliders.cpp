#include "Colliders.h"
#include "Physics.h"

SphereCollider::SphereCollider(XMFLOAT3 position, XMFLOAT4 color, float radius, float mass)
{
	XMFLOAT3 topVertex(0.0f, +radius, 0.0f);
	XMFLOAT3 bottomVertex(0.0f, -radius, 0.0f);

	// assign the collider
	Physics::addCollider(this);
}

void Collider::ApplyForce(XMFLOAT3 force, float dt)
{
	Velocity.x += force.x / Mass * dt;
	Velocity.y += force.y / Mass * dt;
	Velocity.z += force.z / Mass * dt;
}
