inline ScratchEngine::GameObject* ScratchEngine::GameComponent::GetGameObject()
{
	return gameObject;
}

inline bool ScratchEngine::GameComponent::IsActiveSelf()
{
	return isActive;
}

inline void ScratchEngine::GameComponent::SetActive(bool flag)
{
	isActive = flag;
}
