#ifndef COLLIDER_H
#define COLLIDER_H

#include "../header.h"

#include "../Common/Typedefs.h"
#include "../Core/GameComponent.h"


namespace ScratchEngine
{
	namespace Physics
	{
		class PhysicsEngine;
	}

	using namespace ScratchEngine::Physics;

	//XMVECTOR gravity = XMVectorSet(0, -9.8f, 0, 0);
	//XMVECTOR AxisX = XMVectorSet(1, 0, 0, 0);
	//XMVECTOR AxisY = XMVectorSet(0, 1, 0, 0);
	//XMVECTOR AxisZ = XMVectorSet(0, 0, 1, 0);

	class __declspec(dllexport) Collider : public GameComponent
	{
		friend class PhysicsEngine;


	private:
		i32 id;
		Collider* next;
		Collider* previous;
		unordered_set<Collider*> contacts;


	protected:
		BoundingVolumeType type;
		BoundingVolume* boundingVolume;


		Collider();
		Collider(BoundingVolumeType type);
		~Collider();


	public:
		BoundingVolumeType GetType();
		BoundingVolume* GetBoundingVolume();

		bool IsOverlappingWith(Collider* other);
	};


	class __declspec(dllexport) BoxCollider : public Collider
	{
	private:
		XMVECTOR size;
		XMVECTOR offset;


	public:
		BoxCollider(f32 x, f32 y, f32 z);
		BoxCollider(XMVECTOR size = { 1, 1, 1 }, XMVECTOR offset = { 0, 0, 0 });
		~BoxCollider();


		XMVECTOR GetSize() const;
		XMVECTOR GetOffset() const;

		void SetSize(f32 x, f32 y, f32 z);
		void SetSize(XMVECTOR xyz);
		void SetOffset(f32 x, f32 y, f32 z);
		void SetOffset(XMVECTOR xyz);
	};


	class __declspec(dllexport) SphereCollider : public Collider
	{
	private:
		f32 radius;


	public:
		SphereCollider();
		SphereCollider(f32 radius);
		~SphereCollider();


		f32 GetRadius() const;
	};

	//bool GetSeparatingPlane(XMVECTOR RPos, XMVECTOR Plane, BoxCollider* a, BoxCollider* b);
	//XMVECTOR getCollidedNormal(Collider* Entity, XMVECTOR reflectPlane = { 0,0,0 });
	//XMVECTOR getCollisionPoint(BoxCollider* a, BoxCollider* b);
	//void ForceCalculation(Collider* a, Collider* b, XMVECTOR aNormal, XMVECTOR bNormal, XMVECTOR collisionPoint, float totalTime);
	//float AngularForceCalculation(Collider* a, XMVECTOR collisionPoint, XMVECTOR force, float totalTime);

	////Sphere check
	//bool CollisionCheck(SphereCollider* a, SphereCollider* b, float totalTime);

	////Box check
	//bool CollisionCheck(BoxCollider* a, BoxCollider* b, float totalTime);
	//bool CollisionCheck(BoxCollider* a, SphereCollider* b, float totalTime);

	//// Helper
	//bool isUnique(std::vector<XMVECTOR>points, XMVECTOR test);
	//XMVECTOR getPlaneNormal(BoxCollider* Entity, XMVECTOR point);
	//XMVECTOR getLineIntersection(XMVECTOR start, XMVECTOR end, std::tuple<XMVECTOR, XMVECTOR, XMVECTOR, XMVECTOR> planeVertetices, XMFLOAT3 Max, XMFLOAT3 Min);
}
#endif