#ifndef RENDERER_H
#define RENDERER_H
#pragma once

#include "../Common/Typedefs.h"
#include "../Core/GameComponent.h"


namespace ScratchEngine
{
	namespace Rendering
	{
		class Material;
		class Mesh;
	}

	using namespace ScratchEngine::Rendering;

	class __declspec(dllexport) Renderer : public GameComponent
	{
		friend class Scene;


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