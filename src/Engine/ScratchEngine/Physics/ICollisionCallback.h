#ifndef ICOLLISIONCALLBACK_H
#define ICOLLISIONCALLBACK_H
#pragma once


#include "BoundingVolume.h"


namespace ScratchEngine
{
	class GameObject;

	namespace Physics
	{
		class PhysicsEngine;
	}


	using namespace ScratchEngine::Physics;


	struct __declspec(dllexport) ICollisionCallback
	{
		friend class PhysicsEngine;


	private:
		virtual void OnBeginOverlapping(GameObject* collider) = 0;
		virtual void OnOverlapping(GameObject* collider) = 0;
		virtual void OnEndOverlapping(GameObject* collider) = 0;
	};
}
#endif