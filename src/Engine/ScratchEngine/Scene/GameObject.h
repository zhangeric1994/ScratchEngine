#pragma once

#include <vector>

#include "Component.h"

namespace ScratchEngine
{
	class GameObject : IMessageReceiver
	{
		friend class Scene;

	public:
		GameObject();
		~GameObject();

	private:
		vector<Component*> components;

		IMessageReceiver* GetParent() const;
		void GetSiblings(vector<IMessageReceiver*>& out) const;
		void GetChildren(vector<IMessageReceiver*>& out) const;
	};
}
