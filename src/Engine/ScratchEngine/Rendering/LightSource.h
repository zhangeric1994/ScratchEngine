#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H
#pragma once

namespace ScratchEngine
{
	namespace Rendering
	{
		struct LightSource
		{
			friend class RenderingEngine;


		private:
			XMVECTOR ambientColor;
			XMVECTOR diffuseColor;
			LightType type : 32;
			XMFLOAT3 position;
			float range;
			XMFLOAT3 direction;
		};
	}
}
#endif // !LIGHT_SOURCE_H