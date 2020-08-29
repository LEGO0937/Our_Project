#include "EventHandler.h"

void EventHandler::RegisterEvent(const MessageStruct& msg)
{
	//������ ���� ���� message�� ť�� ���
	MessageStruct message = msg;
	messageQueue.emplace_back(message);
}

void EventHandler::Update()
{
	
	/*
	���� send�� �׿��ִ� �޽��� �޾ƿ���
	recv�� �޾ƿͼ� message Queue�� �߰�
	���� �޽��� ����ŭ ������Ʈ
	*/
	while (messageQueue.size())
	{
		MessageStruct msg = *messageQueue.begin();

		m_pCurScene->ProcessEvent(msg);

		messageQueue.pop_front();  //�޽��� ����
	}
}