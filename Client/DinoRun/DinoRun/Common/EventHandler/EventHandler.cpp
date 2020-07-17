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
		////메시지 내용에 따른 처리구간
		if (msg.msgName == "Add_Model")
		{
			m_pCurScene->AddModelObject(msg);
			//mes.shader->addObject(mes.createMgr, mes.departMat, mes.arriveMat);
		}
		else if (msg.msgName == "Delete_Model")
		{
			m_pCurScene->DeleteModelObject(msg);
		}
		else if (msg.msgName == "Add_Particle")
		{
			m_pCurScene->AddParticle(msg);
		}
		else if (msg.msgName == "DisEnable_Model")
		{
			m_pCurScene->DisEnableModel(msg);
		}
		messageQueue.pop_front();  //메시지 삭제
	}
}