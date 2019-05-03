#include <typeinfo>

#include "../Animation/Animator.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Scene.h"


ScratchEngine::GameObject::GameObject() : Transform(), components(4)
{
	name = "";
	index = Scene::GetCurrentScene()->AddRootObject(this);

	flag = FLAG_ACTIVE;
}

ScratchEngine::GameObject::GameObject(float x, float y, float z) : Transform(x, y, z), components(4)
{
	name = "";
	index = Scene::GetCurrentScene()->AddRootObject(this);

	flag = FLAG_ACTIVE;
}

ScratchEngine::GameObject::GameObject(XMVECTOR position, XMVECTOR rotation, XMVECTOR scale) : Transform(position, rotation, scale), components(4)
{
	name = "";
	index = Scene::GetCurrentScene()->AddRootObject(this);

	flag = FLAG_ACTIVE;
}

ScratchEngine::GameObject::~GameObject()
{
	isDestroyed = true;

	if (!Scene::GetCurrentScene()->RemoveRootObject(this))
		if (!static_cast<GameObject*>(parent)->isDestroyed)
		{
			if (isInSlot)
				static_cast<GameObject*>(parent)->GetComponent<Renderer>()->anim->UnbindFromSlot(this);
			else
				parent->__RemoveChild(this);
		}

	for (auto it = components.begin(); it != components.end(); it++)
		delete it->second;

	for (auto it = children.begin(); it != children.end(); it++)
		delete (*it);
}

__inline string ScratchEngine::GameObject::GetName()
{
	return name;
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

void ScratchEngine::GameObject::SetName(string name)
{
	this->name = name;
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
		it->second->HandleMessage(message);
}

void ScratchEngine::GameObject::SendMessageUp(const Message& message, u32 level)
{
	HandleMessage(message);

	for (auto it = components.begin(); it != components.end(); it++)
		it->second->HandleMessage(message);

	if (level-- > 0 && parent)
		parent->SendMessageUp(message, level);
}

void ScratchEngine::GameObject::SendMessageDown(const Message& message, u32 level)
{
	HandleMessage(message);

	for (auto it = components.begin(); it != components.end(); it++)
		it->second->HandleMessage(message);

	if (level-- > 0)
		for (auto it = children.begin(); it != children.end(); it++)
			(*it)->SendMessageDown(message, level);
}

__inline void ScratchEngine::GameObject::Update(f32 deltaTime, f32 currentTime)
{
	for (auto it = components.begin(); it != components.end(); it++)
	{
		GameComponent* component = it->second;
		if (component->IsActiveSelf())
			component->Update(deltaTime, currentTime);
	}
}

__inline void ScratchEngine::GameObject::LateUpdate(f32 deltaTime, f32 currentTime)
{
	for (auto it = components.begin(); it != components.end(); it++)
	{
		GameComponent* component = it->second;
		if (component->IsActiveSelf())
			component->LateUpdate(deltaTime, currentTime);
	}
}

void ScratchEngine::GameObject::OnBeginOverlapping(GameObject* other)
{
	for (auto it = components.begin(); it != components.end(); it++)
	{
		GameComponent* component = it->second;
		if (component->IsActiveSelf())
			component->OnBeginOverlapping(other);
	}
}

void ScratchEngine::GameObject::OnOverlapping(GameObject* other)
{
	for (auto it = components.begin(); it != components.end(); it++)
	{
		GameComponent* component = it->second;
		if (component->IsActiveSelf())
			component->OnOverlapping(other);
	}
}

void ScratchEngine::GameObject::OnEndOverlapping(GameObject* other)
{
	for (auto it = components.begin(); it != components.end(); it++)
	{
		GameComponent* component = it->second;
		if (component->IsActiveSelf())
			component->OnEndOverlapping(other);
	}
}
