#include "Collision.h"

ScratchEngine::Physics::Collision::Collision(int node1, int node2)
{
	this->node1 = node1;
	this->node2 = node2;
	next = nullptr;
	previous = nullptr;
}
