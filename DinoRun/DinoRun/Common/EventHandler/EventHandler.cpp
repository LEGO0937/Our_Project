#include "EventHandler.h"


void EventHandler::SendEvent(const MessageStruct& msg)
{
	MessageStruct message = msg;
	//서버에 message를 send

	//서버는 받고 6개의 각 메시지큐에 msg를 채워줌
	//그후 클라에서 이벤트 업데이트를 위해 update함수에서 이벤트 요구 send시
	// 각 아이디에 맞는 큐를 넘겨주고 서버는 넘겨준 큐를 clear시킴.
}

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