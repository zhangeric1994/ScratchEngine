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
		class Shadow;


		enum class LightType
		{
			DIRECTIONAL,
			POINT,
			SPOT,
			AMBIENT,
		};


		struct LightSource
		{
		public:
			XMVECTOR color;
			LightType type : 32;
			XMFLOAT3 position;
			f32 range;
			XMFLOAT3 direction;
			f32 intensity;
			f32 fallOff;
			Shadow* shadow;

			union
			{
				XMMATRIX shadowViewProjection[6];
				XMMATRIX shadowProjection;
			};
		};
	}
}
#endif // !LIGHT_SOURCE_H