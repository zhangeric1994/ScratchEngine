#include <typeinfo>

#include "GameObject.h"
#include "Scene.h"

using namespace ScratchEngine;

ScratchEngine::GameObject::GameObject() : Transform(), components(4)
{
	index = Scene::GetCurrentScene()->AddRootObject(this);

	flag = FLAG_ACTIVE;
}

ScratchEngine::GameObject::GameObject(float x, float y, float z) : Transform(x, y, z), components(4)
{
	index = Scene::GetCurrentScene()->AddRootObject(this);

	flag = FLAG_ACTIVE;
}

ScratchEngine::GameObject::GameObject(XMVECTOR position, XMVECTOR rotation, XMVECTOR scale) : Transform(position, rotation, scale), components(4)
{
	index = Scene::GetCurrentScene()->AddRootObject(this);

	flag = FLAG_ACTIVE;
}

ScratchEngine::GameObject::~GameObject()
{
	isDestroyed = true;

	if (!Scene::GetCurrentScene()->RemoveRootObject(this))
		parent->__RemoveChild(this);

	for (auto it = components.begin(); it != components.end(); it++)
		delete (*it).second;

	for (auto it = children.begin(); it != children.end(); it++)
		delete (*it);
}

__inline GameObject* ScratchEngine::GameObject::GetParent()
{
	return static_cast<GameObject*>(parent);
}

__inline GameObject* ScratchEngine::GameObject::GetChild(size_t index)
{
	return static_cast<GameObject*>(Transform::GetChild(index));
}

__inline bool ScratchEngine::GameObject::IsActive()
{
	return parent ? IsActiveSelf() && static_cast<GameObject*>(parent)->IsActive() : IsActiveSelf();
}

__inline bool ScratchEngine::GameObject::IsActiveSelf()
{
	return isActive;
}

__inline bool ScratchEngine::GameObject::IsStatic()
{
	return isStatic;
}

__inline void ScratchEngine::GameObject::SetParent(GameObject* other)
{
	Scene::GetCurrentScene()->RemoveRootObject(this);

	return Transform::SetParent(other);
}

void ScratchEngine::GameObject::SendMessage_(const Message& message)
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

__inline void ScratchEngine::GameObject::Update(f32 deltaTime, f32 currentTime)
{
	for (auto it = components.begin(); it != components.end(); it++)
	{
		GameComponent* component = (*it).second;
		if (component->IsActiveSelf())
			component->Update(deltaTime, currentTime);
	}
}

__inline void ScratchEngine::GameObject::LateUpdate(f32 deltaTime, f32 currentTime)
{
	for (auto it = components.begin(); it != components.end(); it++)
	{
		GameComponent* component = (*it).second;
		if (component->IsActiveSelf())
			component->LateUpdate(deltaTime, currentTime);
	}
}
