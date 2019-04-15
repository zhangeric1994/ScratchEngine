#pragma once
#include <DirectXMath.h>
#include "../Render/Entity.h"
using namespace DirectX;
class Collider 
{
public:
	Collider(Entity* _item,float _radius, float _mass, bool _gravity,bool _static);
	void Update(float dt);
	void ApplyForce(XMFLOAT3 force);
	Entity* Item;
	XMFLOAT3 Position;
	XMFLOAT3 Velocity;
	bool UseGravity;
	bool Static;
	float Radius;
	float Mass;
	float DampenTime;
	// should add game entity reference in here
};



class SphereCollider : public Collider
{
public:
	SphereCollider(XMFLOAT3 position, XMFLOAT4 color, float radius, float mass);
	~SphereCollider();//this is to keep the rate at which collision energy is lost constant. This isn't a very physically accurate simulation, so we can cheat a little here
	XMFLOAT4 Color;
	
};


