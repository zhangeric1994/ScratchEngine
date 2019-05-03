#include "PolyMeshField.h"

namespace ScratchEngine
{
	PolyMeshField::PolyMeshField(
		float* minMeshBounds,
		float* maxMeshBounds,
		float cellSize, float cellHeight,
		int maxVertsPerPoly
	)
	{
		this->minMeshBounds = minMeshBounds;
		this->maxMeshBounds = maxMeshBounds;
		this->cellSize = cellSize;
		this->cellHeight = cellHeight;
		this->maxVertsPerPoly = maxVertsPerPoly;
	}

	int PolyMeshField::GetPolyVertCount(int* poly, int maxVertsPerPoly)
	{
		for (int i = 0; i < maxVertsPerPoly; i++)
		{
			if (polys[i] == NULL_INDEX)
			{
				return i;
			}
			return maxVertsPerPoly;
		}
	}

	int PolyMeshField::GetPolyRegion(int polyIndex)
	{
		if (polyIndex < 0 || polyIndex >= sizeof(polyRegions))
		{
			return -1;
		}
		return polyRegions[polyIndex];
	}

	int* PolyMeshField::GetPolyVerts(int polyIndex)
	{
		int p = polyIndex * maxVertsPerPoly * 2;
		if (polyIndex < 0 || p >= sizeof(polys))
		{
			return nullptr;
		}

		int pVertCount = GetPolyVertCount(polys, maxVertsPerPoly);
		int* result;
		for (int i = 0; i < pVertCount; i++)
		{
			int pVert = polys[p + i] * 3;
			result[i + 3] = verts[pVert];
			result[i * 3 + 1] = verts[pVert + 1];
			result[i * 3 + 2] = verts[pVert + 2];
		}
		return result;
	}

	int PolyMeshField::MaxVertsPerPoly()
	{
		return maxVertsPerPoly;
	}

	int PolyMeshField::PolyCount()
	{
		if (polys == nullptr)
		{
			return 0;
		}
		return sizeof(polys) / (maxVertsPerPoly * 2);
	}

	int PolyMeshField::VertCount()
	{
		if (verts == nullptr)
		{
			return 0;
		}
		return sizeof(verts) / 3;
	}
}
