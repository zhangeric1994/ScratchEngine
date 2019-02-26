#pragma once

#include "Message.h"

namespace ScratchEngine
{
	class __declspec(dllexport) IMessageReceiver
	{
	public:
		void SendMessage(Message message);

	private:
		virtual void GetParent() = 0;
		virtual void GetChildren() = 0;
	};
}
