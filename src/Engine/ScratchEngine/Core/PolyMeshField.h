#pragma once

namespace ScratchEngine
{
	class PolyMeshField
	{
	public:
		PolyMeshField(float* minMeshBounds, float* maxMeshBounds, float cellSize, float cellHeight, int maxVertsPerPoly);
		~PolyMeshField();

		int GetPolyVertCount(int* poly, int maxVertsPerPoly);
		int GetPolyRegion(int polyIndex);
		int* GetPolyVerts(int polyIndex);
		int MaxVertsPerPoly();
		int PolyCount();
		int VertCount();

		// an index value for a non-existent vertex
		static int NULL_INDEX;

	private:
		float* minMeshBounds;
		float* maxMeshBounds;
		float cellSize;
		float cellHeight;
		int maxVertsPerPoly;
		int* verts;
		int* polys; // poly index
		int* polyRegions; // region data for each poly (regionID)
	};
}
