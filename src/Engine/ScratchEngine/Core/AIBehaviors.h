#pragma once

#include <vector>
#include <DirectXMath.h>
#include "GameObject.h"

namespace ScratchEngine
{
	class AIBehaviors
	{
	public:
		AIBehaviors(GameObject*);
		~AIBehaviors();
		GameObject* agent = nullptr;

		void SetWaypoints(std::vector<DirectX::XMFLOAT3>);
		// void WaypointsLerp(std::vector<DirectX::XMFLOAT3>);
		void WaypointsLerp(DirectX::XMFLOAT3, DirectX::XMFLOAT3);
		float lerp(float, float, float);

		bool initLerp;
		int lerpPeriods;

	private:
		std::vector<DirectX::XMFLOAT3> waypoints;
		float t;

		bool innerLerp;
		bool lerping;
		float speed;
	};
}
