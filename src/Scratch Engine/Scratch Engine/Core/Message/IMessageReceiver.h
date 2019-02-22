#pragma once

namespace ScratchEngine
{
	class IMessageReceiver
	{
	protected:
		/// <summary> This function is called when the object is created </summary>
		virtual void Initialize() {};

		/// <summary> This function is called once per frame </summary>
		virtual void Update(float deltaTime, float totalTime) {};

		/// <summary> This function is called when the object is destroyed </summary>
		virtual void Clean() {};

		virtual void HandleKeyDown(int keyCode) {};
		virtual void HandleKeyUp(int keyCode) {};
		virtual void HandleMouseDown() {};
		virtual void HandleMouseUp() {};
		virtual void HandleMouseScroll() {};

		void SendMessage();

		virtual void GetParent() = 0;
		virtual void GetChildren() = 0;
	};
}
