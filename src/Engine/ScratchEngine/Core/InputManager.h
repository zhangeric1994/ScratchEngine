#pragma once
#include <map>
#include <queue>

#include "InputEvent.h"

using namespace std;


namespace ScratchEngine 
{
	class __declspec(dllexport) InputManager
	{
	public:
		InputManager();

		void GetKeyDown(const unsigned char key);
		void GetKeyUp(const unsigned char key);
		void GetKey(const unsigned char key);

		bool IsKeyDown(const unsigned char keyCode);

		bool IsKeyBufferEmpty();
		bool IsCharBufferEmpty();

		InputEvent ReadKey();
		unsigned char ReadChar();

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


	private:
		bool keyStates[256];

		queue<InputEvent> keyBuffer;
		queue<unsigned char> charBuffer;
		queue<InputEvent> mouseBuffer;

		int x = 0;
		int y = 0;
		bool LDown = false;
		bool RDown = false;
		bool MDown = false;
	};
}