#include "../Animation/Animator.h"
#include "../Rendering/Model.h"
#include "../Rendering/RenderingEngine.h"

#include "Game.h"
#include "Renderer.h"
#include "Scene.h"

using namespace ScratchEngine::Rendering;


ScratchEngine::Renderer::Renderer()
{
	this->mesh = nullptr;
	this->material = nullptr;
	this->anim = nullptr;
	this->renderable = null_index;
}

ScratchEngine::Renderer::Renderer(Material* material, Mesh* mesh)
{
	this->mesh = mesh;
	this->material = material;
	this->renderable = null_index;
	this->anim = nullptr;
	this->model = nullptr; 

	Scene::GetCurrentScene()->AddRenderer(this);
}

ScratchEngine::Renderer::Renderer(Material* material, Rendering::Model* model)
{
	this->model = model;
	this->mesh = model->mesh;
	this->material = material;
	this->renderable = null_index;
	this->anim = model->anim;
	//AnimationEngine::GetSingleton()->AddAnimator(this->anim);

	Scene::GetCurrentScene()->AddRenderer(this);
}

ScratchEngine::Renderer::~Renderer()
{
	Scene::GetCurrentScene()->RemoveRenderer(this);
}

void ScratchEngine::Renderer::OnBeginOverlapping(GameObject * other)
{
	material = Game::redMaterial;
}

void ScratchEngine::Renderer::OnEndOverlapping(GameObject * other)
{
	material = Game::greenMaterial;
}
