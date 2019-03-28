#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include <vector>

#include "../Common/Stack.hpp"

#include "Collider.h"
#include "Collision.h"
#include "DynamicBVH.hpp"

using namespace std;

namespace ScratchEngine
{
	namespace Physics
	{
		class PhysicsEngine : IDynamicBVHQueryCallback
		{
		private:
			static PhysicsEngine* singleton;

			static const PhysicsEngine* GetSingleton();
			static void Initialize();

			DynamicBVH<Collider> hierarchy;
			Stack<int> movedObjects;
			Collision* collisionList;

			PhysicsEngine();

			void AddCollision(i32 node1, i32 node2);

			void SolveCollision();
			bool DynamicBVHTestOverlapCallback(i32 node1, i32 node2);
		};
	}
}
#endif