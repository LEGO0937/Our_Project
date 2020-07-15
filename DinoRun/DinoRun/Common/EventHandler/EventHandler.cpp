#include "EventHandler.h"


void EventHandler::CallBack(const MessageStruct& msg)
{
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