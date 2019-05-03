#include"TriangleMesh.h"

namespace ScratchEngine
{
	TriangleMesh::TriangleMesh() 
	{
		indices = nullptr;
		triangleRegions = nullptr;
		vertices = nullptr;
	};

	TriangleMesh::~TriangleMesh() 
	{

	};

	int TriangleMesh::GetTriangleRegion(int index)
	{
		if (index < 0 || index >= sizeof(triangleRegions))
		{
			return -1;
		}
		return triangleRegions[index];
	}

	float* TriangleMesh::GetTriangleVerts(int index)
	{

		int pTriangle = index * 3;
		if (index < 0 || pTriangle >= sizeof(indices))
			return nullptr;

		float* result = new float[9];

		for (int i = 0; i < 3; i++)
		{
			int pVert = indices[pTriangle + i] * 3;
			result[i * 3] = vertices[pVert];
			result[i * 3 + 1] = vertices[pVert + 1];
			result[i * 3 + 2] = vertices[pVert + 2];
		}

		return result;
	}

	int TriangleMesh::TriangleCount()
	{
		return (triangleRegions == nullptr ? 0 : sizeof(triangleRegions));
	}

	int TriangleMesh::VertCount()
	{
		return (vertices == nullptr ? 0 : sizeof(vertices) / 3);
	}
}

