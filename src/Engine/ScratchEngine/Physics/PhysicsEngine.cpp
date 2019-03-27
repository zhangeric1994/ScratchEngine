#include "PhysicsEngine.h"

void ScratchEngine::Physics::PhysicsEngine::Initialize()
{
}

void ScratchEngine::Physics::PhysicsEngine::SolveCollision()
{
	for (auto it = movedObjects.begin(); it != movedObjects.end(); it++)
		hierarchy.Query(*it, CheckExactCollision);
}

bool ScratchEngine::Physics::PhysicsEngine::CheckExactCollision(Collider* collider1, Collider* collider2)
{
	if (collider1->Query(collider2, 0))
	{

	}

	return false;
}
