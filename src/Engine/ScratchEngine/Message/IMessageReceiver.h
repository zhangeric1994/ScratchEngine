#pragma once

#include "../header.h"

#include "Message.h"


namespace ScratchEngine
{
	class IMessageReceiver
	{
	public:
		virtual void SendMessage_(const Message& message) = 0;
		virtual void SendMessageUp(const Message& message, u32 level = UINT_MAX) = 0;
		virtual void SendMessageDown(const Message& message, u32 level = UINT_MAX) = 0;

	private:
		virtual void HandleMessage(const Message& message) = 0;
	};
}
