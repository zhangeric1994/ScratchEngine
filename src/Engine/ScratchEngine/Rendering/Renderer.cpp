#include "../Core/Game.h"

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

ScratchEngine::Renderer::Renderer(Material * material, Model * model, GameObject * Obj)
{
	this->model = model;
	this->material = material;
	this->renderable = null_index;

	for (UINT i = 0; i < model->meshs.size(); i++) {
		Mesh* meshObj = model->meshs.at(i);
		GameObject* go = new GameObject();
		go->AddComponent<Renderer>(material, meshObj);
		go->SetParent(Obj);
	}

	for (UINT i = 0; i < model->childModels.size(); i++) {
		Model* modelObj = model->childModels.at(i);
		GameObject* go1 = new GameObject();
		go1->AddComponent<Renderer>(material, modelObj,go1);
		go1->SetParent(Obj);
	}
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
