#include "Renderer.h"
#include "RenderingEngine.h"

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

	RenderingEngine::GetSingleton()->AddRenderer(this);
}

ScratchEngine::Renderer::~Renderer()
{
	if (renderable != null_index)
		RenderingEngine::GetSingleton()->DestroyRenderable(renderable);

	RenderingEngine::GetSingleton()->RemoveRenderer(this);
}
