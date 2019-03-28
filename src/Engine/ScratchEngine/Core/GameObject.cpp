#include <typeinfo>

#include "GameObject.h"
#include "Scene.h"

using namespace ScratchEngine;

ScratchEngine::GameObject::GameObject() : components(4)
{
	index = Scene::GetCurrentScene()->AddRootObject(this);

	isStatic = false;
}

ScratchEngine::GameObject::GameObject(XMVECTOR position, XMVECTOR rotation, XMVECTOR scale) : Transform(position, rotation, scale)
{
	index = Scene::GetCurrentScene()->AddRootObject(this);

	isStatic = false;
}

ScratchEngine::GameObject::~GameObject()
{
	if (!Scene::GetCurrentScene()->RemoveRootObject(this))
		parent->RemoveChild(this);

	for (auto it = components.begin(); it != components.end(); it++)
		delete (*it).second;

	for (auto it = children.begin(); it != children.end(); it++)
		delete (*it);
}

bool ScratchEngine::GameObject::IsStatic()
{
	return isStatic;
}

GameObject* ScratchEngine::GameObject::GetParent()
{
	return static_cast<GameObject*>(Transform::GetParent());
}

GameObject* ScratchEngine::GameObject::GetChild(size_t index)
{
	return static_cast<GameObject*>(Transform::GetChild(index));
}

void ScratchEngine::GameObject::SetParent(GameObject* other)
{
	Scene::GetCurrentScene()->RemoveRootObject(this);

	return Transform::SetParent(other);
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
