#include "RigidBody.h"

ScratchEngine::Physics::RigidBody::RigidBody()
{
	velocity = XMVectorZero();
	angularVelocity = XMVectorZero();

	mass = 1;
	isAffectedByGravity = true;
}

ScratchEngine::Physics::RigidBody::RigidBody(XMVECTOR velocity, XMVECTOR angularVelocity)
{
	this->velocity = velocity;
	this->angularVelocity = angularVelocity;

	mass = 1;
	isAffectedByGravity = true;
}

void ScratchEngine::Physics::RigidBody::ApplyForce(XMVECTOR force)
{
	if (!gameObject->IsStatic())
		XMVectorAdd(velocity, XMVectorScale(force, 1 / mass));
}

void ScratchEngine::Physics::RigidBody::ApplyAngularForce(XMVECTOR force)
{
	if (!gameObject->IsStatic())
		XMVectorAdd(velocity, XMVectorScale(force, 1 / mass));
}

//void ScratchEngine::Physics::RigidBody::Update(f32 dt, f32 totalTime)
//{
//	if (!gameObject->IsStatic())
//	{
//		gameObject->Translate(XMVectorScale(velocity, dt));
//		gameObject->Rotate(XMQuaternionRotationRollPitchYawFromVector(XMVectorScale(angularVelocity, dt)));
//
//		/* Gravity */
//		if (isAffectedByGravity)
//			XMVectorSubtract(velocity, XMVectorScale(gravity, dt));
//
//		/* Drag */
//		velocity = XMVectorScale(velocity, 1 - dt * 0.5f);
//		angularVelocity = XMVectorScale(angularVelocity, 1 - dt * 0.8f);
//	}
//}
