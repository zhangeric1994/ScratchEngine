#include "ForwardRenderer.h"

ScratchEngine::Rendering::ForwardRenderer::ForwardRenderer(Mesh * mesh, Material * material)
{
	this->mesh = mesh;
	this->material = material;
}
