#include "GameComponent.h"

#include "GameObject.h"


bool ScratchEngine::GameComponent::IsActive()
{
	return IsActiveSelf() && gameObject->IsActive();
}

void ScratchEngine::GameComponent::SendMessage_(const Message & message)
{
	gameObject->SendMessage_(message);
}

void ScratchEngine::GameComponent::SendMessageUp(const Message & message, u32 level)
{
	gameObject->SendMessageUp(message, level);
}

void ScratchEngine::GameComponent::SendMessageDown(const Message & message, u32 level)
{
	gameObject->SendMessageDown(message, level);
}