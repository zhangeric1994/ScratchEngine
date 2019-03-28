#pragma once

#include "../Core/GameComponent.h"

#include "Material.h"
#include "Mesh.h"

using namespace ScratchEngine;

namespace ScratchEngine
{
	namespace Rendering
	{
		class __declspec(dllexport) ForwardRenderer : public GameComponent
		{
		private:
			Mesh* mesh;
			Material* material;

			ForwardRenderer(Mesh* mesh, Material* material);
		};
	}
}