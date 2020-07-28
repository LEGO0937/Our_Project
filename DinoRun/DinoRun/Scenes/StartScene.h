#pragma once


#include "BaseScene.h"

class CObInstancingShader;
class CUiShader;
class CObjectsShader;

class StartScene : public BaseScene
{
public:
	StartScene();
	~StartScene();

	void ProcessInput(HWND hWnd, float deltaTime);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime);

	void BuildObjects(shared_ptr<CreateManager> pCreateManager);

	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void CreateShaderVariables(CreateManager* pCreateManager);
	virtual void UpdateShaderVariables();
	virtual void ReleaseShaderVariables();

	virtual void AnimateObjects(float fTimeElapsed);  //�ִϸ��̼� ������Ʈ
	virtual SceneType Update(CreateManager* pCreateManager, float fTimeElapsed);  // �浹ó�� �� ����

	void Render();

	virtual void setPlayer(CPlayer* player);
	virtual void setCamera(CCamera* camera);

	virtual string GetId() { return gameTexts[0].text; }
	virtual string GetPassWord() { return gameTexts[1].text; }

	virtual void ProcessPacket(char* packet, float fTimeElapsed);
private:
	vector<CObInstancingShader*> instacingBillBoardShaders;
	vector<CUiShader*> instacingUiShaders;
	bool isClickedLogin = false;
	bool isClickedID = false;
	bool isClickedPassWord = false;
};
