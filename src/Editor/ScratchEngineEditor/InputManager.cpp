#include "InputManager.h"
#include <iostream>

using namespace ScratchEngine;

bool InputManager::IsKeyDown(char key)
{
	if (this->memOfKey[key] == 0)
	{
		bool isDown = GetAsyncKeyState(key) & 0x8000;
		this->memOfKey[key] = isDown ? 1 : -1;
	}
	return memOfKey[key];
}

bool InputManager::IsKeyDown(PBYTE keyCode)
{
	if (this->mapOfKeyCodes[keyCode] == 0) 
	{
		bool isDown = GetKeyboardState(keyCode);
		this->mapOfKeyCodes[keyCode] = isDown ? 1 : -1;
	}
	return mapOfKeyCodes[keyCode];
}
void InputManager::SetKeyState(char key, bool isDown)
{
	memOfKey[key] = isDown;
}

void InputManager::Clear()
{
	memset(memOfKey, 0, 1024);
}

void InputManager::OnMouseDown(WPARAM buttonState, int x, int y)
{
	this->mPos.x = x;
	this->mPos.y = y;
	SetCapture(this->hWnd);
}

void InputManager::OnMouseUp(WPARAM buttonState, int x, int y)
{
	//this->mPos.x = NULL;
	//this->mPos.y = NULL;
	ReleaseCapture();
}

void InputManager::OnMouseDrag(WPARAM buttonState)
{
	this->OnMouseDown(buttonState, mPos.x, mPos.y);
	DragDetect(hWnd, mPos);
}

