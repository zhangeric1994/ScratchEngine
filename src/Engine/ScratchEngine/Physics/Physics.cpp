#include "Physics.h"
float dt = 0.01f;
Physics::Physics(size_t _MaxColliderNum)
{
	ColliderHandler = new std::vector<Collider>;
}

Physics::~Physics()
{

}

void Physics::addCollider(Collider obj)
{
	ColliderHandler->push_back(obj);
}

void Physics::CollisionsDetection(int start, int end)
{
	for (int i = start; i < end; i++)
	{
		Collider a = ColliderHandler->at(i);
		for (int j = 0; j < ColliderHandler->size(); j++)
		{
			//calculate squared distance from centers
			Collider b = ColliderHandler->at(j);
			float squared_distance = (a.Position.x - b.Position.x) * (a.Position.x - b.Position.x) +
				(a.Position.y - b.Position.y) * (a.Position.y - b.Position.y) + (a.Position.z -b.Position.z) * (a.Position.z - b.Position.z);
			//calculate the sum of the radii squared
			float radius_sum_squared = (a.Radius + b.Radius) * (a.Radius + b.Radius);
			if (squared_distance < radius_sum_squared && i != j)
			{
				//calculate force
				float force = pow(abs(squared_distance - radius_sum_squared) * 10.0f, 2.0f) * 10.0f;
				//create normalized vectors to apply the forces in the correct direction
				XMVECTOR f1 = XMVectorSet(b.Position.x - a.Position.x, b.Position.y - a.Position.y, b.Position.z - a.Position.z, 0.0f);
				XMVECTOR f2 = XMVectorSet(a.Position.x - b.Position.x, a.Position.y - b.Position.y, a.Position.z - b.Position.z, 0.0f);
				XMFLOAT3 force1;
				XMFLOAT3 force2;
				XMStoreFloat3(&force1, XMVector3Normalize(f1) * force);
				XMStoreFloat3(&force1, XMVector3Normalize(f2) * force);

				//apply the forces
				a.ApplyForce(force1, dt);
				b.ApplyForce(force2, dt);

				float dampening_cooldown_time = 0.16f; //time between dampening updates
				float dampening_constant = 0.9f;

				//dampen the spheres' velocities
				if (a.dampen_time > dampening_cooldown_time)
				{
					a.Velocity.x *= dampening_constant;
					a.Velocity.y *= dampening_constant;
					a.Velocity.z *= dampening_constant;

					a.dampen_time = 0.0f;
				}

				if (b.dampen_time > dampening_cooldown_time)
				{
					b.Velocity.x *= dampening_constant;
					b.Velocity.y *= dampening_constant;
					b.Velocity.z *= dampening_constant;
					
					b.dampen_time = 0.0f;
				}
			}
		}
	}
}
