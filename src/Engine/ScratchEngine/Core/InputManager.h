#pragma once

#include <map>
#include <queue>

#include "InputEvent.h"

using namespace std;


namespace ScratchEngine 
{
	namespace AsyncInput
	{
		bool IsKeyDown(int keyCode);
		bool IsKeyUp(int keyCode);

		int GetMouseX();
		int GetMouseY();
	}


	namespace FrameInput
	{
		bool IsKeyPressed(int keyCode);
		bool IsKeyDown(int keyCode);
		bool IsKeyUp(int keyCode);
		
		int GetMouseX();
		int GetMouseY();
	}


	struct InputData
	{
		bool keyStates[256] = { false };
		int mouseX;
		int mouseY;


		void Diff(const InputData& other, bool* output);
	};


	class IInputEventReceiver;

	class __declspec(dllexport) InputManager
	{
		friend class DXCore;
		friend class Game;

		friend bool AsyncInput::IsKeyDown(int);
		friend bool AsyncInput::IsKeyUp(int);
		friend int AsyncInput::GetMouseX();
		friend int AsyncInput::GetMouseY();
		friend bool FrameInput::IsKeyPressed(int);
		friend bool FrameInput::IsKeyDown(int);
		friend bool FrameInput::IsKeyUp(int);
		friend int FrameInput::GetMouseX();
		friend int FrameInput::GetMouseY();


	private:
		static InputManager* singleton;


		static InputManager* GetSingleton();
		static void Initialize();


		InputData asyncData;
		InputData frameData;
		
		bool isKeyPressed[256];

		//queue<InputEvent> keyBuffer;
		//queue<unsigned char> charBuffer;
		//queue<InputEvent> mouseBuffer;


		InputManager() {}

		//void GetKey(const unsigned char key);

		bool IsKeyPressed(int keyCode) const;
		bool IsKeyDown(int keyCode) const;
		bool IsKeyDownAsync(int keyCode) const;
		int GetMouseX() const;
		int GetMouseXAsync() const;
		int GetMouseY() const;
		int GetMouseYAsync() const;

		//bool IsKeyBufferEmpty();
		//bool IsCharBufferEmpty();

		//InputEvent ReadKey();
		//unsigned char ReadChar();

		void OnKeyDown(int keyCode);
		void OnKeyUp(int keyCode);
		void OnMouseWheelUp(int, int);
		void OnMouseWheelDown(int, int);
		void OnMouseMove(int, int);

		//bool MouseBufferIsEmpty();
		//InputEvent ReadMouse();


		void Capture(IInputEventReceiver* receiver = nullptr);
	};
}


inline bool ScratchEngine::AsyncInput::IsKeyDown(int keyCode)
{
	return InputManager::GetSingleton()->IsKeyDownAsync(keyCode);
}

inline bool ScratchEngine::AsyncInput::IsKeyUp(int keyCode)
{
	return !IsKeyDown(keyCode);
}

inline int ScratchEngine::AsyncInput::GetMouseX()
{
	return InputManager::GetSingleton()->GetMouseXAsync();
}

inline int ScratchEngine::AsyncInput::GetMouseY()
{
	return InputManager::GetSingleton()->GetMouseYAsync();
}

inline bool ScratchEngine::FrameInput::IsKeyPressed(int keyCode)
{
	return InputManager::GetSingleton()->IsKeyPressed(keyCode);
}

inline bool ScratchEngine::FrameInput::IsKeyDown(int keyCode)
{
	return InputManager::GetSingleton()->IsKeyDown(keyCode);
}

inline bool ScratchEngine::FrameInput::IsKeyUp(int keyCode)
{
	return !IsKeyDown(keyCode);
}

inline int ScratchEngine::FrameInput::GetMouseX()
{
	return InputManager::GetSingleton()->GetMouseX();
}

inline int ScratchEngine::FrameInput::GetMouseY()
{
	return InputManager::GetSingleton()->GetMouseY();
}


inline void ScratchEngine::InputData::Diff(const InputData & other, bool* output)
{
	for (int keyCode = 0; keyCode < 256; ++keyCode)
		output[keyCode] = keyStates[keyCode] && !other.keyStates[keyCode];
}


inline ScratchEngine::InputManager* ScratchEngine::InputManager::GetSingleton()
{
	return singleton;
}

inline void ScratchEngine::InputManager::Initialize()
{
	if (!singleton)
		singleton = new InputManager();
}

inline bool ScratchEngine::InputManager::IsKeyPressed(int keyCode) const
{
	return isKeyPressed[keyCode];
}

inline bool ScratchEngine::InputManager::IsKeyDown(int keyCode) const
{
	return frameData.keyStates[keyCode];
}

inline bool ScratchEngine::InputManager::IsKeyDownAsync(int keyCode) const
{
	return asyncData.keyStates[keyCode];
}

inline int ScratchEngine::InputManager::GetMouseX() const
{
	return frameData.mouseX;
}

inline int ScratchEngine::InputManager::GetMouseXAsync() const
{
	return asyncData.mouseX;
}

inline int ScratchEngine::InputManager::GetMouseY() const
{
	return frameData.mouseY;
}

inline int ScratchEngine::InputManager::GetMouseYAsync() const
{
	return asyncData.mouseY;
}
