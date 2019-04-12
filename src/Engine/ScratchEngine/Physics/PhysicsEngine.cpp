#include "../Core/GameObject.h"

#include "PhysicsEngine.h"


ScratchEngine::Physics::PhysicsEngine* ScratchEngine::Physics::PhysicsEngine::singleton = nullptr;

ScratchEngine::Physics::PhysicsEngine* ScratchEngine::Physics::PhysicsEngine::GetSingleton()
{
	if (!singleton)
		Initialize();

	return singleton;
}

void ScratchEngine::Physics::PhysicsEngine::Initialize()
{
	singleton = new PhysicsEngine();
}

ScratchEngine::Physics::PhysicsEngine::PhysicsEngine()
{
	colliderList = nullptr;
}

void ScratchEngine::Physics::PhysicsEngine::AddCollider(Collider* collider)
{
	collider->next = colliderList;
	collider->previous = nullptr;

	if (colliderList)
		colliderList->previous = collider;

	colliderList = collider;
}

void ScratchEngine::Physics::PhysicsEngine::RemoveCollider(Collider* collider)
{
	if (!colliderList)
		return;

	Collider* next = collider->next;
	Collider* previous = collider->previous;

	if (previous)
		previous->next = next;
	else
		colliderList = next;

	if (next)
		next->previous = previous;
}

//void ScratchEngine::Physics::PhysicsEngine::AddCollision(i32 node1, i32 node2)
//{
//	{
//		Collision* collision = new Collision(node1, node2);
//
//		if (collisionList)
//		{
//			collision->next = collisionList;
//			collisionList->previous = collision;
//		}
//		else
//			collisionList = collision;
//	}
//}

void ScratchEngine::Physics::PhysicsEngine::UpdateBoundingVolumes()
{
	for (Collider* collider = colliderList; collider; collider = collider->next)
	{
		GameObject* gameObject = collider->GetGameObject();

		switch (collider->type)
		{
		case AABB:
			break;


		case OBB:
			if (!collider->boundingVolume)
				collider->boundingVolume = new OrientedBoundingBox();

			static_cast<OrientedBoundingBox*>(collider->boundingVolume)->SetData(gameObject->GetPosition(), gameObject->GetRotation(), XMVectorMultiply(gameObject->GetLocalScale(), static_cast<BoxCollider*>(collider)->GetSize()));
			break;


		case Sphere:
			if (!collider->boundingVolume)
				collider->boundingVolume = new BoundingSphere();

			static_cast<BoundingSphere*>(collider->boundingVolume)->SetData(gameObject->GetPosition(), static_cast<SphereCollider*>(collider)->GetRadius());
			break;
		}
	}
}

void ScratchEngine::Physics::PhysicsEngine::SolveCollisions()
{
	for (Collider* colliderA = colliderList; colliderA; colliderA = colliderA->next)
		if (colliderA)
			for (Collider* colliderB = colliderA->next; colliderB; colliderB = colliderB->next)
			{
				auto it = colliderA->contacts.find(colliderB);

				GameObject* gameObjectA = colliderA->GetGameObject();
				GameObject* gameObjectB = colliderB->GetGameObject();

				if (colliderA->IsOverlappingWith(colliderB))
				{
					if (it == colliderA->contacts.end())
					{
						colliderA->contacts.insert(colliderB);
						static_cast<ICollisionCallback*>(gameObjectA)->OnBeginOverlapping(gameObjectB);

						colliderB->contacts.insert(colliderA);
						static_cast<ICollisionCallback*>(gameObjectB)->OnBeginOverlapping(gameObjectA);
					}

					static_cast<ICollisionCallback*>(gameObjectA)->OnOverlapping(gameObjectB);
					static_cast<ICollisionCallback*>(gameObjectB)->OnOverlapping(gameObjectA);
				}
				else if (it != colliderA->contacts.end())
				{
					colliderA->contacts.erase(it);
					static_cast<ICollisionCallback*>(gameObjectA)->OnEndOverlapping(gameObjectB);

					colliderB->contacts.erase(colliderA);
					static_cast<ICollisionCallback*>(gameObjectB)->OnEndOverlapping(gameObjectA);
				}
			}
}

//
//bool ScratchEngine::Physics::PhysicsEngine::DynamicBVHTestOverlapCallback(i32 node1, i32 node2)
//{
//	if (node1 != node2)
//	{
//		AddCollision(node1, node2);
//	}
//
//	return true;
//}
