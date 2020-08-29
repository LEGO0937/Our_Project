#include "EventHandler.h"

void EventHandler::RegisterEvent(const MessageStruct& msg)
{
	//서버로 부터 받은 message를 큐에 등록
	MessageStruct message = msg;
	messageQueue.emplace_back(message);
}

void EventHandler::Update()
{
	
	/*
	먼저 send로 쌓여있는 메시지 받아오기
	recv로 받아와서 message Queue에 추가
	그후 메시지 수만큼 업데이트
	*/
	while (messageQueue.size())
	{
		MessageStruct msg = *messageQueue.begin();

		m_pCurScene->ProcessEvent(msg);

		messageQueue.pop_front();  //메시지 삭제
	}
}