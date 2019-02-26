#pragma once
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


	void addCollider(Collider const obj);
	void CollisionsDetection(int start, int end);
private:
	size_t MaxColliderNum;
	std::vector<Collider const>* ColliderHandler;
};
