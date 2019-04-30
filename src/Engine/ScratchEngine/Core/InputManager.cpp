#define NOMINMAX
#include <Windows.h>

#include "InputManager.h"

using namespace ScratchEngine;


ScratchEngine::InputManager* ScratchEngine::InputManager::singleton = nullptr;

//void InputManager::GetKey(const unsigned char key)
//{
//	charBuffer.push(key);
//}
//
//bool InputManager::IsKeyBufferEmpty()
//{
//	return keyBuffer.empty();
//}
//
//bool InputManager::IsCharBufferEmpty()
//{
//	return charBuffer.empty();
//}
//
//InputEvent InputManager::ReadKey()
//{
//	if (this->keyBuffer.empty())
//	{
//		return InputEvent();
//	}
//	else
//	{
//		InputEvent inp = this->keyBuffer.front();
//		this->keyBuffer.pop();
//		return inp;
//	}
//}

//unsigned char InputManager::ReadChar()
//{
//	if (this->charBuffer.empty())
//	{
//		return NULL;
//	}
//	else
//	{
//		unsigned char c = this->charBuffer.front();
//		this->charBuffer.pop();
//		return c;
//	}
//}

void InputManager::OnKeyDown(int keyCode)
{
	asyncData.keyStates[keyCode] = true;
	//keyBuffer.push(InputEvent(InputEvent::EventType::Down, key));
}

void InputManager::OnKeyUp(int keyCode)
{
	asyncData.keyStates[keyCode] = false;
	//keyBuffer.push(InputEvent(InputEvent::EventType::Up, key));
}

void InputManager::OnMouseWheelUp(int x, int y)
{
	//mouseBuffer.push(InputEvent(InputEvent::EventType::MWheelUp, x, y));
}

void InputManager::OnMouseWheelDown(int x, int y)
{
	//mouseBuffer.push(InputEvent(InputEvent::EventType::MWheelDown, x, y));
}

void InputManager::OnMouseMove(int x, int y)
{
	asyncData.mouseX = x;
	asyncData.mouseY = y;
	//mouseBuffer.push(InputEvent(InputEvent::EventType::MPress, x, y));
}

//bool InputManager::MouseBufferIsEmpty()
//{
//	return mouseBuffer.empty();
//}
//
//InputEvent InputManager::ReadMouse()
//{
//	if (this->mouseBuffer.empty())
//	{
//		return InputEvent();
//	}
//	else
//	{
//		InputEvent inp = this->mouseBuffer.front();
//		this->mouseBuffer.pop();
//		return inp;
//	}
//}

void ScratchEngine::InputManager::Capture(IInputEventReceiver* receiver)
{
	asyncData.Diff(frameData, isKeyPressed);

	frameData = asyncData;
}