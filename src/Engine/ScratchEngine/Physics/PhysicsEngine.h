#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H


#include "Collider.h"
//#include "Collision.h"
//#include "DynamicBVH.hpp"

using namespace ScratchEngine;
using namespace std;


namespace ScratchEngine
{
	namespace Physics
	{
		class PhysicsEngine
		{
			friend class Collider;


		private:
			static PhysicsEngine* singleton;

			static PhysicsEngine* GetSingleton();
			static void Initialize();


			Collider* colliderList;


			PhysicsEngine();


			void AddCollider(Collider* collider);

			void RemoveCollider(Collider* collider);


			void UpdateBoundingVolumes();
			void SolveCollisions();
		};
	}
}
#endif