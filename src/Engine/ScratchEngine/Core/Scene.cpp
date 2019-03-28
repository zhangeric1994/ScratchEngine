#include <stack>

#include "Scene.h"

ScratchEngine::Scene::Scene()
{
}

ScratchEngine::Scene::~Scene()
{
}

void ScratchEngine::Scene::AddGameObject(GameObject * gameObject)
{
	gameObjects.push_back(gameObject);
}

void ScratchEngine::Scene::Update()
{
	stack<GameObject*> s;

	for (auto it = gameObjects.begin(); it != gameObjects.end(); it++)
		s.push(*it);

	GameObject* current;
	while (!s.empty())
	{
		current = s.top();

		s.pop();

		current->Update();

		for (auto it = current->components.begin(); it != current->components.end(); it++)
			(*it).second->Update();
	}
}
