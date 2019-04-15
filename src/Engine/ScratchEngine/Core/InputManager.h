#pragma once
#include <map>
#include "windows.h"
#include "InputEvent.h"
#include <queue>

namespace ScratchEngine 
{
	class __declspec(dllexport) InputManager
	{
	public:
		InputManager();

		// Keyboard Input

		void GetKeyDown(const unsigned char key);
		void GetKeyUp(const unsigned char key);
		void GetKey(const unsigned char key);

		bool IsKeyDown(const unsigned char keyCode);

		bool IsKeyBufferEmpty();
		bool IsCharBufferEmpty();

		InputEvent ReadKey();
		unsigned char ReadChar();

		//bool IsKeyDown(char);
		//bool IsKeyDown(PBYTE);
		//void SetKeyState(char, bool);
		//void Clear();

		// Mouse Input

		void OnMouseLPressed(int, int);
		void OnMouseLReleased(int, int);
		void OnMouseRPressed(int, int);
		void OnMouseRReleased(int, int);
		void OnMouseMPressed(int, int);
		void OnMouseMReleased(int, int);
		void OnMouseWheelUp(int, int);
		void OnMouseWheelDown(int, int);
		void OnMouseMove(int, int);

		bool IsMouseLDown();
		bool IsMouseRDown();
		bool IsMouseMDown();

		int GetPosX();
		int GetPosY();
		MousePoint GetPos();

		bool MouseBufferIsEmpty();
		InputEvent ReadMouse();
		//void OnMouseDown(WPARAM, int, int);
		//void OnMouseUp(WPARAM, int, int);
		//void OnMouseDrag(WPARAM);

	private:
		bool keyStates[256];
		std::queue<InputEvent> keyBuffer;
		std::queue<unsigned char> charBuffer;

		int x = 0, y = 0;
		bool LDown = false;
		bool RDown = false;
		bool MDown = false;
		std::queue<InputEvent> mouseBuffer;
	};
}