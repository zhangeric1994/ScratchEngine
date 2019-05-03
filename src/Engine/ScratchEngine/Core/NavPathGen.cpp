#include <cmath>
#include "windows.h"
#include "NavPathGen.h"

namespace ScratchEngine
{
	float NavPathGen::TriangleArea(float* a, float* b, float* c)
	{
		float ax = b[0] - a[0];
		float ay = b[1] - a[1];
		float bx = c[0] - a[0];
		float by = c[1] - a[1];
		return bx * ay - ax * by;
	}

	int NavPathGen::FindPath(float* portals, int nportals, float* pts, int maxPts)
	{
		int numOfPts = 0;
		float currentPts[2];
		float leftPts[2]; 
		float rightPts[2];

		int currentIndex = 0;
		int leftIndex = 0;
		int rightIndex = 0;

		memcpy(currentPts, &portals[0], sizeof(float));
		memcpy(leftPts, &portals[1], sizeof(float));
		memcpy(rightPts, &portals[2], sizeof(float));
		memcpy(&pts[numOfPts * 2], currentPts, sizeof(float));
		numOfPts++;

		for (int i = 0; i < nportals; i++)
		{
			/// move left
				/// test overlap

			/// move right
				/// test overlap
		}
	}
}