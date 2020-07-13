#pragma once
#include "../stdafx.h"
#include "SingleTon/Singleton.h"
#include "../../Scenes/BaseScene.h"
#include "../../CShaders/Shader.h"

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