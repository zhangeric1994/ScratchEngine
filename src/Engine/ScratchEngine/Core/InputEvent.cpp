#include "InputEvent.h"

using namespace ScratchEngine;

InputEvent::InputEvent()
{
	this->type = EventType::Invalid;
}

InputEvent::InputEvent(const EventType type, const unsigned char key)
{
	this->type = type;
	this->key = key;
}

InputEvent::InputEvent(EventType type, int x, int y)
{
	this->type = type;
	this->x = x;
	this->y = y;
}

bool InputEvent::IsDown() const
{
	return this->type == EventType::Down;
}

bool InputEvent::IsReleased() const
{
	return this->type == EventType::Up;
}

bool InputEvent::IsValid() const
{
	return this->type != EventType::Invalid;
}

unsigned char InputEvent::GetKeyCode() const
{
	return this->key;
}

InputEvent::EventType InputEvent::GetType() const
{
	return this->type;
}

MousePoint InputEvent::GetPos() const
{
	return 
	{ 
		this->x,
		this->y 
	};
}

int InputEvent::GetPosX() const
{
	return this->x;
}

int InputEvent::GetPosY() const
{
	return this->y;

}