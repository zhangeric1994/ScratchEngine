#include "Physics.h"
using namespace Colliders;
Physics::Physics(size_t _MaxColliderNum)
{
	NumCoolidersHandled = 0;
}

Physics::~Physics()
{
}

SphereCollider * Physics::addSphereCollider(Entity * obj, float _radius, float _mass, bool _gravity, bool _static)
{
	SphereCollider* temp = new SphereCollider(obj, _radius, _mass, _gravity, _static);
	ColliderHandler.push_back(temp);
	NumCoolidersHandled++;
	return temp;
}

BoxCollider * Physics::addBoxCollider(Entity * obj,XMFLOAT3 size, float _mass, bool _gravity, bool _static)
{
	BoxCollider* temp = new BoxCollider(obj, size, _mass, _gravity, _static);
	ColliderHandler.push_back(temp);
	NumCoolidersHandled++;
	return temp;
}

void Physics::CollisionsDetection(int start, int end,float deltaTime)
{
	for (int i = start; i < end; i++)
	{
		auto a = ColliderHandler.at(i);
		for (int j = i; j < ColliderHandler.size(); j++)
		{
			//calculate squared distance from centers
			auto b = ColliderHandler.at(j); 
			// in the future, if the collider belongs to the subObject of current checking one, it should has the option to ignore it.
			if (Colliders::CollisionCheck(a, b) && i != j)
			{
				//calculate force
				float force = 0.8f;
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
