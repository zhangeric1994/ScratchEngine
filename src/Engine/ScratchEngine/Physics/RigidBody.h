#ifndef RIGIDBODY_H
#define RIGIDBODY_H
#pragma once

#include "../header.h"

#include "../Common/Typedefs.h"
#include "../Core/GameComponent.h"


namespace ScratchEngine
{
	class RigidBody : public GameComponent
	{
		friend class Collider;


	private:
		XMVECTOR velocity;
		XMVECTOR angularVelocity;

		f32 mass;
		bool isAffectedByGravity;


	public:
		RigidBody();
		RigidBody(XMVECTOR velocity, XMVECTOR angularVelocity);

		void ApplyForce(XMVECTOR force);
		void ApplyAngularForce(XMVECTOR force);
	};
}
#endif