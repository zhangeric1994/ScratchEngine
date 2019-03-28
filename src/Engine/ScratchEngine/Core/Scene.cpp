#include <stack>

#include "Scene.h"

ScratchEngine::Scene* ScratchEngine::Scene::currentScene = nullptr;

ScratchEngine::Scene* ScratchEngine::Scene::GetCurrentScene()
{
	if (!currentScene)
		currentScene = new Scene();

	return currentScene;
}

ScratchEngine::Scene::Scene()
{
}

ScratchEngine::Scene::~Scene()
{
}

size_t ScratchEngine::Scene::AddRootObject(GameObject* gameObject)
{
	roots.push_back(gameObject);

	return roots.size() - 1;
}

bool ScratchEngine::Scene::RemoveRootObject(GameObject* gameObject)
{
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

void ScratchEngine::Scene::Update(f32 deltaTime, f32 currentTime)
{
	Stack<GameObject*> s;

	for (auto it = roots.begin(); it != roots.end(); it++)
		s.Push(*it);

	while (s.GetSize())
	{
		GameObject* gameObject = s.Pop();

		gameObject->Update(deltaTime, currentTime);

		for (auto it = gameObject->components.begin(); it != gameObject->components.end(); it++)
			(*it).second->Update(deltaTime, currentTime);

		for (auto it = gameObject->children.begin(); it != gameObject->children.end(); it++)
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

		for (auto it = gameObject->components.begin(); it != gameObject->components.end(); it++)
			(*it).second->LateUpdate(deltaTime, currentTime);

		for (auto it = gameObject->children.begin(); it != gameObject->children.end(); it++)
			s.Push(static_cast<GameObject*>(*it));
	}
}
