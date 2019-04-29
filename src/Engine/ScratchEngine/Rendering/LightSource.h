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
		enum class LightType
		{
			DIRECTIONAL,
			POINT,
			SPOT,
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