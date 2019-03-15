#include "GameObject.h"

void ScratchEngine::GameComponent::SendMessage(const Message & message)
{
	gameObject->SendMessage(message);
}

void ScratchEngine::GameComponent::SendMessageUp(const Message & message, u32 level)
{
	gameObject->SendMessageUp(message, level);
}

void ScratchEngine::GameComponent::SendMessageDown(const Message & message, u32 level)
{
	gameObject->SendMessageDown(message, level);
}
