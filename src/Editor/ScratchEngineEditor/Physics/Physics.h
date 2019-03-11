#pragma once

#include <DirectXMath.h>
#include <stdio.h>
#include <vector>
#include "Colliders.h"


using namespace DirectX;
class Physics
{
public:
	Physics(size_t _MaxColliderNum);
	~Physics();


	int NumCoolidersHandled;
	Collider* addCollider(Entity* obj, float _radius, float _mass, bool _gravity, bool _static);
	void CollisionsDetection(int start, int end, float deltaTime);
private:
	size_t MaxColliderNum;
	std::vector<Collider*>* ColliderHandler;
};
