#ifndef SPHERE_COLLIDER_H
#define SPHERE_COLLIDER_H

#include "Collider.h"

namespace ScratchEngine
{
	namespace Physics
	{
		class SphereCollider : public Collider
		{
		private:
			f32 radius;

		public:
			SphereCollider(f32 radius);
			~SphereCollider();
		};
	}
}
#endif