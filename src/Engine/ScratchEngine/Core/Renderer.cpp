#include "../Rendering/RenderingEngine.h"

#include "Game.h"
#include "Renderer.h"
#include "Scene.h"


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
