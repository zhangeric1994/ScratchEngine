#pragma once

#include "prerequisites.h"


// --------------------------------------------------------
// A custom vertex definition
//
// You will eventually ADD TO this, and/or make more of these!
// --------------------------------------------------------
namespace ScratchEngine
{
	namespace Rendering
	{

		struct VertexBoneData
		{
			UINT IDs[NUM_BONES_PER_VEREX];
			float Weights[NUM_BONES_PER_VEREX];
		};

		struct Vertex
		{
			XMFLOAT3 Position;
			f32 hasBones = 0;
			XMFLOAT3 Normal;
			XMFLOAT2 UV;
			XMFLOAT3 tangent;
			u32 IDs[NUM_BONES_PER_VEREX] = { 0 };
			f32 Weights[NUM_BONES_PER_VEREX] = { 0 };
		};
	}
}