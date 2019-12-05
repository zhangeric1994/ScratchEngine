#include "Renderer.h"

#include "../Animation/Animator.h"
#include "../Rendering/RenderingEngine.h"

#include "Game.h"
#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"

using namespace ScratchEngine::Rendering;


ScratchEngine::Renderer::Renderer()
{
	this->mesh = nullptr;
	this->material = nullptr;
	this->anim = nullptr;
	this->model = nullptr;

	this->bvhNode = null_index;
	this->renderable = null_index;
}

ScratchEngine::Renderer::Renderer(Material* material, Mesh* mesh)
{
	this->mesh = mesh;
	this->material = material;
	this->anim = nullptr;
	this->model = nullptr;

	this->bvhNode = null_index;
	this->renderable = null_index;

	//this->slotIndex = null_index;
	//this->slot = nullptr;

	Scene::GetCurrentScene()->AddRenderer(this);
}

ScratchEngine::Renderer::Renderer(Material* material, Model* model)
{
	this->mesh = model->mesh;
	this->material = material;
	this->anim = model->anim;
	this->model = model;

	this->bvhNode = null_index;
	this->renderable = null_index;

	//this->slotIndex = null_index;
	//this->slot = nullptr;

	Scene::GetCurrentScene()->AddRenderer(this);
}

ScratchEngine::Renderer::~Renderer()
{
	Scene::GetCurrentScene()->RemoveRenderer(this);
}

ScratchEngine::Physics::AxisAlignedBoundingBox ScratchEngine::Renderer::GetEnlargedAABB(f32 enlargement)
{
	XMVECTOR s = gameObject->GetScale();

	f32 r = __max(s.m128_f32[0], __max(s.m128_f32[1], s.m128_f32[2])) * mesh->GetMaxSize() + enlargement;

	XMVECTOR v = XMVectorSet(r, r, r, 0);
	XMVECTOR p = gameObject->GetPosition();

	return AxisAlignedBoundingBox(XMVectorSubtract(p, v), XMVectorAdd(p, v));
}

