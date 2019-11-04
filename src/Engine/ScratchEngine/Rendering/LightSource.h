#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H
#pragma once

#include "prerequisites.h"


namespace ScratchEngine
{
	namespace Rendering
	{
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