#pragma once
#include "../stdafx.h"
#include "SingleTon/Singleton.h"
#include "../../Scenes/BaseScene.h"
#include "../../CShaders/Shader.h"
struct MessageStruct
{
	string name;
	CObInstancingShader* shader;
	CreateManager* createMgr;
	XMFLOAT4X4 departMat;
	XMFLOAT4X4 arriveMat;

	MessageStruct() {}
	MessageStruct(const MessageStruct& msg)
	{
		name = msg.name;
		shader = msg.shader;
		createMgr = msg.createMgr;
		departMat = msg.departMat;
		arriveMat = msg.arriveMat;
	}
};

class EventHandler : public Singleton<EventHandler>
{
private:
	shared_ptr<BaseScene> m_pCurScene;
	deque<MessageStruct> messageQueue;
public:
	EventHandler() {}
	void SetCurScene(shared_ptr<BaseScene> curScene) { m_pCurScene = curScene; }
	void ResetCurScene() { m_pCurScene.reset(); }
	void CallBack(const MessageStruct& msg);

	void Update();
};