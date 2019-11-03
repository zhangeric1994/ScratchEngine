#pragma once

#include "prerequisites.h"


namespace ScratchEngine
{
	namespace Rendering
	{
		struct Vertex
		{
			union
			{
				XMFLOAT3 position;
				struct
				{
					f32 x;
					f32 y;
					f32 z;
				};
			};
			bool hasBones = false;
			XMFLOAT3 normal;
			XMFLOAT2 uv;
			XMFLOAT3 tangent;
			u32 boneIDs[NUM_BONES_PER_VEREX] = { 0 };
			f32 boneWeights[NUM_BONES_PER_VEREX] = { 0 };
		};
	}
}