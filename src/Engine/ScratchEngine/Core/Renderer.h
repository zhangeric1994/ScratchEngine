#ifndef RENDERER_H
#define RENDERER_H
#pragma once

#include "../Common/Typedefs.h"

#include "GameComponent.h"


namespace ScratchEngine
{
	namespace Rendering
	{
		struct Material;
		struct Mesh;
		struct Model;
	}

	using namespace ScratchEngine::Rendering;

	class Animator;

	class __declspec(dllexport) Renderer : public GameComponent
	{
		friend class Scene;


	protected:
		Material* material;
		Mesh* mesh; // renderer can draw a single a mesh  or a complicated model with multiple mesh
		Model* model;
		Animator* anim;
		i32 renderable;

		Renderer* next;
		Renderer* previous;


	public:
		Renderer();
		Renderer(Material* material, Mesh* mesh);
		Renderer(Material* material, Model* model);
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