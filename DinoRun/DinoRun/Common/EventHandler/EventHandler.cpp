#include "EventHandler.h"


void EventHandler::SendEvent(const MessageStruct& msg)
{
	MessageStruct message = msg;
	//서버에 message를 send
}

void EventHandler::RegisterEvent(const MessageStruct& msg)
{
	//서버로 부터 받은 message를 큐에 등록
	MessageStruct message = msg;
	messageQueue.emplace_back(message);
}

void EventHandler::Update()
{
	while (messageQueue.size())
	{
		MessageStruct msg = *messageQueue.begin();

		m_pCurScene->ProcessEvent(msg);

		messageQueue.pop_front();  //메시지 삭제
	}
}