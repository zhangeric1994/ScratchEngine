#ifndef RENDERER_H
#define RENDERER_H
#pragma once

#include "../Common/Typedefs.h"
#include "../Rendering/Mesh.h"
#include "../Rendering/Model.h"
#include "../Physics/DynamicBVH.hpp"

#include "GameComponent.h"


namespace ScratchEngine
{
	namespace Rendering
	{
		struct Material;
	}


	using namespace ScratchEngine::Rendering;


	class Animator;
	class GameObject;
	struct Transform;

	class __declspec(dllexport) Renderer : public GameComponent
	{
		friend class GameObject;
		friend class Scene;


	protected:
		Material* material;
		Mesh* mesh;
		Model* model;
		Animator* anim;

		i32 bvhNode;
		i32 renderable;

		Renderer* next;
		Renderer* previous;


	public:
		Renderer();
		Renderer(Material* material, Mesh* mesh);
		Renderer(Material* material, Model* model);
		~Renderer();


		void SetMaterial(Material* material);


	private:
		AxisAlignedBoundingBox GetEnlargedAABB(f32 enlargement);
	};
}


inline void ScratchEngine::Renderer::SetMaterial(Material* material)
{
	this->material = material;
}

#endif