#pragma once
#include <map>
#include "windows.h"

namespace ScratchEngine 
{
	// class __declspec(dllexport) InputManager
	class InputManager
	{
	private:
		HWND hWnd;
		char* memOfKey;
		std::map<PBYTE, bool> mapOfKeyCodes;
		std::map<char, bool> mapOfKeys;
		POINT mPos;
	public:
		// keyboard input
		void GetKeyDown(char);
		void GetKey(char);
		bool IsKeyUp(char);
		bool IsKeyDown(char);
		bool IsKeyDown(PBYTE);
		void SetKeyState(char, bool);
		void Clear();

		// mouse input
		void OnMouseDown(WPARAM, int, int);
		void OnMouseUp(WPARAM, int, int);
		void OnMouseDrag(WPARAM, int, int, float);
	};
}