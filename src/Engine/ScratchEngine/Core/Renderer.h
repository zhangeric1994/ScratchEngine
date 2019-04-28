#ifndef RENDERER_H
#define RENDERER_H
#pragma once

#include "../Common/Typedefs.h"
#include "../Core/GameComponent.h"

#include "Material.h"
#include "Mesh.h"

using namespace ScratchEngine::Rendering;

namespace ScratchEngine
{
	class __declspec(dllexport) Renderer : public GameComponent
	{
		friend class RenderingEngine;


	protected:
		Material* material;
		Mesh* mesh;
		i32 renderable;

		Renderer* next;
		Renderer* previous;


	public:
		Renderer();
		Renderer(Material* material, Mesh* mesh);
		~Renderer();


	private:
		void OnBeginOverlapping(GameObject* other);
		void OnEndOverlapping(GameObject* other);
	};


	class __declspec(dllexport) ForwardRenderer final : public Renderer
	{
	};
}
#endif