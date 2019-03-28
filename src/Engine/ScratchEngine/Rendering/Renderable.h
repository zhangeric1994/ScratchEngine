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


		private:
			Mesh* mesh;
			Material* material;

			Renderable();
		};
	}
}
#endif