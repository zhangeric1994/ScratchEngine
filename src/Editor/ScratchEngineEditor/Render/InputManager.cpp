#include "InputManager.h"
#include <iostream>

using namespace ScratchEngine;

void InputManager::GetKeyDown(char key)
{
	if (GetAsyncKeyState(key) & 0x8000)
	{
		mapOfKeys[key] = true;
		// ...
		printf("pressed");
		// ----------------
		mapOfKeys[key] = false;
	}
}

void InputManager::GetKey(char key)
{
	if (GetAsyncKeyState(key) & 0x8000)
	{
		mapOfKeys[key] = true;
		// ...
		printf("pressing");
		IsKeyUp(key);
	}
}

bool InputManager::IsKeyUp(char key)
{
	return true;
}
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

void InputManager::OnMouseDrag(WPARAM buttonState, int x, int y, float slowdown)
{
	int dragDistX = (y - mPos.y) * 0.001f;
	int dragDistY = (x - mPos.x) * 0.001f;

	if (dragDistX != 0 && dragDistY != 0) 
	{
		printf("dragging");
	}
	printf("dragging");
	mPos.x = x;
	mPos.y = y;
}

