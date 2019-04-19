#include "../Core/Game.h"

#include "Renderer.h"
#include "RenderingEngine.h"
#include "../Animation/AnimationEngine.h"

ScratchEngine::Renderer::Renderer()
{
	this->mesh = nullptr;
	this->material = nullptr;
	this->renderable = null_index;
}

ScratchEngine::Renderer::Renderer(Material* material, Mesh* mesh)
{
	this->mesh = mesh;
	this->material = material;
	this->renderable = null_index;
	this->anim = nullptr;
	this->model = nullptr; 
	RenderingEngine::GetSingleton()->AddRenderer(this);
}

ScratchEngine::Renderer::Renderer(Material * material, Model * model)
{
	this->model = model;
	this->mesh = model->mesh;
	this->material = material;
	this->renderable = null_index;
	this->anim = model->anim;
	//AnimationEngine::GetSingleton()->AddAnimator(this->anim);
	RenderingEngine::GetSingleton()->AddRenderer(this);
}

ScratchEngine::Renderer::~Renderer()
{
	if (renderable != null_index)
		RenderingEngine::GetSingleton()->DestroyRenderable(renderable);

	RenderingEngine::GetSingleton()->RemoveRenderer(this);
}

void ScratchEngine::Renderer::OnBeginOverlapping(GameObject * other)
{
	material = Game::redMaterial;
}

void ScratchEngine::Renderer::OnEndOverlapping(GameObject * other)
{
	material = Game::greenMaterial;
}
