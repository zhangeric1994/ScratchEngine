#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H
#pragma once

#include <DirectXMath.h>

#include "../Common/Typedefs.h"

using namespace DirectX;

namespace ScratchEngine
{
	namespace Rendering
	{
		enum LightType : i32
		{
			Directional = 0,
			Point = 1,
			Spot = 2,
		};


		struct LightSource
		{
		public:
			XMVECTOR ambientColor;
			XMVECTOR diffuseColor;
			LightType type : 32;
			XMFLOAT3 position;
			f32 range;
			XMFLOAT3 direction;
			//int shadowMapID;
		};
	}
}
#endif // !LIGHT_SOURCE_H