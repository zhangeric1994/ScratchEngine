#pragma once

#include <DirectXMath.h>

using namespace DirectX;
class Collider 
{
public:
	void Update(float dt);
	void ApplyForce(XMFLOAT3 force, float dt);
	XMFLOAT3 Position;
	XMFLOAT3 Velocity;
	float Radius;
	float Mass;
	float dampen_time;
};



class SphereCollider : public Collider
{
public:
	SphereCollider(XMFLOAT3 position, XMFLOAT4 color, float radius, float mass);
	~SphereCollider();//this is to keep the rate at which collision energy is lost constant. This isn't a very physically accurate simulation, so we can cheat a little here
	XMFLOAT4 Color;
	
};


