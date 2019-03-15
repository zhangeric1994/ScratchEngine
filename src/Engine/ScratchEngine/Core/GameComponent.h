#pragma once

#include "../Message/IMessageReceiver.h"

namespace ScratchEngine
{
	class GameObject;

	class __declspec(dllexport) GameComponent : IMessageReceiver
	{
		friend class Scene;
		friend class GameObject;


	private:
		GameObject* gameObject;


	public:
		virtual void SendMessage(const Message& message);
		virtual void SendMessageUp(const Message& message, u32 level = UINT_MAX);
		virtual void SendMessageDown(const Message& message, u32 level = UINT_MAX);


	private:
		virtual void Update() { };

		virtual void HandleMessage(const Message& message) { }
	};
}