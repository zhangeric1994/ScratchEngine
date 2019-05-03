#pragma once
#include <DirectXMath.h>

namespace ScratchEngine
{
	class TriangleMesh
	{
	public:
		TriangleMesh();
		~TriangleMesh();

		int GetTriangleRegion(int index);
		float* GetTriangleVerts(int index);
		int TriangleCount();
		int VertCount();

	private:
		int* indices;
		int* triangleRegions;
		float* vertices;
	};
}
