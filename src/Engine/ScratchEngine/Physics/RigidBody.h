#pragma once

#include <DirectXMath.h>

#include "../Core/GameObject.h"
#include "../Utility/Typedefs.h"

using namespace DirectX;

namespace ScratchEngine
{
	namespace Physics
	{
		struct RigidBody : public GameComponent
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
}