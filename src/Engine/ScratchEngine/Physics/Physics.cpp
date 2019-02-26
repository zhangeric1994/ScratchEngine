#include "Physics.h"

Physics::Physics(size_t _MaxColliderNum)
{
	ColliderHandler = new std::vector<const SphereCollider>;
}

Physics::~Physics()
{

}

void Physics::addCollider(const SphereCollider obj)
{
	ColliderHandler->push_back(obj);
}

void Physics::CollisionsDetection(int start, int end)
{
	for (int i = start; i < end; i++)
	{
		for (int j = 0; j < ColliderHandler->size(); j++)
		{
			//calculate squared distance from centers
			float squared_distance = (Spheres[i].Position.x - Spheres[j].Position.x) * (Spheres[i].Position.x - Spheres[j].Position.x) +
				(Spheres[i].Position.y - Spheres[j].Position.y) * (Spheres[i].Position.y - Spheres[j].Position.y) + (Spheres[i].Position.z - Spheres[j].Position.z) * (Spheres[i].Position.z - Spheres[j].Position.z);
			//calculate the sum of the radii squared
			float radius_sum_squared = (Spheres[i].Radius + Spheres[j].Radius) * (Spheres[i].Radius + Spheres[j].Radius);
			if (squared_distance < radius_sum_squared && i != j)
			{
				//calculate force
				float force = pow(abs(squared_distance - radius_sum_squared) * 10.0f, 2.0f) * 10.0f;
				//create normalized vectors to apply the forces in the correct direction
				XMVECTOR f1 = XMVectorSet(Spheres[j].Position.x - Spheres[i].Position.x, Spheres[j].Position.y - Spheres[i].Position.y, Spheres[j].Position.z - Spheres[i].Position.z, 0.0f);
				XMVECTOR f2 = XMVectorSet(Spheres[i].Position.x - Spheres[j].Position.x, Spheres[i].Position.y - Spheres[j].Position.y, Spheres[i].Position.z - Spheres[j].Position.z, 0.0f);
				XMFLOAT3 force1;
				XMFLOAT3 force2;
				XMStoreFloat3(&force1, XMVector3Normalize(f1) * force);
				XMStoreFloat3(&force1, XMVector3Normalize(f2) * force);

				//apply the forces
				Spheres[i].ApplyForce(force1, dt);
				Spheres[j].ApplyForce(force2, dt);

				float dampening_cooldown_time = 0.16f; //time between dampening updates
				float dampening_constant = 0.9f;

				//dampen the spheres' velocities
				if (Spheres[i].dampen_time > dampening_cooldown_time)
				{
					Spheres[i].Velocity.x *= dampening_constant;
					Spheres[i].Velocity.y *= dampening_constant;
					Spheres[i].Velocity.z *= dampening_constant;

					Spheres[i].dampen_time = 0.0f;
				}

				if (Spheres[j].dampen_time > dampening_cooldown_time)
				{
					Spheres[j].Velocity.x *= dampening_constant;
					Spheres[j].Velocity.y *= dampening_constant;
					Spheres[j].Velocity.z *= dampening_constant;

					Spheres[j].dampen_time = 0.0f;
				}
			}
		}
	}
}
