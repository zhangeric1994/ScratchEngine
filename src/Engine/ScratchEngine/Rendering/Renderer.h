#pragma once

#include "../Common/Typedefs.h"
#include "../Core/GameComponent.h"

#include "Material.h"
#include "Mesh.h"
#include "Renderable.h"

using namespace ScratchEngine::Rendering;

namespace ScratchEngine
{
	class Renderer : public GameComponent
	{
		friend class RenderingEngine;


	protected:
		i32 mesh;
		i32 material;
		i32 renderable;

		Renderer* next;
		Renderer* previous;

		Renderer();
		Renderer(i32 mesh, i32 material);
		~Renderer();
	};

	class __declspec(dllexport) ForwardRenderer : public Renderer
	{
	};
}