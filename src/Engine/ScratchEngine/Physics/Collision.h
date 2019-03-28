#ifndef COLLISION_H
#define COLLISION_H

#include "DynamicBVHNode.hpp"

namespace ScratchEngine
{
	namespace Physics
	{
		struct Collision
		{
			friend class PhysicsEngine;


		private:
			int node1;
			int node2;
			Collision* next;
			Collision* previous;


		public:
			Collision(int node1, int node2);
		};
	}
}
#endif