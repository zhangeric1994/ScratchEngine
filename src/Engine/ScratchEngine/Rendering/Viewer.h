#ifndef CAMERA_PROXY_H
#define CAMERA_PROXY_H
#pragma once

#include <DirectXMath.h>

using namespace DirectX;


namespace ScratchEngine
{
	namespace Rendering
	{
		struct Viewer
		{
		public:
			XMVECTOR position = {0, 0, -5.0f};
			XMMATRIX viewMatrix;
			XMMATRIX projectionMatrix;
		};
	}
}
#endif