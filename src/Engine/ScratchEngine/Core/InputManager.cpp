
#include "InputManager.h"

using namespace ScratchEngine;

InputManager::InputManager()
{
	for (int i = 0; i < 256; i++) 
	{
		this->keyStates[i] = false; 
	}
}

void InputManager::GetKeyDown(const unsigned char key)
{
	this->keyStates[key] = true;
	this->keyBuffer.push(InputEvent(InputEvent::EventType::Down, key));
}

void InputManager::GetKeyUp(const unsigned char key)
{
	this->keyStates[key] = false;
	this->keyBuffer.push(InputEvent(InputEvent::EventType::Up, key));
}

void InputManager::GetKey(const unsigned char key)
{
	this->charBuffer.push(key);
}

bool InputManager::IsKeyDown(const unsigned char keyCode)
{
	return this->keyStates[keyCode];
}

bool InputManager::IsKeyBufferEmpty()
{
	return this->keyBuffer.empty();
}

bool InputManager::IsCharBufferEmpty()
{
	return this->charBuffer.empty();
}

InputEvent InputManager::ReadKey()
{
	if (this->keyBuffer.empty())
	{
		return InputEvent();
	}
	else
	{
		InputEvent inp = this->keyBuffer.front();
		this->keyBuffer.pop();
		return inp;
	}
}

unsigned char InputManager::ReadChar()
{
	if (this->charBuffer.empty())
	{
		return NULL;
	}
	else
	{
		unsigned char c = this->charBuffer.front();
		this->charBuffer.pop();
		return c;
	}
}

void InputManager::OnMouseLPressed(int x, int y)
{
	this->LDown = true;
	InputEvent inp(InputEvent::EventType::LPress, x, y);
	this->mouseBuffer.push(inp);
}

void InputManager::OnMouseLReleased(int x, int y)
{
	this->LDown = false;
	this->mouseBuffer.push(InputEvent(InputEvent::EventType::LRelease, x, y));
}

void InputManager::OnMouseRPressed(int x, int y)
{
	this->RDown = true;
	this->mouseBuffer.push(InputEvent(InputEvent::EventType::RPress, x, y));
}

void InputManager::OnMouseRReleased(int x, int y)
{
	this->RDown = false;
	this->mouseBuffer.push(InputEvent(InputEvent::EventType::RRelease, x, y));
}

void InputManager::OnMouseMPressed(int x, int y)
{
	this->MDown = true;
	this->mouseBuffer.push(InputEvent(InputEvent::EventType::MPress, x, y));
}

void InputManager::OnMouseMReleased(int x, int y)
{
	this->MDown = false;
	this->mouseBuffer.push(InputEvent(InputEvent::EventType::MRelease, x, y));
}

void InputManager::OnMouseWheelUp(int x, int y)
{
	this->mouseBuffer.push(InputEvent(InputEvent::EventType::MWheelUp, x, y));
}

void InputManager::OnMouseWheelDown(int x, int y)
{
	this->mouseBuffer.push(InputEvent(InputEvent::EventType::MWheelDown, x, y));
}

void InputManager::OnMouseMove(int x, int y)
{
	this->x = x;
	this->y = y;
	this->mouseBuffer.push(InputEvent(InputEvent::EventType::MPress, x, y));
}

bool InputManager::IsMouseLDown()
{
	return this->LDown;
}

bool InputManager::IsMouseRDown()
{
	return this->RDown;
}

bool InputManager::IsMouseMDown()
{
	return this->MDown;
}

int InputManager::GetPosX()
{
	return this->x;
}

int InputManager::GetPosY()
{
	return this->y;
}

MousePoint InputManager::GetPos()
{
	return
	{
		this->x,
		this->y
	};
}

bool InputManager::MouseBufferIsEmpty()
{
	return this->mouseBuffer.empty();
}

InputEvent InputManager::ReadMouse()
{
	if (this->mouseBuffer.empty())
	{
		return InputEvent();
	}
	else
	{
		InputEvent inp = this->mouseBuffer.front();
		this->mouseBuffer.pop();
		return inp;
	}
}