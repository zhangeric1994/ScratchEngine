#pragma once

#include <DirectXMath.h>
#include <stdio.h>
#include <vector>
#include "Collider.h"
using namespace DirectX;
namespace Colliders
{
	class CollisionManager
	{
	public:
		CollisionManager(size_t _MaxColliderNum);
		~CollisionManager();

		int NumCoolidersHandled;
		SphereCollider* addSphereCollider(Entity* obj, float _radius, float _mass, bool _gravity, bool _static);
		BoxCollider* addBoxCollider(Entity * obj, XMFLOAT3 size, float _mass, bool _gravity, bool _static);
		void CollisionsDetection(int start, int end, float deltaTime, float totalTime);
	private:
		size_t MaxColliderNum;
		std::vector<Collider*> ColliderHandler;
	};
}
