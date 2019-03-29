#ifndef RENDERABLE_H
#define RENDERABLE_H
#pragma once

#include "Material.h"
#include "Mesh.h"

namespace ScratchEngine
{
	namespace Rendering
	{
		struct Renderable
		{
			friend class RenderingEngine;

			friend bool SortRenderables(Renderable, Renderable);


		private:
			Mesh* mesh;
			Material* material;

			XMMATRIX worldMatrix;

			Renderable();
		};
	}
}
#endif