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
	register로 큐에 쌓인 명령들을 일괄처리하는 부분.
	주로 오브젝트의 활성화여부, 삭제, 추가등을 관리한다. 
	*/
	while (messageQueue.size())
	{
		MessageStruct msg = *messageQueue.begin();

		m_pCurScene->ProcessEvent(msg);

		messageQueue.pop_front();  //메시지 삭제
	}
}