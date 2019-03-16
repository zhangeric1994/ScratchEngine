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
			if (i != j)
			{
				Colliders::CollisionCheck(a, b);
			}
		}
		a->Update(deltaTime);
	}
}
