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
	bool completeBuild = false;

public:
	EventHandler() {}
	void SetCurScene(shared_ptr<BaseScene> curScene) { m_pCurScene = curScene; }
	void ResetCurScene() { m_pCurScene.reset(); messageQueue.clear(); }
	void RegisterEvent(const MessageStruct& msg);

	bool GetIsBuild() { return completeBuild; }
	void SwitchIsBuild() { completeBuild = !completeBuild; }

	void Update();

public:
	string m_sWinner = "";
	int m_iMinute = 0;
	float m_fSecond = 0.0f;

};