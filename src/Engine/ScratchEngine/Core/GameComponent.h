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
		GameObject* gameObject;
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
				bool : 1;
			};
		};


	public:
		GameObject* GetGameObject();
		bool IsActive();
		bool IsActiveSelf();

		void SendMessage_(const Message& message);
		void SendMessageUp(const Message& message, u32 level = UINT_MAX);
		void SendMessageDown(const Message& message, u32 level = UINT_MAX);


	private:
		virtual void Update(f32 deltaTime, f32 currentTime) { };
		virtual void LateUpdate(f32 deltaTime, f32 currentTime) { };

		virtual void HandleMessage(const Message& message) { }
	};
}