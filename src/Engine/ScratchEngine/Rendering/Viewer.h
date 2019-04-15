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
			XMVECTOR position;
			XMMATRIX viewMatrix;
			XMMATRIX projectionMatrix;
		};
	}
}
#endif