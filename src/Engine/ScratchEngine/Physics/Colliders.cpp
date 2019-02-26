#include "Colliders.h"
#include "Physics.h"

SphereCollider::SphereCollider(XMFLOAT3 position, XMFLOAT4 color, float radius, float mass)
{
	XMFLOAT3 topVertex(0.0f, +radius, 0.0f);
	XMFLOAT3 bottomVertex(0.0f, -radius, 0.0f);

	// assign the collider
	Physics::addCollider(this);
}
