#ifndef RENDERABLE_H
#define RENDERABLE_H
#pragma once

#include <DirectXMath.h>

#include "Material.h"
#include "Mesh.h"

using namespace DirectX;


namespace ScratchEngine
{
	namespace Rendering
	{
		struct Renderable
		{
		public:
			Mesh* mesh;
			Material* material;

			XMMATRIX worldMatrix;
		};
	}
}
#endif