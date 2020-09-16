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
	register�� ť�� ���� ��ɵ��� �ϰ�ó���ϴ� �κ�.
	�ַ� ������Ʈ�� Ȱ��ȭ����, ����, �߰����� �����Ѵ�. 
	*/
	while (messageQueue.size())
	{
		MessageStruct msg = *messageQueue.begin();

		m_pCurScene->ProcessEvent(msg);

		messageQueue.pop_front();  //�޽��� ����
	}
}