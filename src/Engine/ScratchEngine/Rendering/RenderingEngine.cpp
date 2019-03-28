#include "RenderingEngine.h"

ScratchEngine::Rendering::RenderingEngine* ScratchEngine::Rendering::RenderingEngine::singleton = nullptr;

ScratchEngine::Rendering::RenderingEngine* ScratchEngine::Rendering::RenderingEngine::GetSingleton()
{
	if (!singleton)
		Initialize();

	return singleton;
}

void ScratchEngine::Rendering::RenderingEngine::Initialize(i32 maxNumMaterials, i32 maxNumMeshes, i32 defaultNumRenderables)
{
	singleton = new RenderingEngine(maxNumMeshes, maxNumMeshes, defaultNumRenderables);
}

ScratchEngine::Rendering::RenderingEngine::RenderingEngine(i32 maxNumMaterials, i32 maxNumMeshes, i32 defaultNumRenderables) : materialAllocator(maxNumMeshes), meshAllocator(maxNumMeshes), renderableAllocator(defaultNumRenderables)
{
	rendererList = nullptr;
}

ScratchEngine::Rendering::RenderingEngine::~RenderingEngine()
{
	singleton = nullptr;
}

void ScratchEngine::Rendering::RenderingEngine::AddRenderer(Renderer* renderer)
{
	renderer->next = rendererList;
	renderer->previous = nullptr;

	if (rendererList)
		rendererList->previous = renderer;
	else
		rendererList = renderer;
}

ScratchEngine::i32 ScratchEngine::Rendering::RenderingEngine::CreateRenderable()
{
	return renderableAllocator.Allocate();
}

void ScratchEngine::Rendering::RenderingEngine::RemoveRenderer(Renderer* renderer)
{
	if (!rendererList)
		return;

	Renderer* next = renderer->next;
	Renderer* previous = renderer->previous;

	if (previous)
		previous->next = next;

	if (next)
		next = previous;

	if (renderer == rendererList)
		rendererList = next;
}

void ScratchEngine::Rendering::RenderingEngine::RemoveRenderable(i32 id)
{
	renderableAllocator.Free(id);
}
