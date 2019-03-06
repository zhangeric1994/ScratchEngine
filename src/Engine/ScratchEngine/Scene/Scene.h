#pragma once

#include <vector>

#include "GameObject.h"

using namespace std;

namespace ScratchEngine
{
	class Scene
	{
	public:
		Scene();
		~Scene();

	private:
		vector<GameObject*> gameObjects;
	};
}