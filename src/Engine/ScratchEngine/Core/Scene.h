#ifndef SCENE_H
#define SCENE_H
#pragma once

#include <vector>

#include "../Common/LinkedList.hpp"
#include "../Common/Stack.hpp"
#include "../Memory/DynamicPoolAllocator.hpp"
#include "../Memory/StackAllocator.hpp"

#include "GameObject.h"
#include "IUpdatable.h"

using namespace ScratchEngine::Memory;


namespace ScratchEngine
{
	class Camera;
	class Renderer;
	class Light;

	class Scene : private IUpdatable
	{
		friend class Camera;
		friend class Game;
		friend class GameObject;
		friend class Light;
		friend class Renderer;


	private:
		static Scene* currentScene;
	

	public:
		static Scene* GetCurrentScene();


	private:
		vector<GameObject*> roots;

		union
		{
			i32 flag;
			struct
			{
				bool isActive : 1;
				bool : 1;
				bool : 1;
				bool : 1;

				bool : 1;
				bool : 1;
				bool : 1;
				bool : 1;

				bool : 1;
				bool : 1;
				bool : 1;
				bool : 1;

				bool : 1;
				bool : 1;
				bool : 1;
				bool : 1;

				bool : 1;
				bool : 1;
				bool : 1;
				bool : 1;

				bool : 1;
				bool : 1;
				bool : 1;
				bool : 1;

				bool : 1;
				bool : 1;
				bool : 1;
				bool : 1;

				bool : 1;
				bool : 1;
				bool : 1;
				bool isDestroyed : 1;
			};
		};

		DynamicStackAllocator<Renderable> renderableAllocator;
		DynamicPoolAllocator<Viewer> viewerAllocator;
		DynamicStackAllocator<LightSource> lightSourceAllocator;

		Camera* cameraList;
		Light* lightList;
		Renderer* rendererList;

		Scene();
		~Scene();

		size_t AddRootObject(GameObject* gameObject);
		void AddRenderer(Renderer* renderer);
		void AddCamera(Camera* camera);
		void AddLight(Light* light);

		bool RemoveRootObject(GameObject* gameObject);
		void RemoveRenderer(Renderer* renderer);
		void RemoveCamera(Camera* camera);
		void RemoveLight(Light* light);


		void UpdateRenderables();
		void UpdateViewers();
		void UpdateLightSources();
		void SortRenderables();

		void DestroyViewer(i32 id);

		void CacheRenderingData();
	

		void Update(f32 deltaTime, f32 currentTime);
		void LateUpdate(f32 deltaTime, f32 currentTime);
	};
}
#endif