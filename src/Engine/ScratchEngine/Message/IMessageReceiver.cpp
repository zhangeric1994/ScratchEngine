#include "IMessageReceiver.h"

void ScratchEngine::IMessageReceiver::SendMessage(const Message & message) const
{
	HandleMessage(message);

	vector<IMessageReceiver*> list;

	GetSiblings(list);
	for (vector<IMessageReceiver*>::iterator it = list.begin(); it != list.end(); it++)
		(*it)->SendMessage(message);
}

void ScratchEngine::IMessageReceiver::SendMessageUp(const Message & message, u32 level) const
{
	if (level == 0)
		SendMessage(message);
	else
	{
		HandleMessage(message);

		vector<IMessageReceiver*> list;

		GetSiblings(list);
		for (vector<IMessageReceiver*>::iterator it = list.begin(); it != list.end(); it++)
			(*it)->SendMessageUp(message, level);

		IMessageReceiver* parent = GetParent();
		if(parent)
			parent->SendMessageUp(message, --level);
	}
}

void ScratchEngine::IMessageReceiver::SendMessageDown(const Message & message, u32 level) const
{
	if (level == 0)
		SendMessage(message);
	else
	{
		HandleMessage(message);

		vector<IMessageReceiver*> list;

		GetSiblings(list);
		for (vector<IMessageReceiver*>::iterator it = list.begin(); it != list.end(); it++)
			(*it)->SendMessageDown(message, level);

		--level;

		GetChildren(list);
		for (vector<IMessageReceiver*>::iterator it = list.begin(); it != list.end(); it++)
			(*it)->SendMessageDown(message, level);
	}
}
