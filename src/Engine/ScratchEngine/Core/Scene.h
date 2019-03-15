#pragma once

#include <vector>

#include "GameObject.h"

using namespace std;

namespace ScratchEngine
{
	class Scene
	{
	private:
		vector<GameObject*> gameObjects;

	
	public:
		Scene();
		~Scene();

		void AddGameObject(GameObject* gameObject);
	
	private:
		void Update();
	};
}