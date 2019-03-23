#pragma once

#include <typeindex>
#include <unordered_map>
#include <vector>

#include "GameComponent.h"
#include "Transform.h"

namespace ScratchEngine
{
	class __declspec(dllexport) GameObject : public Transform
	{
		friend class Scene;


	private:
		unordered_map<type_index, GameComponent*> components;


	public:
		GameObject();
		~GameObject() { }

		template<class T> GameComponent* GetComponent()
		{
			type_index id = typeid(T);

			return components.find(id) == components.end() ? nullptr : components[id];
		}

		template<class T> T* AddComponent()
		{
			type_index id = typeid(T);

			if (components.find(id) != components.end())
				throw "You cannot add duplicated components!";

			T* component = new T();

			components[id] = component;

			return component;
		}

		template<class T> void RemoveComponent()
		{
			type_index id = typeid(T);

			auto it = components.find(id);

			if (it != components.end())
				components.erase(it);
		}

		void SendMessage(const Message& message);
		void SendMessageUp(const Message& message, u32 level = UINT_MAX);
		void SendMessageDown(const Message& message, u32 level = UINT_MAX);


	protected:
		void virtual Update() { }


	private:
		virtual void HandleMessage(const Message& message) { }
	};
}
