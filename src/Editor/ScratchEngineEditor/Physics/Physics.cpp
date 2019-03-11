#include "Physics.h"
Physics::Physics(size_t _MaxColliderNum)
{
	ColliderHandler = new std::vector<Collider*>;
	NumCoolidersHandled = 0;
}

Physics::~Physics()
{
	delete ColliderHandler;
}

Collider* Physics::addCollider(Entity* obj, float _radius, float _mass, bool _gravity = false, bool _static = false)
{
	Collider* temp = new Collider(obj, _radius, _mass, _gravity, _static);
	ColliderHandler->push_back(temp);
	NumCoolidersHandled++;
	return temp;
}

void Physics::CollisionsDetection(int start, int end,float deltaTime)
{
	for (int i = start; i < end; i++)
	{
		Collider* a = ColliderHandler->at(i);
		for (int j = i; j < ColliderHandler->size(); j++)
		{
			//calculate squared distance from centers
			Collider* b = ColliderHandler->at(j);
			float squared_distance = (a->Position.x - b->Position.x) * (a->Position.x - b->Position.x) +
				(a->Position.y - b->Position.y) * (a->Position.y - b->Position.y) + (a->Position.z -b->Position.z) * (a->Position.z - b->Position.z);
			//calculate the sum of the radii squared
			float radius_sum_squared = (a->Radius + b->Radius) * (a->Radius + b->Radius);
			// in the future, if the collider belongs to the subObject of current checking one, it should has the option to ignore it.
			if (squared_distance < radius_sum_squared && i != j)
			{
				//calculate force
				float force = pow(abs(squared_distance - radius_sum_squared) * 10.0f, 2.0f);
				//create normalized vectors to apply the forces in the correct direction
				XMVECTOR f1 = XMVectorSet(b->Position.x - a->Position.x, b->Position.y - a->Position.y, b->Position.z - a->Position.z, 0.0f);
				XMVECTOR f2 = XMVectorSet(a->Position.x - b->Position.x, a->Position.y - b->Position.y, a->Position.z - b->Position.z, 0.0f);
				XMFLOAT3 force1;
				XMFLOAT3 force2;
				XMStoreFloat3(&force1, XMVector3Normalize(f2) * force);
				XMStoreFloat3(&force2, XMVector3Normalize(f1) * force);

				//apply the forces
				a->ApplyForce(force1);
				b->ApplyForce(force2);

				//float dampening_cooldown_time = 0.16f; //time between dampening updates
				//float dampening_constant = 0.9f;

				////dampen the spheres' velocities
				//if (a->DampenTime > dampening_cooldown_time)
				//{
				//	a->Velocity.x *= dampening_constant;
				//	a->Velocity.y *= dampening_constant;
				//	a->Velocity.z *= dampening_constant;

				//	a->DampenTime = 0.0f;
				//}

				//if (b->DampenTime > dampening_cooldown_time)
				//{
				//	b->Velocity.x *= dampening_constant;
				//	b->Velocity.y *= dampening_constant;
				//	b->Velocity.z *= dampening_constant;
				//	
				//	b->DampenTime = 0.0f;
				//}
			}
		}
		a->Update(deltaTime);
	}
}
