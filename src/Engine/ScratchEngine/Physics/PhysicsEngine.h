#pragma once

#include <vector>

#include "Collider.h"
#include "DynamicBVH.h"

using namespace std;

namespace ScratchEngine
{
	namespace Physics
	{
		class PhysicsEngine
		{
		private:
			static PhysicsEngine* singleton;


		public:
			static void Initialize();


		private:
			DynamicBVH<Collider> hierarchy;
			vector<int> movedObjects;

			PhysicsEngine();


		public:
			void SolveCollision();
		

		private:
			bool CheckExactCollision(Collider* collider1, Collider* collider2);
		};
	}
}