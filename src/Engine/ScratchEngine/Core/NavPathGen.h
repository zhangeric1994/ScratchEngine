#pragma once

namespace ScratchEngine
{
	class NavPathGen
	{
	public:
		float TriangleArea(float* a, float* b, float* c);
		bool Vequal(float* a, float* b);
		int FindPath(float* portals, int nportals, float* pts, int maxPts);
	private:

	};
}