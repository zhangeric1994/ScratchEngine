#pragma once

#include "../Message/IMessageReceiver.h"

#include "IUpdatable.h"

namespace ScratchEngine
{
	class GameObject;

	class __declspec(dllexport) GameComponent : public IMessageReceiver, private IUpdatable
	{
		friend class Scene;
		friend class GameObject;


	protected:
		bool isActive;
		GameObject* gameObject;


	public:
		bool IsActive();
		GameObject* GetGameObject();

		void SendMessage_(const Message& message);
		void SendMessageUp(const Message& message, u32 level = UINT_MAX);
		void SendMessageDown(const Message& message, u32 level = UINT_MAX);


	private:
		virtual void Update(f32 deltaTime, f32 currentTime) { };
		virtual void LateUpdate(f32 deltaTime, f32 currentTime) { };

		virtual void HandleMessage(const Message& message) { }
	};
}