#include "GameObject.h"

__inline ScratchEngine::GameObject * ScratchEngine::GameComponent::GetGameObject()
{
	return gameObject;
}

__inline bool ScratchEngine::GameComponent::IsActive()
{
	return IsActiveSelf() && gameObject->IsActive();
}

__inline bool ScratchEngine::GameComponent::IsActiveSelf()
{
	return isActive;
}

__inline void ScratchEngine::GameComponent::SendMessage_(const Message & message)
{
	gameObject->SendMessage_(message);
}

__inline void ScratchEngine::GameComponent::SendMessageUp(const Message & message, u32 level)
{
	gameObject->SendMessageUp(message, level);
}

__inline void ScratchEngine::GameComponent::SendMessageDown(const Message & message, u32 level)
{
	gameObject->SendMessageDown(message, level);
}
