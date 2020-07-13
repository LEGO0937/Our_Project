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
		MessageStruct mes = *messageQueue.begin();
		vector<string> vStrings = split(mes.name, "_");
		////�޽��� ���뿡 ���� ó������
		if (vStrings[0] == "ADD")
		{
			mes.shader->addObject(mes.createMgr, mes.departMat, mes.arriveMat);
		}
		else if (vStrings[0] == "DELETE")
		{
		
		}
		messageQueue.pop_front();  //�޽��� ����
	}
}