#pragma once

#include "../Message/IMessageReceiver.h"

namespace ScratchEngine
{
	class GameObject;

	class __declspec(dllexport) GameComponent : IMessageReceiver
	{
		friend class Scene;
		friend class GameObject;


	protected:
		GameObject* gameObject;


	public:
		GameObject* GetGameObject();

		void SendMessage(const Message& message);
		void SendMessageUp(const Message& message, u32 level = UINT_MAX);
		void SendMessageDown(const Message& message, u32 level = UINT_MAX);


	private:
		virtual void Update() { };

		virtual void HandleMessage(const Message& message) { }
	};
}