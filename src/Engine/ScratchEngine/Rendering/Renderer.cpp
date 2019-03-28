#include "RenderingEngine.h"
#include "Renderer.h"

ScratchEngine::Renderer::Renderer()
{
	this->mesh = null_index;
	this->material = null_index;
	this->renderable = null_index;
}

ScratchEngine::Renderer::Renderer(i32 mesh, i32 material)
{
	this->mesh = mesh;
	this->material = material;
	this->renderable = RenderingEngine::GetSingleton()->CreateRenderable();

	RenderingEngine::GetSingleton()->AddRenderer(this);
}

ScratchEngine::Renderer::~Renderer()
{
	RenderingEngine::GetSingleton()->RemoveRenderable(renderable);
}
