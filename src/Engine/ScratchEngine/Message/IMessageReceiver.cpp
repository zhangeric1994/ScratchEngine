#include "IMessageReceiver.h"

void ScratchEngine::IMessageReceiver::SendMessage(const Message & message) const
{
}

void ScratchEngine::IMessageReceiver::SendMessageUp(const Message & message, u32 level) const
{
	if (level-- > 0)
	{
		HandleMessage(message);
		
		IMessageReceiver* parent = GetParent();
		if(parent)
			parent->SendMessageUp(message, level);
	}
}

void ScratchEngine::IMessageReceiver::SendMessageDown(const Message & message, u32 level) const
{
}
