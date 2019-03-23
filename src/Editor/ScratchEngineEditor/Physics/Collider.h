#pragma once
#include <DirectXMath.h>
#include "Entity.h"
#include <tuple>
using namespace DirectX;
namespace Colliders {
	enum ColliderType { Sphere, Box };
	class Collider
	{

	public:
		//Collider(Entity* _item,float _radius, float _mass, bool _gravity,bool _static);
		void Update(float dt, float totalTime);
		void ApplyForce(XMFLOAT3 force);
		void ApplyAngularForce(XMFLOAT3 force);
		void ApplyGravity(float dt);
		Entity* Item;
		XMFLOAT3 Position;
		XMFLOAT3 Rotation;
		XMFLOAT3 Velocity;
		XMFLOAT3 AngularVelocity;
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
		float defaultMinX,defaultMaxX,defaultMinY,defaultMaxY,defaultMinZ,defaultMaxZ;
		XMVECTOR AxisX, AxisY, AxisZ;
		XMFLOAT3 size;
		std::vector<XMVECTOR> planes;
		std::vector<std::tuple<XMVECTOR, XMVECTOR, XMVECTOR, XMVECTOR>> planesInfo;
		std::vector<std::tuple<XMVECTOR,XMVECTOR>> edges;
		BoxCollider(Entity* _item, XMFLOAT3 size, float _mass, bool _gravity, bool _static);
		~BoxCollider();
		void UpdateVertex();
		//bool CollidsionCheck(SphereCollider* sphere);
		//bool CollidsionCheck(BoxCollider* box);
	};
	bool getSeparatingPlane(XMVECTOR RPos, XMVECTOR Plane, BoxCollider* a, BoxCollider* b);
	XMVECTOR getCollidedNormal(Collider* Entity, XMVECTOR reflectPlane = { 0,0,0 });
	XMVECTOR getCollisionPoint(BoxCollider* a, BoxCollider* b);
	void ForceCalculation(Collider* a,Collider* b, XMVECTOR aNormal, XMVECTOR bNormal, XMVECTOR collisionPoint,float totalTime);
	float AngularForceCalculation(Collider* a, XMVECTOR collisionPoint, XMVECTOR force, float totalTime);
	bool CollisionCheck(Collider* a, Collider* b, float totalTime);
	//Sphere check
	bool CollisionCheck(SphereCollider* a, SphereCollider* b, float totalTime);

	//Box check
	bool CollisionCheck(BoxCollider* a, BoxCollider* b, float totalTime);
	bool CollisionCheck(BoxCollider* a, SphereCollider* b, float totalTime);



	// Helper

	bool isUnique(std::vector<XMVECTOR>points, XMVECTOR test);
	XMVECTOR getPlaneNormal(BoxCollider* Entity, XMVECTOR point);
	XMVECTOR getLineIntersection(XMVECTOR start, XMVECTOR end, std::tuple<XMVECTOR, XMVECTOR, XMVECTOR, XMVECTOR> planeVertetices,XMFLOAT3 Max, XMFLOAT3 Min);
}
	

