#include "PhysicsEngine.h"

ScratchEngine::Physics::PhysicsEngine* ScratchEngine::Physics::PhysicsEngine::singleton = nullptr;

const ScratchEngine::Physics::PhysicsEngine* ScratchEngine::Physics::PhysicsEngine::GetSingleton()
{
	if (!singleton)
		Initialize();

	return singleton;
}

void ScratchEngine::Physics::PhysicsEngine::Initialize()
{
}

ScratchEngine::Physics::PhysicsEngine::PhysicsEngine()
{
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
//
//void ScratchEngine::Physics::PhysicsEngine::SolveCollision()
//{
//	for (int i = 0; i < movedObjects.GetSize(); ++i)
//		hierarchy.IsOverlappingWith(movedObjects[i], this);
//}
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
