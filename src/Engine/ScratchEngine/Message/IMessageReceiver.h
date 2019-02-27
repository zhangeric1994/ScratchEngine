#pragma once

#include <limits.h>

#include "Message.h"

namespace ScratchEngine
{
	class IMessageReceiver
	{
	public:
		void SendMessage(const Message& message) const;
		void SendMessageUp(const Message& message, u32 level = UINT_MAX) const;
		void SendMessageDown(const Message& message, u32 level = UINT_MAX) const;

		virtual void HandleMessage(const Message& message) const {};

	private:
		virtual IMessageReceiver* GetParent() const = 0;
		virtual IMessageReceiver* GetChildren() const = 0;
	};
}
