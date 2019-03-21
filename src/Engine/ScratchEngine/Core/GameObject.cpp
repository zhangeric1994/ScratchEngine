#include <typeinfo>

#include "GameObject.h"

using namespace ScratchEngine;

ScratchEngine::GameObject::GameObject()
{
}

void ScratchEngine::GameObject::SendMessage(const Message& message)
{
	HandleMessage(message);

	for (auto it = components.begin(); it != components.end(); it++)
		(*it).second->HandleMessage(message);
}

void ScratchEngine::GameObject::SendMessageUp(const Message& message, u32 level)
{
	HandleMessage(message);

	for (auto it = components.begin(); it != components.end(); it++)
		(*it).second->HandleMessage(message);

	if (level-- > 0 && parent)
		parent->SendMessageUp(message, level);
}

void ScratchEngine::GameObject::SendMessageDown(const Message& message, u32 level)
{
	HandleMessage(message);

	for (auto it = components.begin(); it != components.end(); it++)
		(*it).second->HandleMessage(message);

	if (level-- > 0)
		for (auto it = children.begin(); it != children.end(); it++)
			(*it)->SendMessageDown(message, level);
}
