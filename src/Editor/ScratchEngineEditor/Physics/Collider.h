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
		void Update(float dt);
		void ApplyForce(XMFLOAT3 force);
		Entity* Item;
		XMFLOAT3 Position;
		XMFLOAT3 Velocity;
		bool UseGravity;
		bool Static;
		float Mass;
		ColliderType type;
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
		BoxCollider(Entity* _item, XMFLOAT3 size, float _mass, bool _gravity, bool _static);
		~BoxCollider();
		//bool CollidsionCheck(SphereCollider* sphere);
		//bool CollidsionCheck(BoxCollider* box);
	};
	void ForceCalculation(Collider* a, XMVECTOR forceToA,Collider* b, XMVECTOR forceToB);
	bool CollisionCheck(Collider* a, Collider* b);
	//Sphere check
	bool CollisionCheck(SphereCollider* a, SphereCollider* b);
	bool CollisionCheck(SphereCollider* a, BoxCollider* b);

	//Box check
	bool CollisionCheck(BoxCollider* a, BoxCollider* b);
	bool CollisionCheck(BoxCollider* a, SphereCollider* b);
}
	

