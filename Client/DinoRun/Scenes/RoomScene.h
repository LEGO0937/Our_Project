#pragma once

#include "BaseScene.h"

class CUiShader;


class RoomScene : public BaseScene
{
public:
	RoomScene();
	~RoomScene();

	void ProcessInput(HWND hWnd, float deltaTime);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime);

	void BuildObjects(shared_ptr<CreateManager> pCreateManager);
	virtual void BuildLights() {}
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void CreateShaderVariables(shared_ptr<CreateManager> pCreateManager);
	virtual void UpdateShaderVariables();
	virtual void ReleaseShaderVariables();

	virtual void AnimateObjects(float fTimeElapsed);  //�ִϸ��̼� ������Ʈ
	virtual SceneType Update(float fTimeElapsed);  // �浹ó�� �� ����

	void Render();

	virtual void setPlayer(CPlayer* player);
	virtual void setCamera(CCamera* camera);

private:
	vector<CUiShader*> instacingUiShaders;
	bool isClickedButton = false;

};
