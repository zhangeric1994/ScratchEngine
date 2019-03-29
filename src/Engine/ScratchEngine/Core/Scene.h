#ifndef SCENE_H
#define SCENE_H
#pragma once

#include <vector>

#include "../Common/LinkedList.hpp"
#include "../Common/Stack.hpp"

#include "GameObject.h"
#include "IUpdatable.h"

namespace ScratchEngine
{
	class Scene : IUpdatable
	{
		friend class GameObject;


	private:
		static Scene* currentScene;
	

	public:
		static Scene* GetCurrentScene();


	private:
		vector<GameObject*> roots;

		Scene();
		~Scene();

		size_t AddRootObject(GameObject* gameObject);
		bool RemoveRootObject(GameObject* gameObject);
	
		void Update(f32 deltaTime, f32 currentTime);
		void LateUpdate(f32 deltaTime, f32 currentTime);
	};
}
#endif