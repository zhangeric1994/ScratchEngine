#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H


#include "Collider.h"
//#include "Collision.h"
#include "DynamicBVH.hpp"

using namespace ScratchEngine;
using namespace std;


namespace ScratchEngine
{
	namespace Physics
	{
		class PhysicsEngine : private IDynamicBVHQueryCallback<Collider *>
		{
			friend class Collider;


		public:
			static PhysicsEngine* singleton;

			static PhysicsEngine* GetSingleton();
			static void Initialize();


		private:
			DynamicBVH<Collider*> dynamicBVH;
			Collider* colliderList;


			PhysicsEngine();


			void AddCollider(Collider* collider);

			void RemoveCollider(Collider* collider);


		public:
			void UpdateBoundingVolumes();
			void SolveCollisions();


		private:
			bool DynamicBVHTestOverlapCallback(const DynamicBVHNode<Collider*>& node);
			bool DynamicBVHTestOverlapCallback(const DynamicBVHNode<Collider*>& node1, const DynamicBVHNode<Collider*>& node2);

			void __UpdateBoundingVolume(GameObject* gameObject, Collider* collider);
		};
	}
}
#endif