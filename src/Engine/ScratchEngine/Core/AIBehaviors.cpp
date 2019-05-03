#include <vector>
#include "AIBehaviors.h"

using namespace ScratchEngine;

AIBehaviors::AIBehaviors(GameObject* ai)
{
	this->agent = ai;
	initLerp = false;
	lerping = false;
	innerLerp = false;
	speed = 0.00015f;
	lerpPeriods = 0;
}

AIBehaviors::~AIBehaviors()
{
	delete this->agent;
}

void AIBehaviors::SetWaypoints(std::vector<XMFLOAT3> pts)
{
	this->waypoints = pts;
}

/*
void AIBehaviors::WaypointsLerp(std::vector<DirectX::XMFLOAT3> pts) {

	int numOfPts = pts.size();

	if (!initLerp) {
		initLerp = true;
		for (int i = 0; i < numOfPts; ++i) {

			if (!innerLerp) {
				t = 0;
				innerLerp = true;
				agent->SetPosition(pts[i].x, pts[i].y, pts[i].z);
			}
			t += speed;
			if (t > 1) {
				t = 1;
				innerLerp = false;
				lerping = false;
			}

			float tempX = lerp(pts[i].x, pts[i+1].x, t);
			float tempY = lerp(pts[i].y, pts[i+1].y, t);
			float tempZ = lerp(pts[i].z, pts[i+1].z, t);

			agent->SetPosition(tempX, tempY, tempZ);
		}
	}
}

*/

void AIBehaviors::WaypointsLerp(DirectX::XMFLOAT3 pt1, DirectX::XMFLOAT3 pt2) {

	if (!initLerp) {
		t = 0;
		initLerp = true;
		agent->SetPosition(pt1.x, pt1.y, pt1.z);
		lerping = true;
	}

	t += speed;

	if (t > 1) {
		t = 1;
		lerpPeriods++;
		initLerp = false;
		lerping = false;
	}

	if (lerping) {
		float tempX = lerp(pt1.x, pt2.x, t);
		float tempY = lerp(pt1.y, pt2.y, t);
		float tempZ = lerp(pt1.z, pt2.z, t);

		agent->SetPosition(tempX, tempY, tempZ);
	}

}

float AIBehaviors::lerp(float a, float b, float t) {

	return a + (b - a) * t;
}