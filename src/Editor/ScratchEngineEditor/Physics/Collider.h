#pragma once
#include <DirectXMath.h>
#include "Entity.h"
using namespace DirectX;
namespace Colliders {
	enum ColliderType { Sphere, Box };
	class Collider
	{

	public:
		//Collider(Entity* _item,float _radius, float _mass, bool _gravity,bool _static);
		void Update(float dt, float totalTime);
		void ApplyForce(XMFLOAT3 force);
		void ApplyGravity(float dt);
		Entity* Item;
		XMFLOAT3 Position;
		XMFLOAT3 Velocity;
		bool UseGravity;
		bool Static;
		float Mass;
		ColliderType type;
		std::unordered_map<Collider*, float> CollidedWith;
		// should add game entity reference in here
	};

	class SphereCollider : public Collider
	{
	public:
		float Radius;
		SphereCollider(Entity* _item, float _radius, float _mass, bool _gravity, bool _static);
		~SphereCollider();
		//bool CollidsionCheck(SphereCollider* sphere);
		//bool CollidsionCheck(BoxCollider* box);
	};

	class BoxCollider : public Collider
	{
	public:
		float minX, maxX, minY, maxY, minZ, maxZ;
		XMFLOAT3 size;
		BoxCollider(Entity* _item, XMFLOAT3 size, float _mass, bool _gravity, bool _static);
		~BoxCollider();
		void UpdateVertex();
		//bool CollidsionCheck(SphereCollider* sphere);
		//bool CollidsionCheck(BoxCollider* box);
	};
	XMVECTOR getCollidedNormal(Collider* b, XMFLOAT3 collisionPoint);
	void ForceCalculation(Collider* a,Collider* b, XMFLOAT3 collisionPoint,float totalTime);
	bool CollisionCheck(Collider* a, Collider* b, float totalTime);
	//Sphere check
	bool CollisionCheck(SphereCollider* a, SphereCollider* b, float totalTime);
	bool CollisionCheck(SphereCollider* a, BoxCollider* b, float totalTime);

	//Box check
	bool CollisionCheck(BoxCollider* a, BoxCollider* b, float totalTime);
	bool CollisionCheck(BoxCollider* a, SphereCollider* b, float totalTime);
}
	

