#include "Colliders.h"
#include "Physics.h"

Collider::Collider(XMFLOAT3 _position, float _radius, float _mass, bool _gravity)
{
	Position = _position;
	Radius = _radius;
	Mass = _mass;

}

void Collider::Update(float dt)
{
	if (Mass < 9999) {
		// if mass is larger than 9999, it is a terrain.
		Velocity.y += 9.8f *Mass;
		Position.x += Velocity.x * dt;
		Position.y += Velocity.y * dt;
		Position.z += Velocity.z * dt;
		dampen_time += dt;
	}
}

void Collider::ApplyForce(XMFLOAT3 force)
{
	if (Mass < 9999) {
		Velocity.x += force.x / Mass;
		Velocity.y += force.y / Mass;
		Velocity.z += force.z / Mass;
	}

}

void Collider::BoundPostionWithObj()
{
	// gameEnenty.Position = Position
}
