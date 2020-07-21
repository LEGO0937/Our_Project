#include "EventHandler.h"


void EventHandler::SendEvent(const MessageStruct& msg)
{
	MessageStruct message = msg;
	//������ message�� send
}

void EventHandler::RegisterEvent(const MessageStruct& msg)
{
	//������ ���� ���� message�� ť�� ���
	MessageStruct message = msg;
	messageQueue.emplace_back(message);
}

void EventHandler::Update()
{
	while (messageQueue.size())
	{
		MessageStruct msg = *messageQueue.begin();

		m_pCurScene->ProcessEvent(msg);

		messageQueue.pop_front();  //�޽��� ����
	}
}