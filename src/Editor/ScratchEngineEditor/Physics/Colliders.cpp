#include "Colliders.h"
#include "Physics.h"

Collider::Collider(Entity* _item,float _radius, float _mass, bool _gravity, bool _static)
{
	Item = _item;
	Radius = _radius;
	Mass = _mass;
	UseGravity = _gravity;
	Static = _static;
	Position = Item->individualPositon;
	Velocity = {0,0,0};
}

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

		//drag 
		XMStoreFloat3(&Velocity,(XMLoadFloat3(&Velocity)*(1.0f - dt * 0.5f)));

		Item->SetTranslation(Position.x, Position.y, Position.z);
		DampenTime += dt;
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
