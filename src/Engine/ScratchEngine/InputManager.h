#pragma once

#include "windows.h"

namespace ScratchEngine 
{
	class __declspec(dllexport) InputManager
	{
	private:
		char* mem;
		POINT mPos;
	public:
		// keyboard input
		bool IsKeyDown(char);
		bool SetKeyState(char, bool);
		void Clear();

		// mouse input
		void OnMouseDown(WPARAM, int, int);
		void OnMouseUp(WPARAM, int, int);
	};
}