#pragma once

#include <DirectXMath.h>

#define NUM_BONES_PER_VEREX 4
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
			DirectX::XMFLOAT3 Position;	    // The position of the vertex
			//DirectX::XMFLOAT4 Color;        // The color of the vertex
			DirectX::XMFLOAT3 Normal;
			DirectX::XMFLOAT2 UV;
			DirectX::XMFLOAT4 IDs;
			DirectX::XMFLOAT4 Weights;
		};
	}
}