#ifndef BOX_COLLIDER_H
#define BOX_COLLIDER_H

#include "Collider.h"

namespace ScratchEngine
{
	namespace Physics
	{
		class BoxCollider : public Collider
		{
		private:
			XMVECTOR size;

			//XMVECTOR min;
			//XMVECTOR max;

			// float minX, maxX, minY, maxY, minZ, maxZ;
			// float defaultMinX, defaultMaxX, defaultMinY, defaultMaxY, defaultMinZ, defaultMaxZ;

			//std::vector<XMVECTOR> planes;
			//std::vector<std::tuple<XMVECTOR, XMVECTOR, XMVECTOR, XMVECTOR>> planesInfo;
			//std::vector<std::tuple<XMVECTOR, XMVECTOR>> edges;

		public:
			BoxCollider(XMVECTOR size);
			~BoxCollider();
		};
	}
}
#endif