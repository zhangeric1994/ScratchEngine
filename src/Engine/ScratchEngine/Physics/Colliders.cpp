#include "Colliders.h"
#include "Physics.h"

void Collider::Update(float dt)
{
	if (Velocity.x * Velocity.y * Velocity.z != 0) {
		Position.x += Velocity.x * dt;
		Position.y += Velocity.y * dt;
		Position.z += Velocity.z * dt;
	}
}

void Collider::ApplyForce(XMFLOAT3 force)
{
	Velocity.x += force.x / Mass;
	Velocity.y += force.y / Mass;
	Velocity.z += force.z / Mass;
}

void Collider::BoundPostionWithObj()
{
	// gameEnenty.Position = Position
}
