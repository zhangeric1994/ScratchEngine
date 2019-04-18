#ifndef CAMERA_PROXY_H
#define CAMERA_PROXY_H
#pragma once

#include <DirectXMath.h>

namespace ScratchEngine
{
	namespace Rendering
	{
		struct Viewer
		{
			friend class RenderingEngine;


		private:
			XMVECTOR position = {0, 0, -5.0f};
			XMMATRIX viewMatrix;
			XMMATRIX projectionMatrix;
		};
	}
}
#endif