#include "GameObject.h"

using namespace ScratchEngine;

ScratchEngine::GameObject::GameObject()
{
}

ScratchEngine::GameObject::~GameObject()
{
}

IMessageReceiver * ScratchEngine::GameObject::GetParent() const
{
	return nullptr;
}

void ScratchEngine::GameObject::GetSiblings(vector<IMessageReceiver*>& out) const
{
}

void ScratchEngine::GameObject::GetChildren(vector<IMessageReceiver*>& out) const
{
}
