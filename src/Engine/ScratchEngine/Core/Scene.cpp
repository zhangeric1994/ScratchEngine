#include <stack>

#include "../Animation/Animator.h"
#include "../Common/Settings.h"
#include "../Rendering/LightSource.h"
#include "../Rendering/Renderable.h"
#include "../Rendering/Viewer.h"

#include "Camera.h"
#include "Global.h"
#include "Light.h"
#include "Renderer.h"
#include "Scene.h"


ScratchEngine::Scene* ScratchEngine::Scene::currentScene = nullptr;

ScratchEngine::Scene* ScratchEngine::Scene::GetCurrentScene()
{
	if (!currentScene)
		currentScene = new Scene();

	return currentScene;
}

ScratchEngine::Scene::Scene() : roots(8), renderableAllocator(128), viewerAllocator(2), lightSourceAllocator(8)
{
	flag = FLAG_ACTIVE;

	rendererList = nullptr;
	cameraList = nullptr;
	lightList = nullptr;
}

ScratchEngine::Scene::~Scene()
{
	isDestroyed = true;

	for (auto it = roots.begin(); it != roots.end(); it++)
		delete *it;
}

size_t ScratchEngine::Scene::AddRootObject(GameObject* gameObject)
{
	roots.push_back(gameObject);

	return roots.size() - 1;
}

void ScratchEngine::Scene::AddRenderer(Renderer* renderer)
{
	renderer->next = rendererList;
	renderer->previous = nullptr;

	if (rendererList)
		rendererList->previous = renderer;

	rendererList = renderer;
}

void ScratchEngine::Scene::AddCamera(Camera* camera)
{
	camera->next = cameraList;
	camera->previous = nullptr;

	if (cameraList)
		cameraList->previous = camera;

	cameraList = camera;
}

void ScratchEngine::Scene::AddLight(Light* light)
{
	light->next = lightList;
	light->previous = nullptr;

	if (lightList)
		lightList->previous = light;

	lightList = light;
}

bool ScratchEngine::Scene::RemoveRootObject(GameObject* gameObject)
{
	if (isDestroyed)
		return true;

	if (gameObject->parent)
		return false;

	for (auto it = roots.begin(); it != roots.end(); it++)
		if (*it == gameObject)
		{
			roots.erase(it);
			break;
		}

	return true;
}

void ScratchEngine::Scene::RemoveRenderer(Renderer* renderer)
{
	if (!rendererList)
		return;

	Renderer* next = renderer->next;
	Renderer* previous = renderer->previous;

	if (previous)
		previous->next = next;
	else
		rendererList = next;

	if (next)
		next->previous = previous;
}

void ScratchEngine::Scene::RemoveCamera(Camera * camera)
{
	if (!rendererList)
		return;

	Camera* next = camera->next;
	Camera* previous = camera->previous;

	if (previous)
		previous->next = next;
	else
		cameraList = next;

	if (next)
		next->previous = previous;
}

void ScratchEngine::Scene::RemoveLight(Light* light)
{
	if (!rendererList)
		return;

	Light* next = light->next;
	Light* previous = light->previous;

	if (previous)
		previous->next = next;
	else
		lightList = next;

	if (next)
		next->previous = previous;
}

void ScratchEngine::Scene::UpdateRenderables()
{
	renderableAllocator.Flush();

	for (Renderer* renderer = rendererList; renderer; renderer = renderer->next)
	{
		if (renderer->IsActive())
		{
			Renderable& renderable = renderableAllocator[renderableAllocator.Allocate()];

			renderable.worldMatrix = XMMatrixTranspose(renderer->GetGameObject()->GetWorldMatrix());
			renderable.material = renderer->material;
			renderable.mesh = renderer->mesh;

			if (renderer->anim != nullptr && renderer->anim->currentAnimationIndex != null_index)
			{
				std::vector<XMMATRIX> temp = renderer->anim->GetTransforms();

				for (UINT i = 0; i < temp.size(); ++i)
					renderable.bones[i] = temp[i];
			}
			else
				for (UINT i = 0; i < MAX_NUM_BONES_PER_MODEL; ++i)
					renderable.bones[i] = { XMMatrixIdentity() };
		}
	}
}

void ScratchEngine::Scene::UpdateViewers()
{
	f32 screenRatio = Global::GetScreenRatio();

	for (Camera* camera = cameraList; camera; camera = camera->next)
	{
		if (camera->IsActive())
		{
			if (camera->viewer == null_index)
				camera->viewer = viewerAllocator.Allocate();

			Viewer& viewer = viewerAllocator[camera->viewer];

			GameObject* gameObject = camera->GetGameObject();

			XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(gameObject->GetRotation());

			viewer.position = gameObject->GetPosition();
			viewer.viewMatrix = XMMatrixTranspose(XMMatrixLookToLH(viewer.position, XMVector3Transform({ 0, 0, 1 }, rotationMatrix), { 0, 1, 0 }));
			viewer.projectionMatrix = XMMatrixTranspose(XMMatrixPerspectiveFovLH(camera->fov, screenRatio, camera->nearZ, camera->farZ));
		}
		else
			DestroyViewer(camera->viewer);
	}
}

void ScratchEngine::Scene::UpdateLightSources()
{
	lightSourceAllocator.Flush();

	for (Light* light = lightList; light; light = light->next)
	{
		if (light->IsActive())
		{
			LightSource& lightSource = lightSourceAllocator[lightSourceAllocator.Allocate()];
			lightSource.ambientColor = light->ambientColor;
			lightSource.diffuseColor = light->diffuseColor;
			lightSource.type = light->type;
			lightSource.range = 0;

			XMStoreFloat3(&lightSource.position, light->GetGameObject()->GetPosition());
			XMStoreFloat3(&lightSource.direction, static_cast<DirectionalLight*>(light)->GetGameObject()->GetForwardVector());

			//if (light->DoCastShadow())
			//	lightSource.shadowMapID = null_index;
			//else
			//	lightSource.shadowMapID = null_index;
		}
	}
}

void ScratchEngine::Scene::DestroyViewer(i32 id)
{
	viewerAllocator.Free(id);
}

void ScratchEngine::Scene::CacheRenderingData()
{
	UpdateRenderables();
	UpdateLightSources();
	UpdateViewers();

	SortRenderables();
}

void ScratchEngine::Scene::SortRenderables()
{
	renderableAllocator.Sort([](Renderable a, Renderable b)
	{
		uptr ma = reinterpret_cast<uptr>(a.material);
		uptr mb = reinterpret_cast<uptr>(b.material);

		return ma < mb;
	});
}

void ScratchEngine::Scene::Update(f32 deltaTime, f32 currentTime)
{
	Stack<GameObject*> s;

	for (auto it = roots.begin(); it != roots.end(); it++)
		s.Push(*it);

	while (s.GetSize())
	{
		GameObject* gameObject = s.Pop();

		gameObject->Update(deltaTime, currentTime);

		for (auto it = gameObject->children.begin(); it != gameObject->children.end(); it++)
			if (static_cast<GameObject*>(*it)->IsActiveSelf())
				s.Push(static_cast<GameObject*>(*it));
	}
}

void ScratchEngine::Scene::LateUpdate(f32 deltaTime, f32 currentTime)
{
	Stack<GameObject*> s;

	for (auto it = roots.begin(); it != roots.end(); it++)
		s.Push(*it);

	while (s.GetSize())
	{
		GameObject* gameObject = s.Pop();

		gameObject->LateUpdate(deltaTime, currentTime);

		for (auto it = gameObject->children.begin(); it != gameObject->children.end(); it++)
			if (static_cast<GameObject*>(*it)->IsActiveSelf())
				s.Push(static_cast<GameObject*>(*it));
	}
}
